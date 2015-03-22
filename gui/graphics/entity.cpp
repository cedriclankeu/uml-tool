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
#include "entity.h"

#include <QPainter>
#include <QEvent>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsScene>

#include <gui/editentitydialog.h>

#include <entity/type.h>

namespace graphics {

    namespace {
        constexpr double margin    = 2.  ;
        constexpr double tmpHeight = 100.;
        constexpr double tmpWidth  = 100.;
    }

    /**
     * @brief Entity::Entity
     * @param parent
     */
    Entity::Entity(const entity::SharedType & type, QGraphicsItem *parent)
        : QGraphicsObject(parent)
        , m_Menu(std::make_unique<QMenu>())
        , m_EditDialog(std::make_unique<gui::EditEntityDialog>())
        , m_Type(type)
    {
        setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
        addMenuActions();
    }

    /**
     * @brief Entity::~Entity
     */
    Entity::~Entity()
    {

    }

    /**
     * @brief Entity::boundingRect
     * @return
     */
    QRectF Entity::boundingRect() const
    {
        return QRectF(-tmpWidth / 2 - margin, -tmpHeight / 2 - margin,
                      tmpWidth + margin, tmpHeight + margin);
    }

    /**
     * @brief Entity::paint
     * @param painter
     * @param option
     * @param widget
     */
    void Entity::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setBrush(Qt::black);
        painter->drawRect(QRectF(-tmpWidth / 2, -tmpHeight / 2, tmpWidth, tmpHeight));
        painter->setPen(Qt::white);
        painter->drawText(boundingRect(), Qt::AlignCenter, m_Type ? m_Type->name() : "Stub");
    }

    /**
     * @brief Entity::typeObject
     * @return
     */
    entity::SharedType Entity::typeObject() const
    {
        return m_Type;
    }

    /**
     * @brief Entity::setTypeObject
     * @param type
     */
    void Entity::setTypeObject(const entity::SharedType &type)
    {
        m_Type = type;
    }

    /**
     * @brief Entity::event
     * @param ev
     * @return
     */
    bool Entity::event(QEvent *ev)
    {
        switch (ev->type()) {

            case QEvent:: GraphicsSceneContextMenu:
            {
                QGraphicsSceneContextMenuEvent *menuEvent =
                    static_cast<QGraphicsSceneContextMenuEvent *>(ev);
                showMenu(menuEvent->screenPos());

                return true;
            }

            default: ;
        }

        return QObject::event(ev);
    }

    /**
     * @brief Entity::showMenu
     * @param pos
     */
    void Entity::showMenu(const QPoint &pos)
    {
        m_Menu->exec(pos);
    }

    void Entity::addMenuActions()
    {
        auto actionEdit = m_Menu->addAction(tr("Edit"));
        connect(actionEdit, &QAction::triggered,
                [this]() {
                    // We know that main window is the parent of scene
                    Q_ASSERT(scene());
                    m_EditDialog->setParent(static_cast<QWidget *>(scene()->parent()), Qt::Dialog);
                    m_EditDialog->exec();
                });

        m_Menu->addSeparator();
        m_Menu->addAction(tr("Delete"));
    }

} // grpahics