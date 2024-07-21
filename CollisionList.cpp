#include "CollisionList.hpp"
#include <fstream>
#include <iomanip>

using namespace std;

/**
 * @brief Construct a new Collision List object with specified size and verbosity.
 * 
 * @param N Number of particles.
 * @param verbose Verbose mode flag.
 */
CollisionList::CollisionList(size_t N, bool verbose):m_n(N), m_verbose(verbose), m_flush(false)
{
    m_maxSize = 10000000000;
    m_hasFlushed = false;
}

/**
 * @brief Construct a new Collision List object with specified size, verbosity, and result directory.
 * 
 * Enables the flush mode, to export the list to a file when the list gets too big, to save on memory. But not fully coded yet, it was not necessary... 
 * 
 * @param N Number of particles.
 * @param verbose Verbose mode flag.
 * @param resultDir Directory to store the results.
 */
CollisionList::CollisionList(size_t N, bool verbose, string resultDir):m_n(N), m_verbose(verbose), m_flush(true), m_resultDir(resultDir)
{
    m_maxSize = 10000000000;
    // m_maxSize = 10;
    m_flushFile = m_resultDir + "/collisionList.uchi";
    ofstream outfile(m_flushFile);
    m_hasFlushed = false;

    m_flush = false; // option pas encore codée ! Pas sûr que ce soit nécessaire non plus. 
}

/**
 * @brief Adds a collision event to the list.
 * 
 * @param t Time of the collision.
 * @param i Index of the first particle.
 * @param j Index of the second particle.
 * @return true If the collision was added successfully.
 * @return false If the collision was not added successfully.
 */
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

/**
 * @brief Prints the entire list of collisions.
 * 
 * @return true If the list was printed successfully.
 * @return false If the list was not printed successfully.
 */
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

/**
 * @brief Prints the head of the list of collisions.
 * 
 * @param head Number of collisions to print from the top.
 * @return true If the list was printed successfully.
 * @return false If the list was not printed successfully.
 */
bool CollisionList::printList(size_t head) 
{
    if (head > m_list.size())
        head = m_list.size();
    cout << "--------------------------------" << endl;
    cout << "Collision list (" << head << "/" << m_list.size() << " entries)" << endl;
    cout << "  Time    i    j" << endl;
    cout << "--------------------------------" << endl;
    size_t count = 0;
    for (const auto& [key, value] : m_list) {
        std::cout << std::setw(6) << key << std::setw(5) << value.first << std::setw(5) << value.second << std::endl;
        count++;
        if (count > head)
            break;
    }
    cout << "--------------------------------" << endl << endl;
    return true;
}

/**
 * @brief Retrieves the time of the next collision.
 * 
 * @return double Time of the next collision or -1 if the list is empty.
 */
double CollisionList::nextColTime()
{
    if (m_list.empty()) 
        return -1;
    auto it = m_list.begin();
    return it->first;
}

/**
 * @brief Retrieves the particles involved in the next collision.
 * 
 * @return std::pair<size_t, size_t> Pair of particle indices involved in the next collision.
 */
std::pair<size_t, size_t> CollisionList::nextColParts()
{
    if (m_list.empty())
        return std::make_pair(0, 0);
    auto it = m_list.begin();
    return it->second;
}

/**
 * @brief Removes all collisions involving a specified particle.
 * 
 * @param index Index of the particle.
 * @return true If the collisions were removed successfully.
 * @return false If the collisions were not removed successfully.
 */
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

/**
 * @brief Retrieves the size of the collision list.
 * 
 * @return size_t Number of collisions in the list.
 */
size_t CollisionList::size()
{
    return m_list.size();
}

/**
 * @brief Flushes the collision list to a file.
 * 
 * @return true If the list was flushed successfully.
 * @return false If the list was not flushed successfully.
 */
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

/**
 * @brief Unflushes the collision list from a file.
 * 
 * NOT CODED YET. You may do it if memory consumption is too large for your application and the collision summary takes too much ram :) 
 * 
 * @return true If the list was unflushed successfully.
 * @return false If the list was not unflushed successfully.
 */
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
