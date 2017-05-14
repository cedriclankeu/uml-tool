/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 28/06/2015.
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

#include "TestProjectTranslator.h"

#include <templates.cpp>

TEST_F(ProjectTranslatorTest, Type)
{
    QString futureResult("int");
    auto code(m_Translator->translate(m_int));
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "foo_scope::Foo";
    auto childScope = m_ProjectScope->addChildScope("foo_scope");
    auto foo = childScope->addType("Foo");
    code = m_Translator->translate(foo);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());
}

TEST_F(ProjectTranslatorTest, ExtendedType)
{
    auto type = std::make_shared<Entity::ExtendedType>();
    type->setId(Common::ID::firstFreeID() + 6);
    m_ProjectScope->addExistsType(type);
    type->setTypeId(m_int->id());

    QString futureResult("int");
    auto code(m_Translator->translate(type));
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "const int";
    type->setConstStatus(true);
    code = m_Translator->translate(type);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "const int &";
    type->addLinkStatus();
    code = m_Translator->translate(type);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "const int &&";
    type->addLinkStatus();
    code = m_Translator->translate(type);
    ASSERT_EQ(futureResult, code.toHeader);

    type->removeLinkStatus();
    type->removeLinkStatus();

    futureResult = "const int * const";
    type->addPointerStatus(true);
    code = m_Translator->translate(type);
    ASSERT_EQ(futureResult, code.toHeader);

    type->removePointerStatus();
    type->setConstStatus(false);

    futureResult = "vector<int>";
    auto vector = std::make_shared<Entity::Type>("vector", m_GlobalScope->id(),
                                                 Common::ID::firstFreeID() + 7);
    m_GlobalScope->addExistsType(vector);
    type->setTypeId(vector->id());
    type->addTemplateParameter(m_int->id());
    code = m_Translator->translate(type);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "using Ints = vector<int>;";
    type->setName("Ints");
    code = m_Translator->translate(type, Translation::ProjectTranslator::WithAlias |
                                        Translation::ProjectTranslator::WithNamespace);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "std::set<int>";
    m_ProjectScope->removeType(type->id());
    type = std::make_shared<Entity::ExtendedType>();
    type->setId(Common::ID::firstFreeID() + 8);
    m_ProjectScope->addExistsType(type);

    auto stdScope = std::make_shared<Entity::Scope>("std");
    stdScope->setId(Common::ID::stdScopeID());
    m_GlobalScope->addExistsChildScope(stdScope);

    auto set = std::make_shared<Entity::Type>("set", stdScope->id(),
                                              Common::ID::firstFreeID() + 9);
    stdScope->addExistsType(set);
    type->setTypeId(set->id());
    type->addTemplateParameter(m_int->id());
    code = m_Translator->translate(type);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());
}

TEST_F(ProjectTranslatorTest, Field)
{
    auto field = std::make_shared<Entity::Field>("Number", m_int->id());
    field->setPrefix("m_");

    QString futureResult("int m_Number");
    auto code(m_Translator->translate(field));
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "static int number_";
    field->setName("number");
    field->removePrefix();
    field->setSuffix("_");
    field->addKeyword(Entity::FieldStatic);
    code = m_Translator->translate(field);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "static const int number_";
    auto type = m_GlobalScope->addType<Entity::ExtendedType>();
    type->setTypeId(m_int->id());
    type->setConstStatus(true);
    field->setTypeId(type->id());
    code = m_Translator->translate(field);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());
}

