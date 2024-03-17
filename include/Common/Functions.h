#pragma once
#include "Types.h"
#include <cstdlib>
#define MODULE_TEST 1
inline Color RandomColor()
{
    Color color;
    color.r = (float)(rand() % 255) / 255.0f;
    color.g = (float)(rand() % 255) / 255.0f;
    color.b = (float)(rand() % 255) / 255.0f;
    color.a = 1.0f;
    return color;
};