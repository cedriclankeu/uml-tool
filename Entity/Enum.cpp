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

#include "Enum.h"

#include <algorithm>
#include <utility>

#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>

#include <range/v3/algorithm/find_if.hpp>

#include <Utility/helpfunctions.h>

#include "Constants.h"
#include "enums.h"

namespace Entity {

    static const QString newElementName = Enumerator::tr("newElement");
    static const QString nameMark       = "Name";
    static const QString numberMark     = "Number";
    static const QString defaultName    = Enum::tr("Enumeration");
    static const QString nulloptValue   = QString::null;

    /**
     * @brief Element::Element
     */
    Enumerator::Enumerator()
        : Enumerator(defaultName)
    {}

    /**
     * @brief Element::Element
     * @param name
     * @param value
     */
    Enumerator::Enumerator(const QString &name, OptionalValue value)
        : BasicElement(name)
        , m_Value(value)
    {}

    /**
     * @brief Variable::toJson
     * @return
     */
    Enumerator::Enumerator(const QJsonObject &src, QStringList &errorList)
        : BasicElement(src, errorList)
        , m_Value({0, Dec})
    {
        fromJson(src, errorList);
    }

    /**
     * @brief operator ==
     * @param lhs
     * @param rhs
     * @return
     */
    bool operator ==(const Enumerator &lhs, const Enumerator &rhs)
    {
        return static_cast<Common::BasicElement const&>(lhs) ==
               static_cast<Common::BasicElement const&>(rhs) &&
               lhs.m_Value == rhs.m_Value;
    }

    /**
     * @brief Element::toJson
     * @return
     */
    QJsonObject Enumerator::toJson() const
    {
        QJsonObject result = BasicElement::toJson();
        result.insert(numberMark, m_Value ? valToString(m_Value.value()) : nulloptValue);

        return result;
    }

    /**
     * @brief Variable::fromJson
     * @param src
     * @param errorList
     */
    void Enumerator::fromJson(const QJsonObject &src, QStringList &errorList)
    {
        BasicElement::fromJson(src, errorList);
        Util::checkAndSet(src, numberMark, errorList, [&src, this](){
            auto val = src[numberMark].toString(QString::null);
            if (val == nulloptValue)
                m_Value.reset();
            else
                m_Value = valFromString(val);
        });
    }

    /**
     * @brief Enumerator::value
     * @return
     */
    Enumerator::OptionalValue Enumerator::value() const
    {
        return m_Value;
    }

    /**
     * @brief Enumerator::setValue
     * @param value
     */
    void Enumerator::setValue(const OptionalValue &value)
    {
        m_Value = value;
    }

    /**
     * @brief Enumerator::setValue
     * @param value
     */
    void Enumerator::setValue(int value)
    {
        setValue(std::make_pair(value, Dec)) ;
    }

    /**
     * @brief Enumerator::enumeratorValToString
     * @param v
     * @return
     */
    QString Enumerator::valToString(Enumerator::Value const& v)
    {
        QString prefix;
        switch (v.second) {
            case Oct:
                prefix = "0";
                break;

            case Hex:
                prefix = "0x";
                break;

            default: ;
        }

        return prefix + QString::number(v.first, v.second);
    }

    Enumerator::Value Enumerator::valFromString(QString s)
    {
        ValueBase base = Dec;
        if (s.startsWith("0")) {
            s.remove(0, 1);
            base = Oct;
        } else if (s.startsWith("0x")) {
            s.remove(0, 2);
            base = Hex;
        }

        return {s.toInt(nullptr, base), base};
    }

    /**
     * @brief Enum::Enum
     */
    Enum::Enum()
        : Enum(defaultName, Common::ID::globalScopeID())
    {
    }

    /**
     * @brief Enum::Enum
     * @param name
     * @param scopeId
     */
    Enum::Enum(const QString &name, const Common::ID &scopeId)
        : Type(name.isEmpty() ? defaultName : name, scopeId)
        , m_EnumTypeId(Common::ID::nullID())
        , m_StrongStatus(false)
    {
        m_KindOfType = KindOfType::Enum;
    }

    /**
     * @brief Enum::Enum
     * @param src
     */
    Enum::Enum(Enum &&src) noexcept
        : Type(std::move(src))
        , m_EnumTypeId(std::move(src.m_EnumTypeId))
        , m_StrongStatus(std::move(src.m_StrongStatus))
        , m_Elements(std::move(src.m_Elements))
    {
    }

    /**
     * @brief Enum::Enum
     * @param src
     */
    Enum::Enum(const Enum &src)
        : Type(src)
        , m_EnumTypeId(src.m_EnumTypeId)
        , m_StrongStatus(src.m_StrongStatus)
        , m_Elements(src.m_Elements)
    {
    }

    /**
     * @brief Enum::operator =
     * @param src
     * @return
     */
    Enum &Enum::operator=(Enum &&src) noexcept
    {
        static_cast<Type&>(*this) = static_cast<Type&&>(src);
        m_EnumTypeId = std::move(src.m_EnumTypeId);
        m_StrongStatus = std::move(src.m_StrongStatus);
        m_Elements = std::move(src.m_Elements);

        return *this;
    }

    /**
     * @brief Enum::operator =
     * @param src
     * @return
     */
    Enum &Enum::operator=(const Enum &src)
    {
        Enum tmp(src);
        *this = std::move(tmp);

        return *this;
    }

    /**
     * @brief operator ==
     * @param lhs
     * @param rhs
     * @return
     */
    bool operator==(const Enum &lhs, const Enum &rhs)
    {
        return lhs.isEqual(rhs);
    }

