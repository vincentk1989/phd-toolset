#pragma once

#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"
#include "Auction.h"

class c_LocalIterativeSearch : public c_Auction {
public:
    c_LocalIterativeSearch();
    FacilityClass BenchmarkFacility;
    vector<vector<float>> RecursiveScenarios(std::vector< NodeClass > NodeList, FacilityClass Facility, int pos, float currentprobability, vector<vector<float>> ScenarioCostList);
    vector<FacilityClass> RecursivePossibilities(vector<NodeClass> NodeList, FacilityClass Facility, vector<int> KnownNodesRemaining, vector<FacilityClass> Possibilities);
    FacilityClass LocalSearch(vector<NodeClass> NodeList, FacilityClass Facility);
    bool Improvement(vector<NodeClass> NodeList, FacilityClass OriginalFacility, FacilityClass ResultFacility);
    
    float EstimateSecondStageCost(std::vector< NodeClass > NodeList, FacilityClass Facility);
    
    FacilityClass GurobiSolFirstStage(std::vector< NodeClass > NodeList, FacilityClass Facility);
    
    float E_secondstage(vector<NodeClass> NodeList, FacilityClass Facility);
    
    FacilityClass RemoveNode(vector<NodeClass> NodeList, FacilityClass Facility, int RemoveVersion);
    FacilityClass InsertNode(vector<NodeClass> NodeList, FacilityClass Facility, int InsertVersion);
    FacilityClass AllocateTimeWindows(vector<NodeClass> NodeList, FacilityClass Facility, int TimeWindowVersion);
    
    FacilityClass BestRemoval(vector<NodeClass> NodeList, FacilityClass Facility);
    FacilityClass RandomRemoval(vector<NodeClass> NodeList, FacilityClass Facility);
    
    FacilityClass BestInsertion(vector<NodeClass> NodeList, FacilityClass Facility);
    FacilityClass FirstInsertion(vector<NodeClass> NodeList, FacilityClass Facility);
    FacilityClass RandomInsertion(vector<NodeClass> NodeList, FacilityClass Facility);
    
    
    
    bool BranchAndCut(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<vector<int>> Constraints);
    
    vector<vector<float>>  AltRecursiveScenarios(std::vector< NodeClass > NodeList, FacilityClass Facility, int pos, float currentprobability, vector<vector<float>> ScenarioCostList, vector<vector<int>> relationships);
    
    float AE_secondstage(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> relationships);
    
//    vector<vector<vector<float>>> ReturnFunctionals(vector<NodeClass> NodeList, FacilityClass Facility);
//    vector<vector<float>> CheckInsertion(vector<NodeClass> NodeList, int i, int j, vector<int> insertingnodes);
//    vector<vector<int>> ReturnCombinations(vector<vector<int>> combination, vector<int> set, vector<int> remaining);
    
    
    //For Tree search
    float Zstar;
    vector<vector<int>> Xstar;
    
    int TotalNodes;
    int FathomedNodesNonInteger;
    int FathomedNodesInteger;
    
    float TimeNode;
    float TimeLBF;
    float TimeSolution;
    
    vector<float> Lowerbounds;
    vector<float> ActualCosts;
    vector<float> FoundInTime;
    
    
    
    
};
