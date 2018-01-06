/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 07/06/2015.
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

#include <Entity/EntityTypes.hpp>

namespace GUI {

    namespace Ui {
        class EditMethodDialog;
    }

    /// The EditMethodDialog class
    class EditMethodDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit EditMethodDialog(QWidget *parent = 0);
        ~EditMethodDialog();

        Entity::SharedMethod currentMethod() const;
        void setCurrentMethod(const Entity::SharedMethod &currentMethod);

    protected:
        void closeEvent(QCloseEvent *ev) override;

    private:
        void clear();

        QScopedPointer<Ui::EditMethodDialog> ui;
        Entity::SharedMethod m_CurrentMethod;
    };


} // namespace gui
