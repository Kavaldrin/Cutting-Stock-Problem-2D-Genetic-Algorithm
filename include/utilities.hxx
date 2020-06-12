#ifndef __UTILITIES_HXX__
#define __UTILITIES_HXX__

#include <functional>
#include <ga/ga.h>
#include <ga/GARealGenome.h>
#include <memory>

namespace genetic_utilities
{

struct Rect
{
    int width;
    int height;
    Rect& operator=(const Rect& ) = default;
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



class ReadWriteManager
{

public:
    static std::vector<Rect> readInputFile(const std::string& path);
    static void saveOutputFile(const std::vector<Gene>& result, const std::string& path);

private:
    ReadWriteManager() = delete;
};


class CSGenome : public GAGenome
{
public:

    static float CSObjectiveFunction(GAGenome& );

    static void CSInitializer(GAGenome& );
    static int CSCrossover(const GAGenome&, const GAGenome&, GAGenome*, GAGenome* );
    static int CSMutator(GAGenome&, float);
    static float CSComparator(const GAGenome&, const GAGenome& );
    static float CSEvaluator(GAGenome& );
    static float CSEvaluatorWithoutPenalties(GAGenome& );

public:
    CSGenome(const CSGenome& orig);
    CSGenome& operator=(const GAGenome& orig);
    void copy(const GAGenome& orig);

    CSGenome(std::vector<Rect>& rectangles, const Rect& boardSize);
    std::vector< Gene > getGenes() const { return m_genes; }

    GAGenome* clone(GAGenome::CloneMethod) const override { return new CSGenome(*this); }

private:
    std::vector< Gene > m_genes;
    Rect m_boardSize;
};






}


#endif