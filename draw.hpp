#pragma once

#include "geometry.hpp"

// constants and defines

constexpr ftype screenWidth = 600, screenHeight = 600,
                scale = min(screenHeight, screenWidth) / 2;
const ftype pointradius = 2, lineweight = 2;

// declerations

point transfer(const point& a);
void drawpoint(const point& a, const Color& c = WHITE);
void drawline(const point& a, const point& b, const Color& c = WHITE);
void drawscatter(const vector<point>& a, const Color& c = WHITE);
void drawpolygon(const vector<point>& a, const Color& fc = WHITE);
void drawpolygonboundary(const vector<point>& a, const Color c = RED);

// implementations

point transfer(const point& a) {
    return {screenWidth / 2 + scale * a.x, screenHeight / 2 - scale * a.y};
}  // the coordinate plane is [-1, 1] X [-1, 1] for a square window and [-1, 1]
   // along the smaller axis otherwise

void drawpoint(const point& a, const Color& c) {
    DrawCircleV(transfer(a), pointradius, c);
}

void drawline(const point& a, const point& b, const Color& c) {
    DrawLineEx(transfer(a), transfer(b), lineweight, c);
}

void drawscatter(const vector<point>& a, const Color& c) {
    for (int i = 0; i < a.size(); i++) drawpoint(a[i], c);
}  // depends on drawpoint

void drawpolygon(const vector<point>& a, const Color& fc) {
    const int n = a.size();
    for (int i = 1; i < n - 1; i++)
        DrawTriangle(transfer(a[0]), transfer(a[i]), transfer(a[i + 1]), fc);
}  // depends on nothing

void drawpolygonboundary(const vector<point>& a, const Color c) {
    const int n = a.size();

    for (int i = 0; i < n - 1; i++)
        DrawLineEx(transfer(a[i]), transfer(a[i + 1]), lineweight, c);
    DrawLineEx(transfer(a[0]), transfer(a[n - 1]), lineweight, c);

    for (int i = 0; i < n; i++) drawpoint(a[i]);
}  // depends on drawopoint
