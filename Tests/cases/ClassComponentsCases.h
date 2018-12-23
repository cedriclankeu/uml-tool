/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 16/08/2015.
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
#include "Tests/TestClassComponents.h"

#include "Constants.h"

#define check_moving(type, name) \
    type name##1(*name);\
    type name##2(std::move(name##1));\
    ASSERT_EQ(*name, name##2);\
    type name##3 = std::move(name##2);\
    ASSERT_EQ(*name, name##3);

#define test_basic_prop(Name, name) \
    ASSERT_TRUE(!!property->add##Name().name()); \
    property->delete##Name(); \
    ASSERT_FALSE(!!property->name());

#define test_script_des(Name, name) \
    ASSERT_TRUE(property->is##Name()); \
    ASSERT_TRUE(property->is##Name##Default()); \
    ASSERT_TRUE(!!property->add##Name##Getter().name##Getter()); \
    property->delete##Name##Getter(); \
    ASSERT_FALSE(!!property->name##Getter()); \
    ASSERT_FALSE(property->set##Name(false).is##Name()); \

#define check_additional(Name) \
    ASSERT_TRUE(property->is##Name##Default()); \
    ASSERT_FALSE(property->set##Name(false).is##Name());

TEST_F(ClassComponents, Field)
{
    // Check a several methods which are not covered in previous tests
    auto field = std::make_shared<Entity::Field>();
    ASSERT_TRUE(field->prefix().isEmpty());
    ASSERT_TRUE(field->suffix().isEmpty());
    ASSERT_FALSE(field->hasKeywords());

    field->setPrefix("m_");
    field->setSuffix("P");
    field->addKeyword(Entity::FieldKeyword::Volatile);
    ASSERT_EQ(field->prefix(), "m_");
    ASSERT_EQ(field->suffix(), "P");
    ASSERT_TRUE(field->containsKeyword(Entity::FieldKeyword::Volatile));

    // Check moving
    check_moving(Entity::Field, field)
}

TEST_F(ClassComponents, Method)
{
    // Check a several methods which are not covered in previous tests
    auto method = std::make_shared<Entity::ClassMethod>();
    ASSERT_FALSE(method->hasParameters());
    ASSERT_FALSE(method->hasLhsIdentificators());

    auto p1 = method->addParameter("p1", Common::ID::nullID());
    method->addLhsIdentificator(Entity::LhsIdentificator::Inline);
    ASSERT_TRUE(method->containsParameter(p1->name()));
    ASSERT_TRUE(method->containsLhsIdentficator(Entity::LhsIdentificator::Inline));

    // Slot/signal
    ASSERT_FALSE(method->isSlot());
    ASSERT_FALSE(method->isSignal());

    method->setIsSlot(true);
    ASSERT_TRUE(method->isSlot());
    ASSERT_EQ(method->section(), Entity::Section::Public);
    ASSERT_FALSE(method->isSignal());

    method->setIsSignal(true);
    ASSERT_TRUE(method->isSignal());
    ASSERT_EQ(method->section(), Entity::Section::None);
    ASSERT_FALSE(method->isSlot());

    // Check moving
    check_moving(Entity::ClassMethod, method)
}

TEST_F(ClassComponents, Property)
{
    // Check a several methods which are not covered in previous tests
    auto property = std::make_shared<Entity::Property>();
    property->setTypeSearcher(m_GlobalDb);
    ASSERT_EQ(property->name(), DEFAULT_NAME);
    ASSERT_EQ(property->typeId(), Common::ID::nullID());

    property->setTypeId(Common::ID::firstFreeID() + 1);
    ASSERT_EQ(property->typeId(), Common::ID::firstFreeID() + 1);

    ASSERT_TRUE(!!property->field());
    ASSERT_EQ(property->field()->section(), Entity::Private);

    check_additional(Stored)
    check_additional(User)
    check_additional(Constant)
    check_additional(Final)
    check_additional(Member)

    test_basic_prop(Getter,   getter)
    test_basic_prop(Setter,   setter)
    test_basic_prop(Resetter, resetter)
    test_basic_prop(Notifier, notifier)
    test_basic_prop(Field,    field)

    ASSERT_TRUE(property->isRevisionDefault());
    ASSERT_EQ(10, property->setRevision(10).revision());

    test_script_des(Scriptable, scriptable)
    test_script_des(Designable, designable)

    ASSERT_EQ(property->hashType(), Entity::Property::staticHashType());
    ASSERT_EQ(property->marker(), Entity::Property::staticMarker());

    property->setId(Common::ID::firstFreeID() + 2);
    ASSERT_EQ(property->id(), Common::ID::firstFreeID() + 2);

    // Check moving
    check_moving(Entity::Property, property)

    // Check slot signals
    property->addField("Foo");
    property->addSetter().addNotifier().addResetter();
    ASSERT_TRUE(!!property->setter());
    ASSERT_TRUE(property->setter()->isSlot());

    ASSERT_TRUE(!!property->notifier());
    ASSERT_TRUE(property->notifier()->isSignal());

    ASSERT_TRUE(!!property->resetter());
    ASSERT_TRUE(property->resetter()->isSlot());
}
