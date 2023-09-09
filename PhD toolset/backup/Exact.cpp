//# include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"
#include "Auction.h"
#include "LocalIterativeSearch.h"
#include "Exact.h"




c_Exact::c_Exact() {
    
};

bool c_Exact::CalculateExact(vector<NodeClass> NodeList, FacilityClass Facility) {
    int pos = (int)Facility.FirstStageNodes.size();
    GenerateScenarios(NodeList, Facility, pos, 1.0);
    
    Zstar = 0;
    Xstar.clear();
    
    vector<ScenarioClass> sc = Scenarios;
    
    vector<vector<int>> InitialConstraints;
    InitialConstraints.clear();
    
    Branch(sc, NodeList, Facility, InitialConstraints);
    
    return true;
}

float c_Exact::CalculateSkip(vector<NodeClass> NodeList, FacilityClass Facility) {
    int pos = (int)Facility.FirstStageNodes.size();
    GenerateScenarios(NodeList, Facility, pos, 1.0);
    
    Zstar = 0;
    Xstar.clear();
    
    FacilityClass CopyFacility = Facility;
    CopyFacility.ServiceNodes = Facility.FirstStageNodes;
    
    CopyFacility = GurobiSol(NodeList, CopyFacility);
    
    Facility.RouteList = CopyFacility.RouteList;
    
    vector<ScenarioClass> sc = Scenarios;
    
    float savings = 0;
    
    for (int i = 0; i < sc.size(); i++) {
        float obj;
        Facility.ServiceNodes = sc[i].PresentNodes;
        sc[i].CurOpt = GurobiSolSecondStage(NodeList, Facility);
//        if(Facility.FirstStageNodes.size() != sc[i].PresentNodes.size()){
        CopyFacility.RouteList = sc[i].CurOpt;
//        } else {
//            sc[i].CurOpt = CopyFacility.RouteList;
//        }
        sc[i].CurCost = CalculateFacilityCosts(CopyFacility, NodeList);
        savings = savings + sc[i].Probability * sc[i].CurCost;
    }
    
    Zstar = savings;

    return savings;
}

float c_Exact::CalculateAll(vector<NodeClass> NodeList, FacilityClass Facility) {
    int pos = (int)Facility.FirstStageNodes.size();
    GenerateScenarios(NodeList, Facility, pos, 1.0);
    
    Zstar = 0;
    Xstar.clear();
    
    FacilityClass CopyFacility = Facility;
    //CopyFacility.ServiceNodes = Facility.FirstStageNodes;
    
    CopyFacility = GurobiSol(NodeList, CopyFacility);
    
    //remove all second stage nodes.
    
    for (int i = 0; i < CopyFacility.SecondStageNodes.size(); i++) {
        for (int j = 0; j < CopyFacility.RouteList.size(); j++) {
            for (int k = 0; k < CopyFacility.RouteList[j].size(); k++) {
                if( CopyFacility.RouteList[j][k] == CopyFacility.SecondStageNodes[i]) {
                    CopyFacility.RouteList[i].erase(CopyFacility.RouteList[i].begin() + k);
                }
            }
        }
    }
    
    vector<ScenarioClass> sc = Scenarios;
    
    float savings = 0;
    
    for (int i = 0; i < sc.size(); i++) {
        float obj;
        Facility.ServiceNodes = sc[i].PresentNodes;
        sc[i].CurOpt = GurobiSolSecondStage(NodeList, Facility);
        CopyFacility.RouteList = sc[i].CurOpt;
        sc[i].CurCost = CalculateFacilityCosts(CopyFacility, NodeList);
        savings = savings + sc[i].Probability * sc[i].CurCost;
    }
    
    Zstar = savings;
    
    return savings;
}


