#include "utilities.hxx"

using namespace cuttingStock;

Rect GeneHelper::boardSize = Rect{};

bool GeneHelper::checkForCollision(const Gene& geneL, const Gene& geneR)
{
    return !(geneR.x >= geneL.x + geneL.getFixedWidth()
        || geneL.x >= geneR.x + geneR.getFixedWidth()
        || geneR.y >= geneL.y + geneL.getFixedHeight()
        || geneL.y >= geneR.y + geneR.getFixedHeight());
}

bool GeneHelper::isOutside(const Gene& gene)
{
    return gene.x + gene.getFixedWidth() > boardSize.width
        || gene.y + gene.getFixedHeight() > boardSize.height
        || gene.x < 0 || gene.y < 0;
}