#ifndef Corrade_Utility_Endianness_h
#define Corrade_Utility_Endianness_h
/*
    This file is part of Corrade.

    Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
                2017, 2018, 2019 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Namespace @ref Corrade::Utility::Endianness
 */

#include <cstdint>

#include "Corrade/configure.h"
#include "Corrade/Utility/TypeTraits.h"
#include "Corrade/Utility/utilities.h"

namespace Corrade { namespace Utility {

/**
@brief Endianness-related functions

This library is built if `WITH_UTILITY` is enabled when building Corrade. To
use this library with CMake, request the `Utility` component of the `Corrade`
package and link to the `Corrade::Utility` target:

@code{.cmake}
find_package(Corrade REQUIRED Utility)

# ...
target_link_libraries(your-app PRIVATE Corrade::Utility)
@endcode

See also @ref building-corrade and @ref corrade-cmake for more information.
*/
namespace Endianness {

namespace Implementation {
    template<std::size_t size> struct TypeFor {};
    template<> struct TypeFor<1> { typedef std::uint8_t  Type; };
    template<> struct TypeFor<2> { typedef std::uint16_t Type; };
    template<> struct TypeFor<4> { typedef std::uint32_t Type; };
    template<> struct TypeFor<8> { typedef std::uint64_t Type; };

    inline std::uint8_t swap(std::uint8_t value) {
        return value;
    }
    inline std::uint16_t swap(std::uint16_t value) {
        return (value >> 8) |
               (value << 8);
    }
    inline std::uint32_t swap(std::uint32_t value) {
        return (value >> 24) |
              ((value << 8) & 0x00ff0000u) |
              ((value >> 8) & 0x0000ff00u) |
               (value << 24);
    }
    inline std::uint64_t swap(std::uint64_t value) {
        return (value >> 56) |
              ((value << 40) & 0x00ff000000000000ull) |
              ((value << 24) & 0x0000ff0000000000ull) |
              ((value <<  8) & 0x000000ff00000000ull) |
              ((value >>  8) & 0x00000000ff000000ull) |
              ((value >> 24) & 0x0000000000ff0000ull) |
              ((value >> 40) & 0x000000000000ff00ull) |
               (value << 56);
    }
}

/**
@brief Endian-swap bytes of given value

@see @ref swapInPlace(), @ref littleEndian(), @ref bigEndian()
*/
template<class T> inline T swap(T value) {
    return bitCast<T>(Implementation::swap(bitCast<typename Implementation::TypeFor<sizeof(T)>::Type>(value)));
}

/**
@brief Endian-swap bytes of each argument in-place
@m_since_latest

Calls @ref swap() on each value.
@see @ref littleEndianInPlace(), @ref bigEndianInPlace()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class ...T> void swapInPlace(T&... values);
#else
inline void swapInPlace() {}
/* to avoid the StridedArrayView overloads being taken by this one */
template<class T, class ...U, class = typename std::enable_if<!IsIterable<T>::value>::type> inline void swapInPlace(T& first, U&... next) {
    first = swap(first);
    swapInPlace(next...);
}
#endif

/**
@brief Whether actual system is Big-Endian

@see @ref CORRADE_TARGET_BIG_ENDIAN
*/
constexpr bool isBigEndian() {
    #ifdef CORRADE_TARGET_BIG_ENDIAN
    return true;
    #else
    return false;
    #endif
}

/**
@brief Convert number from or to Big-Endian

On Little-Endian systems calls @ref swap(), on Big-Endian systems returns the
value unchanged.
@see @ref isBigEndian(), @ref CORRADE_TARGET_BIG_ENDIAN,
    @ref bigEndianInPlace()
*/
template<class T> inline T bigEndian(T value) {
    #ifdef CORRADE_TARGET_BIG_ENDIAN
    return value;
    #else
    return swap(value);
    #endif
}

/**
@brief Convert values from or to Big-Endian in-place

On Little-Endian systems calls @ref swapInPlace(T&... values), on Big-Endian
systems does nothing.
@see @ref isBigEndian(), @ref CORRADE_TARGET_BIG_ENDIAN,
    @ref littleEndianInPlace(), @ref bigEndian()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class ...T> void bigEndianInPlace(T&... values);
#else
/* to avoid the StridedArrayView overloads being taken by this one */
template<class T, class ...U, class = typename std::enable_if<!IsIterable<T>::value>::type> inline void bigEndianInPlace(T& first, U&...
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        next
        #endif
    ) {
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    swapInPlace(first, next...);
    #else
    static_cast<void>(first);
    #endif
}
#endif

/**
@brief Convert value from or to Little-Endian

On Big-Endian systems calls @ref swap(), on Little-Endian systems returns the
value unchanged.
@see @ref isBigEndian(), @ref CORRADE_TARGET_BIG_ENDIAN,
    @ref littleEndianInPlace()
*/
template<class T> inline T littleEndian(T value) {
    #ifdef CORRADE_TARGET_BIG_ENDIAN
    return swap(value);
    #else
    return value;
    #endif
}

/**
@brief Convert values from or to Little-Endian in-place

On Big-Endian systems calls @ref swapInPlace(T&... values), on Little-Endian
systems does nothing.
@see @ref isBigEndian(), @ref CORRADE_TARGET_BIG_ENDIAN,
    @ref bigEndianInPlace(), @ref littleEndian()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class ...T> void littleEndianInPlace(T&... values);
#else
/* to avoid the StridedArrayView overloads being taken by this one */
template<class T, class ...U, class = typename std::enable_if<!IsIterable<T>::value>::type> inline void littleEndianInPlace(T& first, U&...
        #ifdef CORRADE_TARGET_BIG_ENDIAN
        next
        #endif
    ) {
    #ifdef CORRADE_TARGET_BIG_ENDIAN
    swapInPlace(first, next...);
    #else
    static_cast<void>(first);
    #endif
}
#endif

}

}}

#endif
