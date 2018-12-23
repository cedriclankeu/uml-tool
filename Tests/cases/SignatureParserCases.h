/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 29/08/2015.
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

#include <QPair>
#include <QDebug>

#include <Models/ComponentsModel.h>
#include <Entity/Components/token.h>

#include "Tests/TestComponentSignatureParser.h"

namespace {

    using TestData = const QVector<QPair<QString, bool>>;

    TestData fieldData =
    {
        {"",                                              false},
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
        {"int *const a",                                  true },
        {"std::vector<int> vec",                          true },
        {"std::vector<int, MyAlloc> vec",                 true },
        {"std::vector<int,> vec",                         true },
        {"std::vector<int><int> vec",                     false},
        {"std::vector< vec",                              false},
        {"std::vector> vec",                              false},
        {"std::vector<class, const> vec",                 false},
        {"std::vector<std::vector> vec",                  true },
    };

    TestData methodData =
    {
        {"",                                       false},
        {"const foo::bar * get()",                 true },
        {"const std::vector<int> & get()",         true },
        {"const std::<int> get() &",               false},
        {"int get()",                              true },
        {"get()",                                  false},
        {"get int()",                              false},
        {"int get() const",                        true },
        {"int const * get() const",                true },
        {"int get() final",                        true },
        {"int get() const final",                  true },
        {"int get() override",                     true },
        {"int get() bla-bla-bla",                  false},
        {"static int get()",                       true },
        {"int get() = 0",                          true },
        {"int get() const = 0",                    true },
        {"int get() = default",                    true },
        {"int get() = delete",                     true },
        {"int get() = bla-bla-bla",                false},
        {"int get(int arg1, double arg2)",         true },
        {"int get(const int* arg1, double& arg2)", true },
        {"explicit int get()",                     true },
        {"inline int get()",                       true },
        {"inline inline int get()",                false},
        {"static int get()",                       true },
        {"virtual int get()",                      true },
        {"friend int get()",                       true },
    };

    TestData propertyData =
    {
        {"",                                              false},
        {"int width",                                     true },
        {"int width MEMBER m_width",                      true },
        {"  int   width   MEMBER    m_width     ",        true },
        {"true name",                                     false},
        {"MEMBER m_width",                                false},
        {"int width member m_width",                      false},
        {"false width member m_width",                    false},
        {"int width MEMBER m_width READ getWidth "
         "WRITE setWidth",                                true },
        {"int width MEMBER m_width READ getWidth "
         "WRITE setWidth RESET clearWidth NOTIFY "
         "widthChanged",                                  true },
        {"int width MEMBER m_width READ getWidth "
         "WRITE setWidth RESET clearWidth NOTIFY "
         "widthChanged REVISION 10",                      true },
        {"int width MEMBER m_width READ getWidth "
         "WRITE setWidth RESET clearWidth NOTIFY "
         "widthChanged REVISION 10 DESIGNABLE "
         "false SCRIPTABLE false STORED false "
         "USER true",                                     true },
        {"int width MEMBER m_width READ getWidth "
         "WRITE setWidth RESET clearWidth NOTIFY "
         "widthChanged REVISION 10 DESIGNABLE "
         "isDesignable SCRIPTABLE isScriptable"
         " STORED false USER true",                       true },
        {"int width MEMBER m_width READ getWidth "
         "WRITE setWidth RESET clearWidth NOTIFY "
         "widthChanged REVISION 10 DESIGNABLE "
         "isDesignable SCRIPTABLE isScriptable"
         " STORED false USER true CONSTANT FINAL",        true },
    };
}

TEST_F(SignatureParser, TestingTokenClass)
{
    Components::SharedToken token = std::make_shared<Components::Token>();
    ASSERT_TRUE(token->isEmpty());
    ASSERT_FALSE(token->isSingle());
    ASSERT_FALSE(token->isMulti());
    ASSERT_EQ(token->toStdString(), "");
    token->dump();

    token = std::make_shared<Components::Token>("one_token");
    ASSERT_FALSE(token->isEmpty());
    ASSERT_TRUE(token->isSingle());
    ASSERT_FALSE(token->isMulti());
    ASSERT_EQ(token->token(), "one_token");
    ASSERT_EQ(token->toStdString(), "one_token");
    token->dump();

    Components::Tokens tokens;
    tokens << std::make_shared<Components::Token>("1_token") <<
              std::make_shared<Components::Token>("2_token") <<
              std::make_shared<Components::Token>("3_token");
    token = std::make_shared<Components::Token>(tokens);
    ASSERT_FALSE(token->isEmpty());
    ASSERT_FALSE(token->isSingle());
    ASSERT_TRUE(token->isMulti());
    ASSERT_EQ(token->tokens(), tokens);
    ASSERT_EQ(token->toStdString(), "1_token 2_token 3_token");
    token->dump();
}

TEST_F(SignatureParser, ParsingFieldSignature)
{
    for (auto &&testData: fieldData)
        ASSERT_EQ(m_Parser->parse(testData.first, Models::DisplayPart::Fields), testData.second)
            << "Signature: " << testData.first.toStdString();
}

TEST_F(SignatureParser, ParsingMethodSignature)
{
    for (auto &&testData: methodData)
        ASSERT_EQ(m_Parser->parse(testData.first, Models::DisplayPart::Methods), testData.second)
            << "Signature: " << testData.first.toStdString();
}

TEST_F(SignatureParser, ParsingPropertySignature)
{
    for (auto &&testData: propertyData)
        ASSERT_EQ(m_Parser->parse(testData.first, Models::DisplayPart::Properties), testData.second)
            << "Signature: " << testData.first.toStdString();
}
