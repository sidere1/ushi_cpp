#include "Particle.hpp"
#include <iostream>


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

Particle::Particle(size_t ind, double x, double y, double u, double v, double eps):m_index(ind), m_u(u), m_v(v), m_x(x), m_y(y), m_eps(eps)
{
    // check that u/v sont pas tout les deux nuls ni tous les deux non nuls
    if ((u == 0 && v == 0) || (u != 0 && v != 0))
    {
        cout << "CAUTION ! invalid particle velocity" << endl;
    }
    m_dtBeforeNextWall = computeTimeBeforeNextWall();
}
bool Particle::move(double dt)
{
    m_x += m_u*dt;
    m_y += m_v*dt;
    return true;
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
    return true;
}

bool Particle::computeTimeBeforeNextWall()
{
    if (m_u > 0) 
    {// next wall is on the right 
        if (m_y > 0)
        { // next wall is quadran 1
            m_dtBeforeNextWall = 10;
        } 
        else 
        {// next wall is quadran 4
            m_dtBeforeNextWall = 10;
        }
    }
    else if (m_u < 0) 
    {// next wall is on the left 
        if (m_y > 0)
        { // next wall is quadran 2
            m_dtBeforeNextWall = 10;
        } 
        else 
        {// next wall is quadran 3
            m_dtBeforeNextWall = 10;
        }
    }
    else if (m_v > 0) 
    {// next wall is on the top 
        if (m_x > 0)
        { // next wall is quadran 1
            m_dtBeforeNextWall = 10;
        } 
        else 
        {// next wall is quadran 2
            m_dtBeforeNextWall = 10;
        }
    }
    else if (m_v < 0) 
    {// next wall is on the bottom 
        if (m_x > 0)
        { // next wall is quadran 4
            m_dtBeforeNextWall = 10;
        } 
        else 
        {// next wall is quadran 3
            m_dtBeforeNextWall = 10;
        }
    }
    else 
    {
        return false; 
    }
    return true; 
}

/**
 * @brief test if it will collide with another particle 
 * 
 * @param Particle other, particle to check for collision  
 * @return double : time delta between collision, negative if they won't collide. 
 */
double Particle::iWillCollide(Particle other) 
{
    cout << "Looking for collision between " << m_index << " and " << other.index() << endl;
    // making sure they are not the same particle
    if (other.index() == m_index)
        return -1;

    // head-on collisions
    if ((abs(m_y-other.y()) < m_eps) && m_u*other.u() < 0)
    {// particles are moving on the same horizontal line with inverted speeds
        if ((other.x()-m_x)*m_u > 0)
        {// they are getting closer 
            cout << "horizontal head-on collision" << endl; 
            return abs(other.x()-m_x);// /!\ true if v1=v2=1 
        }
        // else
        //     cout << "(other.x()-m_x)*m_u == " << other.x() << " - " << m_x << " * " << m_u << (other.x()-m_x)*m_u << endl;
        return -1;
    }
    if ((abs(m_x-other.x()) < m_eps) && m_v*other.v() < 0)
    {// particles are moving on the same vertical line with inverted speeds
        if ((other.y()-m_y)*m_v > 0)
        {// they are getting closer 
            cout << "vertical head-on collision" << endl; 
            return abs(other.y()-m_y);// /!\ true if v1=v2=1 
        }
        return -1;
    }

    // side-to-side collisions 
    if ((m_u*other.v() !=0) || m_v*other.u() !=0)
    {
        if (m_u != 0)
        {
            if (abs((other.x() - m_x)/sign(m_u) - (m_y - other.y())/sign(other.v())) < m_eps)
            {// archi élégant ! ya puka prier pour que ça marche. 
                cout << "A side-to-side collision" << endl; 
                return abs(m_x-other.x());// /!\ true if v1=v2=1 
            }
        }
        else if (m_v != 0)
        {
            if (abs((other.y() - m_y)/sign(m_v) - (m_x - other.x())/sign(other.u())) < m_eps)
            {
                cout << "B side-to-side collision" << endl; 
                return abs(m_y-other.y());// /!\ true if v1=v2=1 
            }
        }
        else 
        {
            cout << "Hum, not sure I'm supposed to get here... " << endl;
            return -1;
        }
    }
    
    return -1;
}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Particle& particle) {
    os << particle.index() << "     (" << particle.x() << "," << particle.y() << ")" << "     (" << particle.u() << "," << particle.v() << ")";
    return os;
}

bool collide(Particle& p1, Particle& p2) 
{
    if (p1.u()*p2.u() == -1)
    {
        // A FAIRE !!!!! 
        double u = p1.u();
        double v = p1.v();
        p1.setSpeed(p2.u(), p2.v());
        p2.setSpeed(u, v);
    }
    else if (p1.u()*p2.v() !=0 || p1.v()*p2.u() != 0)
    {
        double u = p1.u();
        double v = p1.v();
        p1.setSpeed(p2.u(), p2.v());
        p2.setSpeed(u, v);
    }
    return true;
}