TEST_F(ProjectTranslatorTest, ClassMethod)
{
    auto method = std::make_shared<Entity::ClassMethod>("calc");
    method->setConstStatus(true);

    QString futureResult("void calc() const");
    method->setReturnTypeId(m_void->id());
    auto code(m_Translator->translate(method));
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "double sum(double a, double b) const";
    method->setName("sum");
    method->setReturnTypeId(m_double->id());
    method->addParameter("a", m_double->id());
    method->addParameter("b", m_double->id());
    code = m_Translator->translate(method);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "ps::Foo *getFoo(const QString &id) const";
    auto ps = std::make_shared<Entity::Scope>("ps");
    ps->setId(Common::ID::firstFreeID() + 4);
    m_ProjectDb->addExistsScope(ps);

    auto foo = ps->addExistsType(
                   std::make_shared<Entity::Type>(
                       "Foo", Common::ID::globalScopeID(),
                       Common::ID::firstFreeID() + 4));
    auto fooExt = std::make_shared<Entity::ExtendedType>();
    fooExt->setId(Common::ID::firstFreeID() + 5);
    ps->addExistsType(fooExt);
    fooExt->addPointerStatus();
    fooExt->setTypeId(foo->id());

    auto qstrExt = std::make_shared<Entity::ExtendedType>();
    qstrExt->setId(Common::ID::firstFreeID() + 7);
    m_GlobalScope->addExistsType(qstrExt);
    qstrExt->addLinkStatus();
    qstrExt->setTypeId(m_qstring->id());
    qstrExt->setConstStatus(true);

    method->setName("getFoo");
    method->setReturnTypeId(fooExt->id());
    method->removeParameter("a");
    method->removeParameter("b");
    method->addParameter("id", qstrExt->id());

    code = m_Translator->translate(method, Translation::ProjectTranslator::WithNamespace);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "explicit Foo(const QString &name)";
    method = std::make_shared<Entity::ClassMethod>("Foo");
    method->addLhsIdentificator(Entity::LhsIdentificator::Explicit);
    method->addParameter("name", qstrExt->id());
    code = m_Translator->translate(method);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "virtual ps::Foo *make() = 0";
    method = std::make_shared<Entity::ClassMethod>("make");
    method->setReturnTypeId(fooExt->id());
    method->setRhsIdentificator(Entity::RhsIdentificator::PureVirtual);
    method->addLhsIdentificator(Entity::LhsIdentificator::Virtual);
    code = m_Translator->translate(method, Translation::ProjectTranslator::WithNamespace);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "ps::Foo *make() override";
    method->removeLhsIdentificator(Entity::LhsIdentificator::Virtual);
    method->setRhsIdentificator(Entity::RhsIdentificator::Override);
    code = m_Translator->translate(method, Translation::ProjectTranslator::WithNamespace);
    ASSERT_EQ(futureResult, code.toHeader);
}

TEST_F(ProjectTranslatorTest, TemplateClassMethod)
{
    Entity::SharedTemplateClassMethod method(std::make_shared<Entity::TemplateClassMethod>("swap"));

    QString futureResult("template <>\nswap()");
    auto code(m_Translator->translate(method));
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "template <class T>\nswap()";
    auto t = method->addLocalType("T");
    method->addTemplateParameter(t->id());
    code = m_Translator->translate(method);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "template <class T>\nswap(T *first, T *second)";
    auto ptrT = method->addLocalType<Entity::ExtendedType>();
    ptrT->setTypeId(t->id());
    ptrT->addPointerStatus();
    method->addParameter("first", ptrT->id());
    method->addParameter("second", ptrT->id());
    code = m_Translator->translate(method);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "template <class T = some_scope::Foo>\n"
                   "std::shared_ptr<T> make(const QString &name)";
    method = std::make_shared<Entity::TemplateClassMethod>("make");
    auto constLinkToQstr = m_GlobalScope->addType<Entity::ExtendedType>();
    constLinkToQstr->setTypeId(m_qstring->id());
    constLinkToQstr->addLinkStatus();
    constLinkToQstr->setConstStatus(true);
    method->addParameter("name", constLinkToQstr->id());

    auto some_scope = m_ProjectScope->addChildScope("some_scope");
    auto foo = some_scope->addType("Foo");
    t = method->addLocalType("T");
    method->addTemplateParameter(t->id(), foo->id());

    auto stdScope = m_GlobalDb->addScope("std");
    auto sharedPointer = stdScope->addType("shared_ptr");
    auto sharedPointerToT = stdScope->addType<Entity::ExtendedType>();
    sharedPointerToT->setTypeId(sharedPointer->id());
    sharedPointerToT->addTemplateParameter(t->id());
    method->setReturnTypeId(sharedPointerToT->id());

    code = m_Translator->translate(method, Translation::ProjectTranslator::WithNamespace);
    ASSERT_EQ(futureResult, code.toHeader);
}

