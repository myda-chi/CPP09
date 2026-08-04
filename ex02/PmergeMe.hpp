#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <sys/time.h>
#include <iomanip>
#include <stdexcept>

class PmergeMe
{
private:
    std::vector<int> _vec;
    std::deque<int> _deq;
    double _timeVec;
    double _timeDeq;

    /* ---------- Vector implementation (full Ford-Johnson) ---------- */
    void fordJohnsonSortVector(std::vector<int>& arr);
    void insertLosersJacobsthalVector(std::vector<int>& mainChain,
                                      const std::vector<int>& losers) const;

    /* ---------- Deque implementation (full Ford-Johnson) ---------- */
    void fordJohnsonSortDeque(std::deque<int>& arr);
    void insertLosersJacobsthalDeque(std::deque<int>& mainChain,
                                     const std::deque<int>& losers) const;

    /* ---------- Jacobsthal order generation ---------- */
    std::vector<int> generateJacobsthalOrder(int n) const;
    std::deque<int> generateJacobsthalOrderDeque(int n) const;

    /* ---------- Helpers ---------- */
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