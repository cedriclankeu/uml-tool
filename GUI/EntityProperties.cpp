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
#include "EntityProperties.h"
#include "ui_EntityProperties.h"

#include <Models/ApplicationModel.h>

#include <Entity/Type.h>
#include <Entity/Enum.h>

#include "PropertiesHandlerBase.hpp"

#include "QtHelpers.h"

#include <QDebug>

namespace GUI {

    EntityProperties::EntityProperties(QWidget *parent)
        : QWidget(parent)
        , m_ui(new Ui::EntityProperties)
    {
        m_ui->setupUi(this);

        // Invoke only when ui is setup
        m_handler = std::make_shared<PropertiesHandlerBase>(*m_ui->vl);
        G_CONNECT(static_cast<PropertiesHandlerBase*>(m_handler.get()),
                  &PropertiesHandlerBase::sceneUpdateRequired,
                  this,
                  &EntityProperties::sceneUpdateRequired);
    }

    EntityProperties::~EntityProperties() = default;

    void EntityProperties::onSelectedElementsChanged(const Entity::TypesList &types)
    {
        m_handler->setEntity(types.size() == 1 ? types.first() : nullptr);
    }

} // namespace GUI
