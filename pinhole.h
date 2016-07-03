//
//  pinhole.h
//  Pinhole Camera
//
//  Created by SHAO Jiuru on 6/16/16.
//  Copyright © 2016 UCLA. All rights reserved.
//

#ifndef pinhole_h
#define pinhole_h

#include <assert.h>
#include <iostream>
#include <vector>
#include "matm.h"

using namespace std;

const float TOLERANCE = float(1.0e-04);
enum WALLTYPE {yPos, yNeg, xNeg, xPos, zPos, zNeg};



struct Point
{
    Point(float x0, float y0, float z0) { x = x0; y = y0; z = z0; }
    Point& operator = (Point other);
    bool operator == (Point other);
    float x;
    float y;
    float z;
    void print() {
        cout << "p.x: " << x << endl;
        cout << "p.y: " << y << endl;
        cout << "p.z: " << z << endl;
        cout << endl;
    }
};

typedef Point Vec3;

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
const Point origin = Point(0.0, 0.0, 0.0);


struct Wall
{
    Wall(int l, int h, int n, float rl, float rh, WALLTYPE t);
    // Wall: Ax + By + Cz = D
    float A, B, C;
    int D;
    int xDim;
    int yDim;
    int zDim;
    int near;
    // resolution of the picture
    float length;
    float height;
    
    WALLTYPE type;
    vector<Pixel> pixels;

    // detect if a given point is on the wall
    bool isOnWall(Point p) const;
    // get the closest pixel to the point on the wall
    Pixel get_pixel(Point p) const;
    // get the intersection point(with a vector in parametrized form)
    Point intersect(Vec3 dir, Point p) const;
    bool getIntersection(Vec3 dir, Point cameraPos, Point& intersection) const;
    void print() const;
    void print_pic(int pos) const;

};


struct Surrounding
{
    Surrounding(Wall* Front, Wall* Back, Wall* Left, Wall* Right, Wall* Ceil, Wall* Floor);
    Surrounding() {};
    Wall* yPos;
    Wall* yNeg;
    Wall* xPos;
    Wall* xNeg;
    Wall* zPos;
    Wall* zNeg;
};


class Pinhole
{
public:
    Pinhole(float aperture,
            float hAngle,
            float vAngle,
            Point cameraPos,
            float cameraDist,
            Surrounding walls,
            int xDim, int yDim,
            int xLen, int yLen);
    vector<Pixel> getImage() const { return imagePlane; }
    void render();
    void renderPixel(int x, int y);
    void setColor(int x, int y, Pixel color);
    
private:
    float aperture;     // range of view
    Point cameraPos;
    float cameraDist;     // distance of image plane
    Surrounding walls;
    int xDim, yDim;     // resolution of image plane
    int xLen, yLen;     // size of image plane
    float pixelSize;
    mat3 rotationM_H;     // rotation around z axis
    mat3 rotationM_V;     // rotation around x axis
    vector<Pixel> imagePlane; // the final image produced
};

#endif /* pinhole_h */
