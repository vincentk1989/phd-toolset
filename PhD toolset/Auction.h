#pragma once

#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"

class c_Auction : public c_ClarkeWright {
public:
	vector<vector<vector<int>>> r_Check;
	//vector<vector<float>> InfLim;
	vector<float> CostTracker;
	c_Auction();
    
    vector<vector<float>> Tracker; // original facility -> new facility -> original bid -> new bid
    vector<vector<int>> ExchangedRequests;
    
	int ValuationType = 0;
	float margin = 1;
	std::vector< FacilityClass > OriginalSingleAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int FacilityAmount, ofstream& myfile);
	std::vector< FacilityClass > BlackListSingleAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int FacilityAmount, ofstream& myfile);
	std::vector< FacilityClass > RouteAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int FacilityAmount, ofstream& myfile);
	std::vector< FacilityClass > RecursiveAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile);
	std::vector<vector<int>> r_Auction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int orgf, vector<int> route);
	std::vector< FacilityClass > ConcurrentAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList);
	std::vector< FacilityClass > OptimalSolution(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList);
	std::vector< FacilityClass > ClusterAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList);
    std::vector< FacilityClass > BRRA(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile);

	std::vector<std::array<float, 3>> VFacilityToNode(FacilityClass Facility, std::vector< NodeClass > NodeList);
	std::vector<std::array<float, 3>> VNextAndPrevious(FacilityClass Facility, std::vector< NodeClass > NodeList);
	std::vector<std::array<float, 3>> VSumToOthers(FacilityClass Facility, std::vector< NodeClass > NodeList);
	std::vector<std::array<float, 3>> VClosestNode(FacilityClass Facility, std::vector< NodeClass > NodeList);
	std::vector<std::array<float, 3>> VDaganzo(FacilityClass Facility, std::vector< NodeClass > NodeList);
	std::vector<std::array<float, 3>> VCentroid(FacilityClass Facility, std::vector< NodeClass > NodeList);
	std::vector<std::array<float, 3>> VRandomizedCW(FacilityClass Facility, std::vector< NodeClass > NodeList);
    
    
};
