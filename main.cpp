#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <string>
#include "DynamicsManager.hpp"
#include <fstream>

using json = nlohmann::json;
using namespace std;

int main(int argc, char** argv)
{
    // récupération du fichier input 
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <config_file_uchi.json>" << endl;
        return 1;
    }
    ifstream configFile(argv[1]);
    if (!configFile) {
        cerr << "Could not open uchi config file " << argv[1] << endl;
        return 1;
    }

    // lectire des paramètres 
    json config;
    configFile >> config;

    size_t N = config["N"];
    double alpha = config["alpha"];
    bool verbose = config["verbose"];
    bool exportAnim = config["exportAnim"];
    string resultsDir = config["resultsDir"];
    bool inTore = config["inTore"];
    bool computeBC = config["computeBC"];
    double dtExport = config["dtExport"];
    double endTime = config["endTime"];
    bool rememberSummary = config["rememberSummary"];
    double arenaSize = config["arenaSize"];
    bool generateFromFile = config["generateFromFile"];
    string partListFile = "";
    if (generateFromFile)
        partListFile = config["partListFile"];
    bool postprocessSolely = config["postprocessSolely"];
    string collisionFile = "";
    if (postprocessSolely)
        collisionFile = config["collisionFile"];

    

    if (postprocessSolely)
    {
        // Lecture du fichier de résultats et post-traitement 
        BackwardCluster bc(N, verbose, resultsDir, dtExport, endTime);
        bc.processExternalFile("/Users/silouane/Documents/suivi/IDEE_Ayi/Code/ushi_cpp/output/collisionList.uchi.saved");
    }
    else 
    {
        // Création du dynamicsmanager et run 
        DynamicsManager d(N, alpha, verbose, exportAnim, resultsDir, inTore, computeBC, dtExport, endTime, rememberSummary, arenaSize, generateFromFile, partListFile);
        d.run();
    }

    
    return 0;
}
