#include "Particle.hpp"
#include <iomanip>

using namespace std;

int sign(double i) {
    if (i >= 0)
        return 1;
    else
        return -1;
}

/**
 * @brief Default constructor for the Particle class.
 */
Particle::Particle()
{}


/**
 * @brief Constructs a Particle with specified parameters.
 * 
 * @param ind Index of the particle.
 * @param x X-coordinate of the particle.
 * @param y Y-coordinate of the particle.
 * @param u X-velocity of the particle.
 * @param v Y-velocity of the particle.
 * @param eps particle width (diagonal of the corresponding square).
 * @param L Length of the box.
 */
Particle::Particle(size_t ind, double x, double y, double u, double v, double eps, double L)
    : m_index(ind), m_u(u), m_v(v), m_x(x), m_y(y), m_eps(eps), m_L(L), m_maxCoord(m_L*sqrt(2)/2)
{
    // check that u/v are not both zero nor both non-zero
    if ((u == 0 && v == 0) || (u != 0 && v != 0))
    {
        cout << "CAUTION ! invalid particle velocity" << endl;
    }
    computeTimeBeforeNextWall();
}

/**
 * @brief Checks if this particle is valid (velocity and position)
 * 
 * @return true If the particles is valid.
 * @return false otherwise.
 */
bool Particle::isValid() const 
{
    if (isOutsideTheBox())
        return false;
    if ((u() == 0 && v() == 0) || (u() != 0 && v() != 0))
        return false; 
    return true;
}

/**
 * @brief Checks if this particle intersects with another particle.
 * 
 * @param other The other particle to check intersection with.
 * @return true If the particles intersect.
 * @return false If the particles do not intersect.
 */
bool Particle::intersects(const Particle& other) const 
{
    if  ((abs(m_x-other.x()) < m_eps) && (abs(m_y-other.y()) < m_eps))
    {
        // cout << "m_eps = " << m_eps << " ; abs(m_x-other.x()) = " << abs(m_x-other.x()) << " ; (abs(m_y-other.y() = " << abs(m_y-other.y()) << endl;
        return true ;
    }
    return false;
}

/**
 * @brief Checks if the particle is outside the defined box.
 * 
 * @return true If the particle is outside the box.
 * @return false If the particle is inside the box.
 */
bool Particle::isOutsideTheBox() const
{
    if  ((abs(m_x) + abs(m_y) + m_eps > m_maxCoord*0.98))
    {
        // cout << "Elle est trop loin ta particule mon gars !"  << endl;
        return true ;
    }
    if  ((abs(m_x) < 0.001) ||(abs(m_y) < 0.001)  )
    {
        // cout << "ça risque de taper bizarrement dans les coins, dans le doute je préfère virer ce cas-là" << endl;
        return true ;
    }
    return false;
}


/**
 * @brief Moves the particle by a specified time step.
 * 
 * @param dt Time step to move the particle.
 * @return true If the move was successful and time before next wall impact is computed successfully.
 * @return false If the move was not successful.
 */
bool Particle::move(double dt)
{
    if (dt < 0)
    {
        cout << "Nom de Zeus Marty ! Tu essaies de remonter le temps ! " << endl;
        return false; 
    }
    m_x += m_u*dt;
    m_y += m_v*dt;
    return computeTimeBeforeNextWall();
}

/**
 * @brief Sets the speed of the particle.
 * 
 * @param u New X-velocity of the particle.
 * @param v New Y-velocity of the particle.
 * @return true If the speed was successfully set.
 * @return false If the speed was not valid.
 */
bool Particle::setSpeed(double u, double v)
{
    if (u !=0 && v != 0)
    {
        cout << "Error, illegal speed" << endl;
        return false; 
    }
    if (u ==0 && v == 0)
    {
        cout << "Error, illegal speed" << endl;
        return false; 
    }
    m_u = u;
    m_v = v;
    return computeTimeBeforeNextWall();
}

/**
 * @brief Computes the time before the particle hits the next wall.
 * 
 * @return true If the computation was successful
 * @return false If an error occured.
 */
bool Particle::computeTimeBeforeNextWall()
{
    if (abs(m_x) +abs(m_y) + m_eps/2 > m_L)
    {
        cout << "WARNING ! The particle went too far by " << abs(m_x) +abs(m_y) - m_L << endl;
        return false; 
    }
    if (m_x*m_u > 0 || m_y*m_v > 0)
    {
        m_dtBeforeNextWall = m_maxCoord-abs(m_y)- abs(m_x) - m_eps/2;
    }
    else 
    {
        if (m_u != 0)
            m_dtBeforeNextWall = m_maxCoord - abs(m_y) - m_u*m_x - m_eps/2;
        else if (m_v != 0)
            m_dtBeforeNextWall = m_maxCoord - m_v*m_y - abs(m_x) - m_eps/2;
        else 
            cout << "You vraisemblably created an illegal particle" << endl;
    }

    return true; 
}

/**
 * @brief Handles the wall collision 
 * 
 * @return true If the collision was successfully handled.
 * @return false If an error occurred.
 */
