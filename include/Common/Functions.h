#pragma once

inline Color RandomColor()
{
    Color color;
    color.r = (float)(rand() % 255) / 255.0f;
    color.g = (float)(rand() % 255) / 255.0f;
    color.b = (float)(rand() % 255) / 255.0f;
    color.a = 1.0f;
    return color;
};