#ifndef DEF_DYNAMICSMANAGER
#define DEF_DYNAMICSMANAGER

/*! 
DynamicsManager class
*/

#include<iostream> 
// #include <filesystem>
// #include <Eigen/Dense>
#include <map>
// #include <chrono>
// #include <utility> // pour std::pair
// #include <iomanip> // pour std::put_time
#include "BCMatrix.hpp"
#include "CollisionList.hpp"
#include "Particle.hpp"
#include <array>

// using TimePoint = std::chrono::system_clock::time_point;


class DynamicsManager 
{
public:
    DynamicsManager(size_t N);
    bool generatePartList();
    bool generatePartListDebug();
    bool printPartList();
    bool run();
    bool initializeCL();
    bool move(double dt);
    bool updateTraj();
    bool collide();
    bool wallCollide(size_t index);
    bool updateSpeedFromCollision(Particle& p1, Particle& p2);

    en fait faut jamais créer de nouvelle Particle  part  l'initialisation, toujours les manier par adresse 

protected: 
    size_t m_n;
    BCMatrix m_BCMatrix;
    CollisionList m_CollisionList;
    double m_endTime;
    std::vector<Particle> m_partList;
    double m_arenaSize;
    double m_eps;
    double m_time;
    // Particle[]
    // std::map<double, std::pair<size_t, size_t>> m_list;
};



#endif


