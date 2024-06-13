#include "DynamicsManager.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include "BCMatrix.hpp"
#include "CollisionList.hpp"
#include <random>

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


DynamicsManager::DynamicsManager(size_t N):m_n(N), m_BCMatrix(BCMatrix(N)),m_CollisionList(CollisionList(N))
{
    // initializing basic stuff 
    m_endTime = 100;
    m_eps = 1;
    m_arenaSize = 100;
    // initializing Particle list 
    generatePartList();
    printPartList();
}

bool DynamicsManager::generatePartList()
{
    // Distribution pour des doubles entre 0 et 1 et des entiers entre -1 et 1
    std::random_device rd; // Générateur de nombres aléatoires basé sur du matériel (ou pseudo-aléatoire)
    std::mt19937 gen(rd()); // Mersenne Twister RNG initialisé avec le générateur
    std::uniform_real_distribution<> dis_double(0.0, 1.0);
    // std::uniform_int_distribution<> dis_int(0, 3);
    std::array<int, 3> values = {-1, 0, 1};
    std::uniform_int_distribution<> dis(0, values.size() - 1);


    for (size_t iPart = 0; iPart < m_n; ++iPart)
    {
        // double u = dis_int(gen);
        // double v = dis_int(gen);
        double u = (double) values[dis(gen)];
        double u = (double) values[dis(gen)];
        while ((u == 0 && v == 0) || u*v == 0)
        {
            u = (double) values[dis(gen)];
            u = (double) values[dis(gen)];
        }
        bool intersect(false);
        Particle p;
        do 
        {
            intersect = false;
            double xTemp = dis_double(gen)*m_arenaSize;
            double yTemp = dis_double(gen)*m_arenaSize;
            // flip de pi/4 et recentrage comme on tire x et y entre 0 et L 
            double x = (xTemp+yTemp-m_arenaSize)*sqrt(2)/2;
            double y = (-xTemp+yTemp)*sqrt(2)/2;

            p = Particle(iPart, x, y, u, v, m_eps);
            for (size_t jPart = 0; jPart < iPart; jPart++)
                if (p.intersects(m_partList[jPart])) intersect = true;
        }
        while(intersect);
        m_partList.push_back(p);
    }
    return true;
}

bool DynamicsManager::printPartList()
{
    cout << "Particle       (x, y)          (u, v)" << endl;
    for (size_t iPart = 0; iPart < m_partList.size(); ++iPart)
    {
        cout << m_partList[iPart] << endl;;
    }
    return true;
}

bool DynamicsManager::run()
{
    initializeCL();
    while (m_time < m_endTime)
    {
        double nextTime = m_CollisionList.nextColTime();
        // faudrait voir si il y a un wallCollision, move until this time
        double dt = nextTime - m_time;
        m_Time = nextTime;
        cout << "t = " << m_time;
        if (!move(dt))
            return false; 
        if (!collide())
            return false; 
        if (!updateTraj())
            return false; 
    }
    return true;
}

bool DynamicsManager::initializeCL()
{
    for (size_t iPart = 0 ; iPart < m_partList.size() ; iPart++) 
    {
        for (size_t jPart = iPart+1 ; jPart < m_partList.size() ; jPart++) 
        {
            if (p1.index() != p2.index())
                if (iWillCollide(p1, p2) > 0)
                    m_CollisionList.addCollision(iWillCollide(p1, p2), p1, p2)
        }
    }
    return true;
}

bool DynamicsManager::move(double dt)
{
    for (Particle& particle : m_partList) {
        particle.move(dt);
    }
    return true;
}
bool DynamicsManager::updateTraj()
{
    return true;
}
bool DynamicsManager::collide()
{
    return true;
}



