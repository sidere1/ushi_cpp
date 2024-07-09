/*! 
Main file
creates a LatexFile class. 
*/

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <iostream>
#include <iomanip>
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
    N = 1000;
    bool verbose = false; 
    bool export_anim = false; 
    DynamicsManager d(N, verbose, export_anim);
    d.run();

    

    return 0;
}
