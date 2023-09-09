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
    float Cross(std::array<float, 3> O, std::array<float, 3> A, std::array<float, 3> B);
    vector<int> DetermineTour(vector< NodeClass > NodeList, vector<int> RouteList, float LocationX, float LocationY);
    FacilityClass ClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
	FacilityClass ModifiedClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility);
	FacilityClass GurobiSol(std::vector< NodeClass > NodeList, FacilityClass Facility);
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
    float GurobiAltFormSecondStageK(std::vector< NodeClass > NodeList, FacilityClass Facility);
    bool SatisfiesConstraints(std::vector< NodeClass > NodeList, FacilityClass ConstrainedFacility, FacilityClass Facility);
	bool TimeWindowsCheck(FacilityClass Facility, std::vector< NodeClass > NodeList, vector<int> route);
	bool TimeWindowsDirection(FacilityClass Facility, std::vector< NodeClass > NodeList, vector<int> route);
    
    float GurobiAFSS(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<vector<int>> relationships);
    
    //float GurobiSS_Int(std::vector< NodeClass > NodeList, FacilityClass Facility);
    vector<vector<int>> GurobiAltFormSecondStageKC(std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj);
    
    vector<vector<int>> GurobiScenario(int minva, ScenarioClass sc,std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, vector<vector<int>> Constraints);
    vector<FacilityClass> GurobiMDVRP(std::vector< NodeClass > NodeList, vector<FacilityClass> FacilityList);
    
    ofstream file;
};
