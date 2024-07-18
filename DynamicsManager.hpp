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


class DynamicsManager 
{
public:
    DynamicsManager(size_t N, double alpha, bool verbose, bool exportAnim, std::string resultDir, bool inTore, bool computeBC, double dtExport, double endTime, bool rememberSummary, double arenaSize);
    bool generatePartList();
    bool generatePartListDebug();
    bool printPartList();
    bool printDetailedCollisionList(size_t head);
    bool run();
    bool initializeCL();
    bool move(double dt);
    bool collide();
    bool wallCollide(size_t index);
    bool teleport(size_t index);
    bool updateSpeedFromCollision(size_t p1, size_t p2);
    bool computeNextWallImpact();
    bool initialize_anim_file();
    bool add_anim_step(std::pair<double, double>);
    bool printLoadingBar();


protected: 
    size_t m_n;
    double m_alpha;
    bool m_verbose;
    bool m_exportAnim;
    BCMatrix m_BCMatrix;
    CollisionList m_CollisionList;
    CollisionList m_CollisionSummary;
    double m_endTime;
    std::vector<Particle> m_partList;
    double m_arenaSize;
    double m_eps;
    double m_time;
    size_t m_nextWallImpactPart;
    double m_nextWallImpactTime;
    bool m_rememberSummary;
    std::string m_export_file;
    BackwardCluster m_bc;
    std::string m_resultDir;
    bool m_inTore, m_computeBC;
    double m_dtExport;

    // mesure du temps d'execution 
    double m_verboseTime;
    double m_exportTime;
    double m_initTime;
    double m_wallCollideTime;
    double m_collideTime;
    double m_moveTime;
};


#endif

