#include "association.h"
#include "class.h"
#include "classmethod.h"
#include "enums.h"
#include "types.h"
#include "field.h"
#include "helpfunctions.h"
#include "constants.cpp"

#include <QJsonObject>

namespace relationship {

    Association::Association()
        : Association(STUB_ID, STUB_ID, nullptr, nullptr)
    {
    }

    Association::Association(const QString &tailTypeId, const QString &headTypeId, db::Database *globalDatabase, db::Database *projectDatabase)
        : Relation(tailTypeId, headTypeId, globalDatabase, projectDatabase)
        , m_GetSetTypeId(headTypeId)
    {
        m_RelationType = AssociationRelation;
    }

    bool operator ==(const Association &lhs, const Association &rhs)
    {
        return static_cast<const Relation&>(lhs).isEqual(rhs) &&
               lhs.m_GetSetTypeId == rhs.m_GetSetTypeId &&
               lhs.m_FieldTypeId  == rhs.m_FieldTypeId;
    }

    void Association::make()
    {
        makeField();
        makeGetter();
        makeSetter();
    }

    void Association::clear()
    {
        removeField();
        removeGetter();
        removeSetter();
    }

    void Association::makeGetter()
    {
        QString getterName(m_HeadClass->name().toLower());

        auto getter = m_TailClass->makeMethod(getterName);
        getter->setReturnTypeId(m_GetSetTypeId);
        getter->setConstStatus(true);
    }

    void Association::makeSetter()
    {
        QString setterName(QString("set%1").arg(m_HeadClass->name()));

        auto setter = m_TailClass->makeMethod(setterName);
        auto param = setter->addParameter(QString("src_%1").arg(m_HeadClass->name().toLower()), m_GetSetTypeId);
        param->setPrefix("");
    }

    void Association::removeGetter()
    {
        m_TailClass->removeMethods(m_HeadClass->name().toLower());
    }

    void Association::removeSetter()
    {
        m_TailClass->removeMethods(QString("set%1").arg(m_HeadClass->name()));
    }

    QString Association::fieldtypeId() const
    {
        return m_FieldTypeId;
    }

    void Association::setFieldtypeId(const QString &fieldtypeId)
    {
        m_FieldTypeId = fieldtypeId;
    }

    QJsonObject Association::toJson() const
    {
        auto result = Relation::toJson();

        result.insert("Get and set type ID", m_GetSetTypeId);
        result.insert("Field type ID", m_FieldTypeId);

        return result;
    }

    void Association::fromJson(const QJsonObject &src, QStringList &errorList)
    {
        Relation::fromJson(src, errorList);

        utility::checkAndSet(src, "Get and set type ID", errorList, [&src, this](){
            m_GetSetTypeId = src["Get and set type ID"].toString();
        });
        utility::checkAndSet(src, "Field type ID", errorList, [&src, this](){
            m_FieldTypeId = src["Field type ID"].toString();
        });
    }

    bool Association::isEqual(const Association &rhs) const
    {
        return *this == rhs;
    }

    void Association::makeField()
    {
        m_TailClass->addField(m_HeadClass->name(), m_FieldTypeId);
    }

    void Association::removeField()
    {
        m_TailClass->removeField(m_HeadClass->name());
    }

    QString Association::getGetSetTypeId() const
    {
        return m_GetSetTypeId;
    }

    void Association::setGetSetTypeId(const QString &getSetTypeId)
    {
        m_GetSetTypeId = getSetTypeId;
    }


} // namespace relationship
