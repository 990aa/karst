#pragma once

#ifdef NATIVEDRAWING_EXPORTS
#define NATIVE_API __declspec(dllexport)
#else
#define NATIVE_API __declspec(dllimport)
#endif

#include <vector>

struct Point {
    float x;
    float y;
};

struct Stroke {
    std::vector<Point> points;
    unsigned int color;
    float size;
    int toolType;
};

extern "C" {
    NATIVE_API void SmoothStroke(Stroke* stroke, float tension);
    NATIVE_API void OptimizeStroke(Stroke* stroke, float tolerance);
    NATIVE_API void CalculateStrokeBounds(const Stroke* stroke, float* minX, float* minY, float* maxX, float* maxY);
}