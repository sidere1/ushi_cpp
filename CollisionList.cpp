#include "CollisionList.hpp"
#include <fstream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>


using namespace std;

/**
 * @brief Construct a new Collision List object with specified size and verbosity.
 * 
 * @param N Number of particles.
 * @param verbose Verbose mode flag.
 */
CollisionList::CollisionList(size_t N, bool verbose):m_n(N), m_verbose(verbose), m_flush(false), m_doublon(0)
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
CollisionList::CollisionList(size_t N, bool verbose, string resultDir):m_n(N), m_verbose(verbose), m_flush(true), m_resultDir(resultDir), m_doublon(0)
{
    m_maxSize = 10000000000;
    // m_maxSize = 10;
    m_flushFile = m_resultDir + "/collisionList.uchi";
    ofstream outfile(m_flushFile);
    if (outfile)
        outfile << "# N = " << N << endl;
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
    if (m_list.find(t) != m_list.end()) // in stp::map, only one value per key ! If the time already exists, the previous collision would be erased ! 
    {
        double epsilon(numeric_limits<double>::epsilon());
        do
        {
            t += epsilon;
        }while(m_list.find(t)!= m_list.end());
        m_doublon++;
    }
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
 * @brief Reads the collision list from a file.
 * 
 * @return true If the list was read successfully.
 * @return false Otherwise
 */
// bool CollisionList::read(string collisionFile)
// {
//     ifstream infile(m_flushFile);
//     if (!infile)
//         return false; 
    
//     string temp;
//     double t;
//     size_t N, i, j;
//     infile >> temp; // #
//     cout << "temp = " << temp << endl;
//     infile >> temp; // N
//     cout << "temp = " << temp << endl;
//     infile >> temp; // =
//     cout << "temp = " << temp << endl;
//     infile >> N; 
//     if (N != m_n)
//     {
//         cout << "expected '# N " << m_n << "', but I got " << N << endl;
//         return false;
//     }
//     cout << "N = " << N << endl;

//     while (!infile.eof())
//     {
//         infile >> t;
//         cout << "t = " << t << endl;
//         infile >> i;
//         infile >> j;
//         cout << "adding collision " << t << " : " << i << " ; " << j << endl;
//         // addCollision(t, i, j);
//     }
    
//     m_hasFlushed = false;
//     return true;
// }

bool CollisionList::read(const string& collisionFile)
{
    ifstream infile(collisionFile);
    if (!infile) 
    {
        cout << "Error: Unable to open file " << collisionFile << endl;
        return false;
    }

    // Reading header 
    string line;
    if (!getline(infile, line)) 
    {
        cerr << "Error: File is empty or first line is missing." << endl;
        return false;
    }
    istringstream headerStream(line);
    string hash, n, equalSign;
    size_t N;

    if (!(headerStream >> hash >> n >> equalSign >> N) || hash != "#" || n !="N" || equalSign != "=" || N != m_n) {
        cerr << "Error: Expected '# = " << m_n << "', but got '" << line << "'." << endl;
        return false;
    }

    // Reading the rest of the collision file 
    size_t nColl(0);
    while (getline(infile, line)) 
    {
        if (line.empty()) {
            continue; 
        }

        istringstream lineStream(line);
        double t;
        size_t i, j;

        if (!(lineStream >> t >> i >> j) || i >= N || j >= N) 
        {
            cerr << "Error: Malformed line or indices out of range in line: '" << line << "'." << endl;
            return false;
        }
        // cout << "Adding collision " << t << " : " << i << " ; " << j << endl;
        addCollision(t, i, j);
        nColl++;
    }
    cout << nColl << " collisions have been read from " << collisionFile << endl;
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
    read(m_flushFile);
    m_hasFlushed = false;
    return true;
}
