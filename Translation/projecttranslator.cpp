/*****************************************************************************
**
** Copyright (C) 2014 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 29/10/2014.
**
** This file is part of Q-UML (UML tool for Qt).
**
** Q-UML is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Q-UML is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.

** You should have received a copy of the GNU Lesser General Public License
** along with Q-UML.  If not, see <http://www.gnu.org/licenses/>.
**
*****************************************************************************/
#include "projecttranslator.h"

#include <QRegularExpression>
#include <QDebug>

#include <boost/range/algorithm/find_if.hpp>

#include <DB/Database.h>
#include <DB/ProjectDatabase.h>
#include <Entity/Enum.h>
#include <Entity/ExtendedType.h>
#include <Entity/Scope.h>
#include <Entity/field.h>
#include <Entity/ClassMethod.h>
#include <Entity/Union.h>
#include <Entity/Class.h>
#include <Entity/TemplateClassMethod.h>
#include <Entity/TemplateClass.h>
#include <Entity/Property.h>
#include <Utility/helpfunctions.h>

#include "enums.h"
#include "templates.cpp"
#include "Constants.h"
#include "code.h"
#include "signaturemaker.h"

using namespace boost;

namespace {

    void addNamesapceHelper(QString &code, const QStringList &scopesNames, const QString &indent,
                            QString scopeTemplate)
    {
        if (!code.isEmpty()) {
            if (!indent.isEmpty()) {
                if (!code.startsWith("\n"))
                    code.prepend(indent);
                code.replace(QRegularExpression("(\n)(.)"), "\\1" + indent + "\\2");
            }

            code = scopeTemplate.replace("%name%", scopesNames.front())
                                .replace("%code%", code);
            code.append(" // namespace " + scopesNames.front());
        }
    }

    template <class T, class Map, class Self, class Func>
    void addTranslator(Map &map, Self * this_, Func f)
    {
        using namespace std::placeholders;
        map[T::staticHashType()] =
            std::bind(
                f, this_, std::bind(std::static_pointer_cast<T,Common::BasicElement>, _1), _2, _3, _4
            );
    }
}

namespace Translation {

    /**
     * @brief ProjectTranslator::ProjectTranslator
     */
    ProjectTranslator::ProjectTranslator()
        : ProjectTranslator(nullptr, nullptr)
    {}

    /**
     * @brief ProjectTranslator::ProjectTranslator
     * @param globalDb
     * @param projectDb
     */
    ProjectTranslator::ProjectTranslator(const DB::SharedDatabase &globalDb,
                                         const DB::SharedProjectDatabase &projectDb)
        : m_GlobalDatabase(globalDb)
        , m_ProjectDatabase(projectDb)
    {
        auto &t = m_translators;
        addTranslator<Entity::Type>               (t, this, &ProjectTranslator::translateType    );
        addTranslator<Entity::ExtendedType>       (t, this, &ProjectTranslator::translateExtType );
        addTranslator<Entity::Field>              (t, this, &ProjectTranslator::translateField   );
        addTranslator<Entity::Enum>               (t, this, &ProjectTranslator::translateEnum    );
        addTranslator<Entity::ClassMethod>        (t, this, &ProjectTranslator::translateMethod  );
        addTranslator<Entity::TemplateClassMethod>(t, this, &ProjectTranslator::translateMethod  );
        addTranslator<Entity::Union>              (t, this, &ProjectTranslator::translateUnion   );
        addTranslator<Entity::Class>              (t, this, &ProjectTranslator::translateClass   );
        addTranslator<Entity::TemplateClass>      (t, this, &ProjectTranslator::translateClass   );
    }

    /**
     * @brief ProjectTranslator::checkDb
     */
    void ProjectTranslator::checkDb() const
    {
        Q_ASSERT_X(m_GlobalDatabase, "ProjectTranslator", "global database not found");
        Q_ASSERT_X(m_ProjectDatabase, "ProjectTranslator", "project database not found");
    }

