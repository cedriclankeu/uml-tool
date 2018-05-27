/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 12/07/2015.
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
#include "componentsmaker.h"

#include <range/v3/algorithm/find_if.hpp>

#include <Entity/field.h>
#include <Entity/Scope.h>
#include <Entity/Type.h>
#include <Entity/ExtendedType.h>
#include <Entity/Property.h>
#include <Entity/Components/componentsignatureparser.h>

#include <Models/ApplicationModel.h>

#include <Utility/helpfunctions.h>

#include "token.h"
#include "enums.h"
#include "componentscommon.h"
#include "Constants.h"

namespace Components {

    namespace {

        template <class Method>
        inline void addCommon(const Tokens &tokens, PropGroupNames group, Method method, Entity::SharedProperty &property)
        {
            Q_ASSERT(tokens[int(group)]->isSingle());
            const auto &name = tokens[int(group)]->token();
            if (!name.isEmpty())
                (property.get()->*method)(name);
        }

        template <class AddMethod, class SetMethod>
        inline void addDS(const Tokens &tokens, PropGroupNames group, AddMethod addMethod, SetMethod setMethod,
                          Entity::SharedProperty &property)
        {
            Q_ASSERT(tokens[int(group)]->isSingle());
            const auto &name = tokens[int(group)]->token();
            if (name.isEmpty())
                return;

            bool ok = false;
            bool result = Util::toBool(name, ok);

            if (ok)
                (property.get()->*setMethod)(result);
            else
                (property.get()->*addMethod)(name);
        }

        template <class SetMethod>
        inline void addExtra(const Tokens &tokens, PropGroupNames group, SetMethod setMethod,
                             Entity::SharedProperty &property)
        {
            Q_ASSERT(tokens[int(group)]->isSingle());
            const auto &name = tokens[int(group)]->token();
            if (name.isEmpty())
                return;

            bool ok = false;
            bool result = Util::toBool(name, ok);

            // set true or false if it bool (e.g. STORED bool), otherwise set just true (e.g. CONSTANT)
            (property.get()->*setMethod)(ok ? result : true);
        }
    }

    /**
     * @brief ComponentsMaker::ComponentsMaker
     */
    ComponentsMaker::ComponentsMaker()
        : ComponentsMaker(nullptr, nullptr, nullptr)
    {
    }

    /**
     * @brief ComponentsMaker::ComponentsMaker
     * @param model
     * @param entity
     * @param scope
     */
    ComponentsMaker::ComponentsMaker(const Models::SharedApplicationModel &model, const Entity::SharedType &entity,
                                     const Entity::SharedScope &scope)
        : m_Model(model)
        , m_Entity(entity)
        , m_Scope(scope)
    {
        m_ComponentMakerMap[Models::DisplayPart::Fields]     = [&](auto tokens){ return this->makeField(tokens);    };
        m_ComponentMakerMap[Models::DisplayPart::Methods]    = [&](auto tokens){ return this->makeMethod(tokens);   };
        m_ComponentMakerMap[Models::DisplayPart::Properties] = [&](auto tokens){ return this->makeProperty(tokens); };
    }

    /**
     * @brief ComponentsMaker::makeComponent
     * @param signature
     * @return
     */
    OptionalEntity ComponentsMaker::makeComponent(const Components::Tokens &tokens, Models::DisplayPart display)
    {
        if (!tokens.isEmpty())
            return m_ComponentMakerMap[display](tokens);

        return {tr("Wrong signature"), Common::SharedBasicEntity()};
    }

    /**
     * @brief ComponentsMaker::model
     * @return
     */
    Models::SharedApplicationModel ComponentsMaker::model() const
    {
        return m_Model;
    }

    /**
     * @brief ComponentsMaker::setModel
     * @param model
     */
    void ComponentsMaker::setModel(const Models::SharedApplicationModel &model)
    {
        m_Model = model;
    }

    /**
     * @brief ComponentsMaker::entity
     * @return
     */
    Entity::SharedType ComponentsMaker::entity() const
    {
        return m_Entity;
    }

    /**
     * @brief ComponentsMaker::setEntity
     * @param entity
     */
    void ComponentsMaker::setEntity(const Entity::SharedType &entity)
    {
        m_Entity = entity;
    }

    /**
     * @brief ComponentsMaker::scope
     * @return
     */
    Entity::SharedScope ComponentsMaker::scope() const
    {
        return m_Scope;
    }

    /**
     * @brief ComponentsMaker::setScope
     * @param scope
     */
    void ComponentsMaker::setScope(const Entity::SharedScope &scope)
    {
        m_Scope = scope;
    }

