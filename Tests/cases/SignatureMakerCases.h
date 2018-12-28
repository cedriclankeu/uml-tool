/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 13/08/2015.
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

#include "Tests/TestSignatureMaker.hpp"

#include <range/v3/algorithm/find_if.hpp>

namespace
{
    auto findType(const DB::SharedDatabase &d, const QString &name)
    {
        Entity::SharedType type;
        ranges::find_if(d->scopes(), [&](auto &&scope){  type = scope->type(name); return !!type; });

        return type;
    }
}

TEST_F(SignatureMaker, MakingFieldSignature)
{
    auto typeInt = findType(m_GlobalDb, "int");
    ASSERT_TRUE(!!typeInt);

    // type and name
    Entity::SharedField field = std::make_shared<Entity::Field>();
    field->setName("a");
    field->setTypeId(typeInt->id());

    QString actual = m_Maker->signature(field);
    QString expect = "int a";
    ASSERT_EQ(actual, expect);

    // const type name
    auto constInt = m_ProjectScope->addType<Entity::ExtendedType>();
    constInt->setTypeId(typeInt->id());
    constInt->setConstStatus(true);
    field->setTypeId(constInt->id());

    actual = m_Maker->signature(field);
    expect = "const int a";
    ASSERT_EQ(actual, expect);

    // const ptr to const
    constInt->addPointerStatus(true /*pointer to const*/);
    actual = m_Maker->signature(field);
    expect = "const int * const a";
    ASSERT_EQ(actual, expect);

    // mutable
    field->setTypeId(typeInt->id());
    field->addKeyword(Entity::FieldKeyword::Mutable);
    actual = m_Maker->signature(field);
    expect = "mutable int a";
    ASSERT_EQ(actual, expect);

    // std
    auto deque = findType(m_GlobalDb, "deque");
    ASSERT_TRUE(!!deque);
    field->setTypeId(deque->id());
    field->removeKeyword(Entity::FieldKeyword::Mutable);
    actual = m_Maker->signature(field);
    expect = "std::deque a";
    ASSERT_EQ(actual, expect);

    // with template parameters
    auto unMap = findType(m_GlobalDb, "unordered_map");
    ASSERT_TRUE(!!unMap);
    auto str = findType(m_GlobalDb, "string");
    ASSERT_TRUE(!!str);
    auto extMap = m_ProjectScope->addType<Entity::ExtendedType>();
    extMap->setTypeId(unMap->id());
    extMap->addTemplateParameter(str->id());
    extMap->addTemplateParameter(typeInt->id());
    field->setTypeId(extMap->id());

    actual = m_Maker->signature(field);
    expect = "std::unordered_map<std::string, int> a";
    ASSERT_EQ(actual.toStdString(), expect.toStdString());
}

TEST_F(SignatureMaker, MakingMethodSignature)
{
    auto typeInt = findType(m_GlobalDb, "int");
    ASSERT_TRUE(!!typeInt);

    // simple field
    auto method = std::make_shared<Entity::ClassMethod>("getInt");
    method->setReturnTypeId(typeInt->id());
    QString actual = m_Maker->signature(method);
    QString expect = "int getInt()";
    ASSERT_EQ(actual, expect);

    // const getter
    method->setConstStatus(true);
    actual = m_Maker->signature(method);
    expect = "int getInt() const";
    ASSERT_EQ(actual, expect);

    // pure virtual const getter
    method->addLhsIdentificator(Entity::LhsIdentificator::Virtual);
    method->setRhsIdentificator(Entity::RhsIdentificator::PureVirtual);
    actual = m_Maker->signature(method);
    expect = "virtual int getInt() const = 0";
    ASSERT_EQ(actual, expect);

    // with parameter
    method->addParameter("parameter1", typeInt->id());
    actual = m_Maker->signature(method);
    expect = "virtual int getInt(int parameter1) const = 0";
    ASSERT_EQ(actual, expect);

    // with parameters
    auto p2 = method->addParameter("parameter2", typeInt->id());
    actual = m_Maker->signature(method);
    expect = "virtual int getInt(int parameter1, int parameter2) const = 0";
    ASSERT_EQ(actual, expect);
}

TEST_F(SignatureMaker, MakingPropertySignature)
{
    auto typeInt = findType(m_GlobalDb, "int");
    ASSERT_TRUE(!!typeInt);

    // Simple property
    auto property = std::make_shared<Entity::Property>("width", typeInt->id());
    property->setTypeSearcher(m_GlobalDb);
    property->field()->setPrefix("m_");
    QString actual = m_Maker->signature(property);
    QString expect = "int width MEMBER m_width";
    ASSERT_EQ(actual, expect);

    property->addGetter("getWidth").addSetter("setWidth");
    actual = m_Maker->signature(property);
    expect = "int width MEMBER m_width READ getWidth WRITE setWidth";
    ASSERT_EQ(actual, expect);

    property->addResetter("clearWidth").addNotifier("widthChanged");
    actual = m_Maker->signature(property);
    expect = "int width MEMBER m_width READ getWidth WRITE setWidth"
             " RESET clearWidth NOTIFY widthChanged";
    ASSERT_EQ(actual, expect);

    property->setRevision(10);
    actual = m_Maker->signature(property);
    expect = "int width MEMBER m_width READ getWidth WRITE setWidth"
             " RESET clearWidth NOTIFY widthChanged REVISION 10";
    ASSERT_EQ(actual, expect);

    property->setDesignable(false).setScriptable(false).setStored(false).setUser(true);
    actual = m_Maker->signature(property);
    expect = "int width MEMBER m_width READ getWidth WRITE setWidth"
             " RESET clearWidth NOTIFY widthChanged REVISION 10"
             " DESIGNABLE false SCRIPTABLE false STORED false USER true";
    ASSERT_EQ(actual, expect);

    property->addDesignableGetter("isDesignable").addScriptableGetter("isScriptable");
    actual = m_Maker->signature(property);
    expect = "int width MEMBER m_width READ getWidth WRITE setWidth"
             " RESET clearWidth NOTIFY widthChanged REVISION 10"
             " DESIGNABLE isDesignable SCRIPTABLE isScriptable"
             " STORED false USER true";
    ASSERT_EQ(actual, expect);

    property->setFinal(true).setConstant(true);
    actual = m_Maker->signature(property);
    expect = "int width MEMBER m_width READ getWidth WRITE setWidth"
             " RESET clearWidth NOTIFY widthChanged REVISION 10"
             " DESIGNABLE isDesignable SCRIPTABLE isScriptable"
             " STORED false USER true CONSTANT FINAL";
    ASSERT_EQ(actual, expect);
}
