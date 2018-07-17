/*****************************************************************************
**
** Copyright (C) 2017 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 17.
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

#include <QWidget>
#include <QPointer>

#include <Entity/EntityTypes.hpp>
#include <Entity/Type.h>

#include "GuiTypes.hpp"

namespace GUI {

    class Section;
    class IPropertiesHandler;

    namespace Ui {
        class EntityProperties;
    }

    /**
     * @brief The EntityProperties class
     */
    class EntityProperties : public QWidget
    {
        Q_OBJECT

    public:
        explicit EntityProperties(QWidget *parent = nullptr);
        ~EntityProperties();

    public slots:
        void onSelectedElementsChanged(const Entity::TypesList &types);

    private: // Types
        using HandlersByType = QHash<Entity::KindOfType, SharedPropHandler>;

    private: // Data
        QScopedPointer<Ui::EntityProperties> m_ui;

        HandlersByType m_PropHandlersByType;
        SharedPropHandler m_ActiveHandler;

//        SectionsList m_AllSections;
    };

} // namespace GUI