    /**
     * @brief ProjectTranslator::generateCodeForExtTypeOrType
     * @param id
     * @param options
     * @param localeDatabase
     * @param classDatabase
     * @return
     */
    QString ProjectTranslator::generateCodeForExtTypeOrType(const Common::ID &id,
                                                            const TranslatorOptions &options,
                                                            const DB::SharedDatabase &localeDatabase,
                                                            const DB::SharedDatabase &classDatabase) const
    {
        checkDb();
        auto t = Util::findType(id, localeDatabase, classDatabase, m_ProjectDatabase, m_GlobalDatabase);
        if (!t)
            return "";

        return t->hashType() == Entity::ExtendedType::staticHashType()
                   ? translateExtType(std::dynamic_pointer_cast<Entity::ExtendedType>(t),
                                      options, localeDatabase, classDatabase).toHeader
                   : translateType(t, options, localeDatabase, classDatabase).toHeader;
    }

    /**
     * @brief ProjectTranslator::generateClassSection
     * @param _class
     * @param localeDatabase
     * @param section
     * @param out
     */
    void ProjectTranslator::generateClassSection(const Entity::SharedClass &_class,
                                                 const DB::SharedDatabase &localeDatabase,
                                                 Entity::Section section, QString &out) const
    {
        // Extract all kinds off methods (include optional methods)
        Entity::MethodsList methods;
        Entity::MethodsList _slots;
        Entity::MethodsList _signals;
        for (auto const& m : _class->allMethods(section)) {
            if (m->isSlot())
                _slots << m;
            else if (!m->isSignal())
                methods << m;
            else
                _signals << m;
        }

        // Extract all fields
        Entity::FieldsList fields = _class->fields(section);
        fields << _class->optionalFields(section);

        // Check if section is not empty
        if (methods.isEmpty() && _slots.isEmpty() && _signals.isEmpty() && fields.isEmpty())
            return;

        // Add empty string above section
        out.append("\n");

        // Check if we need to add section name
        bool needSection = _class->kind() == Entity::ClassType ||
                           (_class->kind() != Entity::StructType && section != Entity::Public);

        // Generate methods
        generateSectionMethods(methods, localeDatabase, needSection, section, ":\n"/*marker*/, out);
        generateSectionMethods(_slots, localeDatabase, needSection, section, " SLOTS:\n", out);
        generateSectionMethods(_signals, localeDatabase, needSection, section, "signals:\n", out);

        // Generate fields
        if (!fields.isEmpty()) {
            if (needSection)
                out.append(INDENT + Util::sectionToString(section) + ":\n");

            generateFileds(fields, _class, localeDatabase, needSection ? DOUBLE_INDENT : INDENT, out);
        }
    }

    /**
     * @brief ProjectTranslator::generateFieldsAndMethods
     * @param methods
     * @param localeDatabase
     * @param indent
     * @param out
     */
    void ProjectTranslator::generateMethods(const Entity::MethodsList &methods,
                                            const DB::SharedDatabase &localeDatabase,
                                            const QString &indent,
                                            QString &out) const
    {
        QStringList methodsList;
        for(auto &&m : methods)
            methodsList << translate(m, WithNamespace, localeDatabase).toHeader.prepend(indent);

        out.append(methodsList.join(";\n"));
        if (!methodsList.isEmpty())
            out.append(";\n");
    }

    /**
     * @brief ProjectTranslator::generateFileds
     * @param fields
     * @param _class
     * @param localeDatabase
     * @param indent
     * @param out
     */
    void ProjectTranslator::generateFileds(const Entity::FieldsList &fields,
                                           const Entity::SharedClass &_class,
                                           const DB::SharedDatabase &localeDatabase,
                                           const QString &indent, QString &out) const
    {
        QStringList fieldsList;
        for (auto &&field : fields) {
            auto t = Util::findType(field->typeId(), localeDatabase,
                                       m_GlobalDatabase, m_ProjectDatabase);
            if (!t) {
                qDebug() << "Failed to find field with type:" << QString::number(field->typeId().value());
                break;
            }

            fieldsList << translate(field,
                                    t->scopeId() == _class->scopeId() ? NoOptions : WithNamespace,
                                    localeDatabase)
                          .toHeader.prepend(indent);
        }
        out.append(fieldsList.join(";\n"));

        if (!fieldsList.isEmpty())
            out.append(";\n");
    }

