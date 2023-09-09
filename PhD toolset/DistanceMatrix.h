#pragma once

#include "bClass.h"

class coordinates {
public:
    float coordinateX;
    float coordinateY;
};

class listCoordinates {
public:
    vector<coordinates> listOfCoordinates;
};

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
    vector<vector<listCoordinates>> DrawMatrix;
	c_DistanceMatrix();
	float NodeDistance(float OriginX, float OriginY, float DestinationX, float DestinationY);
    float RealNodeDistance(float x, float y, float x2, float y2, listCoordinates &drawRoute);
	float NodeDistance(int i, bool fi, int j, bool fii, int ListCount);
	float CalculateRouteLength(std::vector< NodeClass > NodeList, FacilityClass Facility, int RouteNr);
    float CalculateRL(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<int> route);
	float CalculateRouteDemand(std::vector< NodeClass > NodeList, std::vector<int> RouteList);
    float CalculateRouteDemandwP(std::vector< NodeClass > NodeList, std::vector<int> RouteList);
	float CalculateMinimumDistances(int n, FacilityClass Facility, std::vector< NodeClass > NodeList);
	float CalculateTotalCosts(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList);
	float CalculateFacilityCosts(FacilityClass Facility, std::vector< NodeClass > NodeList);
    float WorstCaseCalculateRouteDemand(std::vector< NodeClass > NodeList, std::vector<int> RouteList);
    float AltRouteLength(std::vector< NodeClass > NodeList, FacilityClass Facility, std::vector<int> Route);
	vector<vector<float>> GenerateDM(std::vector< NodeClass > NodeList, std::vector< FacilityClass > FacilityList);
    vector<vector<float>> GenerateRealDM(std::vector< NodeClass > NodeList, std::vector< FacilityClass > FacilityList);
    vector<vector<vector<double>>> GenerateCoordinates(vector<FacilityClass> FacilityList, vector<NodeClass> NodeList);
    void exportCoordinates(vector<NodeClass> NodeList, FacilityClass Facility);
};