bool Particle::wallCollide()
{
    if (m_u > 0) 
    {// next wall is on the right 
        m_u = 0;
        if (m_y > 0)
        { // next wall is quadran 1
            m_v = -1;
        } 
        else 
        {// next wall is quadran 4
            m_v = 1;
        }
    }
    else if (m_u < 0) 
    {// next wall is on the left 
        m_u = 0;
        if (m_y > 0)
        { // next wall is quadran 2
            m_v = -1;
        } 
        else 
        {// next wall is quadran 3
            m_v = 1;
        }
    }
    else if (m_v > 0) 
    {// next wall is on the top 
        m_v = 0;
        if (m_x > 0)
        { // next wall is quadran 1
            m_u = -1;
        } 
        else 
        {// next wall is quadran 2
            m_u = 1;
        }
    }
    else if (m_v < 0) 
    {// next wall is on the bottom 
        m_v = 0;
        if (m_x > 0)
        { // next wall is quadran 4
            m_u = -1;
        } 
        else 
        {// next wall is quadran 3
            m_u = 1;
        }
    }
    else 
    {
        cout << "I'm not supposed to get here, Particle velocity does not seem to be correct : m_u = " << m_u << " ; m_v = " << m_v << endl;
        return false; 
    }
    return computeTimeBeforeNextWall();
}

/**
 * @brief Teleports the particle to the opposite side to emulate a periodic boundary condition 
 * 
 * @return true If the teleportation was successful.
 * @return false If an error occurred.
 */
bool Particle::teleport()
{
    if (m_u != 0)
    {
        m_x = -m_x;
    }
    else if (m_v != 0)
    {
        m_y = - m_y;
    }
    else 
    {
        return false; 
    }
    return computeTimeBeforeNextWall();
}

/**
 * @brief Tests if the particle will collide with another particle.
 * 
 * The collisions may be head-on or side-to-side 
 * 
 * @param other The other particle to check for collision.
 * @return double Time delta between collision, negative if they won't collide.
 */
double Particle::iWillCollide(Particle other) 
{
    if (other.index() == m_index) // making sure they are not the same particle
        return -1;

    // head-on collisions
    if ((abs(m_y-other.y()) < m_eps) && m_u*other.u() < 0)
    {// particles are moving on the same horizontal line with inverted speeds
        if ((other.x()-m_x)*m_u > 0) 
        {
            // cout << "Particle " << m_index << " with velocity " << m_u << m_v << " will collide with " << other.index() << " with velocity " << other.u() << other.v() << " in " << (abs(other.x()-m_x))/2 << endl;
            return (abs(other.x()-m_x))/2; // they are getting closer : horizontal head-on collision /!\ true if v1=v2=1 
        }
        return -1;
    }
    if ((abs(m_x-other.x()) < m_eps) && m_v*other.v() < 0)
    {// particles are moving on the same vertical line with inverted speeds
        if ((other.y()-m_y)*m_v > 0)
        {
            // cout << "Particle " << m_index << " with velocity " << m_u << m_v << " will collide with " << other.index() << " with velocity " << other.u() << other.v() << " in " << (abs(other.y()-m_y))/2 << endl;
            return (abs(other.y()-m_y))/2;// they are getting closer : vertical head-on collision   /!\ true if v1=v2=1 
        } 
        return -1;
    }

    // side-to-side collisions 
    if ((m_u*other.v() !=0) || m_v*other.u() !=0)
    {
        double dx = abs(other.x() - m_x);
        double dy = abs(other.y() - m_y);
        double dist1(sqrt(dx*dx+dy*dy));
        if (abs(dx-dy) < m_eps)
        {
            double dt = abs(m_x-other.x());
            dx = abs(other.x()+ other.u()*dt - (m_x + m_u*dt));
            dy = abs(other.y()+ other.v()*dt - (m_y + m_v*dt)); 
            double dist2(sqrt(dx*dx+dy*dy));
            if ((abs((m_x + m_u*dt) - (other.x()+other.u()*dt)) < m_eps) && (abs((m_y + m_v*dt) - (other.y()+other.v()*dt)) < m_eps) && dist2 < dist1) 
                return dt; //A side-to-side collision
                // return dt-m_eps; //A side-to-side collision
        }

    }
    return -1;
}

/**
 * @brief Handles a head-on collision with another particle.
 * 
 * @param other The other particle to collide with.
 * @return true If the collision was successfully handled.
 */
bool Particle::headOnCollide(Particle& other)
{
    double u(0);
    double v(0);
    if (m_u == 1)
    {
        u = 0; v = 1;
    }else if (m_v == 1)
    {
        u = -1; v = 0;
    }else if (m_u == -1)
    {
        u = 0; v = -1;
    }else //(m_v == -1)
    {
        u = 1; v = 0;
    }
    setSpeed(u, v);

    if (other.u() == 1)
    {
        u = 0; v = 1;
    }else if (other.v() == 1)
    {
        u = -1; v = 0;
    }else if (other.u() == -1)
    {
        u = 0; v = -1;
    }else //(other.v() == -1)
    {
        u = 1; v = 0;
    }
    other.setSpeed(u, v);
    return true;
}

/**
 * @brief Handles a side-to-side collision with another particle.
 * 
 * @param other The other particle to collide with.
 * @return true If the collision was successfully handled.
 */
bool Particle::sideToSideCollide(Particle& other)
{    
    double u = m_u;
    double v = m_v;
    setSpeed(other.u(), other.v());
    other.setSpeed(u, v);
    return true;
}

/**
 * @brief Overloads the output stream operator for the Particle class.
 * 
 * @param os The output stream.
 * @param particle The particle to output.
 * @return std::ostream& The output stream with the particle's information.
 */
std::ostream& operator<<(std::ostream& os, const Particle& particle) {
    os << std::setw(4) << particle.index() << "    (" <<  std::setw(5) << particle.x() << "," << std::setw(5) << particle.y() << ")" << "    (" << std::setw(2) <<particle.u() << "," <<  std::setw(2) << particle.v() << ")";
    return os;
}



