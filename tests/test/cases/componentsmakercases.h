/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 13/07/2015.
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

#include "ComponentsMaker.h"

#include <entity/field.h>

namespace {
    const QVector<QPair<QString, bool>> fieldData =
    {
        {"int ",                                          false},
        {"int a",                                         true },
        {"const int",                                     false},
        {"foo::bar::int a",                               true },
        {"const foo::bar::int a",                         true },
        {"constfoo::bar::int a",                          true },
        {"scons foo::bar::int a",                         false},
        {"constt foo::bar::int a",                        false},
        {"static const foo::bar::int a",                  true },
        {"staticconst foo::bar::int a",                   false},
        {"static const std::foo::baz::int * const **& a", true },
        {"int *consta",                                   true },
    };

    auto to_f(const entity::BasicEntity* e){ return static_cast<const entity::Field*>(e); }
}

TEST_F(ComponentsMaker, CheckSignature)
{
    // Test for field
    for (auto &&testData: fieldData)
        ASSERT_EQ(m_Maker->signatureValid(testData.first, models::DisplayPart::Fields), testData.second)
            << "Signature: " << testData.first.toStdString().c_str();
}

TEST_F(ComponentsMaker, MakingField)
{
    auto result = m_Maker->makeComponent("int a", models::DisplayPart::Fields);
    ASSERT_TRUE(result.first.isEmpty()) << "There are some message: " << result.first.toStdString().c_str();

    auto field = to_f(result.second.get());
    ASSERT_EQ(field->name(), QString("a"));

    auto t = m_GlobalDatabase->depthTypeSearch(field->typeId());
    ASSERT_TRUE(!!t) << "Type with name int is not found in global database";
    ASSERT_STREQ(t->name().toStdString().c_str(), QString("int").toStdString().c_str());
}