bool c_Exact::GenerateScenarios(vector<NodeClass> NodeList, FacilityClass Facility, int pos, float currentprobability) {
    float bprob = currentprobability;
    
    bool result = true;
    
    //Calculate Cost of current scenario
    NodeList[pos].Service = true;
    for (int i = 0; i < NodeList[pos].DiscreteDemand.size(); i++) {
        bprob = currentprobability;
        NodeList[pos].Demand = NodeList[pos].DiscreteDemand[i][0];
        bprob = bprob * NodeList[pos].Probability * NodeList[pos].DiscreteDemand[i][2];
        if (pos == NodeList.size() - 1) {
            
            ScenarioClass Scenario;
            Scenario.PresentNodes.clear();
            for (int j = 1; j < NodeList.size(); j++ ) {
                if (NodeList[j].Service == true) {
                    Scenario.PresentNodes.push_back(j);
                }
            }
            
            for (int j = 1; j < NodeList.size(); j++ ) {
                Scenario.Demands.push_back(NodeList[j].Demand);
            }
            
            Scenario.Probability = bprob;
            
            Scenario.CurCost = 0;
            
            Scenarios.push_back(Scenario);
        } else {
            result = GenerateScenarios(NodeList,Facility,pos+1,bprob);
        }
    }
    
    
    if (NodeList[pos].Probability < 1.0) {
        NodeList[pos].Service = false;
        bprob = currentprobability;
        bprob = bprob * NodeList[pos].Probability;
        if (pos == NodeList.size() - 1) {
            
            
            ScenarioClass Scenario;
            Scenario.PresentNodes.clear();
            for (int j = 1; j < NodeList.size(); j++ ) {
                if (NodeList[j].Service == true) {
                    Scenario.PresentNodes.push_back(j);
                }
            }
            
            for (int j = 1; j < NodeList.size(); j++ ) {
                Scenario.Demands.push_back(NodeList[j].Demand);
            }
            
            Scenario.Probability = bprob;
            
            Scenario.CurCost = 0;
            
            Scenarios.push_back(Scenario);
        } else {
            result = GenerateScenarios(NodeList,Facility,pos+1,bprob);
        }
    }
    
    return result;
};

