/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 28/03/2015.
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

#include <QCoreApplication>
#include <QUndoCommand>

namespace Commands {

    /// The Base Command class
    class BaseCommand : public QObject, public QUndoCommand
    {
        Q_DECLARE_TR_FUNCTIONS(BaseCommand)

    public:
        explicit BaseCommand(QUndoCommand *parent = nullptr);
        explicit BaseCommand(const QString &text, QUndoCommand *parent = nullptr);

        ~BaseCommand() override;

    protected:
        /// Perform some cleanups in destructor
        virtual void cleanup() {}
        /// Perform checking objects state
        virtual void sanityCheck() {}

        bool m_CleaningRequired = false;
        bool m_Done = false; // do first time -- false, redo -- true
    };

} // namespace Commands
