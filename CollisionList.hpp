#ifndef DEF_COLLISIONLIST
#define DEF_COLLISIONLIST

/*! 
CollisionList class
*/

#include<iostream> 
#include <filesystem>
#include <Eigen/Dense>
#include <map>
#include <chrono>
#include <utility> // pour std::pair
#include <iomanip> // pour std::put_time

// using TimePoint = std::chrono::system_clock::time_point;


class CollisionList 
{
public:
    CollisionList(size_t N);
    bool printList();
    bool printList(size_t head);
    bool addCollision(double t, size_t i, size_t j);
    double nextColTime();
    std::pair<size_t, size_t> nextColParts();
    // bool removeColsFromNextCollidingParts();
    bool removeColsFromPart(size_t index);
    size_t size();
protected: 
    size_t m_n;
    std::map<double, std::pair<size_t, size_t>> m_list;
};



#endif


