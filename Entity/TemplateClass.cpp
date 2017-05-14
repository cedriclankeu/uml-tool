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

#include "TemplateClass.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

#include <Utility/helpfunctions.h>

#include <Common/ID.h>

#include "Constants.h"
#include "enums.h"

namespace {
    const QString defaultName = Entity::Template::tr("Template class");
}

namespace Entity {

    /**
     * @brief TemplateClass::TemplateClass
     */
    TemplateClass::TemplateClass()
        : TemplateClass(defaultName, Common::ID::globalScopeID())
    {
    }

    /**
     * @brief TemplateClass::TemplateClass
     * @param name
     * @param scopeId
     */
    TemplateClass::TemplateClass(const QString &name, const Common::ID &scopeId)
        : Class (name.isEmpty() ? defaultName : name, scopeId)
    {
        m_KindOfType = KindOfType::TemplateClass;
    }

    /**
     * @brief operator ==
     * @param lhs
     * @param rhs
     * @return
     */
    bool operator ==(const TemplateClass &lhs, const TemplateClass &rhs)
    {
        return lhs.isEqual(rhs);
    }

    /**
     * @brief TemplateClass::toJson
     * @return
     */
    QJsonObject TemplateClass::toJson() const
    {
        QJsonObject result(Class::toJson());
        result.insert("Template part", Template::templateToJson());

        return result;
    }

    /**
     * @brief TemplateClass::fromJson
     * @param src
     * @param errorList
     */
    void TemplateClass::fromJson(const QJsonObject &src, QStringList &errorList)
    {
        Class::fromJson(src, errorList);

        Util::checkAndSet(src, "Template part", errorList, [&src, &errorList, this](){
            Template::templateLoadFromJson(src["Template part"].toObject(), errorList);
        });
    }

    /**
     * @brief TemplateClass::isEqual
     * @param rhs
     * @return
     */
    bool TemplateClass::isEqual(const Type &rhs, bool withTypeid) const
    {
        return Class::isEqual(rhs, withTypeid) &&
               templatePartEq(dynamic_cast<const TemplateClass &>(rhs));
    }

} // namespace entity
