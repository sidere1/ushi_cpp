#ifndef DEF_COLLISIONLIST
#define DEF_COLLISIONLIST

/*! 
\class CollisionList
\brief Class for managing a collision list as a map of timings / pair of Particle index 
*/

#include<iostream> 
#include <filesystem>
#include <map>
#include <chrono>
#include <utility> // pour std::pair
#include <iomanip> // pour std::put_time

class CollisionList 
{
public:
    CollisionList(size_t N, bool verbose);
    CollisionList(size_t N, bool verbose, std::string resultDir);
    bool printList();
    bool printList(size_t head);
    bool addCollision(double t, size_t i, size_t j);
    double nextColTime();
    std::pair<size_t, size_t> nextColParts();
    bool removeColsFromPart(size_t index);
    size_t size();
    bool flush();
    bool unflush();
    bool read(const std::string& collisionFile);

    bool hasFlushed() {return m_hasFlushed;};

    std::map<double, std::pair<size_t, size_t>> list() {return m_list;};
    
protected: 
    size_t m_n;
    std::map<double, std::pair<size_t, size_t>> m_list;
    bool m_verbose;
    bool m_flush;
    bool m_hasFlushed;
    std::string m_resultDir, m_flushFile;
    size_t m_maxSize;
    size_t m_doublon;
};



#endif


