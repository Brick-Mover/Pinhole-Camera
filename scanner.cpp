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
#include <string.h>
#include <vector>
#define DEBUG1 0

using namespace std;


void saveImg(vector<Pixel> pic, int length, int height);
void initializeWalls(Surrounding& s);
vector<Point> lerp(const Point& start, const Point& end, int nPoints);


void eat_comment(ifstream &f)
{
    char linebuf[1024];
    char ppp;
    while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
        f.get();
    if (ppp == '#')
        f.getline(linebuf, 1023);
}


void load_ppm(Wall &img, const string &name)
{
    ifstream f(name.c_str(), ios::binary);
    if (f.fail())
    {
        cout << "Could not open file: " << name << endl;
        return;
    }
    
    // get type of file
    eat_comment(f);
    int mode = 0;
    string s;
    f >> s;
    if (s == "P3")
        mode = 3;
    else if (s == "P6")
        mode = 6;
    
    // get w
    eat_comment(f);
    f >> img.length;
    
    // get h
    eat_comment(f);
    f >> img.height;
    
    // get bits
    eat_comment(f);
    int bits = 0;
    f >> bits;
    
    // error checking
    if (mode != 3 && mode != 6)
    {
        cout << "Unsupported magic number" << endl;
        f.close();
        return;
    }
    if (img.length < 1)
    {
        cout << "Unsupported width: " << img.length << endl;
        f.close();
        return;
    }
    if (img.height < 1)
    {
        cout << "Unsupported height: " << img.height << endl;
        f.close();
        return;
    }
    if (bits < 1 || bits > 255)
    {
        cout << "Unsupported number of bits: " << bits << endl;
        f.close();
        return;
    }
    
    // load image data
    img.pixels.resize(img.length * img.height);
    
    if (mode == 6)
    {
        f.get();
        f.read((char*)&img.pixels[0], img.pixels.size() * 3);
    }
    else if (mode == 3)
    {
        for (int i = 0; i < img.pixels.size(); i++)
        {
            int v;
            f >> v;
            img.pixels[i].R = v;
            f >> v;
            img.pixels[i].G = v;
            f >> v;
            img.pixels[i].B = v;
        }
    }
    
    // close file
    f.close();
}


int main() {
    Wall* f = new Wall(500, 500, 250, FRONT);
    Wall* b = new Wall(500, 500, 250, BACK);
    Wall* l = new Wall(500, 500, 250, LEFT);
    Wall* r = new Wall(500, 500, 250, RIGHT);
    Surrounding s = Surrounding(f, b, l, r);
    initializeWalls(s);
    Canvas* aCanvas = new Canvas(s, -M_PI/2, 200, 200, 100);
    aCanvas->render();
    cout << "size of pic is " << aCanvas->get_pixels().size() << endl;
    vector<Pixel> result = aCanvas->get_pixels();
    saveImg(result, 200, 200);
}


void initializeWalls(Surrounding& s) {
    load_ppm(*s.Front, "front.ppm");
    load_ppm(*s.Back, "back.ppm");
    load_ppm(*s.Left, "left.ppm");
    load_ppm(*s.Right, "right.ppm");
}


Canvas::Canvas(Surrounding walls, float angle, int height, int length, int near)
{
    this->viewAngle = angle;
    this->walls = walls;
    this->height = height;
    this->length = length;
    this->near = near;
    viewRange = 2*atan(length/2.0/near);
    // cout << "view range: " << viewRange << endl;
    pixels.reserve(height*length);
}


