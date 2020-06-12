#include <iostream>
#include <algorithm>
#include <functional>
#include <random>
#include <fstream>
#include <type_traits>
#include <array>
#include <chrono>


#include "ReadWriteManager.hxx"
#include "CSGenome.hxx"

using namespace cuttingStock;

int main(int argc, char** argv)
{

    auto timeStart = std::chrono::steady_clock::now();
    std::cout << "=================================== \n";
    std::cout << "Author: Milosz Filus\n";
    std::cout << "=================================== \n";

    auto input = ReadWriteManager::readInputFile("maleplyty.txt");
    if(input.empty())
    {
        std::cout << "Invalid input file... \n Valid file should be maleplyty.txt in program directory" 
            << std::endl; 
    }

    GeneHelper::boardSize = Rect{2800, 2070};
    CSGenome genome(input, Rect{2800, 2070});
    genome.initialize();

    GASimpleGA ga(genome);
    ga.populationSize(500);
    ga.nGenerations(4000);
    ga.pMutation(0.05);
    ga.pCrossover(0.5);
    ga.selector(GARouletteWheelSelector{GASelectionScheme::RAW});
    ga.scaling(GANoScaling{});
    ga.maximize();
    ga.evolve();

    genome = ga.statistics().bestIndividual();
    auto result = genome.getGenes();
    ReadWriteManager::saveOutputFile(genome.computeArea(), result, "output.txt");
    std::cout << "Fitness (MAX: 1): " << genome.score() << std::endl;
    std::cout << "Area (MAX: " << genome.getMaxArea() << "): " << genome.computeArea() << std::endl;
    std::cout << "Elapsed Time: " << 
        std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - timeStart).count() << "s" << std::endl;

    return 0;
}
 

