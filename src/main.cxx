#include <iostream>
#include <algorithm>
#include <functional>
#include <random>
#include <fstream>
#include <type_traits>
#include <array>

#include "utilities.hxx"

//zycze zle temu kto wymyslil tego include'a
#include <ga/GARealGenome.C>

using namespace genetic_utilities;

int main(int argc, char** argv)
{

    std::cout << "=================================== \n";
    std::cout << "Author: Milosz Filus\n";
    std::cout << "=================================== \n";


    auto input = ReadWriteManager::readInputFile("../input.txt");
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
    ReadWriteManager::saveOutputFile(result, "output.txt");


    return 0;
}
 