    /**
     * @brief ProjectTranslator::generateTemplatePart
     * @param result
     * @param t
     * @param withDefaultTypes
     */
    void ProjectTranslator::generateTemplatePart(QString &result, const Entity::SharedTemplate &t,
                                                 bool withDefaultTypes) const
    {
        if (!t)
           return;

        result.prepend(TEMPLATE_TEMPLATE + "\n");
        QStringList parameters;
        for (auto &&parameter : t->templateParameters()) {
            parameters << generateCodeForExtTypeOrType(parameter.first,
                                                       NoOptions,
                                                       t->database())
                          .prepend("class ")
                          .trimmed();
            if (parameter.second.isValid() && parameter.second != Common::ID::nullID() && withDefaultTypes) {
                parameters.last()
                          .append(" = ")
                          .append(generateCodeForExtTypeOrType(parameter.second,
                                                               WithNamespace,
                                                               t->database()));
                parameters.last() = parameters.last().trimmed();
            }
        }
        result.replace("%template_parameters%", parameters.join(", "));
    }

    /**
     * @brief ProjectTranslator::toHeader
     * @param m
     * @param classDatabase
     * @return
     */
    bool ProjectTranslator::toHeader(const Entity::SharedMethod &m,
                                     const DB::SharedDatabase &classDatabase) const
    {
        if (m->type() == Entity::TemplateMethod)
           return true;
        if (!classDatabase)
           return false;

        Entity::FieldsList fields(m->parameters());
        auto it = range::find_if(fields,
                                 [&](auto &&f) { return classDatabase->typeByID(f->typeId()); });
        return it != fields.end() || classDatabase->typeByID(m->returnTypeId());
    }

    /**
     * @brief ProjectTranslator::generateSectionMethods
     * @param methods
     * @param localeDatabase
     * @param needSection
     * @param section
     */
    void ProjectTranslator::generateSectionMethods(const Entity::MethodsList &methods,
                                                   const DB::SharedDatabase &localeDatabase,
                                                   bool needSection,
                                                   Entity::Section section,
                                                   const QString& marker,
                                                   QString &out) const
    {
        if (!methods.isEmpty()) {
            if (needSection)
                out.append(INDENT + Util::sectionToString(section) + marker);

            generateMethods(methods, localeDatabase, needSection ? DOUBLE_INDENT : INDENT, out);
        }
    }

    /**
     * @brief ProjectTranslator::translate
     * @param _enum
     * @param options
     * @param localeDatabase
     * @param classDatabase
     * @return
     */
    Code ProjectTranslator::translateEnum(const Entity::SharedEnum &_enum,
                                          const TranslatorOptions  &options,
                                          const DB::SharedDatabase &localeDatabase,
                                          const DB::SharedDatabase &classDatabase) const
    {
        // compatibility with API
        Q_UNUSED(localeDatabase)
        Q_UNUSED(classDatabase)

        if (!_enum)
            return Code("\ninvalid enum\n", "");
        checkDb();

        QString result(ENUM_TEMPLATE);

        result.replace("%class%", _enum->isStrong() ? "class " : "");
        result.replace("%name%",  _enum->name());

        QString typeName("");
        auto typeId(_enum->enumTypeId());
        if (typeId.isValid()) {
            auto t = Util::findType(typeId, m_GlobalDatabase, m_ProjectDatabase);
            if (t)  typeName.append(" : ").append(t->name());
        }
        result.replace("%type%", typeName);

        QStringList values;
        if (options & GenerateNumbers) {
            for (auto &&v : _enum->enumerators())
                values << QString("%1 = %2").arg(v->name(), QString::number(v->value()));
        } else {
            for (auto &&v : _enum->enumerators())
                values << v->name();
        }
        result.replace("%values%", values.isEmpty() ? "" : values.join(", "));

        return Code(result, "");
    }

