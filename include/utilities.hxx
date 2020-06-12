#ifndef __UTILITIES_HXX__
#define __UTILITIES_HXX__

#include <utility>

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

    int getFixedWidth() const { return rotated ? rect.height : rect.width; }
    int getFixedHeight() const { return rotated ? rect.width : rect.height; }

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

class GeneHelper
{
public:

    static bool checkForCollision(const Gene& geneL, const Gene& geneR);
    static bool isOutside(const Gene& gene);

    static Rect boardSize;
private:
    GeneHelper() = delete;

};



}


#endif