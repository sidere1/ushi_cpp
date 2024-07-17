#include "CollisionList.hpp"
#include <fstream>
#include <iomanip>
// #include <boost/filesystem.hpp>

using namespace std;
// using TimePoint = std::chrono::system_clock::time_point;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


CollisionList::CollisionList(size_t N, bool verbose):m_n(N), m_verbose(verbose), m_flush(false)
{
    m_maxSize = 10000000000;
    m_hasFlushed = false;
}

CollisionList::CollisionList(size_t N, bool verbose, string resultDir):m_n(N), m_verbose(verbose), m_flush(true), m_resultDir(resultDir)
{
    m_maxSize = 10000000000;
    // m_maxSize = 10;
    m_flushFile = m_resultDir + "/collisionList.uchi";
    ofstream outfile(m_flushFile);
    m_hasFlushed = false;

    m_flush = false; // option pas encore codée ! Pas sûr que ce soit nécessaire non plus. 
}

bool CollisionList::addCollision(double t, size_t i, size_t j) 
{
    if (i > m_n) cout << "you asked for particle " << i << " which does not exist" << endl;
    if (j > m_n) cout << "you asked for particle " << j << " which does not exist" << endl;
    m_list[t] = std::make_pair(i, j);
    if (m_flush)
        if (m_list.size() > m_maxSize)
            return flush();
    return true;
}

bool CollisionList::printList() 
{
    cout << "--------------------------------" << endl;
    cout << "Collision list ("  << m_list.size() << " entries)" << endl;
    cout << "  Time       i    j" << endl;
    cout << "--------------------------------" << endl;
    for (const auto& [key, value] : m_list) {
        std::cout << std::setw(10) << key << std::setw(5) << value.first << std::setw(5) << value.second << std::endl;
    }
    cout << "--------------------------------" << endl << endl;
    return true;
}

bool CollisionList::printList(size_t head) 
{
    cout << "--------------------------------" << endl;
    cout << "Collision list ("  << m_list.size() << " entries)" << endl;
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

bool CollisionList::removeColsFromPart(size_t index)
{
    if (m_list.size() == 0)
        return false;
    for (auto it = m_list.begin(); it != m_list.end(); ) 
    {
        if (it->second.first == index || it->second.second == index) 
        {
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

bool CollisionList::flush()
{
    ofstream outfile(m_flushFile, ios::app);
    if (!outfile)
        return false; 
    
    for (const auto& [key, value] : m_list) 
    {
        outfile << std::setw(6) << key << std::setw(5) << value.first << std::setw(5) << value.second << std::endl;
    }
    for (auto it = m_list.begin(); it != m_list.end(); ) 
    {
        it = m_list.erase(it); // erase renvoie l'itérateur suivant
    }
    m_hasFlushed = true;
    return true;
}

bool CollisionList::unflush()
{
    flush();
    ifstream infile(m_flushFile);
    if (!infile)
        return false; 
    
    cout << "LA FONCTION UNFLUSH N'A PAS ENCORE ÉTÉ CODÉE" << endl;
    // string temp;
    // while (infile >> temp)
    // {};
    
    m_hasFlushed = false;
    return true;
}
