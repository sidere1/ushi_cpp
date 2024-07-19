#ifndef DEF_PARTICLE
#define DEF_PARTICLE

#include <iostream>


/*! 
\class Particle
\brief Class representing a particle with position, velocity, and methods to handle its movement and collisions.
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
    bool teleport();
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
    size_t m_index; ///< Index of the particle.
    double m_u, m_v; ///< Velocities of the particle.
    double m_x, m_y; ///< Coordinates of the particle.
    double m_eps, m_L, m_maxCoord; ///< Epsilon, length of the box, and m_maxCoord = m_L/sqrt(2), the maximum coordinate.
    double m_dtBeforeNextWall; ///< Time before the particle hits the next wall.

};





#endif


