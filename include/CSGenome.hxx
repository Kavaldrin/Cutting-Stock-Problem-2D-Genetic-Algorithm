
#ifndef __CSGENOME_HXX__
#define __CSGENOME_HXX__

#include <ga/ga.h>
#include <vector>
#include "utilities.hxx"

namespace cuttingStock
{

class CSGenome : public GAGenome
{
public:

    static float CSObjectiveFunction(GAGenome& );

    static void CSInitializer(GAGenome& );
    static int CSCrossover(const GAGenome&, const GAGenome&, GAGenome*, GAGenome* );
    static int CSMutator(GAGenome&, float);
    static float CSComparator(const GAGenome&, const GAGenome& );
    static float CSEvaluator(GAGenome& );

public:
    CSGenome(const CSGenome& orig);
    CSGenome& operator=(const GAGenome& orig);
    void copy(const GAGenome& orig);

    CSGenome(std::vector<Rect>& rectangles, const Rect& boardSize);
    std::vector< Gene > getGenes() const { return m_genes; }

    GAGenome* clone(GAGenome::CloneMethod) const override { return new CSGenome(*this); }

    int computeArea() const;
    int getMaxArea() const;

private:

    std::vector< Gene > m_genes;
    Rect m_boardSize;
};

}

#endif