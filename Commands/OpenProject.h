/*****************************************************************************
**
** Copyright (C) 2017 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 23.
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

#include <Models/ModelsTypes.hpp>
#include <GUI/graphics/GraphicsTypes.h>
#include <Project/ProjectTypes.hpp>

#include "BaseCommand.h"
#include "CommandsTypes.h"

QT_BEGIN_NAMESPACE
class QMainWindow;
class QMenu;
QT_END_NAMESPACE

namespace Commands
{

    /**
     * @brief The OpenProject class
     */
    class OpenProject : public BaseCommand
    {
        Q_OBJECT

    public:
        OpenProject(const QString &name, const QString &path,const Models::SharedApplicationModel &appModel,
                    const Graphics::ScenePtr &scene, QUndoCommand *parent = nullptr);

    public: // Types
        using ProjectAdder  = std::function<void(const QString& /*Project name*/)>;
        using MenuRebuilder = std::function<void(QMenu& /*Recent projects menu*/)>;

    public: // QUndoCommand overrides
        void undoImpl()    override;
        void redoImpl()    override;

    signals:
        void recentProjectAdded(const QString &);
        void projectErrors(const QString &);

    private:
        QString                        m_ProjectPath;
        Models::SharedApplicationModel m_AppModel;
        Graphics::ScenePtr             m_Scene;

        Projects::SharedProject m_Project;
    };

} // Commands
