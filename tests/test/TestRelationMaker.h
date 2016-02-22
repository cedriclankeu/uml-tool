/*****************************************************************************
**
** Copyright (C) 2014 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 29/10/2014.
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

#include <gtest/gtest.h>

#include <db/database.h>
#include <db/projectdatabase.h>
#include <entity/scope.h>
#include <entity/type.h>
#include <entity/class.h>
#include <entity/classmethod.h>
#include <entity/field.h>
#include <relationship/node.h>
#include <relationship/relation.h>
#include <relationship/generalization.h>
#include <relationship/dependency.h>
#include <relationship/association.h>
#include <relationship/multiplyassociation.h>
#include <relationship/realization.h>

#include <types.h>

#include <constants.h>

#include "TestProjectBase.h"

class RelationMaker : public ProjectBase
{
protected:
    virtual void SetUp() override
    {
        m_FirstProjectScope  = m_ProjectDb->addScope("First scope");
        m_SecondProjectScope = m_ProjectDb->addScope("Second scope");

        m_FirstClass  = m_FirstProjectScope->addType<entity::Class>("First");
        m_SecondClass = m_SecondProjectScope->addType<entity::Class>("Second");
    }

    entity::SharedScope m_FirstProjectScope;
    entity::SharedScope m_SecondProjectScope;
    entity::SharedClass m_FirstClass;
    entity::SharedClass m_SecondClass;
};
