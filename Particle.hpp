#ifndef DEF_PARTICLE
#define DEF_PARTICLE

#include <iostream>


/*! 
Particle class
*/



class Particle 
{
public:
    Particle();
    Particle(size_t ind, double x, double y, double u, double v, double eps);
    bool intersects(const Particle& other) const {
        return (abs(m_x-other.x()) < m_eps) && (abs(m_y-other.y()) < m_eps);
    }
    bool move(double dt);
    bool computeTimeBeforeNextWall();
    bool wallCollide();

    double inline x() const {return m_x;};
    double inline y() const {return m_y;};
    double inline u() const {return m_u;};
    double inline v() const {return m_v;};
    size_t inline index() const {return m_index;};
    bool setSpeed(double u, double v);

    size_t inline index() const {return m_index;};

    friend std::ostream& operator<<(std::ostream& os, const Particle& particle); 

protected: 
    size_t m_index, m_u, m_v;
    double m_x, m_y, m_eps;
    double m_dtBeforeNextWall;
};





#endif


