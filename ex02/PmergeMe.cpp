#include "PmergeMe.hpp"
#include <cctype>

namespace
{
    bool parsePositiveInt(const std::string& token, int& value)
    {
        if (token.empty())
            return false;
        for (size_t i = 0; i < token.size(); ++i)
        {
            if (i == 0 && token[i] == '-' && token.size() > 1)
                continue;
            if (!std::isdigit(static_cast<unsigned char>(token[i])))
                return false;
        }
        std::istringstream iss(token);
        long long parsedValue;
        iss >> parsedValue;
        if (iss.fail())
            return false;
        iss >> std::ws;
        if (!iss.eof() || parsedValue <= 0 || parsedValue > 2147483647)
            return false;
        value = static_cast<int>(parsedValue);
        return true;
    }
}

PmergeMe::PmergeMe() : _timeVec(0.0), _timeDeq(0.0)
{
}

PmergeMe::~PmergeMe()
{
}

PmergeMe::PmergeMe(const PmergeMe& other)
{
    *this = other;
}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
    if (this != &other)
    {
        _vec = other._vec;
        _deq = other._deq;
        _timeVec = other._timeVec;
        _timeDeq = other._timeDeq;
    }
    return *this;
}

double PmergeMe::getTimeInMicroseconds() const
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<double>(tv.tv_sec) * 1000000.0
         + static_cast<double>(tv.tv_usec);
}

std::vector<int> PmergeMe::generateJacobsthalOrder(int n) const
{
    std::vector<int> order;
    if (n <= 0)
        return order;
    order.push_back(0);
    if (n == 1)
        return order;
    int prev = 1;
    int curr = 1;
    while (static_cast<int>(order.size()) < n)
    {
        int next = curr + 2 * prev;
        int high = std::min(next - 1, n - 1);
        for (int i = high; i >= curr; --i)
        {
            order.push_back(i);
            if (static_cast<int>(order.size()) >= n)
                return order;
        }
        prev = curr;
        curr = next;
    }
    for (int i = 0; i < n; ++i)
    {
        if (std::find(order.begin(), order.end(), i) == order.end())
            order.push_back(i);
    }
    return order;
}

void PmergeMe::insertLosersJacobsthalVector(std::vector<int>& mainChain, const std::vector<int>& losers) const
{
    int numLosers = static_cast<int>(losers.size());
    if (numLosers == 0)
        return;
    std::vector<int> insertOrder = generateJacobsthalOrder(numLosers);
    for (size_t k = 0; k < insertOrder.size(); ++k)
    {
        int idx = insertOrder[k];
        int val = losers[idx];
        std::vector<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), val);
        mainChain.insert(pos, val);
    }
}

void PmergeMe::fordJohnsonSortVector(std::vector<int>& arr)
{
    int n = static_cast<int>(arr.size());
    if (n <= 1)
        return;
    bool hasOdd = (n % 2 != 0);
    int straggler = 0;
    int numPairs = n / 2;
    std::vector<int> winners(numPairs);
    std::vector<int> losers(numPairs);
    for (int i = 0; i < numPairs; ++i)
    {
        int a = arr[2 * i];
        int b = arr[2 * i + 1];
        if (a >= b)
        {
            winners[i] = a;
            losers[i] = b;
        }
        else
        {
            winners[i] = b;
            losers[i] = a;
        }
    }
    if (hasOdd)
        straggler = arr[n - 1];
    if (numPairs == 0)
    {
        if (hasOdd)
            arr[0] = straggler;
        return;
    }
    std::vector<int> winnersBeforeSort = winners;
    fordJohnsonSortVector(winners);
    int numWinners = numPairs;
    std::vector<int> sortedLosers(numWinners);
    std::vector<bool> used(numWinners, false);
    for (int i = 0; i < numWinners; ++i)
    {
        int targetVal = winners[i];
        bool found = false;
        for (int j = 0; j < numWinners && !found; ++j)
        {
            if (!used[j] && winnersBeforeSort[j] == targetVal)
            {
                sortedLosers[i] = losers[j];
                used[j] = true;
                found = true;
            }
        }
        if (!found)
            sortedLosers[i] = 0;
    }
    std::vector<int> mainChain = winners;
    insertLosersJacobsthalVector(mainChain, sortedLosers);
    if (hasOdd)
    {
        std::vector<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }
    arr = mainChain;
}