bool c_Exact::Branch(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints) {
    
    vector<ScenarioClass> t_Scenarios = i_Scenarios;
    
    #pragma omp parallel for schedule(static, 5)
    for (int i = 0; i < t_Scenarios.size(); i++) {
        
        cout << "Scenario " << i << " calculated on thread: " << omp_get_thread_num() << endl;
        
        bool valid = true;
        // check for joint

        for (int a = 0; a < Constraints.size(); a++) {
            if (Constraints[a][0] == 0) { //joint
                int r1 = 0;
                int r2 = 0;
                for (int j = 0; j < t_Scenarios[i].CurOpt.size(); j++) {
                    for (int k = 0; k < t_Scenarios[i].CurOpt[j].size(); k++) {
                        if ( t_Scenarios[i].CurOpt[j][k] == Constraints[a][1] ) {
                            r1 = j;
                        }
                        if ( t_Scenarios[i].CurOpt[j][k] == Constraints[a][2] ) {
                            r2 = j;
                        }
                    }
                }
                if (r2 != r1) {
                    valid = false;
                }
            }
            if (Constraints[a][0] == 1) { //disjoint
                int r1 = 0;
                int r2 = 0;
                for (int j = 0; j < t_Scenarios[i].CurOpt.size(); j++) {
                    for (int k = 0; k < t_Scenarios[i].CurOpt[j].size(); k++) {
                        if ( t_Scenarios[i].CurOpt[j][k] == Constraints[a][1] ) {
                            r1 = j;
                        }
                        if ( t_Scenarios[i].CurOpt[j][k] == Constraints[a][2] ) {
                            r2 = j;
                        }
                    }
                }
                if (r2 == r1) {
                    valid = false;
                }
            }
        }
        
        if (valid == false || t_Scenarios[i].CurCost == 0) { //recalculating scenario
            float obj = 0;
            vector<vector<int>> BfC = DetermineBaseFromConstraints(NodeList, Facility, Constraints);
            int minva = BfC.size();
            t_Scenarios[i].CurOpt = GurobiScenario(minva, t_Scenarios[i], NodeList, Facility, &obj, Constraints);
            cout << "Scenario " << i << " calculated on thread: " << omp_get_thread_num() << endl; 
            if (t_Scenarios[i].CurOpt.size() == 0) {
                cout << "ERROR ERROR ERROR ERROR!" << endl;
            }
//            Facility.RouteList = t_Scenarios[i].CurOpt;
            t_Scenarios[i].CurCost = obj;//CalculateFacilityCosts(Facility, NodeList);
        }
    }
    
    float t_Z = 0;
    for (int i = 0; i < t_Scenarios.size(); i++) {
        t_Z = t_Z + (t_Scenarios[i].CurCost * t_Scenarios[i].Probability);
    }
    
    
//    stringstream ss;
//    for (int i = 0; i < Constraints.size(); i++) {
//        if (Constraints[i][0] ==  0) {
//            ss << Constraints[i][1] << " -> " << Constraints[i][2];
//        }
//        if (Constraints[i][0] ==  1) {
//            ss << Constraints[i][1] << " -n-> " << Constraints[i][2];
//        }
//        ss << " & ";
//    }
//
//    ss << t_Z << ".txt";
//
//    string hello = ss.str();
//
//    ofstream xfile;
//    xfile.open(ss.str());
//    for (int i = 0; i < t_Scenarios.size(); i++) {
//        xfile << t_Scenarios[i].CurCost << "\n";
//    }
//    xfile.close();
    
    
    
    // All scenarios are now Complete
    
    // Find relevant pair of requests for Branching.
    
    vector<vector<int>> OnSameRoute;
    vector<vector<int>> NotOnSameRoute;
    OnSameRoute.resize(Facility.FirstStageNodes.size());
    NotOnSameRoute.resize(Facility.FirstStageNodes.size());
    for (int i = 0; i < OnSameRoute.size(); i++) {
        OnSameRoute[i].resize(Facility.FirstStageNodes.size());
        NotOnSameRoute[i].resize(Facility.FirstStageNodes.size());
    }
    for (int i = 0; i < OnSameRoute.size(); i++) {
        for (int j = 0; j < OnSameRoute[i].size(); j++) {
            OnSameRoute[i][j] = 0;
            NotOnSameRoute[i][j] = 0;
        }
    }
    
    for (int i = 0; i < t_Scenarios.size(); i++) {
        for (int j = 0; j < t_Scenarios[i].CurOpt.size(); j++) {
            if (t_Scenarios[i].CurOpt[j].size() > 1) {
                for (int k = 0; k < t_Scenarios[i].CurOpt[j].size() - 1; k++) {
                    OnSameRoute[t_Scenarios[i].CurOpt[j][k] - 1][t_Scenarios[i].CurOpt[j][k + 1] - 1] += 1 ;
                    OnSameRoute[t_Scenarios[i].CurOpt[j][k + 1] - 1][t_Scenarios[i].CurOpt[j][k] - 1] += 1 ;
                }
            }
        }
    }
    
    for (int i = 0; i < t_Scenarios.size(); i++) {
        for (int j = 0; j < t_Scenarios[i].CurOpt.size(); j++) {
            if (t_Scenarios[i].CurOpt[j].size() > 1) {
                for (int k = 0; k < t_Scenarios[i].CurOpt[j].size(); k++) {
                    for (int w = j+1; w < t_Scenarios[i].CurOpt.size(); w++) {
                        if (t_Scenarios[i].CurOpt[w].size() > 1) {
                            for (int u = 0; u < t_Scenarios[i].CurOpt[w].size(); u++) {
                                NotOnSameRoute[t_Scenarios[i].CurOpt[j][k] - 1][t_Scenarios[i].CurOpt[w][u] - 1] += 1;
                                NotOnSameRoute[t_Scenarios[i].CurOpt[w][u] - 1][t_Scenarios[i].CurOpt[j][k] - 1] += 1;
                            }
                        }
                    }
                }
            }
        }
    }
    
    vector<vector<int>> BranchingPossibilities;
    vector<vector<float>> BranchingProportions;
    
    bool ff = false;
    
    for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
        for ( int j = i + 1; j < Facility.FirstStageNodes.size(); j++){
            if (OnSameRoute[i][j] != 0 && NotOnSameRoute[i][j] != 0) {
                ff = true;
                BranchingPossibilities.push_back(vector<int>());
                BranchingPossibilities[BranchingPossibilities.size()-1].push_back(i);
                BranchingPossibilities[BranchingPossibilities.size()-1].push_back(j);
                BranchingProportions.push_back(vector<float>());
                float test1 = OnSameRoute[i][j];
                float test2 = (OnSameRoute[i][j] + NotOnSameRoute[i][j]);
                float test3 = test1 / test2;
                BranchingProportions[BranchingProportions.size()-1].push_back(test3);
                test1 = NotOnSameRoute[i][j];
                test2 = (OnSameRoute[i][j] + NotOnSameRoute[i][j]);
                test3 = test1 / test2;
                BranchingProportions[BranchingProportions.size()-1].push_back(test3);
            }
        }
    }
    

    
    if (ff == false) { // If no branching pair exists -> candidate found
        // Candidate Found -> update where necessary

        if (t_Z > Zstar) {
            
            t_Scenarios = DetermineBestOrder(t_Scenarios, NodeList, Facility, Constraints);
            
            t_Z = 0;
            for (int i = 0; i < t_Scenarios.size(); i++) {
                t_Z = t_Z + (t_Scenarios[i].CurCost * t_Scenarios[i].Probability);
            }
            
            Zstar = t_Z;
            Xstar = t_Scenarios[0].CurOpt;
            
            cout << " CANDIDATE FOUND! Z: " << t_Z << endl;
        }
    } else {
        if (t_Z > Zstar) {
            // Select Best Branching Possibility
            
            float BestJoint = 0;
            float BestDisjoint = 0;
            int ChosenBranchJoint;
            int ChosenBranchDisjoint;
            
            for (int i = 0; i < BranchingProportions.size(); i++) {
                if(BranchingProportions[i][0] >= BestJoint) {
                    BestJoint = BranchingProportions[i][0];
                    ChosenBranchJoint = i;
                }
                if(BranchingProportions[i][1] >= BestDisjoint) {
                    BestDisjoint = BranchingProportions[i][1];
                    ChosenBranchDisjoint = i;
                }
            }
            
            if( BestJoint > BestDisjoint ) {
                vector<vector<int>> t_Constraints = Constraints;
                t_Constraints.push_back(vector<int>());
                t_Constraints[t_Constraints.size()-1].push_back(0);
                t_Constraints[t_Constraints.size()-1].push_back(Facility.FirstStageNodes[BranchingPossibilities[ChosenBranchJoint][0]]);
                t_Constraints[t_Constraints.size()-1].push_back(Facility.FirstStageNodes[BranchingPossibilities[ChosenBranchJoint][1]]);
                Branch(t_Scenarios,NodeList,Facility,t_Constraints);
                t_Constraints = Constraints;
                t_Constraints.push_back(vector<int>());
                t_Constraints[t_Constraints.size()-1].push_back(1);
                t_Constraints[t_Constraints.size()-1].push_back(Facility.FirstStageNodes[BranchingPossibilities[ChosenBranchJoint][0]]);
                t_Constraints[t_Constraints.size()-1].push_back(Facility.FirstStageNodes[BranchingPossibilities[ChosenBranchJoint][1]]);
                Branch(t_Scenarios,NodeList,Facility,t_Constraints);
                
            } else {
                vector<vector<int>> t_Constraints = Constraints;
                t_Constraints.push_back(vector<int>());
                t_Constraints[t_Constraints.size()-1].push_back(1);
                t_Constraints[t_Constraints.size()-1].push_back(Facility.FirstStageNodes[BranchingPossibilities[ChosenBranchDisjoint][0]]);
                t_Constraints[t_Constraints.size()-1].push_back(Facility.FirstStageNodes[BranchingPossibilities[ChosenBranchDisjoint][1]]);
                Branch(t_Scenarios,NodeList,Facility,t_Constraints);
                t_Constraints = Constraints;
                t_Constraints.push_back(vector<int>());
                t_Constraints[t_Constraints.size()-1].push_back(0);
                t_Constraints[t_Constraints.size()-1].push_back(Facility.FirstStageNodes[BranchingPossibilities[ChosenBranchDisjoint][0]]);
                t_Constraints[t_Constraints.size()-1].push_back(Facility.FirstStageNodes[BranchingPossibilities[ChosenBranchDisjoint][1]]);
                Branch(t_Scenarios,NodeList,Facility,t_Constraints);
            }
        }
    }
    
    return true;
};