TEST_F(ProjectTranslatorTest, Enum)
{
    auto fooEnum = m_ProjectScope->addType<Entity::Enum>("Foo");

    QString futureResult("enum Foo {};");
    auto code(m_Translator->translate(fooEnum));
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "enum class Foo {};";
    fooEnum->setStrongStatus(true);
    code = m_Translator->translate(fooEnum);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "enum class Foo : int {};";
    fooEnum->setEnumTypeId(m_int->id());
    code = m_Translator->translate(fooEnum);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "enum class Foo : int {bar, baz};";
    fooEnum->addElement("bar");
    fooEnum->addElement("baz");
    code = m_Translator->translate(fooEnum);
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "enum class Foo : int {bar = 0, baz = 1};";
    code = m_Translator->translate(fooEnum, Translation::ProjectTranslator::GenerateNumbers);
    ASSERT_EQ(futureResult, code.toHeader);
}

TEST_F(ProjectTranslatorTest, Union)
{
    auto fooUnion = m_ProjectScope->addType<Entity::Union>("Foo");

    QString futureResult("union Foo {};");
    auto code(m_Translator->translate(fooUnion));
    ASSERT_EQ(futureResult, code.toHeader);

    futureResult = "union Foo {\n    double a;\n    int b;\n};";
    fooUnion->addField("a", m_double->id());
    fooUnion->addField("b", m_int->id());
    code = m_Translator->translate(fooUnion);
    ASSERT_EQ(futureResult, code.toHeader);
}

TEST_F(ProjectTranslatorTest, Class)
{
    auto fooClass = m_ProjectScope->addType<Entity::Class>("Foo");

    QString futureResult("class Foo {};");
    auto code(m_Translator->translate(fooClass));
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "struct Foo {};";
    fooClass->setKind(Entity::StructType);
    code = m_Translator->translate(fooClass);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "class Foo : public Bar {};";
    fooClass->setKind(Entity::ClassType);
    auto barClass = m_ProjectScope->addType<Entity::Class>("Bar");
    fooClass->addParent(barClass->id(), Entity::Public);
    code = m_Translator->translate(fooClass);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    futureResult = "class Foo : public Bar, protected Baz {};";
    auto bazClass = m_ProjectScope->addType<Entity::Class>("Baz");
    fooClass->addParent(bazClass->id(), Entity::Protected);
    code = m_Translator->translate(fooClass);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    fooClass->removeParent(barClass->id());
    fooClass->removeParent(bazClass->id());

    futureResult = QString("class Foo\n{\n"
                           "%1private:\n"
                           "%1%1int a_;\n"
                           "%1%1Baz b_;\n"
                           "};").arg(INDENT);
    auto aField = fooClass->addField("a", m_int->id());
    aField->setSection(Entity::Private);
    aField->setSuffix("_");
    auto bField = fooClass->addField("b", bazClass->id());
    bField->setSection(Entity::Private);
    bField->setSuffix("_");
    code = m_Translator->translate(fooClass);
    ASSERT_EQ(futureResult.toStdString(), code.toHeader.toStdString());

    fooClass->removeField("a");
    fooClass->removeField("b");

    futureResult = QString("class Foo\n{\n"
                           "%1public:\n"
                           "%1%1int a;\n\n"
                           "%1protected:\n"
                           "%1%1int b;\n\n"
                           "%1private:\n"
                           "%1%1int c;\n"
                           "};").arg(INDENT);
    aField = fooClass->addField("a", m_int->id());
    bField = fooClass->addField("b", m_int->id());
    bField->setSection(Entity::Protected);
    auto cField = fooClass->addField("c", m_int->id());
    cField->setSection(Entity::Private);
    code = m_Translator->translate(fooClass);
    ASSERT_EQ(futureResult, code.toHeader);

    fooClass->removeField("a");
    fooClass->removeField("b");


    futureResult = QString("class Foo\n{\n"
                           "%1public:\n"
                           "%1%1int c() const;\n"
                           "%1%1void setC(const int &newC);\n\n"
                           "%1private:\n"
                           "%1%1int c;\n"
                           "};").arg(INDENT);
    auto cGetter = fooClass->makeMethod("c");
    cGetter->setConstStatus(true);
    cGetter->setReturnTypeId(cField->typeId());
    cGetter->setSection(Entity::Public);

    auto cSetter = fooClass->makeMethod("setC");
    cSetter->setReturnTypeId(m_void->id());
    cSetter->setSection(Entity::Public);
    auto constLintToInt = m_ProjectScope->addType<Entity::ExtendedType>();
    constLintToInt->setTypeId(cField->typeId());
    constLintToInt->setConstStatus(true);
    constLintToInt->addLinkStatus();
    cSetter->addParameter("newC", constLintToInt->id());

    code = m_Translator->translate(fooClass);
    ASSERT_EQ(futureResult, code.toHeader);
}

