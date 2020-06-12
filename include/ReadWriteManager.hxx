
#ifndef __READWRITEMANAGER_HXX__
#define __READWRITEMANAGER_HXX__

#include "utilities.hxx"
#include <vector>
#include <string>

namespace cuttingStock
{

class ReadWriteManager
{

public:
    static std::vector<Rect> readInputFile(const std::string& path);
    static void saveOutputFile(const int& area, const std::vector<Gene>& result, const std::string& path);

private:
    ReadWriteManager() = delete;
};

}


#endif