#pragma once

#include "bClass.h"

class c_DistanceMatrix : public NodeClass, public FacilityClass {
public: 
	int GVMaxCoordX = 100;
	int GVMaxCoordY = 100;
	float floatbigM = 99999999999999;
	int intbigM = 999999999;
    float confidence = 0.9;
    float VehicleCapacity = 12.5;
    int Certain;
    int Uncertain;
	vector<int> Seen;
	vector<vector<float>> DistanceMatrix;
	c_DistanceMatrix();
	float NodeDistance(float OriginX, float OriginY, float DestinationX, float DestinationY);
	float NodeDistance(int i, bool fi, int j, bool fii, int ListCount);
	float CalculateRouteLength(std::vector< NodeClass > NodeList, FacilityClass Facility, int RouteNr);
	float CalculateRouteDemand(std::vector< NodeClass > NodeList, std::vector<int> RouteList);
    float CalculateRouteDemandwP(std::vector< NodeClass > NodeList, std::vector<int> RouteList);
	float CalculateMinimumDistances(int n, FacilityClass Facility, std::vector< NodeClass > NodeList);
	float CalculateTotalCosts(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList);
	float CalculateFacilityCosts(FacilityClass Facility, std::vector< NodeClass > NodeList);
    float WorstCaseCalculateRouteDemand(std::vector< NodeClass > NodeList, std::vector<int> RouteList);
    float AltRouteLength(std::vector< NodeClass > NodeList, FacilityClass Facility, std::vector<int> Route);
	vector<vector<float>> GenerateDM(std::vector< NodeClass > NodeList, std::vector< FacilityClass > FacilityList);
};

