/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 20/06/2015.
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

#include <QDialog>

namespace GUI {

    namespace Ui {
        class ChooseGlobalDatabaseDialog;
    }

    /// The ChooseGlobalDatabaseDialog class
    class ChooseGlobalDatabaseDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit ChooseGlobalDatabaseDialog(const QString &name = "", const QString &path = "", QWidget *parent = 0);
        ~ChooseGlobalDatabaseDialog();

        void setPath(const QString &path);
        QString path() const;

        QString name() const;
        void setName(const QString &name);

    private:
        void showEvent(QShowEvent *ev) override;

        QScopedPointer<Ui::ChooseGlobalDatabaseDialog> ui;
        QString m_Path;
        QString m_Name;
    };

} // namespace gui
