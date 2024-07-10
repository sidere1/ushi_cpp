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
#include "BackwardCluster.hpp"
#include "Particle.hpp"
#include <array>

// using TimePoint = std::chrono::system_clock::time_point;


class DynamicsManager 
{
public:
    DynamicsManager(size_t N, bool verbose, bool m_exportAnim);
    bool generatePartList();
    bool generatePartListDebug();
    bool printPartList();
    bool run();
    bool initializeCL();
    bool move(double dt);
    bool updateTraj(size_t index);
    bool collide();
    bool wallCollide(size_t index);
    bool updateSpeedFromCollision(size_t p1, size_t p2);
    bool computeNextWallImpact();
    bool initialize_anim_file();
    bool add_anim_step();

protected: 
    size_t m_n;
    BCMatrix m_BCMatrix;
    CollisionList m_CollisionList;
    CollisionList m_CollisionSummary;
    double m_endTime, m_dt;
    std::vector<Particle> m_partList;
    double m_arenaSize;
    double m_eps;
    double m_time;
    size_t m_nextWallImpactPart;
    double m_nextWallImpactTime;
    bool m_verbose, m_exportAnim, m_rememberSummary;
    std::string m_export_file;
    BackwardCluster m_bc;
    // Particle[]
    // std::map<double, std::pair<size_t, size_t>> m_list;
};



#endif


