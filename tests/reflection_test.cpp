/* blah_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Experimental tests
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


/******************************************************************************/
/* BAR                                                                        */
/******************************************************************************/

namespace test {

struct Bar
{
    Bar() : bar(0) {}

    int bar;
};

} // namespace test

reflectClass(test::Bar)
{
    reflectField(bar);
}


/******************************************************************************/
/* FOO                                                                        */
/******************************************************************************/

namespace test {

struct Foo : public Bar
{
    Foo() : field(0), constField(0), value(0) {}

    int field;
    const int constField;

    void void_() {}

    const int& getter() const { return value; }
    void setter(int i) { value = i; }

    void copy(int i) { value = i; }
    int copy() const { return value; }

    void lValue(int& i) { value = i; }
    int& lValue() { return value; }

    void constLValue(const int& i) { value = i; }
    const int& constLValue() const { return value; }

    void rValue(int&& i) { value = std::move(i); }
    int rValue() { return std::move(value); }

    void fn(int a, int b, int c) { value += a * b + c; };
    static Foo make() { return Foo(); };

    int value;
};

} // namespace test


reflectClass(test::Foo)
{
    reflectParent(test::Bar);

    reflectField(void_);
    reflectField(field);

    reflectField(getter);
    reflectField(setter);

    reflectField(constField);
    reflectField(copy);
    reflectField(lValue);
    reflectField(constLValue);
    reflectField(rValue);

    reflectFunction(fn);
    reflectFunction(make);

    reflectCustom(custom) (test::Foo& obj, int a, int b) {
        obj.setter(a + b);
    };
}


/******************************************************************************/
/* TESTS                                                                      */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(basics)
{
    Type* typeBar = type("test::Bar");
    Type* typeFoo = type("test::Foo");

    BOOST_CHECK(typeBar);
    BOOST_CHECK(typeFoo);

    std::cerr << typeBar->print() << std::endl;
    std::cerr << typeFoo->print() << std::endl;

    BOOST_CHECK_EQUAL(typeFoo->parent(), typeBar);
    BOOST_CHECK( typeFoo->isConvertibleTo(typeBar));
    BOOST_CHECK(!typeBar->isConvertibleTo(typeFoo));

    BOOST_CHECK( typeBar->hasField("bar"));
    BOOST_CHECK(!typeBar->hasField("field"));

    BOOST_CHECK( typeFoo->hasField("bar"));
    BOOST_CHECK(!typeFoo->hasField("baz"));
    BOOST_CHECK(!typeFoo->hasField("void_"));
    BOOST_CHECK( typeFoo->hasField("field"));
    BOOST_CHECK( typeFoo->hasField("getter"));
    BOOST_CHECK( typeFoo->hasField("setter"));
    BOOST_CHECK( typeFoo->hasField("copy"));
    BOOST_CHECK( typeFoo->hasField("rValue"));
    BOOST_CHECK( typeFoo->hasField("fn"));
    BOOST_CHECK( typeFoo->hasField("custom"));

    Value vFoo = typeFoo->call<Value>("make");
    const auto& foo = vFoo.get<test::Foo>();
    const auto& bar = vFoo.get<test::Bar>();

    vFoo.call<void>("bar", 1);
    BOOST_CHECK_EQUAL(foo.bar, 1);
    BOOST_CHECK_EQUAL(bar.bar, 1);
    BOOST_CHECK_EQUAL(vFoo.call<int>("bar"), foo.bar);

    vFoo.call<void>("setter", 1);
    BOOST_CHECK_EQUAL(foo.value, 1);
    BOOST_CHECK_EQUAL(vFoo.call<int>("getter"), foo.value);

    vFoo.call<void>("custom", 1, 2);
    BOOST_CHECK_EQUAL(foo.value, 1 + 2);
}