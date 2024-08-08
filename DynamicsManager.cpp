#include "DynamicsManager.hpp"
#include <fstream>
#include <random>

using namespace std;
using json = nlohmann::json;
#define now std::chrono::high_resolution_clock::now() ;


/**
 * @brief Constructor for DynamicsManager
 *
 * @param N Number of particles
 * @param alpha Controls the particle size, m_eps = m_alpha/m_n 
 * @param verbose Violent console flooding on agreement of all concerned parties 
 * @param exportAnim Enable the writing of the file required for animation creation
 * @param resultDir Directory for result files
 * @param inTore Perdiodic boundary conditions
 * @param computeBC Compute backward cluster
 * @param dtExport Time step for export
 * @param endTime Simulation end time
 * @param rememberSummary Remember collision summary and write corresponding file 
 * @param arenaSize Domain size 
 */
DynamicsManager::DynamicsManager(size_t N, double alpha, bool verbose, bool exportAnim, std::string resultDir, bool inTore, bool computeBC, double dtExport, double endTime, bool rememberSummary, double arenaSize, bool generateFromFile, std::string partListFile):
    m_n(N),
    m_alpha(alpha),
    m_verbose(verbose),
    m_exportAnim(exportAnim),
    // m_BCMatrix(BCMatrix(N)),
    m_CollisionList(CollisionList(N, verbose)),
    m_CollisionSummary(CollisionList(N, verbose, resultDir)),
    m_endTime(endTime),
    m_arenaSize(arenaSize),
    m_generateFromFile(generateFromFile),
    m_partListFile(partListFile),
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
    cout << "Beginning uchi computation " << endl;
    cout << " * m_n = " << m_n << endl << " * m_endTime = " << m_endTime << endl << " * m_arenaSize = " << m_arenaSize << endl << " * m_eps = " << m_eps << endl << " * m_resultDir = " << m_resultDir << endl << " * m_inTore = " << m_inTore << endl << " * m_computeBC = " << m_computeBC << endl << " * m_dtExport = " << m_dtExport << endl;
    cout << "-------------------------- " << endl << endl;
    
    
    cout << "generating particle list" << endl;

    auto t1 = now;
    // generatePartListDebug(); // l'appel à generatePartListDebug() au lieu de generatePartList permet de créer manuellement la liste des particules. 
    // generatePartList();

    if (m_generateFromFile)
        generatePartListFromFile();
    else 
        generatePartList();
    if (verbose)
        cout << "initializing collision list" << endl;
    initializeCL();
    auto t2 = now;
    m_initTime += chrono::duration<double, std::milli>(t2-t1).count();
    if (m_verbose)
        printPartList();
    if (m_exportAnim)
        initialize_anim_file();
}

/**
 * @brief generates the Particle list with random locations
 * 
 * Particle locations are generated randomly, and are rejected if they intersect a previous particle, or if they are too close to the walls
 * 
 * @return bool, true if the execution was successful 
 */
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

/**
 * @brief Generates the Particle list by hand. TO BE REMOVED SHORTLY. 
 *
 * Please do not use me, I am deprecated and will be removed in a very very near future. 
 *
 * @return bool True if the execution was successful
 */
