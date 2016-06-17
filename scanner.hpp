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
#include <vector>

using namespace std;

struct Wall {
    int length;
    int height;
    int near;
    vector<vec3> pixels;
};

struct Surrounding {
    Surrounding(Wall* Front, Wall* Back, Wall* Left, Wall* Right);
    Surrounding();
    Wall* Front;
    Wall* Back;
    Wall* Left;
    Wall* Right;
};

class Canvas {
public:
    Canvas(Surrounding walls,
           int angle,
           int height,
           int length,
           int near);
    vector<vec3> get_pixels() { return pixels; }
    
private:
    int height;
    int length;
    int near;
    int viewAngle;
    Surrounding walls;
    vector<vec3> pixels;
};




Surrounding::Surrounding(Wall* Front, Wall* Back, Wall* Left, Wall* Right) {
    this->Front = Front;
    this->Back = Back;
    this->Left = Left;
    this->Right = Right;
}




#endif /* scanner_h */
