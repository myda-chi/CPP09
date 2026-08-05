#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <algorithm>
#include <sys/time.h>
#include <iomanip>

class PmergeMe
{
private:
    std::vector<int> _vec;
    std::deque<int> _deq;
    double _timeVec;
    double _timeDeq;

    void fordJohnsonSortVector(std::vector<int>& arr);
    void insertLosersJacobsthalVector(std::vector<int>& mainChain,
                                      const std::vector<int>& losers) const;

    void fordJohnsonSortDeque(std::deque<int>& arr);
    void insertLosersJacobsthalDeque(std::deque<int>& mainChain,
                                     const std::deque<int>& losers) const;

    std::vector<int> generateJacobsthalOrder(int n) const;
    double getTimeInMicroseconds() const;

public:
    PmergeMe();
    ~PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);

    bool parseArguments(int argc, char** argv);
    void sortAndDisplay();
};

#endif
