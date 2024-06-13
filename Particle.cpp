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
    m_timeBeforeNextWall = 10;
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



