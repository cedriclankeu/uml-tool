/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 09/03/2015.
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

#include <memory>

#include <QGraphicsObject>

#include "types.h"

class QMenu;

namespace gui
{
    class EditEntityDialog;
}

/// namespace graphics
namespace graphics {

    /// The Entity class
    class Entity : public QGraphicsObject
    {
        Q_OBJECT
    public:
        Entity(const entity::SharedType & type, QGraphicsItem *parent = nullptr);
        ~Entity();

        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr);

        entity::SharedType typeObject() const;
        void setTypeObject(const entity::SharedType &type);

    private:
        bool event(QEvent *ev) override;
        void showMenu(const QPoint &pos);
        void addMenuActions();

        // NOTE: widget and menu for each graphic object may be costly
        std::unique_ptr<QMenu> m_Menu;
        std::unique_ptr<gui::EditEntityDialog> m_EditDialog;

        entity::SharedType m_Type;
    };

} // grpahics