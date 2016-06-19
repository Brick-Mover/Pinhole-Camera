//
//  scanner.cpp
//  Scanner
//
//  Created by SHAO Jiuru on 6/16/16.
//  Copyright © 2016 UCLA. All rights reserved.
//
#include "scanner.hpp"
#include "vecm.h"
#include "matm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void loadImg();
void savePPM(int width, int height, const char* fname, unsigned char* pixels);
void saveImg();
void initializeWalls();
vector<Point> lerp(Point start, Point end, int nPoints);

// Helper functions
vec4 toVec4(const string& s1, const string& s2, const string& s3, const string& s4);
vec3 toVec3(const string& s1, const string& s2, const string& s3);
float toFloat(const string& s);


// USAGE: ./scanner viewAngle canvasHeight canvasWidth canvasNear
int main(int argc, char** argv)
{
    Wall w = Wall(20, 20, 10, FRONT);
    Point p = w.intersect(vec3(-5,5,1), Point(-1,0,0));
    cout << "p.x: " << p.x << endl;
    cout << "p.y: " << p.y << endl;
    cout << "p.z: " << p.z << endl;
}





Canvas::Canvas(Surrounding walls, int angle, int height, int length, int near)
{
    this->viewAngle = angle;
    this->walls = walls;
    this->height = height;
    this->length = length;
    this->near = near;
    viewRange = atan(length/2.0/near);
}


void Canvas::render() {
    assert(viewAngle < M_PI && viewAngle > 0);
    int count = 0;
    float r = length/2.0/sin(viewRange/2);
    float thetaStart = viewAngle - viewRange/2 - M_PI/2;
    float thetaEnd = viewAngle + viewRange/2 - M_PI/2;
    for (int i = height - 1; i >= 0; i--) {
        Point start = Point(r * cos(thetaStart), r * sin(thetaStart), i);
        Point end = Point(r * cos(thetaEnd), r * sin(thetaEnd), i);
        vector<Point> rowPoints = lerp(start, end, length);
        
        for (int j = 0; j < length; j++) {
            Point p = rowPoints[j];
            vec3 dir = vec3(p.x, p.y, p.z);
            float theta = atan(p.x / p.y);
            theta += M_PI/2;    // adjust to 0 ~ 2π
            WALLTYPE w;
            const double cAngle = M_PI/4;
            if (theta > cAngle && theta <= 3*cAngle)
                w = BACK;
            else if (theta > 3*cAngle && theta <= 5*cAngle)
                w = LEFT;
            else if (theta > 5*cAngle && theta <= 7*cAngle)
                w = FRONT;
            else
                w = RIGHT;
            Pixel aPixel = Pixel();
            switch (w) {
                case FRONT:
                {
                    Point q = walls.Front->intersect(dir, p);
                    aPixel = walls.Front->get_pixel(q);
                    break;
                }
                case RIGHT:
                {
                    Point q = walls.Right->intersect(dir, p);
                    aPixel = walls.Right->get_pixel(q);
                    break;
                }
                case BACK:
                {
                    Point q = walls.Back->intersect(dir, p);
                    aPixel = walls.Back->get_pixel(q);
                    break;
                }
                case LEFT:
                {
                    Point q = walls.Left->intersect(dir, p);
                    aPixel = walls.Back->get_pixel(q);
                    break;
                }
                default:
                    break;
            }
            
            // convert (i,j) to a point on Canvas
            pixels[count++] = aPixel;
        }
    }
}


bool Wall::isOnWall(Point p) const {
    switch (type) {
        case FRONT:
            return abs(p.x + near) <= TOLERANCE &&
            abs(length/2 - abs(p.y)) <= TOLERANCE &&
            abs(height - p.z) <= TOLERANCE;
            break;
        case BACK:
            return abs(p.x - near) <= TOLERANCE &&
            abs(length/2 - abs(p.y)) <= TOLERANCE &&
            abs(height - p.z) <= TOLERANCE;
            break;
        case LEFT:
            return abs(p.y + near) <= TOLERANCE &&
            abs(length/2 - abs(p.x)) <= TOLERANCE &&
            abs(height - p.z) <= TOLERANCE;
            break;
        case RIGHT:
            return abs(p.y - near) <= TOLERANCE &&
            abs(length/2 - abs(p.x)) <= TOLERANCE &&
            abs(height - p.z) <= TOLERANCE;
            break;
        default:
            break;
    }
}


Point Wall::intersect(vec3 dir, Point p) const {
    // Ax+By+Cz = D intersect (x0,y0,z0)+(a,b,c)t
    // t = [D - (Ax0+By0+Cz0)] / (Aa+Bb+Cc)
    
    if ((A*dir.x + B*dir.y + C*dir.z) == 0) {
        return Point(0.0, 0.0, 0.0);
    }
    float t = (D - A*p.x - B*p.y - C*p.z) / (A*dir.x + B*dir.y + C*dir.z);
    if (t <= 0)
        return Point(0.0, 0.0, 0.0);
    else
        return Point(p.x + dir.x*t, p.y + dir.y*t, p.z + dir.z*t);
}


Pixel Wall::get_pixel(Point p) const {
    // if the point is not on the wall, fill the canvas with grey
    if (!isOnWall(p))
        return grey_pixel;
    else {
        int row = int(height - p.z);
        int col = 0;
        switch (type) {
            case FRONT:
                col = int(p.y + length/2.0);
                break;
            case BACK:
                col = int(p.y - length/2.0);
                break;
            case LEFT:
                col = int(p.x - length/2.0);
                break;
            case RIGHT:
                col = int(p.x - length/2.0);
                break;
            default:
                break;
        }
        return pixels[row * length + col];
    }
}


Pixel& Pixel::operator=(Pixel other) {
    R = other.R;
    G = other.G;
    B = other.B;
    return *this;
}


// linear interpolate between two points
vector<Point> lerp(Point start, Point end, int nPoints) {
    vector<Point> result(nPoints);
    float deltaX = (start.x - end.x) / nPoints;
    float deltaY = (start.y - end.y) / nPoints;
    float deltaZ = (start.z - end.z) / nPoints;
    for (int i = 0; i < nPoints; i++) {
        float x = start.x + deltaX * i;
        float y = start.y + deltaY * i;
        float z = start.z + deltaZ * i;
        result.push_back(Point(x,y,z));
    }
    return result;
}

                    
vec4 toVec4(const string& s1, const string& s2, const string& s3, const string& s4 = "1")
{
    stringstream ss(s1 + " " + s2 + " " + s3 + " " + s4);
    vec4 result;
    ss >> result.x >> result.y >> result.z >> result.w;
    return result;
}


vec3 toVec3(const string& s1, const string& s2, const string& s3)
{
    stringstream ss(s1 + " " + s2 + " " + s3);
    vec3 result;
    ss >> result.x >> result.y >> result.z;
    return result;
}


float toFloat(const string& s)
{
    stringstream ss(s);
    float f;
    ss >> f;
    return f;
}



