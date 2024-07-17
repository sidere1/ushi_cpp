#include "DynamicsManager.hpp"
#include <fstream>
// #include <boost/filesystem.hpp>
#include "BCMatrix.hpp"
#include "CollisionList.hpp"
#include "Particle.hpp"
#include <random>

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

DynamicsManager::DynamicsManager(size_t N, double alpha, bool verbose, bool exportAnim, std::string resultDir, bool inTore, bool computeBC, double dtExport, double endTime, bool rememberSummary, double arenaSize):
    m_n(N),
    m_alpha(alpha),
    m_verbose(verbose),
    m_exportAnim(exportAnim),
    m_BCMatrix(BCMatrix(N)),
    m_CollisionList(CollisionList(N, verbose)),
    m_CollisionSummary(CollisionList(N, verbose, resultDir)),
    m_endTime(endTime),
    m_arenaSize(arenaSize),
    m_eps(m_alpha/m_n),
    m_time(0),
    m_nextWallImpactTime(1000000),
    m_rememberSummary(rememberSummary),
    m_export_file(resultDir + "/summary.uchi"),
    m_bc(BackwardCluster(N, verbose, resultDir)),
    m_resultDir(resultDir),
    m_inTore(inTore),
    m_computeBC(computeBC),
    m_dtExport(dtExport),
    m_verboseTime(0),
    m_exportTime(0),
    m_initTime(0),
    m_wallCollideTime(0),
    m_collideTime(0),
    m_moveTime(0)
{
    cout << "-------------------------- " << endl;
    cout << "Beginning ushi computation " << endl;
    cout << " * m_n = " << m_n << endl << " * m_endTime = " << m_endTime << endl << " * m_arenaSize = " << m_arenaSize << endl << " * m_eps = " << m_eps << endl << " * m_resultDir = " << m_resultDir << endl << " * m_inTore = " << m_inTore << endl << " * m_computeBC = " << m_computeBC << endl << " * m_dtExport = " << m_dtExport << endl;
    cout << "-------------------------- " << endl << endl;
    
    if (verbose)
        cout << "generating part list" << endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    // generatePartListDebug();
    generatePartList();
    if (verbose)
        cout << "initializing collision list" << endl;
    initializeCL();
    auto t2 = std::chrono::high_resolution_clock::now();
    m_initTime += std::chrono::duration<double, std::milli>(t2-t1).count();
    if (m_verbose)
        printPartList();
    if (m_exportAnim)
        initialize_anim_file();
}

bool DynamicsManager::generatePartList()
{
    // Distribution pour des doubles entre 0 et 1 et des entiers entre -1 et 1
    std::random_device rd; // Générateur de nombres aléatoires basé sur du matériel (ou pseudo-aléatoire)
    std::mt19937 gen(rd()); // Mersenne Twister RNG initialisé avec le générateur
    std::uniform_real_distribution<> dis_double(0.0000001, 0.999999);
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
        }
        while(intersect);
        m_partList.push_back(p);
    }
    return true;
}