    /**
     * @brief ProjectTranslator::translate
     * @param method
     * @param options
     * @param localeDatabase
     * @param classDatabase
     * @return
     */
    Code ProjectTranslator::translateMethod(const Entity::SharedMethod &method,
                                            const TranslatorOptions  &options,
                                            const DB::SharedDatabase &localeDatabase,
                                            const DB::SharedDatabase &classDatabase) const
    {
        // compatibility with API
        Q_UNUSED(classDatabase)

        if (!method)
            return Code("\ninvalid method\n", "");
        checkDb();

        QString result(METHOD_TEMPLATE);

        Entity::SharedTemplateClassMethod m(nullptr);
        DB::SharedDatabase templateDb(nullptr);
        if (method->type() == Entity::TemplateMethod) {
            m = std::dynamic_pointer_cast<Entity::TemplateClassMethod>(method);
            if (m) {
                templateDb = m->database();
                generateTemplatePart(result, std::static_pointer_cast<Entity::Template>(m));
            }
        }

        QString lhsIds("");
        if (!(options & NoLhs)) {
            QStringList lhsIdsList;
            for (auto &&lhsId : method->lhsIdentificators())
                lhsIdsList << Util::methodLhsIdToString(lhsId);
            if (!lhsIdsList.isEmpty())
                lhsIds.append(lhsIdsList.join(" ")).append(" ");
        }
        result.replace("%lhs_k%", lhsIds);

        QString rType = generateCodeForExtTypeOrType(method->returnTypeId(), options, templateDb,
                                                     localeDatabase);
        if (!rType.isEmpty() && !rType.endsWith("*") && !rType.endsWith("&") &&
            !rType.endsWith(QChar::Space))
            rType.append(QChar::Space);
        result.replace("%r_type%", rType);

        result.replace("%name%", method->name());

        QString parameters("");
        QStringList parametersList;
        for (auto &&p : method->parameters()) {
            p->removePrefix(); // TODO check why!
            p->removeSuffix(); // TODO check why!

            TranslatorOptions newOptions((options & NoDefaultName) ? NoDefaultName : NoOptions);
            auto t = Util::findType(p->typeId(), localeDatabase,
                                       m_GlobalDatabase, m_ProjectDatabase);
            if (!t || method->scopeId() != t->scopeId() || !method->scopeId().isValid())
               newOptions |= WithNamespace;

            parametersList << translate(p, newOptions, templateDb, localeDatabase).toHeader;

        }
        if (!parametersList.isEmpty())
            parameters.append(parametersList.join(", "));
        result.replace("%parameters%", parameters);

        QString rhsId(Util::methodRhsIdToString(method->rhsIdentificator()));
        if (method->rhsIdentificator() != Entity::RhsIdentificator::None)
            rhsId.prepend(" ");
        result.replace("%rhs_k%", rhsId);

        result.replace("%const%", method->isConst() ? " const" : "");

        return Code(result, "");
    }

    /**
     * @brief ProjectTranslator::translate
     * @param _union
     * @param options
     * @param localeDatabase
     * @param classDatabase
     * @return
     */
    Code ProjectTranslator::translateUnion(const Entity::SharedUnion &_union,
                                           const TranslatorOptions &options,
                                           const DB::SharedDatabase &localeDatabase,
                                           const DB::SharedDatabase &classDatabase) const
    {
        // compatibility with API
        Q_UNUSED(options)
        Q_UNUSED(localeDatabase)
        Q_UNUSED(classDatabase)

        if (!_union)
            return Code("\ninvalid union\n", "");
        checkDb();

        QString result(UNION_TEMPLATE);

        result.replace("%name%", _union->name());

        QStringList fields;
        for (auto &&field : _union->fields())
            fields << translate(field).toHeader.prepend(INDENT);

        QString resultFields(fields.join(";\n"));
        if (!resultFields.isEmpty())
            resultFields.append(";\n").prepend("\n");

        result.replace("%variables%", resultFields);

        return Code(result, "");
    }

