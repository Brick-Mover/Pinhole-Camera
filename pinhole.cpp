//
//  camera.cpp
//  Fisheye Camera
//
//  Created by SHAO Jiuru on 6/25/16.
//  Copyright © 2016 UCLA. All rights reserved.
//


#include "pinhole.h"
#include "matm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <assert.h>
#include <math.h>


using namespace std;


void eat_comment(ifstream &f);
void load_ppm(Wall* img, const string &name);
void saveImg(vector<Pixel> pic, int length, int height);
void initializeWalls(const Surrounding& s);


int main(int argc, const char * argv[])
{
    Wall* fr = new Wall(500, 500, 250, 500, 500, yPos);
    Wall* ba = new Wall(500, 500, 250, 500, 500, yNeg);
    Wall* ri = new Wall(500, 500, 250, 500, 500, xPos);
    Wall* le = new Wall(500, 500, 250, 500, 500, xNeg);
    Wall* ce = new Wall(500, 500, 500, 500, 500, zPos);
    Wall* fl = new Wall(500, 500, 0  , 500, 500, zNeg);
    
    Surrounding s = Surrounding(fr, ba, le, ri, ce, fl);
    initializeWalls(s);
    
    Pinhole* f = new Pinhole(M_PI_2,0,0,Point(0,0,250),50,s,300,300,200,200);
    f->render();
    vector<Pixel> result = f->getImage();
    saveImg(result, 300, 300);
}


Pinhole::Pinhole(float aperture,
                 float hAngle,
                 float vAngle,
                 Point cPos,
                 float cDist,
                 Surrounding walls,
                 int xDim, int yDim,
                 int xLen, int yLen): cameraPos(cPos)
{
    assert(0 < aperture < M_PI);
    assert(float(xDim)/yDim == float(xLen)/yLen);
    this->aperture = aperture;
    this->walls = walls;
    this->xDim = xDim;
    this->yDim = yDim;
    this->xLen = xLen;
    this->yLen = yLen;
    this->cameraDist = cDist;
    this->pixelSize = (float)xLen / xDim;
    imagePlane.reserve(yDim*xDim);
    rotationM_H = mat3(
                       vec3(cosf(hAngle), -sinf(hAngle), 0),
                       vec3(sinf(hAngle),  cosf(hAngle), 0),
                       vec3(0.0, 0.0, 1.0)
                       );
    rotationM_V = mat3(
                       vec3(cosf(-vAngle), 0.0, sinf(-vAngle)),
                       vec3(0.0, 1.0, 0.0),
                       vec3(-sinf(-vAngle), 0, cosf(-vAngle))
                       );
    
    
}


void Pinhole::render()
{
    // On view plane, render each pixel
    for (int r = 0; r < yDim; r++) {
        for (int c = 0; c < xDim; c++) {
            renderPixel(r, c);
        }
    }
}


void Pinhole::renderPixel(int row, int col)
{
    float y = -(-xLen/2.0 + pixelSize/2 + pixelSize*col);
    float z = yLen/2.0 - pixelSize/2 - pixelSize*row;
    float x = cameraDist;
    
    
    vec3 rp = rotationM_V * rotationM_H * vec3(x,y,z);
    Vec3 dir = Vec3(rp.x, rp.y, rp.z);
    
//    cout << "\n" << pixelSize << endl;
//    dir.print();
    
    
    Point intersection = Point(0,0,0);
    Wall* w;
    
    if (walls.yPos->getIntersection(dir, cameraPos, intersection) == true)
        w = walls.yPos;
    else if (walls.yNeg->getIntersection(dir, cameraPos, intersection) == true)
        w = walls.yNeg;
    else if (walls.xNeg->getIntersection(dir, cameraPos, intersection) == true)
        w = walls.xNeg;
    else if (walls.xPos->getIntersection(dir, cameraPos, intersection) == true)
        w = walls.xPos;
    else if (walls.zPos->getIntersection(dir, cameraPos, intersection) == true)
        w = walls.zPos;
    else {
        assert(walls.zNeg->getIntersection(dir, cameraPos, intersection) == true);
        w = walls.zNeg;
    }
    
//        w->print();
//        cout << "intersection: " << endl;
//        intersection.print();
    Pixel color = w->get_pixel(intersection);
//        color.print();
    setColor(row, col, color);
    
    
}


