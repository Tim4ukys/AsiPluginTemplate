/*
    AsiPluginTemplate header file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#include <Windows.h>

#ifndef BASE_CHECK_ARRAYSIZE
#define BASE_CHECK_ARRAYSIZE(arr, dec) \
    static_assert(ARRAYSIZE(arr) == dec && __FILE__ ": ARRAYSIZE != dec")
#endif