    /**
     * @brief ProjectTranslator::translate
     * @param _class
     * @param options
     * @param localeDatabase
     * @param classDatabase
     * @return
     */
    Code ProjectTranslator::translateClass(const Entity::SharedClass &_class,
                                           const TranslatorOptions &options,
                                           const DB::SharedDatabase &localeDatabase,
                                           const DB::SharedDatabase &classDatabase) const
    {
        // compatibility with API
        Q_UNUSED(options)
        Q_UNUSED(localeDatabase)
        Q_UNUSED(classDatabase)

        if (!_class)
            return Code("\ninvalid class\n", "");
        checkDb();

        QString toHeader(CLASS_TEMPLATE);

        // Add q_object
        auto allMethods = _class->allMethods(Entity::All);
        bool addQObject = range::find_if(allMethods, [](auto m){ return m->isSlot() || m->isSignal(); })
                          != boost::end(allMethods);
        toHeader.replace("%qobject%", addQObject ? "\nQ_OBJECT\n" : "");

        // Add properties
        auto properties = _class->properties();
        QStringList signatures;
        if (!properties.isEmpty()) {
            SignatureMaker maker(m_GlobalDatabase, m_ProjectDatabase,
                                 m_ProjectDatabase->scope(_class->scopeId()), _class);
            for (auto &&p : properties)
                signatures << maker.signature(p);
        }
        QString prop = signatures.isEmpty() ? "" : signatures.join(";\n");
        if (addQObject && !prop.isEmpty())
            prop.prepend("\n");
        toHeader.replace("%property%", prop);

        // Add template part if needed
        DB::SharedDatabase templateDb = nullptr;
        if (_class->hashType() == Entity::TemplateClass::staticHashType()) {
            auto tc = std::static_pointer_cast<Entity::TemplateClass>(_class);
            templateDb = tc->database();
            generateTemplatePart(toHeader, tc);
        }

        // Add class type
        toHeader.replace("%kind%", _class->kind() == Entity::ClassType ? "class " : "struct ");

        // Add class name
        toHeader.replace("%name%", _class->name());

        // Add parents
        QString parents;
        if (_class->anyParents()) {
            QStringList parentsList;
            Entity::SharedType t(nullptr);
            for (auto &&p : _class->parents()) {
                t = Util::findType(p.first, templateDb, m_GlobalDatabase, m_ProjectDatabase);
                QString parentName(Util::sectionToString(p.second) + QChar::Space);

                QString typeName("unknown type");
                if (t) {
                    TranslatorOption options =
                        t->scopeId() == _class->scopeId() ? NoOptions : WithNamespace;
                    // Translate as type, because we need only a name
                    typeName = translateType(t, options, templateDb).toHeader;
                }
                parentName.append(typeName);

                parentsList << parentName;
            }
            parents.append(" : ")
                   .append(parentsList.join(", "))
                   .append(!parentsList.isEmpty() ? " " : "");
        }
        if (_class->kind() == Entity::ClassType &&
            (_class->anyFields() || _class->anyMethods()))
            parents.append("\n");
        toHeader.replace("%parents%", parents);

        // Add sections
        QString section;
        generateClassSection(_class, templateDb, Entity::Public, section);
        generateClassSection(_class, templateDb, Entity::None, section); // For signals
        generateClassSection(_class, templateDb, Entity::Protected, section);
        generateClassSection(_class, templateDb, Entity::Private, section);
        if (!prop.isEmpty() && !section.isEmpty())
            section.prepend("\n");
        toHeader.replace("%section%", section);

        if (section.isEmpty() && parents.isEmpty())
           toHeader.replace(_class->name(), _class->name() + " ");

        // Add methods impl
        Code impl = generateClassMethodsImpl(_class, templateDb);

        if (!impl.toHeader.isEmpty())
            toHeader.append("\n").append(impl.toHeader);

        return Code(toHeader, impl.toSource);
    }