void Wall::print_pic(int pos) const
{
    pixels[pos].print();
}


bool Wall::getIntersection(Vec3 dir, Point cameraPos, Point& intersection) const
{
    Point p = intersect(dir, cameraPos);
    if (p == origin || !isOnWall(p))
        return false;
    else {
        intersection = p;
        return true;
    }
}


/* returns origin if there's no collsion(!!!NEED CHANGE LATER)*/
Point Wall::intersect(Vec3 dir, Point p) const
{
    // Ax+By+Cz = D intersect (x0,y0,z0)+(a,b,c)t
    // t = [D - (Ax0+By0+Cz0)] / (Aa+Bb+Cc)
    
    if ((A*dir.x + B*dir.y + C*dir.z) == 0)
        return Point(0,0,0);
    
    float t = (D - A*p.x - B*p.y - C*p.z) / (A*dir.x + B*dir.y + C*dir.z);
    
    if (t <= 0)
        return Point(0,0,0);
    else
        return Point(p.x+dir.x*t, p.y+dir.y*t, p.z+dir.z*t);
}


Pixel Wall::get_pixel(Point p) const
{
    // shouldn't happen!
    if (!isOnWall(p)) {
        cout << "!is not on wall!\n";
        exit(1);
    }
    
    int row, col;
    float pixelSize;
    switch (type) {
        case yPos:
            pixelSize = zDim/height;
            row = int((height - p.z)/pixelSize);
            col = int((p.x + xDim/2.0)/pixelSize);
            break;
        case yNeg:
            pixelSize = zDim/height;
            row = int((height - p.z)/pixelSize);
            col = int((-p.x + xDim/2.0)/pixelSize);
            break;
        case xPos:
            pixelSize = zDim/height;
            row = int((height - p.z)/pixelSize);
            col = int((-p.y + yDim/2.0)/pixelSize);
            break;
        case xNeg:
            pixelSize = zDim/height;
            row = int((height - p.z)/pixelSize);
            col = int((p.y + yDim/2.0)/pixelSize);
            break;
        case zPos:
        case zNeg:
            pixelSize = xDim/length;
            row = int((yDim/2.0 - p.y)/pixelSize);
            col = int((p.x + xDim/2.0)/pixelSize);
            break;
    }
    //    cout << "row: " << row << endl;
    //    cout << "col: " << col << endl;
    //    assert(0 <= row*length+col <= length*height-1);
    
    //    cout << "pixel pos: " << row*length+col << endl;
    //    cout << "(row: " << row << " col: " << col << ")\n";
    return pixels[row*length+col];
}


bool Wall::isOnWall(Point p) const
{
    switch (type) {
        case yPos:
            //cout << "check front\n";
            return fabsf(p.y - near) <= TOLERANCE &&
            fabsf(p.x) <= xDim/2 + TOLERANCE &&
            p.z <= zDim + TOLERANCE && p.z >= -TOLERANCE;
            break;
        case yNeg:
            //cout << "check back\n";
            return fabsf(p.y + near) <= TOLERANCE &&
            fabsf(p.x) <= xDim/2 + TOLERANCE &&
            p.z <= zDim + TOLERANCE && p.z >= -TOLERANCE;
            break;
        case xNeg:
            //cout << "check left\n";
            return fabsf(p.x + near) <= TOLERANCE &&
            fabsf(p.y) <= yDim/2 + TOLERANCE &&
            p.z <= zDim + TOLERANCE && p.z >= -TOLERANCE;
            break;
        case xPos:
            //cout << "check right\n";
            return fabsf(p.x - near) <= TOLERANCE &&
            fabsf(p.y) <= yDim/2 + TOLERANCE &&
            p.z <= zDim + TOLERANCE && p.z >= -TOLERANCE;
            break;
        case zPos:
        case zNeg:
            return fabsf(p.z - near) <= TOLERANCE &&
            fabsf(p.x) <= xDim/2 + TOLERANCE &&
            fabsf(p.y) <= yDim/2 + TOLERANCE;
            break;
    }
}