    /**
     * @brief ComponentsMaker::checkCommonState
     * @return
     */
    bool ComponentsMaker::checkCommonState() const
    {
        return m_Model && m_Model->globalDatabase() && m_Model->currentProject() &&
                m_Model->currentProject()->database();
    }

    /**
     * @brief ComponentsMaker::makeType
     * @param tokens
     * @return
     */
    OptionalEntity ComponentsMaker::makeType(const Tokens &tokens)
    {
        Q_ASSERT(!tokens.isEmpty() && tokens[int(TypeGroups::Typename)]->isSingle() &&
                 !tokens[int(TypeGroups::Typename)]->token().isEmpty());

        // Check common type
        const QString &typeName = tokens[int(TypeGroups::Typename)]->token();
        Entity::SharedType type;
        if (!tokens[int(TypeGroups::Namespaces)]->token().isEmpty()) {
            // TODO: should be already splitted i.e. is not single
            Q_ASSERT(tokens[int(TypeGroups::Namespaces)]->isSingle());
            auto names = tokens[int(TypeGroups::Namespaces)]->token()
                         .split("::", QString::SkipEmptyParts);
            auto scope = m_Model->globalDatabase()->chainScopeSearch(names);
            if (!scope)
                scope = m_Model->currentProject()->database()->chainScopeSearch(names);

            if (scope)
                type = scope->type(typeName);
        } else {
            // First of all check in all scopes of global database
            const Entity::ScopesList &scopes = m_Model->globalDatabase()->scopes();
            ranges::find_if(scopes, [&](auto scope){ type = scope->type(typeName); return !!type; });

            // If not found, try to check project database
            if (!type) {
                auto db = m_Model->currentProject()->database();
                ranges::find_if(db->scopes(), [&](auto scope){ type = scope->type(typeName); return !!type; });
            }
        }

        if (!type)
            return {tr("Wrong type: %1.").arg(typeName), nullptr};

        // Check extra stuff
        Entity::SharedExtendedType extType = std::make_shared<Entity::ExtendedType>();
        extType->setTypeId(type->id());
        extType->setScopeId(m_Scope->id());
        Q_ASSERT(tokens[int(TypeGroups::ConstStatus)]->isSingle());
        extType->setConstStatus(!tokens[int(TypeGroups::ConstStatus)]->token().isEmpty());

        Q_ASSERT(tokens[int(TypeGroups::PLC)]->isSingle());
        if (!tokens[int(TypeGroups::PLC)]->token().isEmpty()) {
            QString plc = tokens[int(TypeGroups::PLC)]->token();
            plc.remove(QChar::Space);

            if (plc.startsWith("const")) {
                extType->setConstStatus(true);
                plc.remove(0, 4);
            }

            while (!plc.isEmpty()) {
                if (plc.startsWith("const")) {
                    plc.remove(0, 5);
                } else if (plc.startsWith("*const")) {
                    extType->addPointerStatus(true);
                    plc.remove(0, 6);
                } else if (plc.startsWith("*")) {
                    extType->addPointerStatus();
                    plc.remove(0, 1);
                } else if (plc.startsWith("&")) {
                    extType->addLinkStatus();
                    plc.remove(0, 1);
                } else {
                    break;
                }
            }
        }

        // TODO: should be already splitted too
        Q_ASSERT(tokens[int(TypeGroups::TemplateArgs)]->isSingle());
        if (!tokens[int(TypeGroups::TemplateArgs)]->token().isEmpty()) {
            QStringList arguments = tokens[int(TypeGroups::TemplateArgs)]->token()
                                    .remove(QChar::Space)
                                    .split(",", QString::SkipEmptyParts);
            Entity::ScopesList scopes = m_Model->currentProject()->database()->scopes();
            for (auto && s : m_Model->globalDatabase()->scopes())
                scopes << s;

            // TODO: add namespaces, * and const
            for (auto &&name : arguments) {
                Entity::SharedType t;
                ranges::find_if(scopes, [&](auto &&sc){ t = sc->type(name); return !!t; });
                if (t)
                    extType->addTemplateParameter(t->id());
                else
                    return {tr("Template parameter \"%1\" not found.").arg(name), nullptr};
            }
        }

        if (extType->isConst() || !extType->templateParameters().isEmpty() || !extType->pl().isEmpty()) {
            const Entity::TypesList &types = m_Scope->types();
            auto it = ranges::find_if(types, [=](const Entity::SharedType &type) {
                                                  return extType->isEqual(*type, false);
                                              });
            if (it == cend(types))
                m_Model->addExistsType(m_Model->currentProject()->name(), m_Scope->id(), extType);

            return {"", extType};
         } else {
            return {"", type};
         }
    }

