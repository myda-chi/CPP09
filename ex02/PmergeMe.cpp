#include "PmergeMe.hpp"

/* ─────────────────────────── Canonical form ─────────────────────────── */

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
        _vec     = other._vec;
        _deq     = other._deq;
        _timeVec = other._timeVec;
        _timeDeq = other._timeDeq;
    }
    return *this;
}

/* ───────────────────────────── Helpers ─────────────────────────────── */

double PmergeMe::getTimeInMicroseconds() const
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<double>(tv.tv_sec) * 1000000.0
         + static_cast<double>(tv.tv_usec);
}

/* ───────────────────────── Jacobsthal order ────────────────────────── */

// Generates the order (as indices into the losers array) in which
// losers should be inserted into the main chain.
// The order follows the Jacobsthal sequence:
//    J(0)=0, J(1)=1, J(k)=J(k-1)+2*J(k-2)
// Insertion groups:  [0],
//                     [J(2)-1 .. J(1)]  = [1..1] → 2, 1  if n>2
//                     [J(3)-1 .. J(2)]  = [3..2] → 4, 3  if n>4
//                     [J(4)-1 .. J(3)]  = [5..3] → 10..5 if n>10
//                     ...
std::vector<int> PmergeMe::generateJacobsthalOrder(int n) const
{
    std::vector<int> order;
    if (n <= 0) return order;

    // Index 0 is always first
    order.push_back(0);
    if (n == 1) return order;

    // Jacobsthal numbers: J[0]=0, J[1]=1, J[2]=1, J[3]=3, J[4]=5, J[5]=11, ...
    std::vector<int> J;
    J.push_back(0); // J0
    J.push_back(1); // J1

    int prev = J[1]; // J(k-1), start with J1 = 1
    int curr = J[1]; // J(k),   start with J1 = 1 (since J2 = J1 + 2*J0 = 1)

    while (static_cast<int>(order.size()) < n)
    {
        // Compute next Jacobsthal number: J(k+1) = curr + 2*prev
        int next = curr + 2 * prev;

        // Group from min(next-1, n-1) down to curr
        int high = std::min(next - 1, n - 1);
        for (int i = high; i >= curr; --i)
        {
            order.push_back(i);
            if (static_cast<int>(order.size()) >= n)
                return order;
        }

        // Slide window
        prev = curr;
        curr = next;
    }

    // If any indices are still missing (shouldn't happen, defensive)
    for (int i = 0; i < n; ++i)
    {
        if (std::find(order.begin(), order.end(), i) == order.end())
            order.push_back(i);
    }

    return order;
}

std::deque<int> PmergeMe::generateJacobsthalOrderDeque(int n) const
{
    // Same logic but returns a deque
    std::deque<int> order;
    if (n <= 0) return order;

    order.push_back(0);
    if (n == 1) return order;

    std::vector<int> J;
    J.push_back(0);
    J.push_back(1);

    int prev = J[1];
    int curr = J[1];

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

/* ─────────────────── Vector Ford-Johnson implementation ─────────────── */

void PmergeMe::insertLosersJacobsthalVector(std::vector<int>& mainChain,
                                             const std::vector<int>& losers) const
{
    int numLosers = static_cast<int>(losers.size());
    if (numLosers == 0) return;

    std::vector<int> insertOrder = generateJacobsthalOrder(numLosers);

    for (size_t k = 0; k < insertOrder.size(); ++k)
    {
        int idx = insertOrder[k];
        int val = losers[idx];

        // Binary search: val ≤ its paired winner at mainChain[idx]
        // (because idx is the position of the winner in the main chain
        //  before any losers are inserted — it's the index of the
        //  winner that this loser was paired with)
        //
        // Since losers may have been inserted before this winner,
        // the actual position of the winner is NOT idx anymore!
        // We search up to position (idx + number_of_losers_already_inserted_before_or_at_idx)
        //
        // Simpler: just search the whole chain up to (mainChain size + 1)
        // This is correct because val ≤ its paired winner which is at
        // some position in the chain.
        //
        // Actually, the standard approach: since losers[i] ≤ winners[i],
        // searching mainChain.begin() to mainChain.begin() + (idx + k_adjustment)
        // gives a tighter bound. But searching the whole chain is also correct.
        //
        // For simplicity and correctness, search the entire main chain:

        std::vector<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), val);

        mainChain.insert(pos, val);
    }
}

void PmergeMe::fordJohnsonSortVector(std::vector<int>& arr)
{
    int n = static_cast<int>(arr.size());
    if (n <= 1) return;

    /* 1. Pair elements into (larger, smaller) */
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
        // Only one element, already sorted
        if (hasOdd)
            arr[0] = straggler;
        return;
    }

    /* 2. Recursively sort the winners AND build the mapping */
    // Save a copy of winners before sorting to match losers later
    std::vector<int> winnersBeforeSort = winners;

    // Recursively sort the winner values
    fordJohnsonSortVector(winners);

    // Now winners[] is sorted. We need to build sortedLosers[] such that
    // sortedLosers[i] corresponds to winners[i] (its paired loser).
    //
    // For each sorted winner, find the FIRST UNUSED original pair
    // whose winner value matches. For duplicates, the first unused one
    // is taken, which is correct because all pairs with equal winners
    // are equivalent in the main chain order.
    //
    // Important: we need a stable 1-to-1 mapping. Without this,
    // two identical winner values would both match the same loser.
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
        // If not found (shouldn't happen), assign 0
        if (!found)
            sortedLosers[i] = 0;
    }

    /* 3. Main chain = sorted winners. Insert losers in Jacobsthal order. */
    std::vector<int> mainChain = winners;
    insertLosersJacobsthalVector(mainChain, sortedLosers);

    /* 4. Insert the straggler (odd element) using binary search */
    if (hasOdd)
    {
        std::vector<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }

    /* 5. Copy sorted result back */
    arr = mainChain;
}