    /**
     * @brief ProjectTranslator::generateClassMethodsImpl
     * @param _class
     * @param localeDatabase
     * @return
     */
    Code ProjectTranslator::generateClassMethodsImpl(const Entity::SharedClass &_class,
                                                     const DB::SharedDatabase &localeDatabase) const
    {
        if (!_class)
            return Code("\ninvalid class\n", "\ninvalid class\n");
        checkDb();

        QStringList methodsCpp;
        QStringList methodsH;
        QString method;

        Entity::SharedTemplateClass tc(std::dynamic_pointer_cast<Entity::TemplateClass>(_class));
        QString templatePart;
        QString methodTemplatePart;
        QString newName;

        if (tc) {
           generateTemplatePart(templatePart, tc, false);

           methodTemplatePart = templatePart;
           methodTemplatePart.remove("template ").remove("class ").remove("\n");

           newName = _class->name().append(methodTemplatePart);
        }

        for (auto &&m : _class->methods()) {
            // Signals have no user visible implementation
            if (m->isSignal())
                continue;

            method = translate(m, NoLhs | WithNamespace | NoDefaultName, localeDatabase).toHeader;
            if (tc)
                method.prepend(templatePart);

            method.replace(m->name(), m->name().prepend(_class->name().append("::")));
            method.append("\n{\n}\n");

            if (tc)
                method.replace(method.indexOf(_class->name()), _class->name().size(), newName);

            (toHeader(m, tc ? tc->database() : nullptr) || tc ? methodsH : methodsCpp) << method;
            method.clear();
        }

        if (tc && !methodsH.isEmpty()) {
           methodsH.last().remove(methodsH.last().size() - 1, 1);
           methodsH.first().prepend("\n");
        }

        return Code(methodsH.join("\n"), methodsCpp.join("\n"));
    }

    /**
     * @brief ProjectTranslator::generateClassMethodsImpl
     * @param _class
     * @return
     */
    Code ProjectTranslator::generateClassMethodsImpl(const Entity::SharedTemplateClass &_class) const
    {
        if (!_class)
           return Code("\ninvalid template class\n", "\ninvalid template class\n");
        checkDb();

        return generateClassMethodsImpl(std::dynamic_pointer_cast<Entity::Class>(_class),
                                        _class->database());
    }

    /**
     * @brief ProjectTranslator::addNamespace
     * @param type
     * @param code
     * @param indentCount
     */
    void ProjectTranslator::addNamespace(const Entity::SharedType &type, Code &code, uint indentCount)
    {
        if (!type || !m_ProjectDatabase)
            return;

        QString newIndent;
        while (indentCount != 0) {
            newIndent.append(INDENT);
            --indentCount;
        }

        QStringList scopesNames(Util::scopesNamesList(type,m_ProjectDatabase));
        while (!scopesNames.isEmpty()) {
            addNamesapceHelper(code.toHeader, scopesNames, newIndent, SCOPE_TEMPLATE_HEADER);
            addNamesapceHelper(code.toSource, scopesNames, newIndent, SCOPE_TEMPLATE_SOURCE);
            scopesNames.pop_front();
        }
    }

    /**
     * @brief ProjectTranslator::translate
     * @param extType
     * @param options
     * @param localeDatabase
     * @param classDatabase
     * @return
     */
    Code ProjectTranslator::translateExtType(const Entity::SharedExtendedType &extType,
                                             const TranslatorOptions &options,
                                             const DB::SharedDatabase &localeDatabase,
                                             const DB::SharedDatabase &classDatabase) const
    {
        if (!extType)
            return Code("\ninvalid extended type\n", "");

        checkDb();
        QString result(EXT_TYPE_TEMPLATE);

        result.replace("%const%", extType->isConst() ? "const " : "");

        if (extType->typeId().isValid()) {
            auto t = Util::findType(extType->typeId(), localeDatabase, classDatabase,
                                       m_ProjectDatabase,
                                       m_GlobalDatabase);
            result.replace("%name%", t ?
                generateCodeForExtTypeOrType(t->id(), options, localeDatabase, classDatabase) : "");
        } else
            result.remove("%name%");

        QString pl("");
        if (!extType->pl().isEmpty()) {
            for (auto &&c : extType->pl()) {
                pl.append(c.first);
                if (c.second) pl.append(" const ");
            }
            pl = pl.trimmed();
            pl.prepend(" ");
        }
        result.replace("%pl%", pl);

        QString params("");
        if (!extType->templateParameters().isEmpty()) {
            QStringList names;
            Entity::SharedType t = nullptr;
            for (auto &&id : extType->templateParameters()) {
                t = Util::findType(id, localeDatabase, classDatabase, m_ProjectDatabase,
                                      m_GlobalDatabase);
                if (t)
                    names << t->name();
            }
            params = names.join(", ");
            params.append(">");
            params.prepend("<");
        }
        result.replace("%template_params%", params);

        if ((options & WithAlias) && extType->name() != DEFAULT_NAME)
            result.prepend(QString("using %1 = ").arg(extType->name())).append(";");

        return Code(result, "");
    }

