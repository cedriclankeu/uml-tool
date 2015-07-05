/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 04/07/2015.
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

#include "TestEntities.h"
#include "constants.h"

#define test_copy_move(entType, obj) \
    auto newCopyType(std::make_unique<entity::entType>(*obj)); \
    ASSERT_EQ(*obj, *newCopyType); \
    \
    newCopyType.reset(); \
    newCopyType = std::make_unique<entity::entType>();\
    *newCopyType = *obj;\
    ASSERT_EQ(*obj, *newCopyType);\
    \
    entity::entType tmp(*obj); \
    entity::entType moveType(std::move(tmp)); \
    ASSERT_EQ(moveType, *obj); \
    \
    entity::entType tmpEq(*obj); \
    entity::entType moveTypeEq; \
    moveTypeEq = std::move(tmpEq); \
    ASSERT_EQ(moveTypeEq, *obj); \

TEST_F(Enteties, SimpleType)
{
    ASSERT_STREQ(_type->name().toStdString().c_str(), BASE_TYPE_NAME);
    ASSERT_STREQ(_type->scopeId().toStdString().c_str(), GLOBAL_SCOPE_ID);

    _type->setName("some name");
    _type->setId("some id");
    _type->setScopeId("global scope id");

    test_copy_move(Type, _type)
}

TEST_F(Enteties, ExtendedType)
{
    test_copy_move(ExtendedType, _extendedType)
}

TEST_F(Enteties, Class)
{
    test_copy_move(Class, _class)
}

TEST_F(Enteties, Union)
{
    test_copy_move(Union, _union)
}

TEST_F(Enteties, Enum)
{
    test_copy_move(Enum, _enum)
}

TEST_F(Enteties, TemplateClass)
{
    test_copy_move(TemplateClass, _templateClass)
}
