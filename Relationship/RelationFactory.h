/*****************************************************************************
**
** Copyright (C) 2016 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 24/03/2016.
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

#include <Relationship/relationship_types.hpp>
#include <Entity/EntityTypes.hpp>

#include <Common/ElementsFactory.h>

#include "enums.h"
#include "QtHelpers.h"

class QGraphicsScene;

namespace Relationship {

    /// Relation maker
    class RelationFactory : public Common::ElementsFactory
    {
        Q_OBJECT

    public:
        NEITHER_COPIABLE_NOR_MOVABLE(RelationFactory)

        static const RelationFactory &instance();

        SharedRelation make(RelationType relType, const Common::ID &tail,
                            const Common::ID &head,CreationOptions options = RelationCommon) const;
        SharedRelation make(const QJsonObject &src, ErrorList &errors,
                            CreationOptions options = RelationCommon) const;
    private:
        explicit RelationFactory(QObject * parent = nullptr);
    };

} // namespace relationship
