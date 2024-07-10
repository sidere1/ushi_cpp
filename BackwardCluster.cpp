#include "BackwardCluster.hpp"
#include <fstream>
#include <iomanip>
#include <random>
#include <boost/filesystem.hpp>

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


BackwardCluster::BackwardCluster(size_t N, bool verbose):m_n(N), m_verbose(verbose)
{
    m_bc.resize(m_n);
    
    m_cardFile = "cardFile.ushi";
    m_rintFile = "rintFile.ushi";
    m_rextFile = "rextFile.ushi";

    m_fullFiles = false;
}

bool BackwardCluster::printBC(size_t nPart, size_t nClu)
{
    cout << "Backward clusters " << endl; 
    for (size_t iPart = 0 ; iPart < nPart ; iPart++)
    {
        cout << endl << "BC_" << iPart << " of size " << m_bc[iPart].size() << " : " ;
        size_t count = 0;
        for (auto it = m_bc[iPart].begin() ; it != m_bc[iPart].end() && count < nClu ; ++it)
        {
            count++;
            cout << it->first << "(" << it->second << ")" <<" ; ";
        }
    }
    return true; 
}

bool BackwardCluster::buildFromList(CollisionList cl, double t0, double t1)
{
    // auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    std::mt19937 gen(std::random_device{}());
    size_t p1;
    size_t p2;
    double t;
    if (t0 > t1)
    {
        cout << "Invalid parameters in builtFromList, t0 = " << t0 << ", t1 = " << t1 << endl;
        return false; 
    }

    std::map<double, std::pair<size_t, size_t>> cl_list = cl.list();
    for (size_t iPart = 0; iPart < m_n; iPart++)
    {
        m_bc[iPart][iPart] = 1;
        for (auto it = cl_list.rbegin(); it != cl_list.rend(); ++it) 
        {
            t = it->first;
            if (t < t0)
            {
                break;
            }
            else if (t < t1)
            {
                p1 = it->second.first;
                p2 = it->second.second;
                if (p1 == iPart)
                {
                    if (m_bc[iPart].find(p2) == m_bc[iPart].end()) 
                        m_bc[iPart][p2] = 1; // entry p2 does not exist in the bc of iPart
                    else 
                        m_bc[iPart][p2]++; // incrementing the count 
                }
                else if (p2 == iPart)
                {
                    if (m_bc[iPart].find(p1) == m_bc[iPart].end()) 
                        m_bc[iPart][p1] = 1; // entry p1 does not exist in the bc of iPart
                    else 
                        m_bc[iPart][p1]++; // incrementing the count 
                }
                else 
                {
                    checkPartInBC(iPart, p1, p2, gen);
                }
            }
        }
    }
    return true; 
}

void BackwardCluster::checkPartInBC(size_t i, size_t p1, size_t p2, std::mt19937 &gen)
{
    // gen et std::uniform_int_distribution<> dist permettent de générer aléatoirement des booléens, voir ci-dessous.  
    std::uniform_int_distribution<> dist(0, 1);
    bool p1inBCi = m_bc[i].find(p1) != m_bc[i].end();
    bool p2inBCi = m_bc[i].find(p2) != m_bc[i].end();
    if (p1inBCi  && !p2inBCi) // 1
    {
        m_bc[i][p2] = 1;
    }
    else if (p2inBCi && !p1inBCi) // 2
    {
        m_bc[i][p1] = 1;
    }
    else if (p1inBCi && p2inBCi) // 3
    {
        if (dist(gen)) // là, ça vaut aléatoirement true/false 
            m_bc[i][p2]++;
        else 
            m_bc[i][p1]++;
    }
}

bool BackwardCluster::computeResults(CollisionList cl, double dt, double endTime)
{
    double t1 = endTime;
    double t0 = t1-dt;

    initializeCard();
    initializeRint();
    initializeRext();
    size_t nInterval = (size_t)ceil(endTime/dt);
    for (size_t iInterval = 0; iInterval < nInterval; iInterval++)
    {
        t0 = t1-dt;
        buildFromList(cl, t0, t1);
        addCard(endTime - t0);
        addRint(endTime - t0);
        addRext(endTime - t0);
        t1 = t0;
    }
    return true; 
}

bool BackwardCluster::initializeCard()
{
    ofstream outfile(m_cardFile.c_str());
    if (!outfile)
        return false; 
    outfile << "t       Card " << endl;
    return true;
}
bool BackwardCluster::addCard(double t)
{
    ofstream outfile(m_cardFile.c_str(), ios::app);
    if (!outfile)
        return false; 
    double card(0);
    
    for (size_t iPart = 0 ; iPart < m_n ; iPart++)
    {
        card+= m_bc[iPart].size();
    }
    outfile << std::setw(12) << t << std::setw(12) << card/m_n << endl;
    return true;
}
bool BackwardCluster::initializeRint()
{
    ofstream outfile(m_rintFile.c_str());
    if (!outfile)
        return false; 
    outfile << "t    p^{i.r.}" << endl;
    if (m_fullFiles)
    {
        // fichier complet csv
        ofstream outfile(m_rintFile+".csv");
        if (!outfile)
            return false; 
        outfile << "t" << endl;
        for (size_t iPart = 0; iPart < m_n; iPart++)
        {
            outfile << ",R_int_" << iPart;
        }
        outfile << endl;
    }
    return true;
}
bool BackwardCluster::addRint(double t)
{
    ofstream outfile(m_rintFile.c_str(), ios::app);
    if (!outfile)
        return false; 
    
    double rint(0);
    double rintProba(0);
    for (size_t iPart = 0; iPart < m_n; iPart++)
    {
        rint = 0;
        for (auto it = m_bc[iPart].begin() ; it != m_bc[iPart].end() ; it++)
        {
            rint += it->second;
        }
        rint -= m_bc[iPart].size();
        // cout << "rint = " << rint << endl;
        if (rint > 0)
            rintProba++;
    }
    outfile << std::setw(12) << t << std::setw(12) << rintProba/m_n << endl;
    
    if (m_fullFiles)
    {
        ofstream outfile(m_rintFile+".csv", ios::app);
        if (!outfile)
            return false; 
        outfile << t;
        double rint(0);
        for (size_t iPart = 0; iPart < m_n; iPart++)
        {
            for (auto it = m_bc[iPart].begin() ; it != m_bc[iPart].end() ; it++)
            {
                rint += it->second;
            }
            rint -= m_bc[iPart].size();
            outfile << "," << rint;
        }
        outfile << endl;
    }
    return true;
}


bool BackwardCluster::initializeRext()
{
    ofstream outfile(m_rextFile.c_str());
    if (!outfile)
        return false; 
    outfile << "t    p^{e.r.}" << endl;
    return true;
}
bool BackwardCluster::addRext(double t)
{
    ofstream outfile(m_rextFile.c_str(), ios::app);
    if (!outfile)
        return false; 
    
    double rext(0);
    double rextProba(0);
    for (size_t iPart = 0; iPart < m_n; iPart++)
    {
        rext = 0;
        auto bci = m_bc[iPart];
        for (size_t jPart = iPart+1; jPart < m_n; jPart++)
        {
            auto bcj = m_bc[jPart];
            for (auto it = bci.begin() ; it != bci.end() ; it++)
            {
                if (bcj.find(it->first) != bcj.end())
                {
                    rext += 1;
                    break;
                } 
            }
        }
        if (rext > 0)
            rextProba++;
    }
    outfile << std::setw(12) << t << std::setw(12) << 2*rextProba/(m_n*(m_n-1)) << endl;
    return true;
}
