/*****************************************************************************
** 
** Copyright (C) 2014 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 03/11/2014.
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

#include <QDir>

#include <gtest/gtest.h>

#include <types.h>
#include <project/project.h>

class Project : public ::testing::Test
{
protected:
    virtual void SetUp() override
    {
        rootPath_.append(sep_).append("tmp");
        QDir().mkpath(rootPath_);

        project_ = std::make_shared<project::Project>("TstProject", rootPath_, errors);
    }

    virtual void TearDown() override
    {
        EXPECT_TRUE(QDir(rootPath_).removeRecursively());
    }

    QString rootPath_ = QDir::current().path();
    QChar   sep_      = QDir(rootPath_).separator();
    SharedErrorList errors = std::make_shared<QStringList>();
    project::SharedProject project_;
};