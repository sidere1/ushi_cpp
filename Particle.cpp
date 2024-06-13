#include "Particle.hpp"
#include <iostream>


using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


Particle::Particle()
{}

Particle::Particle(size_t ind, double x, double y, double u, double v, double eps):m_index(ind), m_u(u), m_v(v), m_x(x), m_y(y), m_eps(eps)
{
    // check that u/v sont pas tout les deux nuls ni tous les deux non nuls
    if ((u == 0 && v == 0) || u*v == 0)
    {
        cout << "CAUTION ! invalid particle velocity" << endl;
    }
    m_dtBeforeNextWall = computeTimeBeforeNextWall();
}
bool Particle::move(double dt)
{
    m_x += m_u*dt;
    m_y += m_v*dt;
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
}

bool Particle::computeTimeBeforeNextWall()
{
    if (m_u > 0) 
    {// next wall is on the right 
        if (m_y > 0)
        { // next wall is quadran 1
            m_timeBeforeNextWall = 10;
        } 
        else 
        {// next wall is quadran 4
            m_timeBeforeNextWall = 10;
        }
    }
    else if (m_u < 0) 
    {// next wall is on the left 
        if (m_y > 0)
        { // next wall is quadran 2
            m_timeBeforeNextWall = 10;
        } 
        else 
        {// next wall is quadran 3
            m_timeBeforeNextWall = 10;
        }
    }
    else if (m_v > 0) 
    {// next wall is on the top 
        if (m_x > 0)
        { // next wall is quadran 1
            m_timeBeforeNextWall = 10;
        } 
        else 
        {// next wall is quadran 2
            m_timeBeforeNextWall = 10;
        }
    }
    else if (m_v < 0) 
    {// next wall is on the bottom 
        if (m_x > 0)
        { // next wall is quadran 4
            m_timeBeforeNextWall = 10;
        } 
        else 
        {// next wall is quadran 3
            m_timeBeforeNextWall = 10;
        }
    }
    else 
    {
        return false; 
    }
    return true; 
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
        p1.set_speed(p2.u(), p2.v());
        p2.set_speed(u, v);
    }
    else if (p1.u()*p2.v() !=0 || p1.v()*p2.u() != 0)
    {
        double u = p1.u();
        double v = p1.v();
        p1.set_speed(p2.u(), p2.v());
        p2.set_speed(u, v);
    }
    return true;
}

double iWillCollide(Particle& p1, Particle& p2) 
{
    if ()
    {
        if ()
        {
            return ??
        }
        return 0
    }

    return 0
}



