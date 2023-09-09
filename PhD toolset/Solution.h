#pragma once

#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"
#include "Auction.h"
#include "LocalIterativeSearch.h"
#include "Exact.h"

class c_Solution : public c_Exact {
public:
	int FacilityAmount = 3;
	int NodesPerFacility = 20;
	//int VehicleCapacity = 12;
	bool TimeWindows = false;
	std::vector< NodeClass > NodeList;
	std::vector< FacilityClass > FacilityList;
	c_Solution(int id, int NPF, int vc, bool tw);
};

class c_SolutionPresence : public c_Exact {
public:
    int FacilityAmount = 3;
    int NodesPerFacility = 20;
    //int VehicleCapacity = 12;
    bool TimeWindows = false;
    int int_id_i;
    int int_id_j;
    std::vector< NodeClass > NodeList;
    std::vector< FacilityClass > FacilityList;
    c_SolutionPresence(int id, int Certain, int Uncertain, int vc, bool tw);
};

class c_SolutionDemand : public c_Exact {
public:
    int FacilityAmount = 3;
    int NodesPerFacility = 20;
    //int VehicleCapacity = 12;
    bool TimeWindows = false;
    std::vector< NodeClass > NodeList;
    std::vector< FacilityClass > FacilityList;
    c_SolutionDemand(int id, int Certain, int Uncertain, int vc, bool tw, int demandlevels);
};

class c_SolutionCombo : public c_Exact {
public:
    int FacilityAmount = 3;
    int NodesPerFacility = 20;
    //int VehicleCapacity = 12;
    bool TimeWindows = false;
    std::vector< NodeClass > NodeList;
    std::vector< FacilityClass > FacilityList;
    c_SolutionCombo(int id, int Certain, int Uncertain, int vc, bool tw, int demandlevels, string path, string X, string Y);
    std::vector<NodeClass> ReadPCC6(string title);
};


class c_VRPSCDinstance : public c_Exact {
public:
    int FacilityAmount = 1;
    int NodesPerFacility = 20;
    bool TimeWindows = false;
    std::vector< NodeClass > NodeList;
    std::vector< FacilityClass > FacilityList;
    c_VRPSCDinstance(int id, int R, int U, bool tw);
    //FacilityClass FirstStage(std::vector< NodeClass > NodeList, FacilityClass Facility);
    FacilityClass CreateStageOneSolution(std::vector< NodeClass > NodeList, FacilityClass Facility);
    FacilityClass Repair(std::vector< NodeClass > NodeList, FacilityClass Facility);
    FacilityClass ExactSolution(vector< NodeClass > NodeList, FacilityClass Facility);
    float Runinstances(int amount);
};

class c_RealSolution : public c_Exact {
public:
	int FacilityAmount = 3;
	int NodesPerFacility = 1000;
	int VehicleCapacity = 30;
	bool TimeWindows = false;
	std::vector< NodeClass > NodeList;
	std::vector< FacilityClass > FacilityList;
	c_RealSolution(int id, int NPF, int vc);
};

class c_SolomonSolution : public c_Exact {
public:
    int FacilityAmount = 3;
    int NodesPerFacility = 20;
    //int VehicleCapacity = 12;
    bool TimeWindows = false;
    std::vector< NodeClass > NodeList;
    std::vector< FacilityClass > FacilityList;
    c_SolomonSolution(int id, const char* filename, int Certain, int Uncertain, int vc, bool tw, int demandlevels);
};

class c_CordeauSolution : public c_Exact {
public:
    int FacilityAmount = 3;
    int NodesPerFacility = 20;
    //int VehicleCapacity = 12;
    bool TimeWindows = false;
    std::vector< NodeClass > NodeList;
    std::vector< FacilityClass > FacilityList;
    c_CordeauSolution(int id, const char* filename);
};


bool Paper1Runsolution();
