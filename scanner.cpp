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
typedef vec3 Point;

void loadImg();
void savePPM(int width, int height, const char* fname, unsigned char* pixels);
void saveImg();
void initializeWalls();

// Helper functions
vec4 toVec4(const string& s1, const string& s2, const string& s3, const string& s4);
vec3 toVec3(const string& s1, const string& s2, const string& s3);
float toFloat(const string& s);


// USAGE: ./scanner viewAngle canvasHeight canvasWidth canvasNear
int main(int argc, char** argv)
{
    int viewAngle = stoi(argv[1]);
    int canvasHeight = stoi(argv[2]);
    int canvasWidth = stoi(argv[3]);
    int canvasNear = stoi(argv[4]);
    Wall* Wall_front = new Wall();
    Wall* Wall_back = new Wall();
    Wall* Wall_left = new Wall();
    Wall* Wall_right = new Wall();
    Surrounding* world = new Surrounding(Wall_front,
                                        Wall_back,
                                        Wall_left,
                                        Wall_right);
    Canvas* aCanvas = new Canvas(*world, viewAngle,
                                 canvasHeight,
                                 canvasWidth,
                                 canvasNear);
    vector<vec3> g_pixels = aCanvas->get_pixels();
    // saveImg
}


Canvas::Canvas(Surrounding walls, int angle, int height, int length, int near)
{
    this->viewAngle = angle;
    this->walls = walls;
    this->height = height;
    this->length = length;
    this->near = near;
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