/* ──────────────────── Deque Ford-Johnson implementation ─────────────── */

void PmergeMe::insertLosersJacobsthalDeque(std::deque<int>& mainChain,
                                            const std::deque<int>& losers) const
{
    int numLosers = static_cast<int>(losers.size());
    if (numLosers == 0) return;

    std::deque<int> insertOrder = generateJacobsthalOrderDeque(numLosers);

    for (std::deque<int>::const_iterator it = insertOrder.begin();
         it != insertOrder.end(); ++it)
    {
        int idx = *it;
        int val = losers[idx];

        std::deque<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), val);

        mainChain.insert(pos, val);
    }
}

void PmergeMe::fordJohnsonSortDeque(std::deque<int>& arr)
{
    int n = static_cast<int>(arr.size());
    if (n <= 1) return;

    /* 1. Pair elements into (larger, smaller) */
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

    /* 2. Recursively sort the winners and build mapping */
    std::deque<int> winnersBeforeSort = winners;
    fordJohnsonSortDeque(winners);

    int numWinners = numPairs;
    std::deque<int> sortedLosers(numWinners);
    std::vector<bool> used(numWinners, false); // vector, not deque, for bool

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

    /* 3. Insert losers in Jacobsthal order into main chain */
    std::deque<int> mainChain = winners;
    insertLosersJacobsthalDeque(mainChain, sortedLosers);

    /* 4. Insert straggler */
    if (hasOdd)
    {
        std::deque<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }

    /* 5. Copy back */
    arr = mainChain;
}

/* ──────────────────────── Argument parsing ─────────────────────────── */

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
        if (token.empty())
            continue;

        // Check for non-digit characters (allow leading '-')
        for (size_t j = 0; j < token.size(); ++j)
        {
            if (j == 0 && token[j] == '-' && token.size() > 1)
                continue;
            if (!std::isdigit(static_cast<unsigned char>(token[j])))
            {
                std::cerr << "Error" << std::endl;
                return false;
            }
        }

        std::istringstream iss(token);
        long long val;
        iss >> val;
        if (iss.fail() || val <= 0 || val > 2147483647)
        {
            std::cerr << "Error" << std::endl;
            return false;
        }

        _vec.push_back(static_cast<int>(val));
        _deq.push_back(static_cast<int>(val));
    }

    if (_vec.empty())
    {
        std::cerr << "Error" << std::endl;
        return false;
    }

    return true;
}

/* ──────────────────────── Sort and display ─────────────────────────── */

void PmergeMe::sortAndDisplay()
{
    // Display unsorted sequence (at most 20 numbers to avoid clutter)
    std::cout << "Before: ";
    int numDisplay = std::min(20, static_cast<int>(_vec.size()));
    for (int i = 0; i < numDisplay; ++i)
        std::cout << _vec[i] << " ";
    if (static_cast<int>(_vec.size()) > 20)
        std::cout << "[...]";
    std::cout << std::endl;

    // --- Sort with vector ---
    std::vector<int> vecCopy = _vec;
    double startVec = getTimeInMicroseconds();
    fordJohnsonSortVector(vecCopy);
    double endVec = getTimeInMicroseconds();
    _timeVec = endVec - startVec;

    // --- Sort with deque ---
    std::deque<int> deqCopy = _deq;
    double startDeq = getTimeInMicroseconds();
    fordJohnsonSortDeque(deqCopy);
    double endDeq = getTimeInMicroseconds();
    _timeDeq = endDeq - startDeq;

    // Verify vector is sorted (debug check)
    for (size_t i = 1; i < vecCopy.size(); ++i)
    {
        if (vecCopy[i - 1] > vecCopy[i])
        {
            std::cerr << "Error: vector not sorted correctly at index "
                      << i << std::endl;
            return;
        }
    }

    // Verify deque is sorted
    for (size_t i = 1; i < deqCopy.size(); ++i)
    {
        if (deqCopy[i - 1] > deqCopy[i])
        {
            std::cerr << "Error: deque not sorted correctly at index "
                      << i << std::endl;
            return;
        }
    }

    // Display sorted sequence
    std::cout << "After:  ";
    for (int i = 0; i < numDisplay; ++i)
        std::cout << vecCopy[i] << " ";
    if (static_cast<int>(vecCopy.size()) > 20)
        std::cout << "[...]";
    std::cout << std::endl;

    // Display timing
    std::cout << "Time to process a range of " << _vec.size()
              << " elements with std::vector : " << std::fixed
              << std::setprecision(5) << _timeVec << " us" << std::endl;
    std::cout << "Time to process a range of " << _deq.size()
              << " elements with std::deque  : " << std::fixed
              << std::setprecision(5) << _timeDeq << " us" << std::endl;
}