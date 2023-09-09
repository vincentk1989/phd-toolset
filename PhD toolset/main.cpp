#include "Solution.h"
#include <chrono>
#include "include/cdflib.hpp"

int main(int argc, char *argv[]);




int main(int argc, char *argv[])
{
    
    string applicationName = "PhD toolset";
    
    string path = argv[0];
    path = path.substr(0, path.size() - applicationName.length());
    
    c_SolutionCombo SolutionUU = c_SolutionCombo(1, 100, 0, 20, false, 3, path, "U", "U");
    
//    SolutionUU.FacilityList[0] = SolutionUU.ClarkeWright(SolutionUU.NodeList, SolutionUU.FacilityList[0]);
//
//    ofstream myfile;
//    myfile.open ("XUYU.txt");
//    SolutionUU.WriteGraphToFile_Real(SolutionUU.FacilityList, SolutionUU.NodeList, myfile);
//    myfile.close();
    
    //========================================================================================================
    
    c_SolutionCombo SolutionDU = c_SolutionCombo(1, 100, 0, 20, false, 3, path, "D", "U");
    
//    SolutionDU.FacilityList[0] = SolutionDU.ClarkeWright(SolutionDU.NodeList, SolutionDU.FacilityList[0]);
//
//    ofstream myfile2;
//    myfile2.open ("XDYU.txt");
//    SolutionDU.WriteGraphToFile_Real(SolutionDU.FacilityList, SolutionDU.NodeList, myfile2);
//    myfile2.close();
    
    //========================================================================================================
    
    c_SolutionCombo SolutionUD = c_SolutionCombo(1, 100, 0, 20, false, 3, path, "U", "D");
    
//    SolutionUD.FacilityList[0] = SolutionUD.ClarkeWright(SolutionUD.NodeList, SolutionUD.FacilityList[0]);
//
//    ofstream myfile3;
//    myfile3.open ("XUYD.txt");
//    SolutionUD.WriteGraphToFile_Real(SolutionUD.FacilityList, SolutionUD.NodeList, myfile3);
//    myfile3.close();
    
    //========================================================================================================
    
    c_SolutionCombo SolutionDD = c_SolutionCombo(1, 100, 0, 20, false, 3, path, "D", "D");
    
//    SolutionDD.FacilityList[0] = SolutionDD.ClarkeWright(SolutionDD.NodeList, SolutionDD.FacilityList[0]);
//
//    ofstream myfile4;
//    myfile4.open ("XDYD.txt");
//    SolutionDD.WriteGraphToFile_Real(SolutionDD.FacilityList, SolutionDD.NodeList, myfile4);
//    myfile4.close();
    
    //========================================================================================================
    
    c_SolutionCombo SolutionX = c_SolutionCombo(1, 100, 0, 20, false, 3, path, "A", "A");
    SolutionX.NodeList = SolutionUU.NodeList;
    for ( int i = 1 ; i < SolutionDU.NodeList.size(); i++) {
        SolutionX.NodeList.push_back(SolutionDU.NodeList[i]);
    };
    for ( int i = 1 ; i < SolutionUD.NodeList.size(); i++) {
        SolutionX.NodeList.push_back(SolutionUD.NodeList[i]);
    };
    for ( int i = 1 ; i < SolutionDD.NodeList.size(); i++) {
        SolutionX.NodeList.push_back(SolutionDD.NodeList[i]);
    };
    SolutionX.FacilityList[0].ServiceNodes.clear();
    for (int i = 1; i < SolutionX.NodeList.size(); i++) {
        SolutionX.FacilityList[0].ServiceNodes.push_back(i);
    }
    
    SolutionX.DistanceMatrix = SolutionX.GenerateRealDM(SolutionX.NodeList, SolutionX.FacilityList);
    
    SolutionX.FacilityList[0] = SolutionX.ClarkeWright(SolutionX.NodeList, SolutionX.FacilityList[0]);
    
    ofstream myfile5;
    myfile5.open ("XX.txt");
    SolutionX.WriteGraphToFile_Real(SolutionX.FacilityList, SolutionX.NodeList, myfile5);
    myfile5.close();
    
    
    
    return 0;
}

