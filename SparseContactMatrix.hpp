#ifndef DEF_SPARSECONTACTMATRIX
#define DEF_SPARSECONTACTMATRIX

/*! 
SparseContactMatrix class
*/

#include<iostream> 
// #include <sys/stat.h>
#include <filesystem>
#include <Eigen/Sparse>

class SparseContactMatrix 
{
public:
    SparseContactMatrix(size_t N);
    // SparseContactMatrix(std::string filename, bool verbose);
    bool printMatrix();
    bool addCollision(size_t i, size_t j);
protected: 
    size_t m_n;
    Eigen::SparseMatrix<size_t> m_matrix ;
    std::vector<Eigen::Triplet<size_t>> m_triplets;
};



#endif