vector<ScenarioClass> c_Exact::DetermineBestOrder(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints) {
    
    //vector<vector<int>> RouteID = DetermineBaseFromConstraints(3, NodeList, Facility, Constraints); //CONTAINS ALL ROUTES (SCRAMBLED)
    
    vector<vector<vector<int>>> PotentialOrders;
    
    vector<vector<int>> RouteID = DetermineBaseFromConstraints( NodeList, Facility, Constraints );
    
    vector<ScenarioClass> o_Scenarios = i_Scenarios;
    
    vector<int> ConstrainedRequests = Facility.FirstStageNodes;
    
    vector<int> OrderID;
    OrderID.resize(o_Scenarios.size());

    for (int i = 0; i < o_Scenarios.size(); i++) { //COLLECT POSSIBLE ORDERINGS
        
        int j = 0;
        int k = 0;

        while (j < o_Scenarios[i].CurOpt.size()) {
            while (k < o_Scenarios[i].CurOpt[j].size()) {
                bool t_found = false;
                for (int w = 0; w < ConstrainedRequests.size(); w++) {
                    if (o_Scenarios[i].CurOpt[j][k] == ConstrainedRequests[w]) {
                        t_found = true;
                    }
                }
                if (t_found == false) {
                    o_Scenarios[i].CurOpt[j].erase(o_Scenarios[i].CurOpt[j].begin() + k);
                } else {
                    k = k + 1;
                }
            }
            j = j + 1;
        }
        
        //PRESORT
    
        int tempID;

        vector<vector<int>> Temp_Order; //CONTAINS CURRENT BASE
        
        for (int w = 0; w < RouteID.size(); w++) {
            for (int j = 0; j < o_Scenarios[i].CurOpt.size(); j++) {
                for (int k = 0; k < o_Scenarios[i].CurOpt[j].size(); k++) {
                    if(o_Scenarios[i].CurOpt[j][k] == RouteID[w][0]) {
                        Temp_Order.push_back(vector<int>());
                        for (int i_k = 0; i_k < o_Scenarios[i].CurOpt[j].size(); i_k++) {
                            Temp_Order[Temp_Order.size()-1].push_back(o_Scenarios[i].CurOpt[j][i_k]);
                        }
                    }
                }
            }
        }
        
        if ( PotentialOrders.size() == 0) {
            PotentialOrders.push_back(vector<vector<int>>());
            PotentialOrders[PotentialOrders.size()-1 ] = Temp_Order;
            OrderID[0] = 0;
        } else {
            
            vector<bool> T_Valid;
            T_Valid.resize(PotentialOrders.size());
            
            for (int j = 0; j < T_Valid.size(); j++) {
                T_Valid[j] = true;
            }
            
            bool CompleteValid = false;
        
            for (int j = 0; j < PotentialOrders.size(); j++) {
                vector<bool> Valid;
                Valid.resize(Temp_Order.size());
                for (int k = 0; k < Valid.size(); k++) {
                    Valid[k] = false;
                }
                
                for (int k = 0; k < Temp_Order.size(); k++) {
                    bool t_f1 = true;
                    for (int w = 0; w < Temp_Order[k].size(); w++) {
                        if (PotentialOrders[j][k][w] != Temp_Order[k][w]) {
                            t_f1 = false;
                        }
                    }
                    
                    bool t_f2 = true;
                    
                    reverse(Temp_Order[k].begin(), Temp_Order[k].end());
                    vector<int> routek = Temp_Order[k];
                    reverse(Temp_Order[k].begin(), Temp_Order[k].end());
                    
                    for (int w = 0; w < routek.size(); w++) {
                        if (PotentialOrders[j][k][w] != routek[w]) {
                            t_f2 = false;
                        }
                    }
                    
                    
                    if (t_f1 == true || t_f2 == true) {
                        Valid[k] = true;
                    }
                }
                

                for (int k = 0; k < Valid.size(); k++) {
                    if (Valid[k] == false) {
                        T_Valid[j] = false;
                    }
                }
            }
            
            for (int j = 0; j < T_Valid.size(); j++) {
                if( T_Valid[j] == true ) {
                    CompleteValid = true;
                    tempID = j;
                }
            }
            
            if (CompleteValid == false) {
                PotentialOrders.push_back(vector<vector<int>>());
                PotentialOrders[PotentialOrders.size()-1 ] = Temp_Order;
                OrderID[i] = (int) PotentialOrders.size()-1;
            } else {
                OrderID[i] = tempID;
            }
        }
        
        if (PotentialOrders.size() > 1) {
            int FinalChoice = 0;
            
            float MaxM = floatbigM;
            
            vector<float> CostOfOrdering;
            CostOfOrdering.resize(PotentialOrders.size());
            for (int k = 0; k < CostOfOrdering.size(); k++) {
                CostOfOrdering[k] = 0;
            }
            
            vector<float> sumscenarios;
            sumscenarios.resize(i_Scenarios.size());
            
            vector<vector<ScenarioClass>> t_Scenarios;
            t_Scenarios.resize(PotentialOrders.size());
            
            for (int j = 0; j < PotentialOrders.size(); j++) {
                t_Scenarios[j] = o_Scenarios;
                for (int k = 0; k < t_Scenarios[j].size(); k++) {
                    if (OrderID[k] != j) {
                        float obj;
                        FacilityClass t_Facility = Facility;
                        t_Facility.RouteList = PotentialOrders[j];
                        t_Scenarios[j][k].CurOpt = GurobiAltFormSecondStageKC(NodeList, t_Facility, &obj);
                        sumscenarios[k] = obj;
                    }
                }
                
                for (int k = 0; k < sumscenarios.size(); k++) {
                    CostOfOrdering[j] = CostOfOrdering[j] + (sumscenarios[k] * i_Scenarios[k].Probability);
                }
                
                if (CostOfOrdering[j] < MaxM) {
                    MaxM = CostOfOrdering[j];
                    FinalChoice = j;
                }
            }
            
            o_Scenarios = t_Scenarios[FinalChoice];
        } else {
            o_Scenarios = i_Scenarios;
        }
    }
    
    return o_Scenarios;
}
//
//
//vector<vector<int>> c_Exact::DetermineBaseFromConstraints(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints) {
//    vector<vector<int>> result;
//
//    vector<vector<bool>> PresentinRoute;
//
//    PresentinRoute.resize(Facility.FirstStageNodes.size());
//
//    float sumdemands = 0;
//    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
//        sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
//    }
//    int VehicleAmount = ceil(sumdemands / Facility.Capacity);
//
//    for (int i = 0; i < PresentinRoute.size(); i++) {
//        PresentinRoute[i].resize(VehicleAmount);
//        for(int j = 0; j < PresentinRoute[i].size(); j++) {
//            PresentinRoute[i][j] = false;
//        }
//    }
//
//    bool ProcessDisjoint = false;
//
//    for (int i = 0; i < Constraints.size(); i++) {
//        if (Constraints[i][0]==1) {
//            ProcessDisjoint = true;
//        };
//    }
//
//    vector<bool> Considered;
//
//    //result.resize(VehicleAmount);
//    for ( int i = 0; i < Constraints.size(); i++) {
//        int r1 = -1;
//        int r2 = -1;
//        if (Constraints[i][0] == 0) { // joint
//
//            for (int j = 0; j < result.size(); j++) {
//                for (int k = 0; k < result[j].size(); k++) {
//                    if (result[j][k] == Constraints[i][1]) {
//                        r1 = j;
//                    }
//                    if (result[j][k] == Constraints[i][2]) {
//                        r2 = j;
//                    }
//                }
//            }
//            if( r1 != r2) {
//                if (r1 >= 0 && r2 < 0) {
//                    result[r1].push_back(Constraints[i][2]);
//                    for(int j = 0; j < PresentinRoute[Constraints[i][2]-1].size(); j++) {
//                        PresentinRoute[Constraints[i][2]-1][j] = true;
//                    }
//                }
//                if (r1 < 0 && r2 >= 0) {
//                    result[r2].push_back(Constraints[i][1]);
//                    for(int j = 0; j < PresentinRoute[Constraints[i][1]-1].size(); j++) {
//                        PresentinRoute[Constraints[i][1]-1][j] = true;
//                    }
//                }
//                if (r1 >= 0 && r2 >= 0) {
//                    for (int j = 0; j < result[r2].size(); j++ ) {
//                        result[r1].push_back(result[r2][j]);
//                    }
//                    result.erase(result.begin() + r2);
//                }
//            } else {
//                if (Constraints[i][0] == 0 && r1 < 0 && r2 < 0) {
//                    result.push_back(vector<int> ());
//                    result[result.size()-1].push_back(Constraints[i][1]);
//                    result[result.size()-1].push_back(Constraints[i][2]);
//                    for(int j = 0; j < PresentinRoute[Constraints[i][1]-1].size(); j++) {
//                        PresentinRoute[Constraints[i][1]-1][j] = true;
//                    }
//                    for(int j = 0; j < PresentinRoute[Constraints[i][2]-1].size(); j++) {
//                        PresentinRoute[Constraints[i][2]-1][j] = true;
//                    }
//                }
//            }
//        }
//    }
//
//    if (ProcessDisjoint == true) {
//
//        int DisjointNode;
//
//        bool DoneAction = true;
//
//        while (DoneAction == true ) {
//
//            for (int i = 0; i < Constraints.size(); i++) {
//
//                int r1 = -1;
//                int r2 = -1;
//                DoneAction = false;
//                if (Constraints[i][0] == 1) { // disjoint
//                    for (int j = 0; j < result.size(); j++) {
//                        for (int k = 0; k < result[j].size(); k++) {
//                            if (result[j][k] == Constraints[i][1]) {
//                                r1 = j;
//                            }
//                            if (result[j][k] == Constraints[i][2]) {
//                                r2 = j;
//                            }
//                        }
//                    }
//                    if( r1 != r2) {
//                        if (r1 >= 0 && r2 < 0) {
//                            PresentinRoute[Constraints[i][2]-1][r1] = true;
//                            DisjointNode = Constraints[i][2];
//                            DoneAction = true;
//                        }
//                        if (r1 < 0 && r2 >= 0) {
//                            PresentinRoute[Constraints[i][1]-1][r2] = true;
//                            DisjointNode = Constraints[i][1];
//                            DoneAction = true;
//                        }
//                    }
//
//                    for( int j = 0; j < PresentinRoute.size(); j++) {
//                        int total = 0;
//                        for (int k = 0; k < PresentinRoute[j].size(); k++) {
//                            if (PresentinRoute[j][k] == false) {
//                                total = total + 1;
//                            }
//                        }
//                        if (total == 1) {
//                            result.push_back(vector<int>());
//                            result[result.size()-1].push_back(DisjointNode);
//                            PresentinRoute[j][result.size()-1] = true;
//                            DoneAction = true;
//                        }
//                        if (total > 1) {
//                            result.push_back(vector<int>());
//                            result[result.size()-1].push_back(DisjointNode);
//                            PresentinRoute[j][result.size()-1] = true;
//                            DoneAction = true;
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    return result;
//}


