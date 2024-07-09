#include "Particle.hpp"
#include <iostream>
#include <iomanip>



using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


int sign(double i) {
    if (i >= 0)
        return 1;
    else
        return -1;
}

Particle::Particle()
{}

bool Particle::intersects(const Particle& other) const 
{
    if  ((abs(m_x-other.x()) < m_eps) && (abs(m_y-other.y()) < m_eps))
    {
        // cout << "m_eps = " << m_eps << " ; abs(m_x-other.x()) = " << abs(m_x-other.x()) << " ; (abs(m_y-other.y() = " << abs(m_y-other.y()) << endl;
        return true ;
    }
    return false;
}

bool Particle::isOutsideTheBox() const
{
    if  ((abs(m_x) + abs(m_y) + m_eps > m_maxCoord))
    {
        // cout << "Elle est trop loin ta particule mon gars !"  << endl;
        return true ;
    }
    if  ((abs(m_x) < 0.0001) ||(abs(m_y) < 0.0001)  )
    {
        // cout << "ça risque de taper bizarrement dans les coins, dans le doute je préfère virer ce cas-là" << endl;
        return true ;
    }
    return false;
}


Particle::Particle(size_t ind, double x, double y, double u, double v, double eps, double L):m_index(ind), m_u(u), m_v(v), m_x(x), m_y(y), m_eps(eps), m_L(L), m_maxCoord(m_L*sqrt(2)/2)
{
    // check that u/v sont pas tout les deux nuls ni tous les deux non nuls
    if ((u == 0 && v == 0) || (u != 0 && v != 0))
    {
        cout << "CAUTION ! invalid particle velocity" << endl;
    }
    computeTimeBeforeNextWall();
}
bool Particle::move(double dt)
{
    if (dt < 0)
    {
        cout << "Nom de Zeus Marty ! Tu essaies de remonter le temps ! " << endl;
        return false; 
    }
    // cout << "moving with dt = " << dt << endl;
    // cout << m_x << "becomes " << m_x + m_u*dt << endl;
    // cout << m_y << "becomes " << m_y + m_v*dt << endl;
    m_x += m_u*dt;
    m_y += m_v*dt;
    return computeTimeBeforeNextWall();
}

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
        return false; 
    }
    return computeTimeBeforeNextWall();
}

/**
 * @brief test if it will collide with another particle 
 * 
 * @param Particle other, particle to check for collision  
 * @return double : time delta between collision, negative if they won't collide. 
 */
double Particle::iWillCollide(Particle other) 
{
    if (other.index() == m_index) // making sure they are not the same particle
        return -1;

    // head-on collisions
    if ((abs(m_y-other.y()) < m_eps) && m_u*other.u() < 0)
    {// particles are moving on the same horizontal line with inverted speeds
        if ((other.x()-m_x)*m_u > 0) return abs(other.x()-m_x)-m_eps; // they are getting closer : horizontal head-on collision /!\ true if v1=v2=1 
        return -1;
    }
    if ((abs(m_x-other.x()) < m_eps) && m_v*other.v() < 0)
    {// particles are moving on the same vertical line with inverted speeds
        if ((other.y()-m_y)*m_v > 0) return abs(other.y()-m_y)-m_eps;// they are getting closer : vertical head-on collision   /!\ true if v1=v2=1 
        return -1;
    }

    // side-to-side collisions 
    if ((m_u*other.v() !=0) || m_v*other.u() !=0)
    {
        if (abs(other.x() - m_x) == abs(other.y() - m_y))
        {
            double dt = abs(m_x-other.x());
            if ((abs((m_x + m_u*dt) - (other.x()+other.u()*dt)) < m_eps) && (abs((m_y += m_v*dt) - (other.y()+other.v()*dt)) < m_eps)) 
                return dt-m_eps; //A side-to-side collision
        }
    }
    return -1;
}

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

bool Particle::sideToSideCollide(Particle& other)
{    
    double u = m_u;
    double v = m_v;
    setSpeed(other.u(), other.v());
    other.setSpeed(u, v);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Particle& particle) {
    os << std::setw(4) << particle.index() << "    (" <<  std::setw(5) << particle.x() << "," << std::setw(5) << particle.y() << ")" << "    (" << std::setw(2) <<particle.u() << "," <<  std::setw(2) << particle.v() << ")";
    return os;
}





