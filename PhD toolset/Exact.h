#pragma once

#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"
#include "Auction.h"
#include "LocalIterativeSearch.h"




class c_Exact : public c_LocalIterativeSearch {
public:
    vector<ScenarioClass> Scenarios;
    c_Exact();
    //bool CalculateExact(vector<NodeClass> NodeList, FacilityClass Facility);
    bool CalculateExact(vector<NodeClass> NodeList, FacilityClass Facility);
    bool CalculateIntegerL(vector<NodeClass> NodeList, FacilityClass Facility);
    float CalculateSecondCost(vector<NodeClass> NodeList, FacilityClass Facility);
    bool CalculateExactArrivals(vector<NodeClass> NodeList, FacilityClass Facility);
    float DetermineUB(vector<NodeClass> NodeList, FacilityClass Facility, vector<int> Route, int additionalnode);
    vector<int> determineNewSequence(vector<NodeClass> NodeList, FacilityClass Facility, vector<int> Route, int requesta, int requestb);
    bool GenerateScenarios(vector<NodeClass> NodeList, FacilityClass Facility, int pos, float currentprobability);
    bool BranchIntegerL(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility);
    float CostIntegerL(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> RouteList );
    bool Branch(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints);
    vector<ScenarioClass> DetermineBestOrder(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints);
    float CalculateSkip(vector<NodeClass> NodeList, FacilityClass Facility);
    float CalculateAll(vector<NodeClass> NodeList, FacilityClass Facility);
    vector<vector<int>> DetermineBaseFromConstraints(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints);
    float RunHeuristic(vector<NodeClass> NodeList, FacilityClass Facility, int sample_size);
    bool VectorCompare(vector<int> vector1, vector<int> vector2, bool inversedirection);
    bool VectorContain(vector<int> vector1, vector<int> vector2);
    vector<ScenarioClass> GenerateSamplePresence(vector<NodeClass> NodeList, FacilityClass Facility, int sample_size, int lowerbound, int upperbound);
    vector<ScenarioClass> GenerateSampleDemand(vector<NodeClass> NodeList, FacilityClass Facility, int sample_size, int lowerbound, int upperbound, int presencelevel);
    vector<int> GenerateSampleDemand(vector<NodeClass> NodeList, FacilityClass Facility, int sample_size);
};
