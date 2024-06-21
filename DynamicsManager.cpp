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
    m_arenaSize = 1;
    double alpha = 1; //  N controls the number of particles, alpha their size and hence the frequency of collisions. For alpha=1, we are in the kinetic regime (Boltzmann) For alpha>>1, we are in the diffusive regime.
    if (m_n > 100)
        m_eps = alpha/m_n;
    else 
        m_eps = 0.1/m_n;
    cout << "m_eps = " <<    m_eps << endl; 
    m_time = 0;
    m_nextWallImpactTime = 1000000;
    // generatePartListDebug();
    cout << "generating part list" << endl;
    generatePartList();
    // printPartList();
    cout << "initializing collision list" << endl;
    initializeCL();
    // m_CollisionList.printList();    
}

bool DynamicsManager::generatePartList()
{
    // Distribution pour des doubles entre 0 et 1 et des entiers entre -1 et 1
    std::random_device rd; // Générateur de nombres aléatoires basé sur du matériel (ou pseudo-aléatoire)
    std::mt19937 gen(rd()); // Mersenne Twister RNG initialisé avec le générateur
    std::uniform_real_distribution<> dis_double(0.0000001, 0.999999);
    // std::uniform_int_distribution<> dis_int(0, 3);
    std::array<int, 3> values = {-1, 0, 1};
    std::uniform_int_distribution<> dis(0, values.size() - 1);


    for (size_t iPart = 0; iPart < m_n; ++iPart)
    {
        double u = (double) values[dis(gen)];
        double v = (double) values[dis(gen)];
        while ((u == 0 && v == 0) || u*v != 0)
        {
            u = (double) values[dis(gen)];
            v = (double) values[dis(gen)];
        }
        bool intersect(false);
        Particle p;
        do 
        {
            intersect = false;
            double xTemp = dis_double(gen)*(m_arenaSize-m_eps);
            double yTemp = dis_double(gen)*(m_arenaSize-m_eps);
            // flip de pi/4 et recentrage comme on tire x et y entre 0 et L 
            double x = (xTemp+yTemp-m_arenaSize)*sqrt(2)/2;
            double y = (-xTemp+yTemp)*sqrt(2)/2;

            p = Particle(iPart, x, y, u, v, m_eps, m_arenaSize);
            for (size_t jPart = 0; jPart < iPart; jPart++)
            {
                if (p.intersects(m_partList[jPart])) 
                    intersect = true;
                if (p.isOutsideTheBox()) 
                    intersect = true;
            }
            // if (intersect)
            // {
                // printPartList();
                // cout << "Rejecting particle " << p << endl;
            // }
        }
        while(intersect);
        m_partList.push_back(p);
    }
    return true;
}


bool DynamicsManager::generatePartListDebug()
{
    // Particle p1 = Particle(1, 0.1, 0, 0, -1, m_eps, m_arenaSize);
    // Particle p2 = Particle(2, 0, -10, 0, -1, m_eps, m_arenaSize);

    Particle p1 = Particle(0, -0.1, 0, 0, 1, m_eps, m_arenaSize);
    Particle p2 = Particle(1, -0.2, 0.1, 1, 0, m_eps, m_arenaSize);
    m_partList.push_back(p1);
    m_partList.push_back(p2);
    return true;
}

bool DynamicsManager::printPartList()
{
    cout << "--------------------------------" << endl;
    cout << "Particle list " << endl;
    cout << "index   (   x ,   y )    ( u, v)" << endl;
    cout << "--------------------------------" << endl;
    for (size_t iPart = 0; iPart < m_partList.size(); ++iPart)
    {
        cout << m_partList[iPart] << endl;;
    }
    cout << "--------------------------------" << endl << endl;

    return true;
}

