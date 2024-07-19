#include "BackwardCluster.hpp"
#include <fstream>
#include <iomanip>
#include <unordered_set>
#include "include/omp.h"

using namespace std;
#define now std::chrono::high_resolution_clock::now() ;

/**
 * @brief Construct a new Backward Cluster object
 * 
 * @param N Number of particles.
 * @param verbose Talkative mode.
 * @param resultDir Directory to save results.
 */
BackwardCluster::BackwardCluster(size_t N, bool verbose, string resultDir):m_n(N), m_verbose(verbose), m_resultDir(resultDir)
{
    m_bc.resize(m_n);
    m_alreadyRExt.resize(m_n);
    
    m_cardFile = m_resultDir + "/cardFile.uchi";
    m_rintFile = m_resultDir + "/rintFile.uchi";
    m_rextFile = m_resultDir + "/rextFile.uchi";
    m_exportFile = m_resultDir + "/backwardCluster.uchi";

    m_fullFiles = false;
}

/**
 * @brief Print the backward clusters to the console.
 * 
 * @param nPart Number of particles to print (default velue is 10).
 * @param nClu Number of clusters to print for each particle (default velue is 10).
 * @return true If printing is successful.
 * @return false If an error occurs.
 */
bool BackwardCluster::printBC(size_t nPart, size_t nClu)
{
    cout << "Backward clusters " << endl; 
    if (nPart < m_n)
        nPart = m_n;
    for (size_t iPart = 0 ; iPart < nPart ; iPart++)
    {
        cout << endl << "BC_" << iPart << " ; K_ " << iPart << " = " << m_bc[iPart].size() << " : " ;
        size_t count = 0;
        for (auto it = m_bc[iPart].begin() ; it != m_bc[iPart].end() && count < nClu ; ++it)
        {
            count++;
            cout << it->first << "(" << it->second << ")" <<" ; ";
        }
    }
    cout << endl;
    return true; 
}

/**
 * @brief Export the backward clusters to a file.
 * 
 * @param nPart Number of particles to export (default velue is 20).
 * @param nClu Number of clusters to export for each particle (default velue is 20).
 * @return true If exporting is successful.
 * @return false If an error occurs.
 */
bool BackwardCluster::exportBC(size_t nPart, size_t nClu)
{
    ofstream outfile(m_exportFile);
    if (!outfile)
        return false; 

    if (nPart < m_n)
        nPart = m_n;
    outfile << "Backward clusters (truncated for the " << nPart << " first particles and their " << nClu << " first items)" << endl; 
    for (size_t iPart = 0 ; iPart < nPart ; iPart++)
    {
        outfile << endl << "BC_" << iPart << " ; K_" << iPart << "+1 = " << m_bc[iPart].size() << " : " ;
        size_t count = 0;
        for (auto it = m_bc[iPart].begin() ; it != m_bc[iPart].end() && count < nClu ; ++it)
        {
            count++;
            outfile << it->first << "(" << it->second << ")" <<" ; ";
        }
    }
    outfile << endl;
    return true; 
}

/**
 * @brief Build backward clusters from a list of collisions.
 * 
 * @param begin Iterator to the beginning of the list.
 * @param end Iterator to the end of the list.
 * @return true If the building is successful.
 * @return false If an error occurs.
 */
