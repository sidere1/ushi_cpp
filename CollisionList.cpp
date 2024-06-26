#include "CollisionList.hpp"
#include <fstream>
#include <iomanip>
#include <boost/filesystem.hpp>

using namespace std;
// using TimePoint = std::chrono::system_clock::time_point;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


CollisionList::CollisionList(size_t N):m_n(N)
{
}

bool CollisionList::addCollision(double t, size_t i, size_t j) 
{
    if (i > m_n) cout << "you asked for particle " << i << " which does not exist" << endl;
    if (j > m_n) cout << "you asked for particle " << j << " which does not exist" << endl;
    m_list[t] = std::make_pair(i, j);
    // cout << " adding collision at time " << t << " between " << i << " and " << j << "." << endl;
    return true;
}

bool CollisionList::printList() 
{
    cout << "--------------------------------" << endl;
    cout << "Collision list " << endl;
    cout << "  Time    i    j" << endl;
    cout << "--------------------------------" << endl;
    for (const auto& [key, value] : m_list) {
        std::cout << std::setw(6) << key << std::setw(5) << value.first << std::setw(5) << value.second << std::endl;
    }
    cout << "--------------------------------" << endl << endl;
    return true;
}

bool CollisionList::printList(size_t head) 
{
    cout << "--------------------------------" << endl;
    cout << "Collision list " << endl;
    cout << "  Time    i    j" << endl;
    cout << "--------------------------------" << endl;
    size_t count = 0;
    for (const auto& [key, value] : m_list) {
        std::cout << std::setw(6) << key << std::setw(5) << value.first << std::setw(5) << value.second << std::endl;
        if (count++ > head)
            break;
    }
    cout << "--------------------------------" << endl << endl;
    return true;
}

double CollisionList::nextColTime()
{
    if (m_list.empty()) 
        return -1;
    auto it = m_list.begin();
    return it->first;
}

std::pair<size_t, size_t> CollisionList::nextColParts()
{
    if (m_list.empty())
        return std::make_pair(0, 0);
    auto it = m_list.begin();
    return it->second;
}

// bool CollisionList::removeColsFromNextCollidingParts()
// {
//     if (m_list.size() == 0)
//         return false;
//     std::pair<size_t, size_t> beginning = nextColParts();
//     size_t part1 = beginning.first;
//     size_t part2 = beginning.second;
//     for (auto it = m_list.begin(); it != m_list.end(); ) 
//     {
//         if (it->second.first == part1 || it->second.second == part1 || it->second.first == part2 || it->second.second == part2 ) {
//             it = m_list.erase(it); // erase renvoie l'itérateur suivant
//         } 
//         else 
//         {
//             ++it;
//         }
//     }
//     return true; 
// }

bool CollisionList::removeColsFromPart(size_t index)
{
    if (m_list.size() == 0)
        return false;
    for (auto it = m_list.begin(); it != m_list.end(); ) 
    {
        if (it->second.first == index || it->second.second == index) {
            cout << "Removing collision at time " << it->first << " between " << it->second.first  << " and " << it->second.second << endl;
            it = m_list.erase(it); // erase renvoie l'itérateur suivant
        } 
        else 
        {
            ++it;
        }
    }
    return true; 
}

size_t CollisionList::size()
{
    return m_list.size();
}
