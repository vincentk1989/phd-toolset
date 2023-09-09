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
    bool CalculateExact(vector<NodeClass> NodeList, FacilityClass Facility);
    bool GenerateScenarios(vector<NodeClass> NodeList, FacilityClass Facility, int pos, float currentprobability);
    bool Branch(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints);
    vector<ScenarioClass> DetermineBestOrder(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints);
    float CalculateSkip(vector<NodeClass> NodeList, FacilityClass Facility);
    float CalculateAll(vector<NodeClass> NodeList, FacilityClass Facility);
    vector<vector<int>> DetermineBaseFromConstraints(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints);
};