    /**
     * @brief ProjectTranslator::translate
     * @param field
     * @param options
     * @param localeDatabase
     * @param classDatabase
     * @return
     */
    Code ProjectTranslator::translateField(const Entity::SharedField &field,
                                           const TranslatorOptions  &options,
                                           const DB::SharedDatabase &localeDatabase,
                                           const DB::SharedDatabase &classDatabase) const
    {
        if (!field)
            return Code("\ninvalid field\n", "");
        checkDb();
        QString result(FIELD_TEMPLATE);

        QStringList keywords;
        if (!field->keywords().isEmpty())
            for (auto &&keyword : field->keywords())
                keywords << Util::fieldKeywordToString(keyword);
        result.replace("%keywords%", keywords.isEmpty() ? "" : keywords.join(" ").append(" "));

        QString type = generateCodeForExtTypeOrType(field->typeId(), options,localeDatabase,
                                                    classDatabase);
        if (!type.isEmpty() && !type.endsWith("*") && !type.endsWith("&") &&
            !type.endsWith(QChar::Space))
            type.append(QChar::Space);
        result.replace("%type%", type);

        result.replace("%name%", field->fullName());

        if (!field->defaultValue().isEmpty() && !(options & NoDefaultName))
            result.append( " = " + field->defaultValue() );

        return Code(result, "");
    }

    /**
     * @brief ProjectTranslator::projectDatabase
     * @return
     */
    DB::SharedProjectDatabase ProjectTranslator::projectDatabase() const
    {
        return m_ProjectDatabase;
    }

    /**
     * @brief ProjectTranslator::setProjectDatabase
     * @param projectDatabase
     */
    void ProjectTranslator::setProjectDatabase(const DB::SharedProjectDatabase &projectDatabase)
    {
        m_ProjectDatabase = projectDatabase;
    }

    Code ProjectTranslator::translate(const Common::SharedBasicEntity &e,
                                      const TranslatorOptions &options,
                                      const DB::SharedDatabase &localeDatabase,
                                      const DB::SharedDatabase &classDatabase) const
    {
        Q_ASSERT(e);
        if (!e)
            return Code();

        auto hash = e->hashType();
        Q_ASSERT(m_translators.contains(hash));
        if (!m_translators.contains(hash))
            return Code();

        return m_translators[hash](e, options, localeDatabase, classDatabase);
    }

    /**
     * @brief ProjectTranslator::translate
     * @param type
     * @param options
     * @param localeDatabase
     * @param classDatabase
     * @return
     */
    Code ProjectTranslator::translateType(const Entity::SharedType &type,
                                          const TranslatorOptions &options,
                                          const DB::SharedDatabase &localeDatabase,
                                          const DB::SharedDatabase &classDatabase) const
    {
        QStringList scopesNames;
        auto id = type->scopeId();
        Entity::SharedScope scope = Util::findScope(id, localeDatabase, classDatabase,
                                                       m_ProjectDatabase, m_GlobalDatabase);
        do {
            if (!scope || id == Common::ID::globalScopeID() ||
                id == Common::ID::localTemplateScopeID())
                break;

            if (scope->name() != DEFAULT_NAME)
                scopesNames.prepend(scope->name());

            id = scope->scopeId();
            scope = Util::findScope(id, localeDatabase, classDatabase,
                                       m_ProjectDatabase, m_GlobalDatabase);
        } while (true);

        if (!scopesNames.isEmpty() && (options & WithNamespace)) {
            scopesNames << type->name();
            return Code(scopesNames.join("::"), "");
        }

        return Code(type->name(), "");
    }

    /**
     * @brief ProjectTranslator::globalDatabase
     * @return
     */
    DB::SharedDatabase ProjectTranslator::globalDatabase() const
    {
        return m_GlobalDatabase;
    }

    /**
     * @brief ProjectTranslator::setGlobalDatabase
     * @param globalDatabase
     */
    void ProjectTranslator::setGlobalDatabase(const DB::SharedDatabase &globalDatabase)
    {
        m_GlobalDatabase = globalDatabase;
    }

} // namespace translation
