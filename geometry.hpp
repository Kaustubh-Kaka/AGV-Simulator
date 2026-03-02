#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <complex>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include "raylib.h"
#include "raymath.h"

using namespace std;
using namespace std::chrono;

// constants and defines

#define all(x) (x).begin(), (x).end()

using ftype = float;
const ftype eps = 1e-7, mdist = 1e9;

struct point {
    ftype x, y;

    point() { x = 0, y = 0; }
    point(ftype xx, ftype yy) { x = xx, y = yy; }
    point(const complex<ftype>& a) { x = a.real(), y = a.imag(); }

    operator Vector2() { return {(float)x, (float)y}; }
    operator complex<ftype>() { return {x, y}; }

    const ftype operator[](int i) const {
        if (i == 0)
            return x;
        else
            return y;
    }

    ftype& operator[](int i) {
        if (i == 0)
            return x;
        else
            return y;
    }
};

using shape = vector<point>;
using segment = pair<point, point>;
using triangle = array<point, 3>;

// declerations

ostream& operator<<(ostream& out, const point& a);
point operator+(const point& a, const point& b);
point operator-(const point& a, const point& b);
point operator*(const ftype& scalar, const point& a);
point rotate(const point& a, const point& about, const ftype& angle);
ftype crossp(const point& a, const point& b);
int orientation(const point& a, const point& b, const point& c);
ftype arg(const point& a);
ftype dist(const point& a, const point& b = {0, 0});
ostream& operator<<(ostream& out, const segment& a);
ostream& operator<<(ostream& out, const vector<point>& a);
vector<point> convexhull(vector<point> a);
vector<point> operator*(const ftype& scalar, const vector<point>& a);
vector<point> operator+(const point& vec, const vector<point>& a);
vector<point> operator+(const vector<point>& a, const point& vec);
vector<point> operator-(const vector<point>& a, const point& vec);
vector<point> rotate(const vector<point>& a, const point& about,
                     const ftype& angle);
vector<point> minkowskisum(const vector<point>& a, const vector<point>& b);
vector<point> operator+(const vector<point>& a, const vector<point>& b);
vector<point> operator-(const vector<point>& a, const vector<point>& b);
bool pointlocation(const segment& a, const point& p);
bool insidetriangle(const triangle& a, const point& p);
bool contains(const vector<point>& a, const point& p);
bool intersect(const vector<point>& a, const vector<point>& b);
point raycast(const point& p, const segment& a, const ftype& angle);
point raycast(const point& p, const vector<point>& a, const ftype& angle);
point raycast(const point& p, const vector<vector<point>>& a,
              const ftype& angle);

// implementations

ostream& operator<<(ostream& out, const point& a) {
    out << '(' << a.x << ' ' << a.y << ')';
    return out;
}

point operator+(const point& a, const point& b) {
    return point(a.x + b.x, a.y + b.y);
}

point operator-(const point& a, const point& b) {
    return point(a.x - b.x, a.y - b.y);
}

point operator*(const ftype& scalar, const point& a) {
    return point(scalar * a.x, scalar * a.y);
}

point rotate(const point& a, const point& about, const ftype& angle) {
    return about +
           point((a.x - about.x) * cos(angle) - (a.y - about.y) * sin(angle),
                 (a.x - about.x) * sin(angle) + (a.y - about.y) * cos(angle));
}

ftype crossp(const point& a, const point& b) { return a.x * b.y - b.x * a.y; }

int orientation(const point& a, const point& b, const point& c) {
    if (abs(crossp(b - a, c - b)) < eps)
        return 0;
    else if (crossp(b - a, c - b) > 0)
        return 1;
    else
        return -1;
}

ftype arg(const point& a) { return atan2(a.y, a.x); }

ftype dist(const point& a, const point& b) {
    return hypot(a.x - b.x, a.y - b.y);
}

ostream& operator<<(ostream& out, const segment& a) {
    out << a.first << ' ' << a.second;
    return out;
}

ostream& operator<<(ostream& out, const vector<point>& a) {
    out << a.size() << '\n';
    for (int i = 0; i < a.size(); i++) cout << a[i] << "\n "[i < a.size() - 1];
    return out;
}