bool BackwardCluster::buildFromList(std::map<double, std::pair<size_t, size_t>>::reverse_iterator begin, std::map<double, std::pair<size_t, size_t>>::reverse_iterator end)
{
    std::mt19937 gen(std::random_device{}());
    size_t p1;
    size_t p2;
    
    for (size_t iPart = 0; iPart < m_n; iPart++)
    {
        m_bc[iPart][iPart] = 1;
        for (auto it = begin; it != end; ++it) 
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
    return true; 
}

/**
 * @brief Compute results based on collisions, time step, and end time.
 * 
 * Build the backward clusters, compute at regular intervals some postprocessings and write them in output files 
 * Postprocessings 
 *  * cardinality
 *  * internal recollisions
 *  * external recollisions 
 * 
 * @param cl Collision list.
 * @param dt Export time step.
 * @param endTime End time of the simulation.
 * @return true If computation is successful.
 * @return false If an error occurs.
 */
bool BackwardCluster::computeResults(CollisionList cl, double dt, double endTime)
{
    double t0 = endTime;

    auto t1 = now;
    auto t2 = now;
    double buildTime(0);
    double cardTime(0);
    double rIntTime(0);
    double rExtTime(0);

    initializeCard();
    initializeRint();
    initializeRext();
    size_t nInterval = (size_t)ceil(endTime/dt);

    std::map<double, std::pair<size_t, size_t>> cl_list = cl.list();
    auto it = cl_list.rbegin();
    auto itEnd = cl_list.rend();

    for (size_t iInterval = 0; iInterval < nInterval; iInterval++)
    {
        t0 -= dt;
        auto begin = it;
        while (it != itEnd && it->first >= t0)
        {
            ++it;
        }
        auto end = it;
        t1 = now;
        buildFromList(begin, end);
        t2 = now;
        buildTime += chrono::duration<double, std::milli>(t2-t1).count();
        t1 = now;
        addCard(endTime - t0);
        t2 = now;
        cardTime += chrono::duration<double, std::milli>(t2-t1).count();
        t1 = now;
        addRint(endTime - t0);
        t2 = now;
        rIntTime += chrono::duration<double, std::milli>(t2-t1).count();
        t1 = now;
        // addRext(endTime - t0);
        addRextFast(endTime - t0);
        t2 = now;
        rExtTime += chrono::duration<double, std::milli>(t2-t1).count();
    }
    std::cout << "Backward cluster detailed chrono :" << endl << "building " << buildTime << " ms " << endl << "card : " << cardTime << " ms " << endl << "rInt : " << rIntTime << " ms " << endl << "rExt : " << rExtTime << endl; 
    return true;
}

/**
 * @brief Check if particles p1 and p2 are in the backward cluster of particle i.
 * 
 * @param i Index of the particle whose BC is being built.
 * @param p1 First colliding particle.
 * @param p2 Second colliding particle.
 * @param gen Random number generator.
 */
void BackwardCluster::checkPartInBC(size_t i, size_t p1, size_t p2, std::mt19937 &gen)
{
    std::uniform_int_distribution<> dist(0, 1);
    bool p1inBCi = m_bc[i].find(p1) != m_bc[i].end();
    bool p2inBCi = m_bc[i].find(p2) != m_bc[i].end();
    if (p1inBCi  && !p2inBCi)
    {
        m_bc[i][p2] = 1;
    }
    else if (p2inBCi && !p1inBCi)
    {
        m_bc[i][p1] = 1;
    }
    else if (p1inBCi && p2inBCi)
    {
        if (dist(gen))
            m_bc[i][p2]++;
        else 
            m_bc[i][p1]++;
    }
}

/**
 * @brief Initialize the cardinality output file.
 * 
 * @return true If initialization is successful.
 * @return false If an error occurs.
 */
bool BackwardCluster::initializeCard()
{
    ofstream outfile(m_cardFile.c_str());
    if (!outfile)
        return false; 
    outfile << "t       Card (" << m_n << " particles) " << endl;
    return true;
}

/**
 * @brief Add the cardinality value to the output file.
 * 
 * @param t Current collision tree length (difference bewtween current time and endtime).
 * @return true If addition is successful.
 * @return false If an error occurs.
 */
bool BackwardCluster::addCard(double t)
{
    ofstream outfile(m_cardFile.c_str(), ios::app);
    if (!outfile)
        return false; 
    double card(0);
    
    for (size_t iPart = 0 ; iPart < m_n ; iPart++)
    {
        card += m_bc[iPart].size();
    }
    outfile << std::setw(12) << t << std::setw(12) << card / m_n << endl;
    return true;
}

/**
 * @brief Initialize the internal recollision output file.
 * 
 * @return true If initialization is successful.
 * @return false If an error occurs.
 */
bool BackwardCluster::initializeRint()
{
    ofstream outfile(m_rintFile.c_str());
    if (!outfile)
        return false; 
    outfile << "t    p^{i.r.}" << endl;
    if (m_fullFiles)
    {
        ofstream outfile(m_rintFile + ".csv");
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

/**
 * @brief Add the internal recollision proba to the output file.
 * 
 * @param t Current collision tree length (difference bewtween current time and endtime).
 * @return true If addition is successful.
 * @return false If an error occurs.
 */
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
        if (rint > 0)
            rintProba++;
    }
    outfile << std::setw(12) << t << std::setw(12) << rintProba / m_n << endl;
    
    if (m_fullFiles)
    {
        ofstream outfile(m_rintFile + ".csv", ios::app);
        if (!outfile)
            return false; 
        outfile << t;
        for (size_t iPart = 0; iPart < m_n; iPart++)
        {
            rint = 0;
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

/**
 * @brief Initialize the external recollision output file.
 * 
 * @return true If initialization is successful.
 * @return false If an error occurs.
 */
bool BackwardCluster::initializeRext()
{
    ofstream outfile(m_rextFile.c_str());
    if (!outfile)
        return false; 
    outfile << "t    p^{e.r.}" << endl;
    return true;
}

/**
 * @brief Add the external recollision proba to the output file.
 * 
 * contrary to addRext, this faster version uses m_alreadyRExt to prevent from recomputing all recollisions at each dtExport 
 *  
 * @param t Current collision tree length (difference bewtween current time and endtime).
 * @return true If the execution was successful 
 * @return false If an error occured.
 */
bool BackwardCluster::addRextFast(double t)
{
    ofstream outfile(m_rextFile.c_str(), ios::app);
    if (!outfile)
        return false; 
    
    double rext(0);
    double rextProba(0);
    #pragma omp parallel for reduction(+:rextProba)
    for (size_t iPart = 0; iPart < m_n; iPart++)
    {
        rext = 0;
        const auto& bci = m_bc[iPart];
        for (size_t jPart = iPart+1; jPart < m_n; jPart++)
        {
            if (m_alreadyRExt[iPart].find(jPart) != m_alreadyRExt[iPart].end())
            {
                rext += 1;
                break;
            }
            const auto& bcj = m_bc[jPart];
            for (auto it = bci.begin() ; it != bci.end() ; it++)
            {
                if (bcj.find(it->first) != bcj.end())
                {
                    rext += 1;
                    m_alreadyRExt[iPart][jPart] = true;
                    break;
                } 
            }
        }
        if (rext > 0)
            rextProba++;
    }
    // cout << "rextProba = " << rextProba << " 2*rextProba/(m_n*(m_n-1)) = " << 2*rextProba/(m_n*(m_n-1)) << endl;
    outfile << std::setw(12) << t << std::setw(12) << 2*rextProba/(m_n*(m_n-1)) << endl;
    return true;
}
