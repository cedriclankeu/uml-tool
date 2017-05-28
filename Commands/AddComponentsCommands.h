/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 11/06/2015.
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

#include <functional>

#include "BaseCommand.h"

#include <Models/ComponentsModel.h>
#include <Models/ModelsTypes.hpp>

namespace Commands {

    /// The AddComponentBaseCommand class
    template<class Component>
    class AddComponentBaseCommand : public BaseCommand
    {
    public:
        AddComponentBaseCommand(const QString &name, const Models::SharedClassComponentsModel &model,
                                const Component &component = nullptr, int pos = -1, QUndoCommand *parent = nullptr)
            : BaseCommand(name, parent)
            , m_Model(model)
            , m_Components(model ? model->components() : nullptr)
            , m_Component(component)
            , m_Pos(pos)
        {}

    protected:
        void redoImpl(const std::function<void()> &addExists, const std::function<Component()> &addNew)
        {
            if (m_Done) {
                Q_ASSERT(m_Component);

                Entity::SharedComponents tmp;

                if (m_Model->components() != m_Components)
                {
                    tmp = m_Model->components();
                    m_Model->setComponents(m_Components);
                }

                addExists();

                if (tmp)
                    m_Model->setComponents(tmp);
            } else {
                if (m_Component)
                    addExists();
                else
                    m_Component = addNew();
            }
        }

        Models::SharedClassComponentsModel m_Model;
        Entity::SharedComponents m_Components;
        Component m_Component;
        int m_Pos;
    };

    /// The AddMethod class
    class AddMethod : public AddComponentBaseCommand<Entity::SharedMethod>
    {
    public:
        explicit AddMethod(const Models::SharedClassComponentsModel &model, const Entity::SharedMethod &method = nullptr,
                           int pos = -1, QUndoCommand *parent = nullptr);

        void redo() override;
        void undo() override;
    };

    /// The AddField class
    class AddField : public AddComponentBaseCommand<Entity::SharedField>
    {
    public:
        explicit AddField(const Models::SharedClassComponentsModel &model, const Entity::SharedField &field = nullptr,
                          int pos = -1, QUndoCommand *parent = nullptr);

        void redo() override;
        void undo() override;
    };

    /// The AddElement class
    class AddElement : public AddComponentBaseCommand<Entity::SharedEnumarator>
    {
    public:
        explicit AddElement(const Models::SharedClassComponentsModel &model, const Entity::SharedEnumarator &element = nullptr,
                            int pos = -1, QUndoCommand *parent = nullptr);

        void redo() override;
        void undo() override;
    };

} // namespace Commands