vector<point> convexhull(vector<point> a) {
    point p = *min_element(all(a), [](const point& p, const point& q) {
        if (abs(p.x - q.x) < eps) return p.y < q.y;
        return p.x < q.x;
    });

    vector<point> hull;
    hull.push_back(p);

    while (hull.size() < a.size()) {
        point nt;
        for (int i = 0; i < a.size(); i++)
            if (a[i] != p) {
                nt = a[i];
                break;
            }
        for (int i = 1; i < a.size(); i++)
            if (crossp(nt - p, a[i] - p) > 0) nt = a[i];
        hull.push_back(nt), p = nt;
        if (nt == hull[0]) break;
    }

    return hull;

}  // correct except shouldn't need break condition in while loop, some
   // random bug that I can't be bothered to fix, optimise to O(nlogn) with
   // graham's scan or maybe quickhull graham bad because precision nonsense

vector<point> operator*(const ftype& scalar, const vector<point>& a) {
    vector<point> c = a;
    for (int i = 0; i < a.size(); i++) c[i] = scalar * c[i];
    return c;
}

vector<point> operator+(const point& vec, const vector<point>& a) {
    vector<point> c = a;
    for (int i = 0; i < a.size(); i++) c[i] = c[i] + vec;
    return c;
}

vector<point> operator+(const vector<point>& a, const point& vec) {
    return vec + a;
}

vector<point> operator-(const vector<point>& a, const point& vec) {
    return a + -1 * vec;
}

vector<point> rotate(const vector<point>& a, const point& about,
                     const ftype& angle) {
    vector<point> res;
    res.reserve(a.size());
    for (int i = 0; i < a.size(); i++)
        res.push_back(rotate(a[i], about, angle));
    return res;
}

vector<point> minkowskisum(const vector<point>& a, const vector<point>& b) {
    vector<point> c;
    c.reserve(a.size() * b.size());
    for (int i = 0; i < a.size(); i++)
        for (int j = 0; j < b.size(); j++) c.push_back(a[i] + b[j]);

    return convexhull(c);
}  // optimise to nlogn

vector<point> operator+(const vector<point>& a, const vector<point>& b) {
    return minkowskisum(a, b);
}

vector<point> operator-(const vector<point>& a, const vector<point>& b) {
    return minkowskisum(a, -1 * b);
}

bool pointlocation(const segment& a, const point& p) {
    return crossp(p - a.first, p - a.second) < 0;
}

bool insidetriangle(const triangle& a, const point& p) {
    for (int i = 0; i < 3; i++)
        if (pointlocation({a[i], a[(i + 1) % 3]}, p) ^
            pointlocation({a[i], a[(i + 1) % 3]}, a[(i + 2) % 3]))
            return false;
    return true;
}

bool contains(const vector<point>& a, const point& p) {
    const int n = a.size();

    for (int i = 1; i < n - 1; i++)
        if (insidetriangle({a[0], a[i], a[i + 1]}, p)) return true;
    return false;
}  // optimise to logn

bool intersect(const vector<point>& a, const vector<point>& b) {
    return contains(a - b, {0, 0});
}  // do GJK

point raycast(const point& p, const segment& s, const ftype& angle) {
    point a = rotate(s.first - p, {0, 0}, -angle),
          b = rotate(s.second - p, {0, 0}, -angle);
    ftype crossingpoint = (b.y * a.x - a.y * b.x) / (b.y - a.y);
    if (crossingpoint >= 0 && crossingpoint <= max(a.x, b.x) &&
        crossingpoint >= min(a.x, b.x))
        return p + rotate({crossingpoint, 0}, {0, 0}, angle);
    else
        return mdist * point(cos(angle), sin(angle));
}

point raycast(const point& p, const vector<point>& a, const ftype& angle) {
    point ans = raycast(p, segment(a[0], a[1]), angle);
    for (int i = 1; i < a.size(); i++) {
        point temp = raycast(p, segment(a[i], a[(i + 1) % a.size()]), angle);
        if (dist(p, temp) < dist(p, ans)) ans = temp;
    }
    return ans;
}

point raycast(const point& p, const vector<vector<point>>& a,
              const ftype& angle) {
    point ans = raycast(p, a[0], angle);
    for (int i = 1; i < a.size(); i++) {
        point temp = raycast(p, a[i], angle);
        if (dist(p, temp) < dist(p, ans)) ans = temp;
    }
    return ans;
}