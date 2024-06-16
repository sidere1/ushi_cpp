#ifndef DEF_BCMATRIX
#define DEF_BCMATRIX

/*! 
BCMatrix class
*/

#include<iostream> 
#include <filesystem>
#include <Eigen/Dense>
#include <vector>


class BCMatrix 
{
public:
    BCMatrix(size_t N);
    // BCMatrix(std::string filename, bool verbose);
    bool printMatrix();
    bool addCollision(size_t i, size_t j);
    bool exportBCCard(double t);
    bool initialiseBCCardFile();
    bool setTrackedPart(size_t i);
protected: 
    size_t m_n;
    Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> m_matrix ;
    std::string m_BCCardFilename;
    std::vector<size_t> m_trackedParts;
};



#endif