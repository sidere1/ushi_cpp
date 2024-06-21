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
    Particle(size_t ind, double x, double y, double u, double v, double eps, double L);
    bool intersects(const Particle& other) const;
    bool isOutsideTheBox() const;
    bool move(double dt);
    bool computeTimeBeforeNextWall();
    bool wallCollide();
    bool headOnCollide(Particle& other);
    bool sideToSideCollide(Particle& other);

    double inline x() const {return m_x;};
    double inline y() const {return m_y;};
    double inline u() const {return m_u;};
    double inline v() const {return m_v;};
    size_t inline index() const {return m_index;};
    bool setSpeed(double u, double v);

    friend std::ostream& operator<<(std::ostream& os, const Particle& particle); 
    double iWillCollide(Particle other);
    double dtBeforeNextWall() const {return m_dtBeforeNextWall;}


protected: 
    size_t m_index; 
    double m_u, m_v;
    double m_x, m_y;
    double m_eps, m_L, m_maxCoord; // m_maxCoord = m_L/sqrt(2), la coordonnée maximale 
    double m_dtBeforeNextWall;
};





#endif


