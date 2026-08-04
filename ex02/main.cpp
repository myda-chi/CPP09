#include "PmergeMe.hpp"

int main(int argc, char** argv)
{
    PmergeMe sorter;

    if (!sorter.parseArguments(argc, argv))
        return 1;

    sorter.sortAndDisplay();
    return 0;
}