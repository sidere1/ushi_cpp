#include "DynamicsManager.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include "BCMatrix.hpp"
#include "CollisionList.hpp"
#include "Particle.hpp"
#include <random>

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;
// bugs : 
// * les vitesses sont pas (1, 0) mais (1, 1)
// * 
// * 
// * 

DynamicsManager::DynamicsManager(size_t N):m_n(N), m_BCMatrix(BCMatrix(N)),m_CollisionList(CollisionList(N))
{
    // initializing basic stuff 
    m_endTime = 100;
    m_eps = 1;
    m_arenaSize = 100;
    // initializing Particle list 
    generatePartListDebug();
    printPartList();
    initializeCL();
    m_CollisionList.printList();
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
        double u = (double) values[dis(gen)];
        double v = (double) values[dis(gen)];
        while ((u == 0 && v == 0) || u*v == 0)
        {
            u = (double) values[dis(gen)];
            v = (double) values[dis(gen)];
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


bool DynamicsManager::generatePartListDebug()
{
    Particle p1 = Particle(1, 0, 0, 1, 0, m_eps);
    Particle p2 = Particle(2, 10, 10, 0, -1, m_eps);
    m_partList.push_back(p1);
    m_partList.push_back(p2);
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
        m_time = nextTime;
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
        Particle p1 = m_partList[iPart];
        for (size_t jPart = iPart+1 ; jPart < m_partList.size() ; jPart++) 
        {
            Particle p2 = m_partList[jPart];
            if (p1.index() != p2.index())
            {
                double collTime = p1.iWillCollide(p2);
                if (collTime > 0) m_CollisionList.addCollision(collTime, p1.index(), p2.index());
            }
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
    std::pair<size_t, size_t> pair = m_CollisionList.nextColParts();
    Particle p1 = m_partList[pair.first]; 
    Particle p2 = m_partList[pair.second]; 
    m_CollisionList.removeColsFromNextCollidingParts();
    for (size_t iPart = 0 ; iPart < m_partList.size() ; iPart++) 
    {
        Particle p3 = m_partList[iPart];
        if (p1.index() != p3.index())
            if (p1.iWillCollide(p3) > 0)
                m_CollisionList.addCollision(p1.iWillCollide(p3), p1.index(), p3.index());
        if (p2.index() != p3.index())
            if (p2.iWillCollide(p3) > 0)
                m_CollisionList.addCollision(p2.iWillCollide(p3), p2.index(), p3.index());
    }
    return true;
}
bool DynamicsManager::collide()
{
    return true;
}



