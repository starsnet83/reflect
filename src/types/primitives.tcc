/* primitives.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection utilities for primitives.
*/

#pragma once

#include "reflect.h"
#include "dsl/basics.h"
#include "dsl/plumbing.h"

#include <limits>

namespace reflect {

/******************************************************************************/
/* REFLECT LIMIT                                                              */
/******************************************************************************/

#define reflectLimit(name) \
    type_->add(#name, [] { return std::numeric_limits<T_>::name(); });


/******************************************************************************/
/* REFLECT NUMBER                                                             */
/******************************************************************************/

template<typename T_>
void reflectNumberImpl(Type* type_)
{
    reflectPlumbing();

    reflectLimit(min);
    reflectLimit(max);

    reflectTrait(primitive);

    reflectCustom(operator+) (const int& obj, int value) {
        return obj + value;
    };

    if (std::is_same<T_, bool>::value)
        reflectTrait(bool);

    else if (std::numeric_limits<T_>::is_integer) {
        reflectTrait(integer);

        if (std::numeric_limits<T_>::is_signed)
            reflectTrait(signed);
        else reflectTrait(unsigned);
    }

    else reflectTrait(float);
}

#define reflectNumber(num) \
    reflectTypeImpl(num) { reflectNumberImpl<T_>(type_); }

} // reflect
