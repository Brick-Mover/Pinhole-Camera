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

const float TOLERANCE = float(1.0e-07);
enum WALLTYPE {FRONT, BACK, LEFT, RIGHT};

struct Point
{
    Point(float x0, float y0, float z0) { x = x0; y = y0; z = z0; }
    Point() { cout << "Error!"; exit(1); }
    float x;
    float y;
    float z;
};

struct Pixel
{
    Pixel(int r, int g, int b) {R = r; G = g; B = b;}
    Pixel() {}
    Pixel& operator = (Pixel other);
    int R;
    int G;
    int B;
    //int a;
};

const Pixel grey_pixel = Pixel(138, 138, 138);

struct Wall
{
    Wall(int l, int h, int n, WALLTYPE t);
    // Wall: Ax + By + Cz = D
    float A, B, C, D;
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
           int angle,
           int height,
           int length,
           int near);
    vector<Pixel> get_pixels() { return pixels; }
    void render();
    
private:
    int height;
    int length;
    int near;
    float viewAngle;
    float viewRange;
    Surrounding walls;
    vector<Pixel> pixels;
    
};


Surrounding::Surrounding(Wall* Front, Wall* Back, Wall* Left, Wall* Right)
{
    this->Front = Front;
    this->Back = Back;
    this->Left = Left;
    this->Right = Right;
}


Wall::Wall(int l, int h, int n, WALLTYPE t) {
    length = l;
    height = h;
    near = n;
    type = t;
    switch (t) {
        case FRONT:
            A = -1.0;
            B = 0.0;
            C = 0.0;
            D = near;
            break;
        case BACK:
            A = 1.0;
            B = 0.0;
            C = 0.0;
            D = near;
            break;
        case LEFT:
            A = 0.0;
            B = -1.0;
            C = 0.0;
            D = near;
            break;
        case RIGHT:
            A = 0.0;
            B = 1.0;
            C = 0.0;
            D = near;
            break;
        default:
            break;
    }
}





#endif /* scanner_h */
