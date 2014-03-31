#include "classmethod.h"
#include "extendedtype.h"
#include "field.h"
#include "enums.h"

namespace entity {

    ClassMethod::ClassMethod()
        : ClassMethod("noname")
    {
    }

    ClassMethod::ClassMethod(const QString &name)
        : m_Name(name)
        , m_Section(Public)
        , m_ConstStatus(false)
        , m_ReturnType(nullptr)
        , m_RhsIdentificator(None)
    {
    }

    QString ClassMethod::name() const
    {
        return m_Name;
    }

    void ClassMethod::setName(const QString &name)
    {
        m_Name = name;
    }

    Section ClassMethod::section() const
    {
        return m_Section;
    }

    void ClassMethod::setSection(Section &section)
    {
        m_Section = section;
    }

    ExtendedType *ClassMethod::returnType() const
    {
        return m_ReturnType;
    }

    void ClassMethod::setReturnType(ExtendedType *returnType)
    {
        m_ReturnType = returnType;
    }

    bool ClassMethod::isConst() const
    {
        return m_ConstStatus;
    }

    void ClassMethod::setConstStatus(bool newStatus)
    {
        m_ConstStatus = newStatus;
    }

    RhsIdentificator ClassMethod::rhsIdentificator() const
    {
        return m_RhsIdentificator;
    }

    void ClassMethod::setRhsIdentificator(RhsIdentificator identificator)
    {
        m_RhsIdentificator = identificator;
    }

    LhsIdentificatorsList ClassMethod::lhsIdentificators() const
    {
        return m_LhsIdentificators.values();
    }

    void ClassMethod::addLhsIdentificator(LhsIdentificator identificator)
    {
        m_LhsIdentificators << identificator;
    }

    bool ClassMethod::containsLhsIdentficator(LhsIdentificator identificator) const
    {
        return m_LhsIdentificators.contains(identificator);
    }

    bool ClassMethod::hasLhsIdentificators() const
    {
        return m_LhsIdentificators.empty();
    }

    void ClassMethod::removeLhsIdentificator(LhsIdentificator identificator)
    {
        m_LhsIdentificators.remove(identificator);
    }

    SharedField ClassMethod::getParameter(const QString &name)
    {
        SharedField result = nullptr;

        auto it = m_Parameters.find(name);
        if ( it != m_Parameters.end()) result = *it;

        return result;
    }

    SharedField ClassMethod::addParameter(const QString &name, ExtendedType *type)
    {
        return *m_Parameters.insert(name, std::make_shared<Field>(name, type));
    }

    bool ClassMethod::containsParameter(const QString &name)
    {
        return m_Parameters.contains(name);
    }

    bool ClassMethod::hasParameters() const
    {
        return m_Parameters.empty();
    }

    void ClassMethod::removeParameter(const QString &name)
    {
        m_Parameters.remove(name);
    }

    FieldsList ClassMethod::parameters() const
    {
        return m_Parameters.values();
    }

} // namespace entity
