/*! 
Main file
creates a LatexFile class. 
*/

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <iostream>
#include <string>
#include <cassert>
#include "BCMatrix.hpp"
#include "CollisionList.hpp"
#include "DynamicsManager.hpp"
#include "Particle.hpp"

// #include "SparseContactMatrix.hpp"

#include <algorithm>


using namespace std;


// dixit stackoverflow
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
        return *itr;
    return 0;
}
// idem 
bool cmdOptionExists(char** begin, char** end, const std::string& option){return std::find(begin, end, option) != end;}



// int main(int argc, char** argv)
int main()
{
    size_t N;
    N = 10;
    // BCMatrix m(N);
    // m.setTrackedPart(0);
    // m.setTrackedPart(1);
    // m.setTrackedPart(2);
    // m.setTrackedPart(3);
    // m.setTrackedPart(4);
    // m.setTrackedPart(5);
    // m.initialiseBCCardFile();
    // m.addCollision(0, 1);
    // m.addCollision(1, 2);
    // m.addCollision(2, 3);
    // m.addCollision(4, 5);
    // m.printMatrix();
    // m.exportBCCard(55.7);
    // m.exportBCCard(89.8);
    // cout << "Done" << endl;


    // CollisionList l(N);
    // l.addCollision(65.78, 1, 2);
    // l.addCollision(54.77, 2, 4);
    // l.addCollision(54.77, 2, 7);
    // l.addCollision(24.69, 3, 9);
    // l.addCollision(23.25, 1, 8);
    // l.printList(); 
    // l.removeColsFromNextCollidingParts();
    // l.printList(); 


    DynamicsManager d(N);
    
    

    return 0;
}
