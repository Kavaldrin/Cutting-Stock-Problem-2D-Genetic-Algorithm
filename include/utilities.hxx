#ifndef __UTILITIES_HXX__
#define __UTILITIES_HXX__


namespace cuttingStock
{

struct Rect
{
    int width;
    int height;
    Rect& operator=(const Rect& ) = default;
    int area() const { return width * height; }
};

struct Gene
{
    Rect rect;
    int x;
    int y;
    bool rotated;
    bool exists;
    
    friend bool operator==(const Gene& lhs, const Gene& rhs)
    {
        return lhs.rect.width == rhs.rect.width &&
        lhs.rect.height == rhs.rect.height &&
        lhs.x == rhs.x &&
        lhs.y == rhs.y &&
        lhs.rotated == rhs.rotated &&
        lhs.exists == rhs.exists;
    }
};




}


#endif