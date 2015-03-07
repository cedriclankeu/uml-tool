/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 09/02/2015.
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
#include "basicentity.h"

#include <utility/helpfunctions.h>

namespace entity {

    namespace {
        const QString stubID = "no_id_for_this_elemnt";
    }

    /**
     * @brief BasicEntity::id
     * @return
     */
    QString BasicEntity::id() const
    {
        return stubID;
    }

    /**
     * @brief BasicEntity::parentID
     * @return
     */
    QString BasicEntity::parentID() const
    {
        return topID;
    }

    /**
     * @brief BasicEntity::setId
     * @param id
     */
    void BasicEntity::setId(const QString &id)
    {
        Q_UNUSED(id);
    }

    /**
     * @brief BasicEntity::writeToFile
     * @param fileName
     */
    void BasicEntity::writeToFile(const QString &fileName) const
    {
        utility::writeToFile(*this, fileName);
    }

    /**
     * @brief BasicEntity::readFromFile
     * @param fileName
     * @return
     */
    bool BasicEntity::readFromFile(const QString &fileName)
    {
        return utility::readFromFile(*this, fileName);
    }

}