    /**
     * @brief ComponentsMaker::makeField
     * @return
     */
    OptionalEntity ComponentsMaker::makeField(const Tokens &tokens)
    {
        Q_ASSERT(!tokens.isEmpty() && tokens[int(FieldGroupNames::Typename)]->isSingle() &&
                 !tokens[int(FieldGroupNames::Typename)]->token().isEmpty() &&
                 tokens[int(FieldGroupNames::Name)]->isSingle() &&
                 !tokens[int(FieldGroupNames::Name)]->token().isEmpty());

        Q_ASSERT(checkCommonState());

        // Make field with lhs keywords
        auto newField = std::make_shared<Entity::Field>();
        newField->setName(tokens[int(FieldGroupNames::Name)]->token());
        if (!tokens[int(FieldGroupNames::LhsKeywords)]->token().isEmpty()) {
            auto keyword =
                Util::fieldKeywordFromString(tokens[int(FieldGroupNames::LhsKeywords)]->token());
            Q_ASSERT(keyword != Entity::FieldKeyword::Invalid);
            newField->addKeyword(keyword);
        }

        // Make type
        Tokens typeTokens(int(TypeGroups::GroupsCount));
        std::copy(std::begin(tokens) + int(FieldGroupNames::ConstStatus),
                  std::begin(tokens) + int(FieldGroupNames::Name),        // do not include name
                  std::begin(typeTokens) + int(TypeGroups::ConstStatus)); // add first group offset
        auto optionalType = makeType(typeTokens);
        if (!optionalType.errorMessage.isEmpty())
            return {optionalType.errorMessage, nullptr};

        Q_ASSERT(optionalType.resultEntity);
        newField->setTypeId(optionalType.resultEntity->id());

        return {"", newField};
    }

    OptionalEntity ComponentsMaker::makeProperty(const Tokens &tokens)
    {
        Q_ASSERT(!tokens.isEmpty() && tokens[int(PropGroupNames::Type)]->isSingle() &&
                 !tokens[int(PropGroupNames::Type)]->token().isEmpty() &&
                 tokens[int(PropGroupNames::Name)]->isSingle() &&
                 !tokens[int(PropGroupNames::Name)]->token().isEmpty());

        Q_ASSERT(checkCommonState());

        Entity::SharedProperty newProperty = std::make_shared<Entity::Property>();
        newProperty->setTypeSearcher(m_Model->globalDatabase());

        // Add name
        newProperty->setName(tokens[int(PropGroupNames::Name)]->token());

        // Add type
        // Not support namespaces for now, so check only in global database.
        // Work with namespaces and custom types in project will be hard due to Qt meta-stuff.
        // And also required more detail work on current code generation functionality. TODO: implement!
        const Entity::SharedScope &globasScope = m_Model->globalDatabase()->scope(Common::ID::globalScopeID());
        if (!globasScope)
            return {tr("Cannot find global scope."), nullptr};

        const auto &typeName = tokens[int(PropGroupNames::Type)]->token();
        auto type = globasScope->type(typeName);
        if (!type)
            return {tr("Wrong type: %1.").arg(typeName), nullptr};
        newProperty->setTypeId(type->id());

        // Member (supports only "m_" prefix for now)
        const auto &member = tokens[int(PropGroupNames::Member)]->token();
        if (!member.isEmpty()) {
            const bool hasPrefix = member.startsWith("m_");
            const auto name(hasPrefix ? QString(member).remove(0, 2) : member);
            const auto prefix(hasPrefix ? "m_" : "");
            newProperty->addField(name).field()->setPrefix(prefix);
            newProperty->setMember(true);
        }

        // Common methods
        addCommon(tokens, PropGroupNames::Getter,   &Entity::Property::addGetter,   newProperty);
        addCommon(tokens, PropGroupNames::Setter,   &Entity::Property::addSetter,   newProperty);
        addCommon(tokens, PropGroupNames::Resetter, &Entity::Property::addResetter, newProperty);
        addCommon(tokens, PropGroupNames::Notifier, &Entity::Property::addNotifier, newProperty);

        // Revision
        Q_ASSERT(tokens[int(PropGroupNames::Revision)]->isSingle());
        const auto &revision = tokens[int(PropGroupNames::Revision)]->token();
        if (!revision.isEmpty()) {
            bool ok = false;
            int rev = revision.toInt(&ok);

            if (ok)
                newProperty->setRevision(rev);
            else
                return {tr("Wrong revision: %1.").arg(revision), nullptr};
        }

        // Add designable and scriptable
        addDS(tokens, PropGroupNames::Designable, &Entity::Property::addDesignableGetter,
              &Entity::Property::setDesignable, newProperty);
        addDS(tokens, PropGroupNames::Scriptable, &Entity::Property::addScriptableGetter,
              &Entity::Property::setScriptable, newProperty);

        // Add stroed, user, const and final options
        addExtra(tokens, PropGroupNames::Stored,   &Entity::Property::setStored,   newProperty);
        addExtra(tokens, PropGroupNames::User,     &Entity::Property::setUser,     newProperty);
        addExtra(tokens, PropGroupNames::Constant, &Entity::Property::setConstant, newProperty);
        addExtra(tokens, PropGroupNames::Final,    &Entity::Property::setFinal,    newProperty);

        return {"", newProperty};
    }

