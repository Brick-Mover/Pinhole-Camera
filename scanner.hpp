//
//  scanner.hpp
//  Scanner
//
//  Created by SHAO Jiuru on 6/16/16.
//  Copyright © 2016 UCLA. All rights reserved.
//

#ifndef scanner_h
#define scanner_h
#include "vecm.h"
#include "matm.h"
#include <assert.h>
#include <vector>

using namespace std;

const float TOLERANCE = float(1.0e-03);
enum WALLTYPE {FRONT, BACK, LEFT, RIGHT};

struct Point
{
    Point(float x0, float y0, float z0) { x = x0; y = y0; z = z0; }
    Point() { /* cout << "Error!"; exit(1); */ }
    Point& operator = (Point other);
    float x;
    float y;
    float z;
    void print() {
        cout << "p.x: " << x << endl;
        cout << "p.y: " << y << endl;
        cout << "p.z: " << z << endl;
    }
};

struct Pixel
{
    Pixel(int r, int g, int b) {R = r; G = g; B = b;}
    Pixel() {}
    Pixel& operator = (Pixel other);
    unsigned char R;
    unsigned char G;
    unsigned char B;
    //int a;
    void print() const {
        cout << "p.R: " << int(R) << endl;
        cout << "p.G: " << int(G) << endl;
        cout << "p.B: " << int(B) << endl;
    }
};

const Pixel grey_pixel = Pixel(128, 128, 128);

struct Wall
{
    Wall(int l, int h, int n, WALLTYPE t);
    // Wall: Ax + By + Cz = D
    float A, B, C;
    int D;
    int length;
    int height;
    int near;
    WALLTYPE type;
    vector<Pixel> pixels;

    
    // detect if a given point is on the wall
    bool isOnWall(Point p) const;
    // get the closest pixel to the point on the wall
    Pixel get_pixel(Point p) const;
    // get the intersection point(with a vector in parametrized form)
    Point intersect(vec3 dir, Point p) const;

};

struct Surrounding
{
    Surrounding(Wall* Front, Wall* Back, Wall* Left, Wall* Right);
    Surrounding() {};
    Wall* Front;
    Wall* Back;
    Wall* Left;
    Wall* Right;
};

class Canvas
{
public:
    Canvas(Surrounding walls,
           float angle,
           int height,
           int length,
           int near);
    vector<Pixel> get_pixels() const { return pixels; }
    void render();
    void renderPixel(int x, int y);
    void setColor(int x, int y, Pixel& color);
    
private:
    int height;
    int length;
    int near;
    float viewAngle;
    float viewRange;
    Surrounding walls;
    vector<Pixel> pixels;
};




#endif /* scanner_h */
