#ifndef DEF_COLLISIONLIST
#define DEF_COLLISIONLIST

/*! 
CollisionList class
*/

#include<iostream> 
#include <filesystem>
#include <map>
#include <chrono>
#include <utility> // pour std::pair
#include <iomanip> // pour std::put_time

// using TimePoint = std::chrono::system_clock::time_point;


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
};



#endif