    /**
     * @brief ComponentsMaker::makeMethod
     * @param tokens
     * @return
     */
    OptionalEntity ComponentsMaker::makeMethod(const Tokens &tokens)
    {
        Q_ASSERT(!tokens.isEmpty() &&
                 tokens[int(MethodsGroupsNames::ReturnType)]->isMulti() &&
                 tokens[int(MethodsGroupsNames::Name)]->isSingle());

        Q_ASSERT(checkCommonState());

        Entity::SharedMethod newMethod = std::make_shared<Entity::ClassMethod>();

        // Add Lhs
        auto lhsToken = tokens[int(MethodsGroupsNames::LhsKeywords)];
        Q_ASSERT(!lhsToken->isEmpty() && lhsToken->isSingle());
        for (auto &&w : lhsToken->token().split(QChar::Space, QString::SkipEmptyParts)) {
            Entity::LhsIdentificator id = Util::methodLhsIdFromString(w);
            Q_ASSERT(id != Entity::LhsIdentificator::None);
            newMethod->addLhsIdentificator(id);
        }


        // Add return type
        auto returnTypeToken = tokens[int(MethodsGroupsNames::ReturnType)];
        Q_ASSERT(returnTypeToken->isMulti() && !returnTypeToken->isEmpty());
        auto returnType = makeType(returnTypeToken->tokens());

        if (!returnType.errorMessage.isEmpty())
            return {returnType.errorMessage, nullptr};

        newMethod->setReturnTypeId(returnType.resultEntity->id());

        // Add name
        newMethod->setName(tokens[int(MethodsGroupsNames::Name)]->token());

        // Add arguments
        auto argumentsToken = tokens[int(MethodsGroupsNames::Arguments)];
        Q_ASSERT(argumentsToken->isEmpty() || argumentsToken->isMulti());
        if (argumentsToken->isMulti()) {
            auto argumentsTokens = argumentsToken->tokens();
            for (auto &&argumentToken : argumentsTokens) {
                Q_ASSERT(argumentToken->isMulti());

                auto argSubTokens = argumentToken->tokens();
                Q_ASSERT(argSubTokens.size() == int(Argument::GroupsCount));

                Q_ASSERT(argSubTokens[int(Argument::Name)]->isSingle());
                auto name = argSubTokens[int(Argument::Name)]->token();

                Q_ASSERT(argSubTokens[int(Argument::Type)]->isMulti());
                auto type = makeType(argSubTokens[int(Argument::Type)]->tokens());
                if (!type.errorMessage.isEmpty())
                    return {tr("Wrong type of argument: %1. Error: %2.").arg(
                                QString::number(argumentsTokens.indexOf(argumentToken)),
                                type.errorMessage
                            ), nullptr};

                Q_ASSERT(type.resultEntity);
                newMethod->addParameter(name, type.resultEntity->id());
            }
        }

        // Const
        auto constArgument = tokens[int(MethodsGroupsNames::Const)];
        Q_ASSERT(!constArgument->isEmpty() && constArgument->isSingle());

        const QString &token = constArgument->token();
        if (!token.isEmpty()) {
            if (token == "const")
                newMethod->setConstStatus(true);
            else
                return {tr("Wrong const status token: %1.").arg(constArgument->token()), nullptr};
        }

        // Rhs
        auto rhsArgument = tokens[int(MethodsGroupsNames::RhsKeywords)];
        Q_ASSERT(!rhsArgument->isEmpty() && rhsArgument->isSingle());

        const QString &rhsToken = rhsArgument->token();
        if (!rhsToken.isEmpty()) {
            Entity::RhsIdentificator rhsId = Util::methodRhsIdFromString(rhsToken);
            if (rhsId != Entity::RhsIdentificator::None)
                newMethod->setRhsIdentificator(rhsId);
            else
                return {tr("Wrong rhs keyword: %1").arg(rhsToken), nullptr};
        }

        return {"", newMethod};
    }

} // namespace components