void PmergeMe::insertLosersJacobsthalDeque(std::deque<int>& mainChain, const std::deque<int>& losers) const
{
    int numLosers = static_cast<int>(losers.size());
    if (numLosers == 0)
        return;
    std::vector<int> insertOrder = generateJacobsthalOrder(numLosers);
    for (size_t k = 0; k < insertOrder.size(); ++k)
    {
        int idx = insertOrder[k];
        int val = losers[idx];
        std::deque<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), val);
        mainChain.insert(pos, val);
    }
}

void PmergeMe::fordJohnsonSortDeque(std::deque<int>& arr)
{
    int n = static_cast<int>(arr.size());
    if (n <= 1)
        return;
    bool hasOdd = (n % 2 != 0);
    int straggler = 0;
    int numPairs = n / 2;
    std::deque<int> winners(numPairs);
    std::deque<int> losers(numPairs);
    for (int i = 0; i < numPairs; ++i)
    {
        int a = arr[2 * i];
        int b = arr[2 * i + 1];
        if (a >= b)
        {
            winners[i] = a;
            losers[i] = b;
        }
        else
        {
            winners[i] = b;
            losers[i] = a;
        }
    }
    if (hasOdd)
        straggler = arr[n - 1];
    if (numPairs == 0)
    {
        if (hasOdd)
            arr[0] = straggler;
        return;
    }
    std::deque<int> winnersBeforeSort = winners;
    fordJohnsonSortDeque(winners);
    int numWinners = numPairs;
    std::deque<int> sortedLosers(numWinners);
    std::vector<bool> used(numWinners, false);
    for (int i = 0; i < numWinners; ++i)
    {
        int targetVal = winners[i];
        bool found = false;
        for (int j = 0; j < numWinners && !found; ++j)
        {
            if (!used[j] && winnersBeforeSort[j] == targetVal)
            {
                sortedLosers[i] = losers[j];
                used[j] = true;
                found = true;
            }
        }
        if (!found)
            sortedLosers[i] = 0;
    }
    std::deque<int> mainChain = winners;
    insertLosersJacobsthalDeque(mainChain, sortedLosers);
    if (hasOdd)
    {
        std::deque<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }
    arr = mainChain;
}

bool PmergeMe::parseArguments(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Error" << std::endl;
        return false;
    }
    _vec.clear();
    _deq.clear();
    for (int i = 1; i < argc; ++i)
    {
        std::string token(argv[i]);
        int value;
        if (!parsePositiveInt(token, value))
        {
            std::cerr << "Error" << std::endl;
            return false;
        }
        _vec.push_back(value);
        _deq.push_back(value);
    }
    if (_vec.empty())
    {
        std::cerr << "Error" << std::endl;
        return false;
    }
    return true;
}

void PmergeMe::sortAndDisplay()
{
    std::cout << "Before: ";
    int numDisplay = std::min(20, static_cast<int>(_vec.size()));
    for (int i = 0; i < numDisplay; ++i)
        std::cout << _vec[i] << " ";
    if (static_cast<int>(_vec.size()) > 20)
        std::cout << "[...]";
    std::cout << std::endl;
    std::vector<int> vecCopy = _vec;
    double startVec = getTimeInMicroseconds();
    fordJohnsonSortVector(vecCopy);
    double endVec = getTimeInMicroseconds();
    _timeVec = endVec - startVec;
    std::deque<int> deqCopy = _deq;
    double startDeq = getTimeInMicroseconds();
    fordJohnsonSortDeque(deqCopy);
    double endDeq = getTimeInMicroseconds();
    _timeDeq = endDeq - startDeq;
    for (size_t i = 1; i < vecCopy.size(); ++i)
    {
        if (vecCopy[i - 1] > vecCopy[i])
        {
            std::cerr << "Error: vector not sorted correctly at index "
                      << i << std::endl;
            return;
        }
    }
    for (size_t i = 1; i < deqCopy.size(); ++i)
    {
        if (deqCopy[i - 1] > deqCopy[i])
        {
            std::cerr << "Error: deque not sorted correctly at index "
                      << i << std::endl;
            return;
        }
    }
    std::cout << "After:  ";
    for (int i = 0; i < numDisplay; ++i)
        std::cout << vecCopy[i] << " ";
    if (static_cast<int>(vecCopy.size()) > 20)
        std::cout << "[...]";
    std::cout << std::endl;
    std::cout << "Time to process a range of " << _vec.size()
              << " elements with std::vector : " << std::fixed
              << std::setprecision(5) << _timeVec << " us" << std::endl;
    std::cout << "Time to process a range of " << _deq.size()
              << " elements with std::deque  : " << std::fixed
              << std::setprecision(5) << _timeDeq << " us" << std::endl;
}
