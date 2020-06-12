#include "utilities.hxx"
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <set>

using namespace genetic_utilities;

/////////////////////////////// FUNCTION_HELPER //////////////////////////////////////

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
    evaluator(CSEvaluatorWithoutPenalties);
}

void CSGenome::CSInitializer(GAGenome& genome)
{
    CSGenome& castedGenome = dynamic_cast<CSGenome&>(genome);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> realDist(0,1);
    std::uniform_int_distribution<> indexDist(0, castedGenome.m_genes.size() - 1);
    std::uniform_int_distribution<> widthDist(0, castedGenome.m_boardSize.width - 1);
    std::uniform_int_distribution<> heightDist(0, castedGenome.m_boardSize.height - 1);

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

//prowizoryczny one point
int CSGenome::CSCrossover(const GAGenome& mother, const GAGenome& father, GAGenome* bro, GAGenome* sis)
{
    const CSGenome& castedMother = dynamic_cast<const CSGenome&>(mother);
    const CSGenome& castedFather = dynamic_cast<const CSGenome&>(father);

    int setSize = castedFather.m_genes.size();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> cutDist(1, setSize - 1);
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
    std::random_device rnd;
    std::mt19937 gen(rnd());
    std::uniform_real_distribution<> dist;
    std::normal_distribution<> stepDist(0, 100);

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
        std::vector<std::pair<int, Gene> > leftClosest;
        auto& genes = castedGenome.m_genes;
        for(unsigned idx = 0; idx < genes.size(); ++idx)
        {
            if(genes[idx].exists)
            {
                leftClosest.emplace_back(idx, genes[idx]);
            }
        }

        std::sort(leftClosest.begin(), leftClosest.end(), 
            [&](const auto& lhs, const auto& rhs){
                return lhs.second.x < rhs.second.x;      
            }
        );


        for(unsigned i = 0; i < leftClosest.size(); ++i)
        {
            auto currentHeight = leftClosest[i].second.rotated ? leftClosest[i].second.rect.width : leftClosest[i].second.rect.height;
            auto currentWidth = leftClosest[i].second.rotated ? leftClosest[i].second.rect.height : leftClosest[i].second.rect.width;

            std::set<int> possibleXes;
            possibleXes.insert(0);
            for(auto& gene : leftClosest)
            {
                possibleXes.insert(gene.second.x + (gene.second.rotated ? gene.second.rect.height : gene.second.rect.width));
            }
            
            for(const auto& possibleX : possibleXes)
            {
                auto isColision = false;
                for(unsigned j = 0; j < leftClosest.size(); ++j)
                {

                    if(i == j){ continue; }

                    auto otherHeight = leftClosest[j].second.rotated ? leftClosest[j].second.rect.width : leftClosest[j].second.rect.height;
                    auto otherWidth = leftClosest[j].second.rotated ? leftClosest[j].second.rect.height : leftClosest[j].second.rect.width;

                    if(!((leftClosest[j].second.x >= possibleX + currentWidth 
                    || possibleX >= leftClosest[j].second.x + otherWidth
                    || leftClosest[j].second.y >= leftClosest[i].second.y + currentHeight 
                    || leftClosest[i].second.y >= leftClosest[j].second.y + otherHeight)
                    && (possibleX + currentWidth < castedGenome.m_boardSize.width && leftClosest[i].second.y + currentHeight < castedGenome.m_boardSize.height)))
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
       
        //////uppper :^)

        std::vector<std::pair<int, Gene> > upperClosest;
        for(unsigned idx = 0; idx < genes.size(); ++idx)
        {
            if(genes[idx].exists)
            {
                upperClosest.emplace_back(idx, genes[idx]);
            }
        }

        std::sort(upperClosest.begin(), upperClosest.end(), 
            [&](const auto& lhs, const auto& rhs){
                return lhs.second.y < rhs.second.y;      
            }
        );

        for(unsigned i = 0; i < upperClosest.size(); ++i)
        {
            auto currentHeight = upperClosest[i].second.rotated ? upperClosest[i].second.rect.width : upperClosest[i].second.rect.height;
            auto currentWidth = upperClosest[i].second.rotated ? upperClosest[i].second.rect.height : upperClosest[i].second.rect.width;

            std::set<int> possibleYes;
            possibleYes.insert(0);
            for(auto& gene : upperClosest)
            {
                possibleYes.insert(gene.second.y + (gene.second.rotated ? gene.second.rect.width : gene.second.rect.height));
            }
            
            for(const auto& possibleY : possibleYes)
            {
                auto isColision = false;
                for(unsigned j = 0; j < upperClosest.size(); ++j)
                {

                    if(i == j){ continue; }

                    auto otherHeight = upperClosest[j].second.rotated ? upperClosest[j].second.rect.width : upperClosest[j].second.rect.height;
                    auto otherWidth = upperClosest[j].second.rotated ? upperClosest[j].second.rect.height : upperClosest[j].second.rect.width;

                    if(!((upperClosest[j].second.x >= upperClosest[i].second.x + currentWidth 
                    || upperClosest[i].second.x >= upperClosest[j].second.x + otherWidth
                    || upperClosest[j].second.y >= possibleY + currentHeight 
                    || possibleY >= upperClosest[j].second.y + otherHeight)
                    && (upperClosest[i].second.x + currentWidth < castedGenome.m_boardSize.width && possibleY + currentHeight < castedGenome.m_boardSize.height)))
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

    int sizeSet = castedBro.m_genes.size();
    int theSame = 0;

    for(int idx = 0; idx < sizeSet; ++idx)
    {
        if(castedBro.m_genes[idx] == castedSis.m_genes[idx]){ ++theSame; }
    }
    
    return theSame / static_cast<float>(sizeSet); 
}

float CSGenome::CSEvaluator(GAGenome& genome)
{
    
    return 0;
}


float CSGenome::CSEvaluatorWithoutPenalties(GAGenome& genome)
{
    CSGenome& castedGenome = dynamic_cast<CSGenome&>(genome);
    int max_area = castedGenome.m_boardSize.height * castedGenome.m_boardSize.width;
    int area = 0;

    auto& gens = castedGenome.m_genes;
    for(unsigned i = 0; i < gens.size(); ++i)
    {
        if(! gens[i].exists ){ continue; }
        int current_area = gens[i].rect.width * gens[i].rect.height;
        int currentWidth = gens[i].rotated ? gens[i].rect.height : gens[i].rect.width;
        int currentHeight = gens[i].rotated ? gens[i].rect.width : gens[i].rect.height;

        if(gens[i].x + currentWidth > castedGenome.m_boardSize.width
          || gens[i].y + currentHeight > castedGenome.m_boardSize.height
          || gens[i].x < 0 || gens[i].y < 0)
        {
            return 0;
        }

        for(unsigned j = 0; j < gens.size(); ++j)
        {
            if(j == i) { continue; }
            if(! gens[j].exists){ continue; }

            int otherWidth = gens[j].rotated ? gens[j].rect.height : gens[j].rect.width;
            int otherHeight = gens[j].rotated ? gens[j].rect.width : gens[j].rect.height;

            if(! (gens[j].x >= gens[i].x + currentWidth || gens[i].x >= gens[j].x + otherWidth
                || gens[j].y >= gens[i].y + currentHeight || gens[i].y >= gens[j].y + otherHeight))
            {
                return 0;
            }
        }
        area += current_area;
    }

    return area / static_cast<float>(max_area);
}


std::vector<Rect> ReadWriteManager::readInputFile(const std::string& path)
{
    std::vector<Rect> res;
    std::ifstream file(path, std::ios::in);
    if(! file.is_open()) { return res; }
    

    std::string line;
    while(std::getline(file, line))
    {
        std::istringstream iss(line);
        std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>() );

        res.push_back(Rect{std::stoi(results[0]), std::stoi(results[1])});
    }

    file.close();
    return res;
}

void ReadWriteManager::saveOutputFile(const std::vector<Gene>& result, const std::string& path)
{
    int area = 0;
    for(const auto& gene : result)
    {
        if(gene.exists)
        {
            area += gene.rect.height * gene.rect.width;
        }
    }
    std::cout << area / (2070. * 2800) << std::endl;

    std::ofstream file(path, std::ios::out);
    if(! file.is_open()){ return; }

    file << area << std::endl;
    for(const auto& gene : result)
    {
        file << gene.rect.width << " " << gene.rect.height << " ";
        if(gene.exists)
        {
            file << gene.x << " " << gene.y << " ";
        }
        else
        {
            file << "-1 -1 ";
        }

        file << (gene.rotated ? 1 : 0) << std::endl;
    }

    file.close();
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