bool DynamicsManager::generatePartListDebug()
{

    // m_eps = 0.01;
    m_n = 13;
    m_eps = m_alpha/m_n;

    // Particle p0 = Particle(0, -0.1, 0.1, 1, 0, m_eps, m_arenaSize);
    // Particle p1 = Particle(1, 0   , 0.21, 0, -1, m_eps, m_arenaSize);
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


/**
 * @brief Generates the Particle list from an input file 
 *
 * Particle locations are generated randomly, and are rejected if they intersect a previous particle, or if they are too close to the walls.
 *
 * @return bool True if the execution was successful
 */
bool DynamicsManager::generatePartListFromFile()
{
    ifstream inputFile(m_partListFile);
    if (!inputFile) {
        cerr << "Could not open uchi config file " << m_partListFile << endl;
        // déclencher un run ? S'arrêter là tout de suite ? Générer aléatoirement ? Une seule particule ? 
        return false;
    }
    cout << "Importing the particle list " << endl;

    json partList;
    inputFile >> partList;
 
    m_n = partList["particles"].size();
    cout << "m_n = " << m_n << endl;
    m_eps = m_alpha/m_n;

    Particle p;
    double x, y, u, v;
    size_t partId(0); 

    for (size_t iPart = 0 ; iPart < m_n ; iPart++)
    {
        x = partList["particles"][iPart]["x"];
        y = partList["particles"][iPart]["y"];
        u = partList["particles"][iPart]["u"];
        v = partList["particles"][iPart]["v"];
        p = Particle(partId, x, y, u, v, m_eps, m_arenaSize);
        if (p.isValid())
        {
            m_partList.push_back(p);
            partId++;
        }
        else 
        {
            cout << "Rejecting invalid particle " << iPart << " : " << p << endl;
        }
    }
    return true;
}

    


/**
 * @brief Prints the Particle list
 *
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::printPartList()
{
    auto t1 = now;
    cout << "--------------------------------" << endl;
    cout << "Particle list " << endl;
    cout << "index   (   x ,   y )    ( u, v)" << endl;
    cout << "--------------------------------" << endl;
    for (size_t iPart = 0; iPart < m_partList.size(); ++iPart)
    {
        cout << m_partList[iPart] << endl;;
    }
    cout << "--------------------------------" << endl << endl;

    auto t2 = now;
    m_verboseTime += chrono::duration<double, std::milli>(t2-t1).count();

    return true;
}


/**
 * @brief Prints the Collision list with details on the locations/speeds of the incriminated particles
 *
 * @param head Number of lines to print 
 * 
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::printDetailedCollisionList(size_t head)
{
    auto t1 = now;
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

    auto t2 = now;
    m_verboseTime += chrono::duration<double, std::milli>(t2-t1).count();

    return true;
}


/**
 * @brief Main simulation call 
 * 
 * Steps : 
 * * initialization(
 * * while time < endTime 
 *      * look for the next collision/wall impact
 *      * jump to the corresponding time and perform the operation 
 * * compute the backward clusters 
 * * output results 
 *
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::run()
{
    pair<double, double> impactLocation(make_pair(0.0,0.0));

    auto t1 = now;
    initializeCL();
    auto t2 = now;
    m_initTime += chrono::duration<double, std::milli>(t2-t1).count();

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
            t1 = now;
            std::pair<size_t, size_t> pair = m_CollisionList.nextColParts();
            size_t p1 = m_partList[pair.first].index(); 
            size_t p2 = m_partList[pair.second].index();  
            if (p1 != p2) //  si la liste est vide, p1 = p2 = 0
                cout << "next collision between " << p1 << " and " << p2 << " in " << dt << " (at t = " << nextColTime << ")" << endl;
            else 
                cout << "no next collision" << endl;
            cout << "next wall impact in " << m_nextWallImpactTime << endl;
            t2 = now;
            m_verboseTime += chrono::duration<double, std::milli>(t2-t1).count();
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
            impactLocation = make_pair(m_partList[m_nextWallImpactPart].x(), m_partList[m_nextWallImpactPart].y());
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
            impactLocation = make_pair(m_partList[p1].x(), m_partList[p1].y());
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
            add_anim_step(impactLocation);
        if (m_verbose)
            m_CollisionList.printList(3);
        else 
            printLoadingBar();
    }
    cout << endl << collisionCount << " collisions happened during the run, and " << wallCount << " wall impacts." << endl;

    std::cout << endl << "Computation chrono :" << endl << "verboseTime : " << m_verboseTime << " ms " << endl << "initTime : " << m_initTime << " ms " << endl << "wallCollideTime : " << m_wallCollideTime << " ms " << endl << "collideTime : " << m_collideTime << " ms" << endl << "moveTime : " << m_moveTime << " ms" << endl << "exportTime : " << m_exportTime << " ms "  << endl << endl;

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
        auto t1 = now;
        m_CollisionSummary.flush();
        if (m_computeBC)
            m_bc.exportBC();
        auto t2 = now;
        m_exportTime += chrono::duration<double, std::milli>(t2-t1).count();
    }
    return true;
}


/**
 * @brief Initializes the collision list
 * 
 * To be called once, at the beginning of the run 
 *
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::initializeCL()
{
    auto t1 = now;

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
    auto t2 = now;
    m_initTime += chrono::duration<double, std::milli>(t2-t1).count();
    return true;
}


/**
 * @brief Manages the Particle displacements 
 *
 * @param dt duration before collision/wall impact 
 * 
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::move(double dt)
{
    auto t1 = now;
    #pragma omp parallel for 
    for (Particle& particle : m_partList) {
        particle.move(dt);
    }
    auto t2 = now;
    m_moveTime += chrono::duration<double, std::milli>(t2-t1).count();
    return true;
}


/**
 * @brief Manages the collisions 
 *
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::collide()
{
    auto t1 = now;
    // on récupère les deux particules de la liste 
    std::pair<size_t, size_t> collidingParts = m_CollisionList.nextColParts();

    size_t p1 = collidingParts.first;
    size_t p2 = collidingParts.second;

    // removing from the list all their collisions
    m_CollisionList.removeColsFromPart(p1);
    m_CollisionList.removeColsFromPart(p2);

    // updating their speed 
    updateSpeedFromCollision(p1, p2);

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
    auto t2 = now;
    m_collideTime += chrono::duration<double, std::milli>(t2-t1).count();

    // updating m_CollisionSummary
    if (m_computeBC || m_rememberSummary)
    {
        m_CollisionSummary.addCollision(m_time, p1, p2);
    }
    return true;
}


/**
 * @brief Manages the wall collisions
 *
 * @param index Id of the particle qui vient de se bouffer un mur
 * 
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::wallCollide(size_t index)
{
    auto t1 = now;
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
    auto t2 = now;
    m_wallCollideTime += chrono::duration<double, std::milli>(t2-t1).count();
    return true;
}

/**
 * @brief Manages the "wall collision" in case of a periodic boundary condition 
 *
 * @param index Id of the particle qui vient de se bouffer un mur
 * 
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::teleport(size_t index)
{
    auto t1 = now;
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
    auto t2 = now;
    m_wallCollideTime += chrono::duration<double, std::milli>(t2-t1).count();
    return true;
}

/**
 * @brief Updates the particle speeds after a collision
 *
 * @param p1 First colliding particle 
 * @param p2 Second colliding particle 
 * 
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
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

/**
 * @brief Computes the next wall collision time 
 *
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
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

/**
 * @brief Animation file creation 
 *
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::initialize_anim_file()
{
    auto t1 = now;

    ofstream outfile(m_export_file.c_str());
    if (!outfile)
        return false;

    outfile << "---------------------- " << endl;
    outfile << "---- Uchi summary file " << endl;
    outfile << "---------------------- " << endl;
    outfile << "nb_particles = " << m_n << " ; eps = " << m_eps << " ; endtime = " << m_endTime << endl;

    auto t2 = now;
    m_exportTime += chrono::duration<double, std::milli>(t2-t1).count();
    
    return true; 
}


/**
 * @brief Add a frame to the animation file 
 * 
 * @param impactLocation Localisation of the collision / wall impact at current time step 
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::add_anim_step(std::pair<double, double> impactLocation)
{
    auto t1 = now;
    ofstream outfile(m_export_file.c_str(), ios::app);
    if (!outfile)
        return false;

    outfile << endl << m_time << endl << impactLocation.first << endl << impactLocation.second << endl;
    for (size_t iPart = 0; iPart < m_partList.size(); ++iPart)
    {
        outfile << m_partList[iPart] << endl;;
    }
    auto t2 = now;
    m_exportTime += chrono::duration<double, std::milli>(t2-t1).count();
    return true; 
}


/**
 * @brief Great function to print a progress bar 
 * 
 * @return true If the operation was successful.
 * @return false Otherwise.
 */
bool DynamicsManager::printLoadingBar() 
{
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




