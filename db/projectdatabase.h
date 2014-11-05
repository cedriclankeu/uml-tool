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

#pragma once

#include "types.h"
#include "database.h"

namespace db {

    /**
     * @brief The ProjectDatabase class
     */
    class ProjectDatabase : public Database
    {
    public:
        ProjectDatabase(ProjectDatabase &&src);
        ProjectDatabase(const ProjectDatabase &src);
        ProjectDatabase(const QString &name = "", const QString &path = "");

        friend bool operator ==(const ProjectDatabase &lhs, const ProjectDatabase &rhs);

        ProjectDatabase &operator =(ProjectDatabase &&rhs);
        ProjectDatabase &operator =(ProjectDatabase rhs);

        relationship::SharedRelation getRelation(const QString &id) const;
        void addRelation(const relationship::SharedRelation &relation);
        bool containsRelation(const QString &id) const;
        void removeRelation(const QString &id);
        relationship::RelationsList relations() const;

        db::SharedDatabase globalDatabase() const;
        void setGlobalDatabase(const db::SharedDatabase &globalDatabase);

        void clear() override;

        QJsonObject toJson() const override;
        void fromJson(const QJsonObject &src, QStringList &errorList) override;

        bool isEqual(const ProjectDatabase &rhs) const;

    protected:
        virtual void copyFrom(const ProjectDatabase &src);
        virtual void moveFrom(ProjectDatabase &src);

    private:
        relationship::Relations m_Relations;
        db::SharedDatabase m_GlobalDatabase;
    };

} // namespace db
