#include "SparseContactMatrix.hpp"
#include <fstream>
#include <boost/filesystem.hpp>

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


// SparseContactMatrix::SparseContactMatrix(std::string filename, bool verbose):m_filename(filename), m_imFlat(false), m_verbose(verbose)
SparseContactMatrix::SparseContactMatrix(size_t N):m_n(N)
{
    // m_matrix = Eigen::Matrix<size_t>(m_n, m_n);
    // cout << m_matrix << endl;
    // m_matrix(1,2)+=1;
    // cout << m_matrix << endl;
    // m_triplet = std::vector<Eigen::Triplet>;
}

bool SparseContactMatrix::printMatrix() 
{
    // cout << m_matrix << endl;
    // cout << "Matrix printed, of size" << m_matrix.size() << endl;
    Eigen::SparseMatrix<size_t> m(m_n, m_n);
    m.setFromTriplets(m_triplets.begin(), m_triplets.end());
    Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> m_full = Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>(m);
    cout << m_full << endl;
    return true;
}

bool SparseContactMatrix::addCollision(size_t i, size_t j) 
{
    // m_matrix(i,j)+=1;
    // cout << "Collision added" << endl;
    m_triplets.push_back(Eigen::Triplet<size_t>(i, j, 1));
    return true;
}
