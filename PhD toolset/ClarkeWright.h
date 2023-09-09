#pragma once

#include "bClass.h"
#include "DistanceMatrix.h"
#include "Write.h"


class c_ClarkeWright: public c_Write {
public:
	int GVIterations = 40000;
	int SolutionMethod = 1;
	int GVMaxCoordX = 100;
	int GVMaxCoordY = 100;
	int GVOriginalCosts;
	int GVOptimalCosts;
	int GVstep;
	c_ClarkeWright();
    
    class InsertionPoint {
        public:
        int Route;
        int Pos;
        int Node;
        float Cost;
    };
    
        vector<int> DetermineTour(vector< NodeClass > NodeList, vector<int> RouteList, float LocationX, float LocationY);
    
    float Cross(std::array<float, 3> O, std::array<float, 3> A, std::array<float, 3> B);
    
//    FacilityClass VRP( FacilityClass Facility, vector<NodeClass> NodeList);
//    FacilityClass VRPSS( FacilityClass Facility, vector<NodeClass> NodeList);
    FacilityClass VRPSS2( FacilityClass Facility, vector<NodeClass> NodeList);

    FacilityClass ClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
    vector<int> ConvexHull(vector< NodeClass > NodeList, vector<int> Route, float LocationX, float LocationY);
	FacilityClass ModifiedClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
    FacilityClass GurobiSol(std::vector< NodeClass > NodeList, FacilityClass Facility);
	FacilityClass GurobiSolOptions(std::vector< NodeClass > NodeList, FacilityClass Facility, bool lessorequal, int vehicle_amount);
    FacilityClass GurobiSolAlt(std::vector< NodeClass > NodeList, FacilityClass Facility);
    vector<vector<int>> GurobiSolSecondStage(std::vector< NodeClass > NodeList, FacilityClass Facility);
	FacilityClass RandomizedClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
	FacilityClass TimeWindowsRandomizedClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
    FacilityClass WorstCaseClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
    FacilityClass StageOneClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
    FacilityClass StageTwoClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
    FacilityClass StageTwoAlgorithm(std::vector< NodeClass > NodeList, FacilityClass Facility);
    vector<vector<float>> GurobiAltForm(std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj);
    float GurobiAltFormSecondStage(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<vector<int>> relationships, vector<vector<int>> Constraints);
    vector<vector<int>> GurobiAltFormSecondStageK(std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, int vehicleamount);
    bool SatisfiesConstraints(std::vector< NodeClass > NodeList, FacilityClass ConstrainedFacility, FacilityClass Facility);
	bool TimeWindowsCheck(FacilityClass Facility, std::vector< NodeClass > NodeList, vector<int> route);
	bool TimeWindowsDirection(FacilityClass Facility, std::vector< NodeClass > NodeList, vector<int> route);
    
    vector<vector<int>> GurobiIntegerL(std::vector< NodeClass > NodeList, FacilityClass Facility);
    
    vector<vector<int>> RouteSplitter(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<int> route);
    
    vector<vector<int>> GurobiAFSS(int minva, ScenarioClass sc, std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, vector<vector<int>> Constraints);
    vector<vector<int>> GurobiAFSSLEQ(int minva, ScenarioClass sc, std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, vector<vector<int>> Constraints);
    
    //float GurobiSS_Int(std::vector< NodeClass > NodeList, FacilityClass Facility);
    vector<vector<int>> GurobiAltFormSecondStageKC(std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj);
    
    vector<vector<int>> GurobiScenario(int minva, ScenarioClass sc,std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, vector<vector<int>> Constraints);
        vector<vector<int>> GurobiScenarioK(int minva, ScenarioClass sc,std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, vector<vector<int>> Constraints);
    vector<FacilityClass> GurobiMDVRP(std::vector< NodeClass > NodeList, vector<FacilityClass> FacilityList);
    
    
    InsertionPoint CheapestInsertionPoint(  int request, FacilityClass Facility, vector<NodeClass> NodeList );
    
    
    vector<vector<int>> GurobiScX(int minva, ScenarioClass sc, std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, vector<vector<int>> Constraints);
    vector<vector<int>> GurobiScenarioTest(int minva, ScenarioClass sc, std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, vector<vector<int>> ForbiddenRouteList);
    
    
    vector<vector<int>> GenerateAll( int size, vector<vector<int>> all, vector<int> current, vector<int> remaining );
    
    
    ofstream file;
};
