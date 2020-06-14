#include <random>
#include "CSGenome.hxx"
#include <set>
#include <algorithm>

using namespace cuttingStock;

CSGenome::CSGenome(std::vector<Rect>& rectangles, const Rect& boardSize) : m_boardSize(boardSize)
{
    m_genes.resize(rectangles.size());
    for(unsigned idx = 0; idx < m_genes.size(); ++idx)
    {
        m_genes[idx].rect = rectangles[idx];
    }

    initializer(CSInitializer);
    crossover(CSCrossover);
    mutator(CSMutator);
    comparator(CSComparator);
    evaluator(CSEvaluator);
}

void CSGenome::CSInitializer(GAGenome& genome)
{
    CSGenome& castedGenome = dynamic_cast<CSGenome&>(genome);

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> realDist(0,1);
    static std::uniform_int_distribution<> indexDist(0, castedGenome.m_genes.size() - 1);
    static std::uniform_int_distribution<> widthDist(0, castedGenome.m_boardSize.width - 1);
    static std::uniform_int_distribution<> heightDist(0, castedGenome.m_boardSize.height - 1);

    for(auto& gene : castedGenome.m_genes)
    {
        gene.exists = false;
        gene.x = widthDist(gen);
        gene.y = heightDist(gen);
        gene.rotated = realDist(gen) > 0.5 ? true : false;
    }

    auto chosenRect = indexDist(gen);
    castedGenome.m_genes[chosenRect].exists = true;
    castedGenome.m_genes[chosenRect].rotated = realDist(gen) > 0.5 ? true : false;
    castedGenome.m_genes[chosenRect].x = 0;
    castedGenome.m_genes[chosenRect].y = 0;

    castedGenome._evaluated = gaFalse;
}

int CSGenome::CSCrossover(const GAGenome& mother, const GAGenome& father, GAGenome* bro, GAGenome* sis)
{
    const CSGenome& castedMother = dynamic_cast<const CSGenome&>(mother);
    const CSGenome& castedFather = dynamic_cast<const CSGenome&>(father);

    static int setSize = castedFather.m_genes.size();
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> cutDist(1, setSize - 1);

    int nc = 0;

    int partition = cutDist(gen);

    if(bro)
    {
        CSGenome* castedBro = dynamic_cast<CSGenome*>(bro);
        for(int idx = 0; idx < partition; ++idx)
        {
            castedBro->m_genes[idx] = castedMother.m_genes[idx];
        }
        for(int idx = partition; idx < setSize; ++idx)
        {
            castedBro->m_genes[idx] = castedFather.m_genes[idx];
        }
        ++nc;
        castedBro->_evaluated = gaFalse;
    }

    if(sis)
    {
        CSGenome* castedSis = dynamic_cast<CSGenome*>(sis);
        for(int idx = 0; idx < partition; ++idx)
        {
            castedSis->m_genes[idx] = castedFather.m_genes[idx];
        }
        for(int idx = partition; idx < setSize; ++idx)
        {
            castedSis->m_genes[idx] = castedMother.m_genes[idx];
        }
        ++nc;
        castedSis->_evaluated = gaFalse;
    }

    
    return nc;
}

