#include "utilities.hxx"

using namespace cuttingStock;

Rect GeneHelper::boardSize = Rect{};

// bool GeneHelper::checkForCollision(std::pair<int, int> pos1, Rect size1, 
//     std::pair<int, int> pos2, Rect size2)
// {

//     return false;
// }

bool GeneHelper::checkForCollision(const Gene& geneL, const Gene& geneR)
{
    return !(geneR.x >= geneL.x + geneL.getFixedWidth()
        || geneL.x >= geneR.x + geneR.getFixedWidth()
        || geneR.y >= geneL.y + geneL.getFixedHeight()
        || geneL.y >= geneR.y + geneR.getFixedHeight());
}

// bool GeneHelper::isOutside(std::pair<int, int> pos, Rect size)
// {
//     return pos.first + size.width > boardSize.width
//           || pos.second + size.height > boardSize.height
//           || pos.first < 0 || pos.second < 0;
// }

bool GeneHelper::isOutside(const Gene& gene)
{
    return gene.x + gene.getFixedWidth() > boardSize.width
        || gene.y + gene.getFixedHeight() > boardSize.height
        || gene.x < 0 || gene.y < 0;
}