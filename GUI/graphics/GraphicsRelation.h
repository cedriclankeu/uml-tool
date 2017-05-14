/*****************************************************************************
**
** Copyright (C) 2016 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 10/01/2016.
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

#include <QGraphicsLineItem>

#include <Relationship/relationship_types.hpp>

#include "GraphicsTypes.h"

namespace Graphics {

    class GraphisEntity;

    /// Graphics represenation of relation (from P1 to P2)
    class Relation : public QObject, public QGraphicsLineItem
    {
        Q_OBJECT

    public:
        Relation(const Relationship::SharedRelation &relation, const EntityPtr &from,
                 const EntityPtr &to, QGraphicsItem *parent = nullptr);


        void setP1(const QPointF &p);
        void setP2(const QPointF &p);

    public: // QGraphicsLineItem overrides
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
        QRectF boundingRect() const override;

        EntityPtr from() const;
        void setFrom(const EntityPtr &from);

        EntityPtr to() const;
        void setTo(const EntityPtr &to);

        Common::ID id() const;

    private slots:
        void recalculateLine();

    private:
        void initEntity(const EntityPtr &e);
        void setEntity(EntityPtr &e, const EntityPtr &newEntity);

        void drawLine(QPainter * p, const QLineF &l) const;
//        void drawArrow(QPainter * p, const QLineF &l, const QPointF &startPoint) const;

        QPointF fromPoint() const;
        void setFromPoint(const QPointF &fromPoint);

        QPointF toPoint() const;
        void setToPoint(const QPointF &toPoint);

        Relationship::SharedRelation m_Relation;
        EntityPtr m_From;
        EntityPtr m_To;
    };

} // namespace graphics