vector<vector<int>> c_Exact::DetermineBaseFromConstraints(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints) {
    vector<vector<int>> result;
    
    vector<vector<bool>> PresentinRoute;
    
    if (Constraints.size() > 0) {
        GRBEnv env = GRBEnv();
        
        GRBModel model = GRBModel(env);
        model.getEnv().set(GRB_IntParam_OutputFlag, 0);
        model.getEnv().set(GRB_IntParam_Threads, 8);
        
        vector<GRBVar> x;
        x.resize(Facility.FirstStageNodes.size());
        
        GRBVar **a;
        a = new GRBVar*[Facility.FirstStageNodes.size()];
        for (int i = 0; i < Facility.FirstStageNodes.size(); i++)
            a[i] = new GRBVar[Facility.FirstStageNodes.size()];
        
        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
            string s = "X_" + to_string(i);
            x[i] = model.addVar(0.0, 1000, 1.0, GRB_INTEGER, s);
        };
        
        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
            for (int j = 0; j < Facility.FirstStageNodes.size(); j++) {
                string s = "A_" + to_string(i) + to_string(j);
                a[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
            }
        }
        
        for (int i = 0; i < Constraints.size(); i++) {
            model.addConstr(x[Constraints[i][1]-1], GRB_GREATER_EQUAL, 1.0);
            model.addConstr(x[Constraints[i][2]-1], GRB_GREATER_EQUAL, 1.0);
            if (Constraints[i][0] == 0) {
                model.addConstr(x[Constraints[i][2]-1], GRB_EQUAL, x[Constraints[i][1]-1]);
            }
            if (Constraints[i][0] == 1) {
                model.addConstr( x[Constraints[i][1]-1] - x[Constraints[i][2]-1] + ( intbigM * a[Constraints[i][1]-1][Constraints[i][2]-1] ), GRB_GREATER_EQUAL, 1.0 );
                model.addConstr( x[Constraints[i][1]-1] - x[Constraints[i][2]-1] + ( intbigM * a[Constraints[i][1]-1][Constraints[i][2]-1] ), GRB_LESS_EQUAL, intbigM - 1 );
            }
        }
        
        model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
        
        model.optimize();
        
        
        vector<int> X_res;
        
        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
            X_res.push_back(x[i].get(GRB_DoubleAttr_X));
        }
        
        int maxnum = 0;
        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
            if (X_res[i] > maxnum) {
                maxnum =X_res[i];
            }
        }
        
        result.resize(maxnum);
        
        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
            if (X_res[i] != 0) {
                result[X_res[i]-1].push_back(i+1);
            }
        }
    }
    
    return result;
}