TEST_F(ProjectTranslatorTest, ClassWithProperties)
{
    auto cl = m_ProjectScope->addType<Entity::Class>("Baz");
    auto prop = cl->addProperty("a", m_int->id());
    prop->addGetter("getA").addSetter("setA").addNotifier("aChanged");

    QString expect = QString("class Baz{\n"
                             "Q_OBJECT"
                             "\n\n"
                             "int a MEMBER m_a READ getA WRITE setA NOTIFY aChanged"
                             "\n\n"
                             "%1public:\n"
                             "%1%1int getA() const;\n"
                             "%1public SLOTS:\n"
                             "%1%1void setA(int a);\n\n"
                             "%1signals:\n"
                             "%1%1void aChanged();\n\n"
                             "%1private:\n"
                             "%1%1int m_a;\n"
                             "};").arg(INDENT);
    ASSERT_EQ(expect.toStdString(), m_Translator->translate(cl).toHeader.toStdString());

    // Add all fields
    prop->addResetter("resetA")
         .setRevision(1)
         .addDesignableGetter("isDisignable")
         .addScriptableGetter("isScriptable")
         .setStored(false)
         .setUser(true)
         .setConstant(true)
         .setFinal(true);

    expect = QString("class Baz{\n"
                     "Q_OBJECT\n\n"
                     "int a MEMBER m_a READ getA WRITE setA RESET resetA NOTIFY aChanged REVISION 1 "
                            "DESIGNABLE isDisignable SCRIPTABLE isScriptable STORED false "
                            "USER true CONSTANT FINAL\n\n"
                     "%1public:\n"
                     "%1%1int getA() const;\n"
                     "%1%1bool isDisignable();\n"
                     "%1%1bool isScriptable();\n"
                     "%1public SLOTS:\n"
                     "%1%1void setA(int a);\n"
                     "%1%1void resetA();\n\n"
                     "%1signals:\n"
                     "%1%1void aChanged();\n\n"
                     "%1private:\n"
                     "%1%1int m_a;\n"
                     "};").arg(INDENT);

    ASSERT_EQ(expect.toStdString(), m_Translator->translate(cl).toHeader.toStdString());
}

TEST_F(ProjectTranslatorTest, TemplateClass)
{
    QString futureResult = QString("template <class T>\n"
                                   "struct Node"
                                   "{\n"
                                   "%1T data_;\n"
                                   "%1Node *next;\n"
                                   "};").arg(INDENT);
    auto nodeStruct = m_ProjectScope->addType<Entity::TemplateClass>("Node");
    nodeStruct->setKind(Entity::StructType);

    auto tType = nodeStruct->addLocalType("T");
    nodeStruct->addTemplateParameter(tType->id());

    auto dataField = nodeStruct->addField("data", tType->id());
    dataField->setSuffix("_");

    auto ptrNode = m_ProjectScope->addType<Entity::ExtendedType>();
    ptrNode->setTypeId(nodeStruct->id());
    ptrNode->addPointerStatus();
    nodeStruct->addField("next", ptrNode->id());

    auto code(m_Translator->translate(nodeStruct));
    ASSERT_EQ(futureResult, code.toHeader);
}