    /**
     * @brief Enum::isStrong
     * @return
     */
    bool Enum::isStrong() const
    {
        return m_StrongStatus;
    }

    /**
     * @brief Enum::setStrongStatus
     * @param status
     */
    void Enum::setStrongStatus(bool status)
    {
        m_StrongStatus = status;
    }

    /**
     * @brief Enum::addVariable
     * @param name
     * @return
     */
    SharedEnumarator Enum::addElement(const QString &name)
    {
        auto element = std::make_shared<Enumerator>(name);
        m_Elements << element;
        return element;
    }

    /**
     * @brief Enum::getVariable
     * @param name
     * @return
     */
    SharedEnumarator Enum::element(const QString &name) const
    {
        auto it = ranges::find_if(m_Elements, [&](auto &&e){ return e->name() == name; });
        return it != cend(m_Elements) ? *it : SharedEnumarator();
    }

    /**
     * @brief Enum::removeVariable
     * @param name
     */
    void Enum::removeEnumerator(const QString &name)
    {
        auto it = ranges::find_if(m_Elements, [&](auto &&v){ return v->name() == name; });
        if (it != m_Elements.end())
            m_Elements.erase(it);
    }

    /**
     * @brief Enum::containsVariable
     * @param name
     * @return
     */
    bool Enum::containsElement(const QString &name) const
    {
        return ranges::find_if(m_Elements, [&](auto &&v){ return v->name() == name; })
               != cend(m_Elements);
    }

    /**
     * @brief Enum::variables
     * @return
     */
    Enumerators Enum::enumerators() const
    {
        return m_Elements;
    }

    /**
     * @brief Enum::enumTypeId
     * @return
     */
    Common::ID Enum::enumTypeId() const
    {
        return m_EnumTypeId;
    }

    /**
     * @brief Enum::setEnumTypeId
     * @param enumTypeId
     */
    void Enum::setEnumTypeId(const Common::ID &enumTypeId)
    {
        m_EnumTypeId = enumTypeId;
    }

    /**
     * @brief Enum::toJson
     * @return
     */
    QJsonObject Enum::toJson() const
    {
        QJsonObject result(Type::toJson());

        result.insert("Enum type id", m_EnumTypeId.toJson());
        result.insert("Strong status", m_StrongStatus);

        QJsonArray elements;
        for (auto &&v : m_Elements)
            elements.append(v->toJson());

        result.insert("Elements", elements);

        return result;
    }

    /**
     * @brief Enum::fromJson
     * @param src
     * @param errorList
     */
    void Enum::fromJson(const QJsonObject &src, QStringList &errorList)
    {
        Type::fromJson(src, errorList);

        Util::checkAndSet(src, "Enum type id",  errorList,
                          [&, this](){ m_EnumTypeId.fromJson(src["Enum type id"], errorList); });
        Util::checkAndSet(src, "Strong status", errorList,
                          [&src, this](){ m_StrongStatus = src["Strong status"].toBool();  });

        m_Elements.clear();
        Util::checkAndSet(src, "Elements", errorList, [&src, &errorList, this](){
            if (src["Elements"].isArray()) {
                for (auto value : src["Elements"].toArray())
                    m_Elements.append(std::make_shared<Enumerator>(value.toObject(), errorList));
            } else {
                errorList << "Error: \"Elements\" is not array";
            }
        });
    }

    /**
     * @brief Enum::isEqual
     * @param rhs
     * @return
     */
    bool Enum::isEqual(const Type &rhs, bool withTypeid) const
    {
        if (!Type::isEqual(rhs, withTypeid))
            return false;

        auto r = static_cast<const Enum&>(rhs);
        return m_EnumTypeId   == r.m_EnumTypeId   &&
               m_StrongStatus == r.m_StrongStatus &&
               Util::seqSharedPointerEq(m_Elements, r.m_Elements);
    }

    void swap(Enum &lhs, Enum &rhs) noexcept
    {
        using std::swap;

        swap(static_cast<Type&>(lhs), static_cast<Type&>(rhs));
        swap(lhs.m_Elements, rhs.m_Elements);
        swap(lhs.m_EnumTypeId, rhs.m_EnumTypeId);
        swap(lhs.m_StrongStatus, rhs.m_StrongStatus);
    }

    /**
     * @brief Enum::addNewElement
     * @return
     */
    SharedEnumarator Enum::addNewEnumerator()
    {
        return addElement(newElementName + QString::number(m_Elements.count()));
    }

    /**
     * @brief Enum::addExistsElement
     * @param element
     * @param pos
     */
    void Enum::addExistsEnumerator(const SharedEnumarator &element, int pos)
    {
        m_Elements.insert(pos > 0 && pos < m_Elements.size() ? pos : m_Elements.size(), element);
    }

    /**
     * @brief Enum::removeElement
     * @param element
     * @return
     */
    int Enum::removeEnumerator(const SharedEnumarator &element)
    {
        int pos = m_Elements.indexOf(element);
        m_Elements.removeAt(pos);
        return pos;
    }

    OptionalDisplayData Entity::Enum::displayData() const
    {
        return DisplayData{
            tr("Enum declaration", "Enum declaration"),
            tr("Enum declaration",
               "Enter enum declaration in the following format:<br>"
               "enum-key<sub>opt</sub> identifier type<sub>opt</sub><br>"
               "enumerator constexpr<br>"
               "enumerator constexpr<br>"
               "...<br>"
               "enumerator constexpr<br><br>"
               "For example:<br>"
               "class Foo int<br>"
               "a 1<br>"
               "b 2")};
    }

} // namespace entity