void Pinhole::setColor(int row, int col, Pixel color)
{
    assert(0 <= row*xDim+col <= xDim*yDim-1);
    imagePlane.push_back(color);
}


Wall::Wall(int l, int h, int n, float rl, float rh, WALLTYPE t)
{
    near = n;
    type = t;
    assert(float(l)/float(h) == float(rl)/float(rh));
    // Ax + By + Cz = D
    switch (t) {
        case yPos:
            A = 0.0;
            B = 1.0;
            C = 0.0;
            D = near;
            xDim = l;
            yDim = 0;
            zDim = h;
            break;
        case yNeg:
            A = 0.0;
            B = -1.0;
            C = 0.0;
            D = near;
            xDim = l;
            yDim = 0;
            zDim = h;
            break;
        case xNeg:
            A = -1.0;
            B = 0.0;
            C = 0.0;
            D = near;
            xDim = 0;
            yDim = l;
            zDim = h;
            break;
        case xPos:
            A = 1.0;
            B = 0.0;
            C = 0.0;
            D = near;
            xDim = 0;
            yDim = l;
            zDim = h;
            break;
        case zPos:
            A = 0.0;
            B = 0.0;
            C = 1.0;
            D = near;
            xDim = l;
            yDim = h;
            zDim = 0;
            break;
        case zNeg:
            A = 0.0;
            B = 0.0;
            C = 1.0;
            D = 0;
            assert(near == 0);
            xDim = l;
            yDim = h;
            zDim = 0;
            break;
    }
}


/* trivial member functions such as assign overloading and constructors */
Point& Point::operator = (Point other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}


bool Point::operator == (Point other)
{
    return other.x == this->x &&
    other.y == this->y &&
    other.z == this->z;
}


Pixel& Pixel::operator = (Pixel other)
{
    R = other.R;
    G = other.G;
    B = other.B;
    return *this;
}


Surrounding::Surrounding(Wall* Front, Wall* Back, Wall* Left, Wall* Right, Wall* Ceil, Wall* Floor)
{
    this->yPos = Front;
    this->yNeg = Back;
    this->xNeg = Left;
    this->xPos = Right;
    this->zPos = Ceil;
    this->zNeg = Floor;
}


void Wall::print() const
{
    switch (type) {
        case yPos:
            cout << "YPOS\n";
            break;
        case yNeg:
            cout << "YNEG\n";
            break;
        case xPos:
            cout << "XPOS\n";
            break;
        case xNeg:
            cout << "XNEG\n";
            break;
        case zPos:
            cout << "ZPOS\n";
            break;
        case zNeg:
            cout << "ZNEG\n";
            break;
    }
}


/* BELOW: load and save images */
void saveImg(vector<Pixel> pic, int length, int height)
{
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


void initializeWalls(const Surrounding& s)
{
    load_ppm(s.yPos, "front.ppm");
    load_ppm(s.yNeg, "back.ppm");
    load_ppm(s.xNeg, "left.ppm");
    load_ppm(s.xPos, "right.ppm");
    load_ppm(s.zPos, "ceil.ppm");
    load_ppm(s.zNeg, "floor.ppm");
}


void eat_comment(ifstream &f)
{
    char linebuf[1024];
    char ppp;
    while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
        f.get();
    if (ppp == '#')
        f.getline(linebuf, 1023);
}


void load_ppm(Wall* img, const string &name)
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
    f >> img->length;
    
    // get h
    eat_comment(f);
    f >> img->height;
    
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
    if (img->length < 1)
    {
        cout << "Unsupported width: " << img->length << endl;
        f.close();
        return;
    }
    if (img->height < 1)
    {
        cout << "Unsupported height: " << img->height << endl;
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
    img->pixels.resize(img->length * img->height);
    
    if (mode == 6)
    {
        f.get();
        f.read((char*)&img->pixels[0], img->pixels.size() * 3);
    }
    else if (mode == 3)
    {
        for (int i = 0; i < img->pixels.size(); i++)
        {
            int v;
            f >> v;
            img->pixels[i].R = v;
            f >> v;
            img->pixels[i].G = v;
            f >> v;
            img->pixels[i].B = v;
        }
    }
    
    // close file
    f.close();
}