TEST_F(ProjectTranslatorTest, ClassImplementation)
{
    QString futureResult("int Foo::getC() const\n"
                         "{\n"
                         "}\n");

    Entity::SharedClass classFoo(m_ProjectScope->addType<Entity::Class>("Foo"));
    Entity::SharedMethod getterForC(classFoo->makeMethod("getC"));
    getterForC->setConstStatus(true);
    getterForC->setReturnTypeId(m_int->id());

    Translation::Code code(m_Translator->generateClassMethodsImpl(classFoo));
    ASSERT_TRUE(code.toHeader.isEmpty())
            << "Header should be empty!";
    ASSERT_EQ(futureResult.toStdString(), code.toSource.toStdString());

    futureResult = "int Foo::getC() const\n"
                   "{\n"
                   "}\n"
                   "\n"
                   "void Foo::setC(int c)\n"
                   "{\n"
                   "}\n";
    Entity::SharedMethod setterForC(classFoo->makeMethod("setC"));
    setterForC->setReturnTypeId(m_void->id());
    setterForC->addParameter("c", m_int->id());

    code = m_Translator->generateClassMethodsImpl(classFoo);
    ASSERT_TRUE(code.toHeader.isEmpty()) << "Header should be empty!";
    ASSERT_EQ(futureResult, code.toSource);

    QString futureResultH("template <class T>\n"
                          "void Foo::swap(T &src)\n"
                          "{\n"
                          "}\n");
    Entity::SharedTemplateClassMethod swapMethod(classFoo->makeMethod<Entity::TemplateClassMethod>("swap"));
    swapMethod->setReturnTypeId(m_void->id());
    Entity::SharedType typeT(swapMethod->addLocalType("T"));
    swapMethod->addTemplateParameter(typeT->id());
    Entity::SharedExtendedType typeTLink(swapMethod->addLocalType<Entity::ExtendedType>());
    typeTLink->setTypeId(typeT->id());
    typeTLink->addLinkStatus();
    swapMethod->addParameter("src", typeTLink->id());

    code = m_Translator->generateClassMethodsImpl(classFoo);
    ASSERT_FALSE(code.toHeader.isEmpty()) << "Header should be generated!";
    ASSERT_FALSE(code.toSource.isEmpty()) << "Source file should be generated!";
    ASSERT_EQ(futureResultH, code.toHeader);
    ASSERT_EQ(futureResult, code.toSource);
}

TEST_F(ProjectTranslatorTest, TemplateClassImplementation)
{
    QString futureResult("\ntemplate <class Value, class Deleter>\n"
                         "void ScopedPointer<Value, Deleter>::reset(Value *other)\n"
                         "{\n"
                         "}");

    Entity::SharedScope _std = m_GlobalDb->scope(Common::ID::stdScopeID());
    ASSERT_TRUE(!!_std);
    Entity::SharedType dd(_std->addType("default_delete"));

    Entity::SharedTemplateClass scopedPointer(m_ProjectScope->addType<Entity::TemplateClass>("ScopedPointer"));
    Entity::SharedType value(scopedPointer->addLocalType("Value"));
    Entity::SharedType deleter(scopedPointer->addLocalType("Deleter"));
    Entity::SharedExtendedType defaultDelete(scopedPointer->addLocalType<Entity::ExtendedType>());
    defaultDelete->setTypeId(dd->id());
    defaultDelete->addTemplateParameter(value->id());
    scopedPointer->addTemplateParameter(value->id());
    scopedPointer->addTemplateParameter(deleter->id(), defaultDelete->id());

    Entity::SharedMethod resetMethod(scopedPointer->makeMethod("reset"));
    resetMethod->setReturnTypeId(m_void->id());
    Entity::SharedExtendedType valuePtr(scopedPointer->addLocalType<Entity::ExtendedType>());
    valuePtr->setTypeId(value->id());
    valuePtr->addPointerStatus();
    resetMethod->addParameter("other", valuePtr->id());

    Translation::Code code(m_Translator->generateClassMethodsImpl(scopedPointer));
    ASSERT_EQ(futureResult, code.toHeader);
}
