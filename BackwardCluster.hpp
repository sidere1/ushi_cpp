#ifndef DEF_BACKWARDCLUSTER
#define DEF_BACKWARDCLUSTER

/*! 
BackwardCluster class
*/

#include<iostream> 
#include <filesystem>
#include <Eigen/Dense>
#include <map>
#include <chrono>
#include <utility> // pour std::pair
#include <iomanip> // pour std::put_time
#include <random>
#include "CollisionList.hpp"



class BackwardCluster
{
public:
    BackwardCluster(size_t N, bool verbose, std::string resultDir);
    bool printBC(size_t nPart = 5, size_t nClu = 10);
    bool buildFromList(CollisionList cl, double t0, double t1);
    void checkPartInBC(size_t i, size_t p1, size_t p2, std::mt19937 &gen);
    bool computeResults(CollisionList list, double dt, double endTime);
    bool initializeCard();
    bool addCard(double t);
    bool initializeRint();
    bool addRint(double t);
    bool initializeRext();
    bool addRext(double t);

protected: 
    size_t m_n;
    std::vector<std::map<size_t, size_t>> m_bc;
    bool m_verbose, m_fullFiles;
    std::string m_cardFile, m_rintFile, m_rextFile, m_resultDir;
};


#endif