int CSGenome::CSMutator(GAGenome& genome, float pMut)
{
    static std::random_device rnd;
    static std::mt19937 gen(rnd());
    static std::uniform_real_distribution<> dist;
    static std::normal_distribution<> stepDist(0, 100);

    int nc = 0;
    CSGenome& castedGenome = dynamic_cast<CSGenome&>(genome);
    for(auto& gene : castedGenome.m_genes)
    {
        if(pMut > dist(gen))
        {
            gene.x = (gene.x + static_cast<int>(std::round(stepDist(gen)))) % castedGenome.m_boardSize.width;
            gene.y = (gene.y + static_cast<int>(std::round(stepDist(gen)))) % castedGenome.m_boardSize.height;
            if(gene.x < 0) { gene.x = 0; }
            if(gene.y < 0) { gene.y = 0; }

            if(0.5 > dist(gen))
            {
                gene.exists = !gene.exists;
            }

            if(0.5 > dist(gen))
            {
                gene.rotated = !gene.rotated;
            }
            ++nc;
        }
    }

    if(0.5 > dist(gen))
    {

        auto& genes = castedGenome.m_genes;

        //move left

        std::vector<std::pair<int, Gene> > leftClosest = 
            GeneHelper::prepareClosestExistingSet(genes, 
                [](const auto& lhs, const auto& rhs){
                    return lhs.second.x < rhs.second.x;      
            });

        for(unsigned i = 0; i < leftClosest.size(); ++i)
        {

            std::set<int> possibleXes;
            possibleXes.insert(0);
            for(auto& gene : leftClosest)
            {
                possibleXes.insert(gene.second.x + gene.second.getFixedWidth());
            }
            
            for(const auto& possibleX : possibleXes)
            {
                auto isColision = false;
                for(unsigned j = 0; j < leftClosest.size(); ++j)
                {
                    if(i == j){ continue; } 

                    Gene tempGene = leftClosest[i].second;
                    tempGene.x = possibleX;       

                    if(GeneHelper::checkForCollision(tempGene, leftClosest[j].second) 
                        || GeneHelper::isOutside(tempGene))
                    {
                        isColision = true;
                        break;
                    }
                }
                if(!isColision)
                {
                    leftClosest[i].second.x = possibleX;
                    break;
                }
            }
        }

        for(const auto& [index, gene] : leftClosest)
        {
            castedGenome.m_genes[index].x = gene.x;
        }
       
        //move up

        std::vector<std::pair<int, Gene> > upperClosest = 
        GeneHelper::prepareClosestExistingSet(genes, 
                [](const auto& lhs, const auto& rhs){
                    return lhs.second.y < rhs.second.y;      
            });

        for(unsigned i = 0; i < upperClosest.size(); ++i)
        {
            std::set<int> possibleYes;
            possibleYes.insert(0);
            for(auto& gene : upperClosest)
            {
                possibleYes.insert(gene.second.y + gene.second.getFixedHeight());
            }
            
            for(const auto& possibleY : possibleYes)
            {
                auto isColision = false;
                for(unsigned j = 0; j < upperClosest.size(); ++j)
                {
                    if(i == j){ continue; }

                    Gene tempGene = upperClosest[i].second;
                    tempGene.y = possibleY;

                    if(GeneHelper::checkForCollision(tempGene, upperClosest[j].second) 
                        || GeneHelper::isOutside(tempGene))
                    {
                        isColision = true;
                        break;
                    }
                }
                if(!isColision)
                {
                    upperClosest[i].second.y = possibleY;
                    break;
                }
            }
        }

        for(const auto& [index, gene] : upperClosest)
        {
            castedGenome.m_genes[index].y = gene.y;
        }
    }

    castedGenome._evaluated = gaFalse;
    return nc;
}

float CSGenome::CSComparator(const GAGenome& bro, const GAGenome& sis)
{
    
    const CSGenome& castedBro = dynamic_cast<const CSGenome&>(bro);
    const CSGenome& castedSis = dynamic_cast<const CSGenome&>(sis);

    static int sizeSet = castedBro.m_genes.size();
    int theSame = 0;

    for(int idx = 0; idx < sizeSet; ++idx)
    {
        if(castedBro.m_genes[idx] == castedSis.m_genes[idx]){ ++theSame; }
    }
    
    return theSame / static_cast<float>(sizeSet); 
}


float CSGenome::CSEvaluator(GAGenome& genome)
{
    CSGenome& castedGenome = dynamic_cast<CSGenome&>(genome);
    static int max_area = castedGenome.m_boardSize.area();
    int area = 0;

    auto& gens = castedGenome.m_genes;
    for(unsigned i = 0; i < gens.size(); ++i)
    {
        if(! gens[i].exists ){ continue; }
        int current_area = gens[i].rect.area();

        if(GeneHelper::isOutside(gens[i]))
        {
            return 0;
        }

        for(unsigned j = 0; j < gens.size(); ++j)
        {
            if(j == i) { continue; }
            if(! gens[j].exists){ continue; }

            if(GeneHelper::checkForCollision(gens[i], gens[j]))
            {
                return 0;
            }
        }
        area += current_area;
    }

    return area / static_cast<float>(max_area);
}


CSGenome::CSGenome(const CSGenome& orig) 
{ 
    copy(orig); 
}

CSGenome& CSGenome::operator=(const GAGenome& orig)
{
    if(&orig != this) { copy(orig); }
    return *this;
}
void CSGenome::copy(const GAGenome& orig)
{
    GAGenome::copy(orig);
    const CSGenome& castedGenome = dynamic_cast<const CSGenome&>(orig);
    this->m_boardSize = castedGenome.m_boardSize;
    this->m_genes = castedGenome.m_genes;
}

int CSGenome::computeArea() const
{
    int area = 0;
    unsigned int idx = 0;
    for(const auto& gene : m_genes)
    {
        if(gene.exists && !GeneHelper::checkForAllCollisions(idx, m_genes) && !GeneHelper::isOutside(gene))
        {
            area += gene.rect.area();
        }
        ++idx;
    }
    return area;
}

int CSGenome::getMaxArea() const
{
    static const int maxArea = m_boardSize.height * m_boardSize.width;
    return maxArea;
}