void Canvas::render() {
    assert(viewRange < M_PI && viewRange > 0);
    
    float r = length/2.0/sin(viewRange/2);
    float thetaStart = viewAngle - viewRange/2;
    float thetaEnd = viewAngle + viewRange/2;
    cout << "viewAngle: " << viewAngle << endl;
    cout << "viewRange: " << viewRange << endl;
    cout << "r: " << r << endl;
    cout << "thetaStart: " << thetaStart << endl;
    cout << "thetaEnd: " << thetaEnd << endl;
    
    for (int i = height - 1; i >= 0; i--) {
        Point start = Point(r * cos(thetaStart), r * sin(thetaStart), i);
        Point end = Point(r * cos(thetaEnd), r * sin(thetaEnd), i);
        
//        cout << "start x: " << start.x << endl;
//        cout << "start y: " << start.y << endl;
//        cout << "start z: " << start.z << endl;
//        
//        cout << "end x: " << end.x << endl;
//        cout << "end y: " << end.y << endl;
//        cout << "end z: " << end.z << endl;
        
        vector<Point> rowPoints = lerp(start, end, length);
        
        for (int j = 0; j < length; j++) {
            Point p = rowPoints[j];
            cout << "p.x " << p.x << endl;
            cout << "p.y " << p.y << endl;
            cout << "p.z " << p.z << endl;
            
            vec3 dir = vec3(p.x, p.y, p.z);
            float theta = atan(p.x / p.y);
            theta += M_PI/2;    // adjust to 0 ~ 2π
            
            
            cout << "theta " << theta << endl;
            //exit(1);
            
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
            cout << "wall type: " << w << endl;
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
                    aPixel = walls.Left->get_pixel(q);
                    break;
                }
                default:
                    break;
            }
#if DEBUG1
            cout << "color at i, j: " << aPixel.R << " " <<
                aPixel.G << " " << aPixel.B << endl;
#endif
            // convert (i,j) to a point on Canvas
            pixels.push_back(aPixel);
#if DEBUG1
            cout << "pixel[" << count - 1 << "] is set to color\n";
#endif
        }
    }
}



bool Wall::isOnWall(Point p) const {
    switch (type) {
        case FRONT:
            //cout << "check front\n";
            return abs(p.x + near) <= TOLERANCE &&
            abs(p.y) <= length/2 + TOLERANCE &&
            p.z <= height + TOLERANCE && p.z >= -TOLERANCE;
            break;
        case BACK:
            //cout << "check back\n";
            return abs(p.x - near) <= TOLERANCE &&
            abs(p.y) <= length/2 + TOLERANCE &&
            p.z <= height + TOLERANCE && p.z >= -TOLERANCE;
            break;
        case LEFT:
            //cout << "check left\n";
            return abs(p.y + near) <= TOLERANCE &&
            abs(p.x) <= length/2 + TOLERANCE &&
            p.z <= height + TOLERANCE && p.z >= -TOLERANCE;
            break;
        case RIGHT:
            //cout << "check right\n";
            return abs(p.y - near) <= TOLERANCE &&
            abs(p.x) <= length/2 + TOLERANCE &&
            p.z <= height + TOLERANCE && p.z >= -TOLERANCE;
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
        }
        //int tt = row * length + col;
        //cout << "return pixel[] " << tt << endl;
        return pixels[row * length + col];
        //return grey_pixel;
    }
}


Point& Point::operator = (Point other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}


Pixel& Pixel::operator = (Pixel other) {
    R = other.R;
    G = other.G;
    B = other.B;
    return *this;
}


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


// linear interpolate between two points
vector<Point> lerp(const Point& start, const Point& end, int nPoints) {
    vector<Point> result;
    result.reserve(nPoints);
    float deltaX = (start.x - end.x) / nPoints;
    float deltaY = (start.y - end.y) / nPoints;
    float deltaZ = (start.z - end.z) / nPoints;
    for (int i = 0; i < nPoints; i++) {
        float x = start.x + deltaX * i;
        float y = start.y + deltaY * i;
        float z = start.z + deltaZ * i;
        result.push_back(Point(x,y,z));
        //cout << "x: " << x << " y: " << y << " z: " << z << endl;
    }
    /*
    cout << "double check x: " << result[0].x << endl;
    cout << "double check y: " << result[0].y << endl;
    cout << "double check z: " << result[0].z << endl;
    */
    
    return result;
}


void saveImg(vector<Pixel> pic, int length, int height) {
    FILE *fp = fopen("result.ppm", "wb");
    (void) fprintf(fp, "P6\n%d %d\n255\n", length, height);
    int count = 0;
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < length; ++i)
        {
            static unsigned char color[3];
            color[0] = pic[count].R;  /* red */
            color[1] = pic[count].G;  /* green */
            color[2] = pic[count].B;  /* blue */
            (void) fwrite(color, 1, 3, fp);
            count++;
        }
    }
    (void) fclose(fp);
}