bool DynamicsManager::run()
{
    initializeCL();
    size_t debugStop(0);
    size_t collisionCount(0);
    size_t wallCount(0);
    m_CollisionList.printList();
    while (m_time < m_endTime)
    {
        if(debugStop++ > 1000)
        {
            cout << "stopping on debug criterion" << endl;
            return true;
        }

        double nextColTime = m_CollisionList.nextColTime();
        if (nextColTime > 0 && nextColTime < m_time)
        {
            cout << "strange error" << endl;
            m_CollisionList.printList();
            printPartList();
            return false; 
        }
        double dt = nextColTime - m_time;
        computeNextWallImpact();
        // debug prints 
        std::pair<size_t, size_t> pair = m_CollisionList.nextColParts();
        size_t p1 = m_partList[pair.first].index(); 
        size_t p2 = m_partList[pair.second].index();  
        cout << "next collision between " << p1 << " and " << p2 << " in " << dt << " (at t = " << nextColTime << ") ; next wall impact in " << m_nextWallImpactTime << endl;
        // end of debug prints 

        if (m_nextWallImpactTime < dt || nextColTime < 0)
        {
            if (m_nextWallImpactTime < 0)
            {
                cout << "Interrupting because of remontage de temps" << endl;
                printPartList();
                m_CollisionList.printList();
                return false;
            }
            m_time += m_nextWallImpactTime;
            cout << "Time = " << m_time << " : wall collision for particle " << m_nextWallImpactPart << endl;
            if (!move(m_nextWallImpactTime))
                return false; 
            if (!wallCollide(m_nextWallImpactPart))
                return false; 
            // if (!updateTraj(m_nextWallImpactPart)) 
            //     return false;
            wallCount++;
        }
        else if (dt >= 0)
        {
            m_time = nextColTime;
            std::pair<size_t, size_t> pair = m_CollisionList.nextColParts();
            size_t p1 = m_partList[pair.first].index(); 
            size_t p2 = m_partList[pair.second].index();  
            cout << "Time = " << m_time << " : collision between " << p1 << " and " << p2 << endl;
            if (!move(dt))
                return false; 
            if (!collide())
                return false; 
            // if (!(updateTraj(p1) || updateTraj(p2)))
            //     return false; 
            collisionCount++;
        }
        else 
        {
            cout << "No valid reason to get here! " << endl; 
            m_CollisionList.printList();
            printPartList();
            return false; 
        }
        // printPartList();
        m_CollisionList.printList(3);
    }
    m_CollisionList.printList();
    cout << collisionCount << " collisions happened during the run, and " << wallCount << " wall impacts." << endl;
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
bool DynamicsManager::updateTraj(size_t index)
{
    // m_CollisionList.removeColsFromNextCollidingParts(); // déjà fait dans collide 
    for (size_t iPart = 0 ; iPart < m_partList.size() ; iPart++) 
    {
        size_t p2 = iPart;
        if (index != p2)
            if (m_partList[index].iWillCollide(m_partList[p2]) > 0)
                m_CollisionList.addCollision(m_partList[index].iWillCollide(m_partList[p2]) + m_time, index, p2);
    }
    return true;
}


bool DynamicsManager::collide()
{
    // on récupère les deux particules de la liste 
    std::pair<size_t, size_t> collidingParts = m_CollisionList.nextColParts();

    size_t p1 = collidingParts.first;
    size_t p2 = collidingParts.second;

    // removing from the list all their collisions
    m_CollisionList.removeColsFromPart(p1);
    m_CollisionList.removeColsFromPart(p2);

    // updating their speed 
    cout << "Before collision, (u1, v1) = (" << m_partList[p1].u() << "," << m_partList[p1].v() << ") ; (u2, v2) = (" << m_partList[p2].u() << "," << m_partList[p2].v() << ") " << endl;
    updateSpeedFromCollision(p1, p2);
    cout << "After collision, (u1, v1) = (" << m_partList[p1].u() << "," << m_partList[p1].v() << ") ; (u2, v2) = (" << m_partList[p2].u() << "," << m_partList[p2].v() << ") " << endl;

    // updating their time before wall impact
    m_partList[p1].computeTimeBeforeNextWall(); 
    m_partList[p2].computeTimeBeforeNextWall(); 

    // updating their next collisions  
    for (size_t jPart = 0 ; jPart < m_partList.size() ; jPart++) 
    {
        size_t p3 = jPart;
        if (p1 != p3)
        {
            double collTime = m_partList[p1].iWillCollide(m_partList[p3]);
            if (collTime > 0) 
            {
                m_CollisionList.addCollision(collTime+m_time, p1, p3);
                if (p2 == p3)
                    cout << "AAAAAAAAAAA " << endl << " strange ! I added the same collision as the one that juste happened " << endl;
            }
        }
        if (p2 != p3)
        {
            double collTime = m_partList[p2].iWillCollide(m_partList[p3]);
            if (collTime > 0) 
            {
                m_CollisionList.addCollision(collTime+m_time, p2, p3);
                if (p1 == p3)
                    cout << "AAAAAAAAAAA " << endl << " strange ! I added the same collision as the one that juste happened " << endl;
            }
        }
    }
    return true;
}

bool DynamicsManager::wallCollide(size_t index)
{
    // poping all its collisions from the list 
    m_CollisionList.removeColsFromPart(index);

    // updating its speed 
    m_partList[index].wallCollide();

    // // updating the time before wall impact
    // m_partList[index].computeTimeBeforeNextWall(); 

    // updating its next collisions 
    for (size_t jPart = 0 ; jPart < m_partList.size() ; jPart++) 
    {
        size_t p2 = jPart;
        if (index != p2)
        {
            double collTime = m_partList[index].iWillCollide(m_partList[p2]);
            if (collTime > 0) m_CollisionList.addCollision(collTime, index, m_partList[p2].index());
        }
    }
    return true;
}

bool DynamicsManager::updateSpeedFromCollision(size_t p1, size_t p2) 
{
    // head-on collision 
    if (m_partList[p1].u()*m_partList[p2].u() == -1 || m_partList[p1].v()*m_partList[p2].v() == -1)
    {
        m_partList[p1].headOnCollide(m_partList[p2]);
        cout << "Head-on collision" << endl;
    }
    // side-to-side collision 
    else if (m_partList[p1].u()*m_partList[p2].v() !=0 || m_partList[p1].v()*m_partList[p2].u() != 0)
    {
        m_partList[p1].sideToSideCollide(m_partList[p2]);
        cout << "Side-to-side collision" << endl;
    }
    else
    {
        cout << "Did not find any collision. " << endl;
        cout << "u1 = " << m_partList[p1].u() << endl;
        cout << "u2 = " << m_partList[p2].u() << endl;
        cout << "v1 = " << m_partList[p1].v() << endl;
        cout << "v2 = " << m_partList[p2].v() << endl;
    }
    return true;
}

bool DynamicsManager::computeNextWallImpact()
{
    m_nextWallImpactTime = 1000;
    for (size_t iPart = 0 ; iPart < m_partList.size() ; iPart++) 
    {
        if (m_partList[iPart].dtBeforeNextWall() < m_nextWallImpactTime)
        {
            m_nextWallImpactTime = m_partList[iPart].dtBeforeNextWall();
            m_nextWallImpactPart = iPart;
        }
    } 
    return true;
}

