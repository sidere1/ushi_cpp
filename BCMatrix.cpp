#include "BCMatrix.hpp"
#include <fstream>
// #include <boost/filesystem.hpp>
#include <fstream>

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


BCMatrix::BCMatrix(size_t N):m_n(N)
{
    m_matrix = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>::Identity(m_n, m_n);
    m_BCCardFilename = "./output/BCCard";
}

bool BCMatrix::printMatrix() 
{
    cout << m_matrix << endl;
    return true;
}

bool BCMatrix::addCollision(size_t i, size_t j) 
{
    if (i > m_n) cout << "you asked for particle " << i << " which does not exist" << endl;
    if (j > m_n) cout << "you asked for particle " << j << " which does not exist" << endl;
    m_matrix.block(i,0, 1, m_n)+=m_matrix.block(j, 0, 1, m_n);
    m_matrix.block(j,0, 1, m_n)+=m_matrix.block(i, 0, 1, m_n);
    return true;
}
bool BCMatrix::setTrackedPart(size_t i) 
{
    m_trackedParts.push_back(i);
    return true;
}


bool BCMatrix::exportBCCard(double t)//, size_t i)
{
    ofstream outFile;
    outFile.open (m_BCCardFilename, std::ios_base::app);
    outFile << t << "   ";
    cout    << t << "   ";
    for (size_t iPart = 0; iPart < m_trackedParts.size() ; iPart++)
    {
        outFile << m_matrix.row(m_trackedParts[iPart]).cast<int>().sum() << " ";
        cout    << m_matrix.row(m_trackedParts[iPart]).cast<int>().sum() << " ";
    }
    outFile << endl;
    cout << endl;
    outFile.close();
    return true; 
}
bool BCMatrix::initialiseBCCardFile()
{
    ofstream outFile;
    outFile.open (m_BCCardFilename);
    outFile << "Time   ";
    for (size_t iPart = 0; iPart < m_trackedParts.size() ; iPart++)
    {
        outFile << m_trackedParts[iPart] << " ";
        cout    << m_trackedParts[iPart] << " ";
    }
    outFile << endl;
    cout << endl;
    outFile.close();
    return true; 
}
