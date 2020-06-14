#include <fstream>
#include <iterator>
#include <sstream>
#include <iostream>

#include "ReadWriteManager.hxx"
using namespace cuttingStock;


std::vector<Rect> ReadWriteManager::readInputFile(const std::string& path)
{
    std::vector<Rect> res;
    std::ifstream file(path, std::ios::in);
    if(! file.is_open()) { return res; }

    std::string line;
    while(std::getline(file, line))
    {
        if(line == "") { break; }
        std::istringstream iss(line);
        std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>() );

        res.push_back(Rect{std::stoi(results[0]), std::stoi(results[1])});
    }

    file.close();
    return res;
}

void ReadWriteManager::saveOutputFile(const int area, const std::vector<Gene>& result, const std::string& path)
{

    std::ofstream file(path, std::ios::out);
    if(! file.is_open()){ std::cout << "Cannot open/create output.txt file" << std::endl; }

    file << area << std::endl;
    unsigned idx = 0;
    for(const auto& gene : result)
    {
        file << gene.rect.width << " " << gene.rect.height << " ";
        if(gene.exists && !GeneHelper::checkForAllCollisions(idx, result) && !GeneHelper::isOutside(gene))
        {
            file << gene.x << " " << gene.y << " ";
        }
        else
        {
            file << "-1 -1 ";
        }

        file << (gene.rotated ? 1 : 0) << std::endl;
        ++idx;
    }
    
    file.close();
}