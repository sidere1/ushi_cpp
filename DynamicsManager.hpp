#ifndef DEF_DYNAMICSMANAGER
#define DEF_DYNAMICSMANAGER




/*! 
\class DynamicsManager
\brief Class for managing a Particle list and a CollisionList, performing a simulation and computing the BackwardCluster  
*/

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
    size_t m_n; ///< Number of particles. 
    double m_alpha; ///< Controls the particle size, m_eps = m_alpha/m_n. 
    bool m_verbose; ///< Talkative mode. 
    bool m_exportAnim;
    CollisionList m_CollisionList; ///< Contains all probable next collisions. This list is updated at each wall contact/collision.
    CollisionList m_CollisionSummary; ///< Contains all collision that actually occured, used for the final postprocessing. 
    double m_endTime;
    std::vector<Particle> m_partList; ///< Vector of Particles.
    double m_arenaSize; ///< Domain size.
    double m_eps; ///< Particle size.
    double m_time; ///< Current time.
    size_t m_nextWallImpactPart; ///< Id of the next Particle that will encounter a wall. 
    double m_nextWallImpactTime; ///< Time of the next wall impact.
    bool m_rememberSummary; ///< Should the summary be printed to a file.
    std::string m_export_file; 
    BackwardCluster m_bc; ///< Backward cluster.
    std::string m_resultDir; ///< Result directory name. 
    bool m_inTore, m_computeBC; 
    double m_dtExport; ///< Frequency of result export. 

    // mesure du temps d'execution 
    double m_verboseTime;
    double m_exportTime;
    double m_initTime;
    double m_wallCollideTime;
    double m_collideTime;
    double m_moveTime;
};


#endif

