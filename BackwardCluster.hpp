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
    bool printBC(size_t nPart = 10, size_t nClu = 10);
    bool exportBC(size_t nPart = 10, size_t nClu = 10);
    bool buildFromList(CollisionList cl, double t0, double t1);
    bool buildFromList(std::map<double, std::pair<size_t, size_t>>::reverse_iterator begin, std::map<double, std::pair<size_t, size_t>>::reverse_iterator end);
    // bool buildFromList(std::map<double, std::pair<size_t, size_t>>::iterator begin, std::map<double, std::pair<size_t, size_t>>::iterator end);

    void checkPartInBC(size_t i, size_t p1, size_t p2, std::mt19937 &gen);
    bool computeResults(CollisionList list, double dt, double endTime);
    bool initializeCard();
    bool addCard(double t);
    bool initializeRint();
    bool addRint(double t);
    bool initializeRext();
    // bool addRext(double t); // deprecated 
    bool addRextFast(double t); // prevents from recomputing all previous external recollisions 


protected: 
    size_t m_n;
    std::vector<std::map<size_t, size_t>> m_bc;
    std::vector<std::map<size_t, bool>> m_alreadyRExt; // booléens pour indiquer si une particule est déjà dans les recollisions externes d'une autre, used in addRextFast 
    bool m_verbose, m_fullFiles;
    std::string m_cardFile, m_rintFile, m_rextFile, m_exportFile, m_resultDir;
};


#endif





