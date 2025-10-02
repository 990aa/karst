#include "NativeDrawing.h"
#include <algorithm>
#include <cmath>
#include <vector>

void SmoothStroke(Stroke* stroke, float tension) {
    if (stroke->points.size() < 3) return;

    std::vector<Point> smoothed;
    smoothed.reserve(stroke->points.size());

    // Copy first point
    smoothed.push_back(stroke->points[0]);

    for (size_t i = 1; i < stroke->points.size() - 1; i++) {
        Point prev = stroke->points[i - 1];
        Point curr = stroke->points[i];
        Point next = stroke->points[i + 1];

        // Catmull-Rom spline
        Point smoothedPoint;
        smoothedPoint.x = 0.5f * ((2 * curr.x) + 
            (next.x - prev.x) * tension + 
            (2 * prev.x - 5 * curr.x + 4 * next.x - next.x) * tension * tension + 
            (-prev.x + 3 * curr.x - 3 * next.x + next.x) * tension * tension * tension);
        
        smoothedPoint.y = 0.5f * ((2 * curr.y) + 
            (next.y - prev.y) * tension + 
            (2 * prev.y - 5 * curr.y + 4 * next.y - next.y) * tension * tension + 
            (-prev.y + 3 * curr.y - 3 * next.y + next.y) * tension * tension * tension);

        smoothed.push_back(smoothedPoint);
    }

    // Copy last point
    smoothed.push_back(stroke->points.back());

    stroke->points = std::move(smoothed);
}

void OptimizeStroke(Stroke* stroke, float tolerance) {
    if (stroke->points.size() < 3) return;

    std::vector<Point> optimized;
    optimized.reserve(stroke->points.size());

    // Douglas-Peucker algorithm
    optimized.push_back(stroke->points[0]);
    
    size_t lastIndex = 0;
    for (size_t i = 1; i < stroke->points.size() - 1; i++) {
        Point prev = stroke->points[lastIndex];
        Point curr = stroke->points[i];
        Point next = stroke->points[i + 1];

        // Calculate distance from point to line
        float dx = next.x - prev.x;
        float dy = next.y - prev.y;
        float length = std::sqrt(dx * dx + dy * dy);
        
        if (length > 0) {
            float distance = std::abs((dy * curr.x - dx * curr.y + next.x * prev.y - next.y * prev.x) / length);
            
            if (distance > tolerance) {
                optimized.push_back(curr);
                lastIndex = i;
            }
        }
    }

    optimized.push_back(stroke->points.back());
    stroke->points = std::move(optimized);
}

void CalculateStrokeBounds(const Stroke* stroke, float* minX, float* minY, float* maxX, float* maxY) {
    if (stroke->points.empty()) {
        *minX = *minY = *maxX = *maxY = 0;
        return;
    }

    *minX = *maxX = stroke->points[0].x;
    *minY = *maxY = stroke->points[0].y;

    for (const auto& point : stroke->points) {
        *minX = std::min(*minX, point.x);
        *maxX = std::max(*maxX, point.x);
        *minY = std::min(*minY, point.y);
        *maxY = std::max(*maxY, point.y);
    }
}