bool DynamicsManager::generatePartListDebug()
{
    m_eps = m_alpha/m_n;
    m_n = 13;
    Particle p0 = Particle(0, -0.5000002 , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p1 = Particle(1, -0.40000001, 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p2 = Particle(2, -0.30001   , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p3 = Particle(3, -0.20002   , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p4 = Particle(4, -0.10003   , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p5 = Particle(5,  0.10004   , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p6 = Particle(6,  0.20005   , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p7 = Particle(7,  0.30006   , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p8 = Particle(8,  0.40007   , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p9 = Particle(9,  0.5001    , 0.1, 1, 0, m_eps, m_arenaSize);
    Particle p10 = Particle(10,0.000001    , 0.1, 1, 0, m_eps, m_arenaSize);

    Particle p11 = Particle(11, -0.4332, -0.1, 1 , 0, m_eps, m_arenaSize);
    Particle p12 = Particle(12, -0.1, -0.4332, 0, 1, m_eps, m_arenaSize);
    m_partList.push_back(p0);
    m_partList.push_back(p1);
    m_partList.push_back(p2);
    m_partList.push_back(p3);
    m_partList.push_back(p4);
    m_partList.push_back(p5);
    m_partList.push_back(p6);
    m_partList.push_back(p7);
    m_partList.push_back(p8);
    m_partList.push_back(p9);
    m_partList.push_back(p10);
    m_partList.push_back(p11);
    m_partList.push_back(p12);
    return true;
}

bool DynamicsManager::printPartList()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    cout << "--------------------------------" << endl;
    cout << "Particle list " << endl;
    cout << "index   (   x ,   y )    ( u, v)" << endl;
    cout << "--------------------------------" << endl;
    for (size_t iPart = 0; iPart < m_partList.size(); ++iPart)
    {
        cout << m_partList[iPart] << endl;;
    }
    cout << "--------------------------------" << endl << endl;

    auto t2 = std::chrono::high_resolution_clock::now();
    m_verboseTime += std::chrono::duration<double, std::milli>(t2-t1).count();

    return true;
}
bool DynamicsManager::printDetailedCollisionList(size_t head)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    cout << "--------------------------------" << endl;
    cout << "Detailed collision list head " << endl;
    cout << "  Time        i    j    (     xi   ,     yi   )(    x    j,    yj    )(ui,vi)(uj,vj)" << endl;
    cout << "--------------------------------" << endl;

    size_t count(0);
    for (const auto& [key, value] : m_CollisionList.list()) {
        if (count++ > head)
            break;
        std::cout << std::setw(10) << key << std::setw(5) << value.first << std::setw(5) << value.second;
        std::cout << "    ("<< std::setw(10) << m_partList[value.first].x() << "," << std::setw(10) << m_partList[value.first].x() << ")";
        std::cout << "("<< std::setw(10) << m_partList[value.second].x() << "," << std::setw(10) << m_partList[value.second].x() << ")";
        std::cout << "("<< std::setw(2) << m_partList[value.first].u() << "," << std::setw(2) << m_partList[value.first].v() << ")";
        std::cout << "("<< std::setw(2) << m_partList[value.second].u() << "," << std::setw(2) << m_partList[value.second].v() << ")" << endl;
    }
    cout << "--------------------------------" << endl << endl;

    auto t2 = std::chrono::high_resolution_clock::now();
    m_verboseTime += std::chrono::duration<double, std::milli>(t2-t1).count();

    return true;
}

bool DynamicsManager::run()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    t1 = std::chrono::high_resolution_clock::now();
    initializeCL();
    t2 = std::chrono::high_resolution_clock::now();
    m_initTime += std::chrono::duration<double, std::milli>(t2-t1).count();

    size_t collisionCount(0);
    size_t wallCount(0);
    m_CollisionList.printList(10);
    while (m_time < m_endTime)
    {
        double nextColTime = m_CollisionList.nextColTime();
        if (nextColTime > 0 && nextColTime < m_time)
        {
            m_CollisionList.printList(10);
            printPartList();
            cout << "Interrupting, incorrect next collision time = "<< nextColTime << " at t = " << m_time  << endl;
            return false; 
        }
        double dt = nextColTime - m_time;
        computeNextWallImpact();
        
        if (m_verbose)
        {
            t1 = std::chrono::high_resolution_clock::now();
            std::pair<size_t, size_t> pair = m_CollisionList.nextColParts();
            size_t p1 = m_partList[pair.first].index(); 
            size_t p2 = m_partList[pair.second].index();  
            if (p1 != p2) //  si la liste est vide, p1 = p2 = 0
                cout << "next collision between " << p1 << " and " << p2 << " in " << dt << " (at t = " << nextColTime << ")" << endl;
            else 
                cout << "no next collision" << endl;
            cout << "next wall impact in " << m_nextWallImpactTime << endl;
            t2 = std::chrono::high_resolution_clock::now();
            m_verboseTime += std::chrono::duration<double, std::milli>(t2-t1).count();
        }

        if (m_nextWallImpactTime < dt || nextColTime < 0)
        {
            if (m_nextWallImpactTime < 0)
            {
                cout << endl << "Interrupting, incorrect time management" << endl;
                cout << "m_time : " << m_time << "; next wall impact time : " << m_nextWallImpactTime << "for particle " << m_nextWallImpactPart << "; next collTime : " << nextColTime << endl;
                m_CollisionList.printList(10);
                return false;
            }
            m_time += m_nextWallImpactTime;
            if (m_verbose)
                cout << "Time = " << m_time << " : wall collision for particle " << m_nextWallImpactPart << endl;
            if (!move(m_nextWallImpactTime))
                return false; 
            if (m_inTore)
            {
                if (!teleport(m_nextWallImpactPart))
                    return false; 
            }
            else
            {
                if (!wallCollide(m_nextWallImpactPart))
                    return false; 
            } 
            wallCount++;
        }
        else if (dt >= 0)
        {
            m_time = nextColTime;
            std::pair<size_t, size_t> pair = m_CollisionList.nextColParts();
            size_t p1 = m_partList[pair.first].index(); 
            size_t p2 = m_partList[pair.second].index();  
            if (m_verbose)
                cout << "Time = " << m_time << " : collision between " << p1 << " and " << p2 << endl;
            if (!move(dt))
                return false; 
            if (!collide())
                return false; 
            collisionCount++;
        }
        else 
        {
            m_CollisionList.printList(10);
            printPartList();
            cout << "Interrupting : invalid dt = " << dt << endl; 
            return false; 
        }
        if (m_exportAnim)
            add_anim_step();
        if (m_verbose)
        {
            m_CollisionList.printList(3);
            printDetailedCollisionList(5);
        }
        else 
        { 
            printLoadingBar();
        }
    }
    if (m_verbose)
    {
        m_CollisionList.printList(10);
    }
    cout << endl << collisionCount << " collisions happened during the run, and " << wallCount << " wall impacts." << endl;

    std::cout << endl << "Computation chrono :" << endl << "m_verboseTime : " << m_verboseTime << " ms " << endl << "m_initTime : " << m_initTime << " ms " << endl << "m_wallCollideTime : " << m_wallCollideTime << " ms " << endl << "m_collideTime : " << m_collideTime << " ms" << endl << "m_moveTime : " << m_moveTime << " ms" << endl << "m_exportTime : " << m_exportTime << " ms "  << endl << endl;

    if (m_computeBC)
    {
        // if (m_CollisionSummary.hasFlushed())
        //     m_CollisionSummary.unflush();
        if (m_verbose)
        {
            cout << "Computing backward cluster" << endl;
            m_CollisionSummary.printList(10);
        }
        
        m_bc.computeResults(m_CollisionSummary, m_dtExport, m_endTime);
    }
    if (m_rememberSummary)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        m_CollisionSummary.flush();
        if (m_computeBC)
            m_bc.exportBC();
        auto t2 = std::chrono::high_resolution_clock::now();
        m_exportTime += std::chrono::duration<double, std::milli>(t2-t1).count();
    }
    return true;
}

bool DynamicsManager::initializeCL()
{
    auto t1 = std::chrono::high_resolution_clock::now();

    for (size_t iPart = 0 ; iPart < m_partList.size() ; iPart++) 
    {
        Particle p1 = m_partList[iPart];
        for (size_t jPart = iPart+1 ; jPart < m_partList.size() ; jPart++) 
        {
            Particle p2 = m_partList[jPart];
            if (p1.index() != p2.index())
            {
                double collTime = p1.iWillCollide(p2);
                if (collTime > 0) m_CollisionList.addCollision(collTime, p1.index(), p2.index()); // collTime is a duration and not a time, but m_time == 0 so it's okay here. 
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    m_initTime += std::chrono::duration<double, std::milli>(t2-t1).count();
    return true;
}

bool DynamicsManager::move(double dt)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for 
    for (Particle& particle : m_partList) {
        particle.move(dt);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    m_moveTime += std::chrono::duration<double, std::milli>(t2-t1).count();
    return true;
}

bool DynamicsManager::collide()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    // on récupère les deux particules de la liste 
    std::pair<size_t, size_t> collidingParts = m_CollisionList.nextColParts();

    size_t p1 = collidingParts.first;
    size_t p2 = collidingParts.second;

    // removing from the list all their collisions
    m_CollisionList.removeColsFromPart(p1);
    m_CollisionList.removeColsFromPart(p2);

    // updating their speed 
    if (m_verbose)
        cout << "Before collision, (u1, v1) = (" << m_partList[p1].u() << "," << m_partList[p1].v() << ") ; (u2, v2) = (" << m_partList[p2].u() << "," << m_partList[p2].v() << ") " << endl;

    updateSpeedFromCollision(p1, p2);

    if (m_verbose)
        cout << "After collision, (u1, v1) = (" << m_partList[p1].u() << "," << m_partList[p1].v() << ") ; (u2, v2) = (" << m_partList[p2].u() << "," << m_partList[p2].v() << ") " << endl;

    // updating their time before wall impact
    m_partList[p1].computeTimeBeforeNextWall(); 
    m_partList[p2].computeTimeBeforeNextWall(); 

    // updating their next collisions  
    for (size_t jPart = 0 ; jPart < m_partList.size() ; jPart++) 
    {
        size_t p3 = jPart;
        if ((p1 != p3) && (p2 != p3))
        {
            double collTime = m_partList[p1].iWillCollide(m_partList[p3]);
            if (collTime > 0) 
            {
                m_CollisionList.addCollision(collTime+m_time, p1, p3);
            }
            collTime = m_partList[p2].iWillCollide(m_partList[p3]);
            if (collTime > 0) 
            {
                m_CollisionList.addCollision(collTime+m_time, p2, p3);
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    m_collideTime += std::chrono::duration<double, std::milli>(t2-t1).count();

    // updating m_CollisionSummary
    if (m_computeBC || m_rememberSummary)
    {
        m_CollisionSummary.addCollision(m_time, p1, p2);
    }
    return true;
}

bool DynamicsManager::wallCollide(size_t index)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    // poping all its collisions from the list 
    m_CollisionList.removeColsFromPart(index);

    // updating its speed 
    m_partList[index].wallCollide();

    // updating its next collisions 
    for (size_t jPart = 0 ; jPart < m_partList.size() ; jPart++) 
    {
        size_t p2 = jPart;
        if (index != p2)
        {
            double collTime = m_partList[index].iWillCollide(m_partList[p2]);
            if (collTime > 0) 
            {
                m_CollisionList.addCollision(collTime + m_time, index, m_partList[p2].index());
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    m_wallCollideTime += std::chrono::duration<double, std::milli>(t2-t1).count();
    return true;
}

bool DynamicsManager::teleport(size_t index)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    // in the tore, instead of a velocity change, there is a position change : we teleport the particle on another side 

    // poping all its collisions from the list 
    m_CollisionList.removeColsFromPart(index);

    // updating its speed 
    m_partList[index].teleport();

    // updating its next collisions 
    for (size_t jPart = 0 ; jPart < m_partList.size() ; jPart++) 
    {
        size_t p2 = jPart;
        if (index != p2)
        {
            double collTime = m_partList[index].iWillCollide(m_partList[p2]);
            if (collTime > 0)
            {
                m_CollisionList.addCollision(collTime + m_time, index, m_partList[p2].index());
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    m_wallCollideTime += std::chrono::duration<double, std::milli>(t2-t1).count();
    return true;
}


bool DynamicsManager::updateSpeedFromCollision(size_t p1, size_t p2) 
{
    // head-on collision 
    if (m_partList[p1].u()*m_partList[p2].u() == -1 || m_partList[p1].v()*m_partList[p2].v() == -1)
    {
        m_partList[p1].headOnCollide(m_partList[p2]);
        if (m_verbose)
            cout << "Head-on collision" << endl;
    }
    // side-to-side collision 
    else if (m_partList[p1].u()*m_partList[p2].v() !=0 || m_partList[p1].v()*m_partList[p2].u() != 0)
    {
        m_partList[p1].sideToSideCollide(m_partList[p2]);
        if (m_verbose)
            cout << "Side-to-side collision" << endl;
    }
    else
    {
        cout << "Did not find any collision. " << endl;
        cout << "u1 = " << m_partList[p1].u() << endl;
        cout << "u2 = " << m_partList[p2].u() << endl;
        cout << "v1 = " << m_partList[p1].v() << endl;
        cout << "v2 = " << m_partList[p2].v() << endl;
        return false; 
    }
    return true;
}

bool DynamicsManager::computeNextWallImpact()
{
    m_nextWallImpactTime = 100000;
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

bool DynamicsManager::initialize_anim_file()
{
    auto t1 = std::chrono::high_resolution_clock::now();

    ofstream outfile(m_export_file.c_str());
    if (!outfile)
        return false;

    outfile << "---------------------- " << endl;
    outfile << "---- Ushi summary file " << endl;
    outfile << "---------------------- " << endl;
    outfile << "nb_particles = " << m_n << " ; eps = " << m_eps << " ; endtime = " << m_endTime << endl;

    auto t2 = std::chrono::high_resolution_clock::now();
    m_exportTime += std::chrono::duration<double, std::milli>(t2-t1).count();
    
    return true; 
}

bool DynamicsManager::add_anim_step()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    ofstream outfile(m_export_file.c_str(), ios::app);
    if (!outfile)
        return false;

    outfile << endl << m_time << endl;
    for (size_t iPart = 0; iPart < m_partList.size(); ++iPart)
    {
        outfile << m_partList[iPart] << endl;;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    m_exportTime += std::chrono::duration<double, std::milli>(t2-t1).count();
    return true; 
}


/**
 * @brief Great function to print a progress bar 
 * 
 * @return bool, true if the execution was successful 
 */
bool DynamicsManager::printLoadingBar() {
    // 
    int barWidth = 50; // Largeur de la barre de progression
    double progress = m_time / m_endTime;
    int pos = static_cast<int>(barWidth * progress);

    // erasing the line and reprinting 
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "*";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %";
    std::cout.flush();
    return true; 
}




