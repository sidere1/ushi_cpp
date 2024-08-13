#ifndef DEF_BACKWARDCLUSTER
#define DEF_BACKWARDCLUSTER
#include <random>
#include "CollisionList.hpp"

/*! 
\class BackwardCluster
\brief Class for managing backward clusters and a few postprocessings, built from a CollisionList
*/

class BackwardCluster
{
public:
    BackwardCluster(size_t N, bool verbose, std::string resultDir, double dtExport, double endTime);
    bool printBC(size_t nPart = 10, size_t nClu = 10);
    bool exportBC(size_t nPart = 20, size_t nClu = 20);
    // bool buildFromList(CollisionList cl, double t0, double t1);
    bool buildFromList(std::map<double, std::pair<size_t, size_t>>::reverse_iterator begin, std::map<double, std::pair<size_t, size_t>>::reverse_iterator end);

    void checkPartInBC(size_t i, size_t p1, size_t p2, std::mt19937 &gen);
    // bool computeResults(CollisionList list, double dt, double endTime); // now in the constructor 
    bool computeResults(CollisionList list); 
    
    bool processExternalFile(std::string collisionFile);

    bool initializeCard();
    bool addCard(double t);
    bool initializeRint();
    bool addRint(double t);
    bool initializeRext();
    // bool addRext(double t); // deprecated 
    bool addRextFast(double t); // prevents from recomputing all previous external recollisions 
    bool printLoadingBar(double t, double endTime);


protected: 
    size_t m_n; ///< Number of particles.
    std::vector<std::map<size_t, size_t>> m_bc; ///< backward cluster, represented by a vector of maps : the key of the maps are the particle ids, the value is the number of collisions
    std::vector<std::map<size_t, bool>> m_alreadyRExt; ///< booléens pour indiquer si une particule est déjà dans les recollisions externes d'une autre, used in addRextFast 
    // std::vector<std::unordered_map<size_t, bool>> m_alreadyRExt; ///< booléens pour indiquer si une particule est déjà dans les recollisions externes d'une autre, used in addRextFast 
    bool m_verbose, m_fullFiles; 
    std::string m_cardFile, m_rintFile, m_rextFile, m_exportFile, m_resultDir; ///< filenames 
    double m_dtExport;
    double m_endTime;
};


#endif


