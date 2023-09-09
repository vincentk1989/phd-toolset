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
    Scenarios.clear();
    int pos = (int)Facility.FirstStageNodes.size() + 1;
    
    for (int n = 0; n < Facility.SecondStageNodes.size(); n++) {
        NodeList[Facility.SecondStageNodes[n]].Service = false;
    }
    
    GenerateScenarios(NodeList, Facility, pos, 1.0);
    
    bool wissel = true;
    
    while (wissel == true) {
        wissel = false;
        for (int i = 0; i < Scenarios.size() - 1; i++) {
            if (Scenarios[i].PresentNodes.size() < Scenarios[i+1].PresentNodes.size()) {
                ScenarioClass t_Scenario = Scenarios[i];
                Scenarios[i] = Scenarios[i+1];
                Scenarios[i+1] = t_Scenario;
                wissel = true;
            }
            float sumdemand1 = 0;
            float sumdemand2 = 0;
            for (int j = 0; j < Scenarios[i].Demands.size(); j++) {
                sumdemand1 = sumdemand1 + Scenarios[i].Demands[j];
            }
            for (int j = 0; j < Scenarios[i + 1].Demands.size(); j++) {
                sumdemand2 = sumdemand2 + Scenarios[i+1].Demands[j];
            }
            if (sumdemand1 < sumdemand2 && Scenarios[i].PresentNodes.size() <= Scenarios[i+1].PresentNodes.size()) {
                ScenarioClass t_Scenario = Scenarios[i];
                Scenarios[i] = Scenarios[i+1];
                Scenarios[i+1] = t_Scenario;
                wissel = true;
            }

        }
    }
    
    for (int i = 0; i < Scenarios.size(); i++) {
        FacilityClass t_Facility = Facility;
        t_Facility.ServiceNodes = Scenarios[i].PresentNodes;
        t_Facility =  GurobiSol(NodeList, t_Facility);
        Scenarios[i].CurOpt = t_Facility.RouteList;
    }
    
    Zstar = floatbigM;
    Xstar.clear();
    
    vector<ScenarioClass> sc = Scenarios;
    
    vector<vector<int>> InitialConstraints;
    InitialConstraints.clear();
    
    Branch(sc, NodeList, Facility, InitialConstraints);
    
    return true;
}

bool c_Exact::CalculateIntegerL(vector<NodeClass> NodeList, FacilityClass Facility) {
    Scenarios.clear();
    int pos = (int)Facility.FirstStageNodes.size() + 1;
    
    for (int n = 0; n < Facility.SecondStageNodes.size(); n++) {
        NodeList[Facility.SecondStageNodes[n]].Service = false;
    }
    
    GenerateScenarios(NodeList, Facility, pos, 1.0);
    
    Zstar = floatbigM;
    Xstar.clear();
    
    vector<ScenarioClass> sc = Scenarios;
    
    BranchIntegerL(sc, NodeList, Facility);
    
    return true;
}


float c_Exact::CalculateSecondCost(vector<NodeClass> NodeList, FacilityClass Facility) {
    Scenarios.clear();
    int pos = (int)Facility.FirstStageNodes.size() + 1;
    
    GenerateScenarios(NodeList, Facility, pos, 1.0);
    
    Zstar = floatbigM;
    Xstar.clear();
    
    vector<ScenarioClass> sc = Scenarios;
    
    float TotalCost = 0;

    for ( int i = 0; i < sc.size(); i++) {
        
        vector<int> SecondRequests = sc[i].PresentNodes;
        
        for (int j = 0; j < Facility.FirstStageNodes.size(); j++) {
            for (int k = 0; k < SecondRequests.size(); k++) {
                if (SecondRequests[k] == Facility.FirstStageNodes[j]) {
                    SecondRequests.erase(SecondRequests.begin() + k);
                }
            }
        }
        
        Facility.ServiceNodes = SecondRequests;
        
        for (int j = 1; j < NodeList.size(); j++) { 
            NodeList[j].Demand = sc[i].Demands[j - 1];
        }
        Facility = GurobiSol(NodeList, Facility);
        cout << CalculateFacilityCosts(Facility, NodeList) << endl;
        TotalCost = TotalCost + (sc[i].Probability * CalculateFacilityCosts(Facility, NodeList));
    }
    
    return TotalCost;
}


bool c_Exact::CalculateExactArrivals(vector<NodeClass> NodeList, FacilityClass Facility) {
    int pos = (int)Facility.FirstStageNodes.size();
    GenerateScenarios(NodeList, Facility, pos, 1.0);
    
    Zstar = 0;
    Xstar.clear();
    
    vector<ScenarioClass> sc = Scenarios;
    
    float obj;
    
 //   sc[0].CurOpt = GurobiAltFormSecondStageK(NodeList, Facility, &obj);
    
    sc[0].CurCost = obj;
    
    vector<vector<int>> t_CurDet = sc[0].CurOpt;
    
    for (int i= 0; i < Facility.SecondStageNodes.size(); i++){
        for (int j = 0; j < t_CurDet.size(); j++) {
            for (int k = 0; k < t_CurDet[j].size(); k++) {
                if (t_CurDet[j][k] == Facility.SecondStageNodes[i]) {
                    t_CurDet[j].erase(t_CurDet[j].begin() + k);
                }
            }
        }
    }

    
    sc[0].CurDet = t_CurDet;
    Facility.RouteList = t_CurDet;
    
    for (int i = 0; i < sc.size(); i++) {
//        sc[i].CurOpt = GurobiAltFormSecondStageK(NodeList, Facility, &obj);
        
        sc[0].CurCost = obj;
        
        vector<vector<int>> t_CurDet = sc[0].CurOpt;
        
        for (int i= 0; i < Facility.SecondStageNodes.size(); i++){
            for (int j = 0; j < t_CurDet.size(); j++) {
                for (int k = 0; k < t_CurDet[j].size(); k++) {
                    if (t_CurDet[j][k] == Facility.SecondStageNodes[i]) {
                        t_CurDet[j].erase(t_CurDet[j].begin() + k);
                    }
                }
            }
        }
        
        sc[0].CurDet = t_CurDet;
    }
    
    
    vector<vector<float>> arrivaltimes;
    
    arrivaltimes.resize(NodeList.size());
    
    for (int i = 0; i < sc.size(); i++) {
        for (int j = 0; j < sc[i].CurOpt.size(); j++) {
            float RouteLength = 0;
            if (sc[i].CurOpt[j].size() > 0) {
                int NodeNumber = sc[i].CurOpt[j][0];
                RouteLength = NodeDistance(Facility.Number, true, sc[i].CurOpt[j][0], false, NodeList.size());
                
                arrivaltimes[sc[i].CurOpt[j][0]].push_back(RouteLength);
                
                
                
                int PrevNode = sc[i].CurOpt[j][0];
                int Size = sc[i].CurOpt[j].size() - 1;
                for (int i = 0; i < Size; i++) {
                    NodeNumber = sc[i].CurOpt[j][i + 1];
                    PrevNode = sc[i].CurOpt[j][i];
                    RouteLength = RouteLength + NodeDistance(PrevNode, false, NodeNumber, false, NodeList.size());
                    
                    arrivaltimes[NodeNumber].push_back(RouteLength);
                }
            }
            return RouteLength;
        }
    }
    
    ofstream myfile;
    string Title = "arrivaltimes.txt";
    myfile.open(Title);

    for (int i = 1; i < arrivaltimes.size(); i++) {
        myfile << i << ";";
        for (int j = 0; j < arrivaltimes[i].size(); j++) {
            myfile << arrivaltimes[i][j] << ";";
        }
        myfile << "\n";
    }
    
    myfile.close();

    return true;
    
    
    vector<vector<int>> ScenarioDistribution();
    
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
    Scenarios.clear();
    int pos = (int)Facility.FirstStageNodes.size() + 1;
    GenerateScenarios(NodeList, Facility, pos, 1.0);
    
    Zstar = 0;
    Xstar.clear();
    
    vector<ScenarioClass> sc = Scenarios;
    
    vector<float> savings;
    savings.resize(sc.size());
    
    

    
    for (int i = 0; i < sc.size(); i++) {
 //       cout << "All sc " << i << " calculated on thread: " << omp_get_thread_num() << endl;
        float obj;
        FacilityClass DupF = Facility;
        FacilityClass CopyFacility = Facility;
        vector<NodeClass> CopyNodeList = NodeList;
        DupF.ServiceNodes.clear();
//        for ( int x = 0; x < Facility.FirstStageNodes.size(); x++) {
//            DupF.ServiceNodes.push_back(Facility.FirstStageNodes[x]);
//        }
        for ( int x = 0; x < sc[i].PresentNodes.size(); x++) {
            DupF.ServiceNodes.push_back(sc[i].PresentNodes[x]);
        }
        for (int j = 1; j < CopyNodeList.size(); j++) {
            CopyNodeList[j].Demand = sc[i].Demands[j - 1];
        }
        float objective;
        
        int vehicle_amount = -1;
        
        while (sc[i].CurOpt.size() == 0) {
            
            if (vehicle_amount == -1) {
                float sumdemands = 0;
                for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
                    sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
                }
                
                vehicle_amount = ceil(sumdemands / Facility.Capacity);
            } else {
                vehicle_amount = vehicle_amount + 1;
            }
            
            sc[i].CurOpt = GurobiAltFormSecondStageK(CopyNodeList, DupF, &objective, vehicle_amount);
        }
        CopyFacility.RouteList = sc[i].CurOpt;
        sc[i].CurCost = CalculateFacilityCosts(CopyFacility, NodeList);
        savings[i] = sc[i].Probability * sc[i].CurCost;
    }
    
    float sumsavings = 0;
    for (int i = 0; i < savings.size(); i++) {
        sumsavings = sumsavings + savings[i];
    }
    
    Zstar = sumsavings;
    
    return sumsavings;
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
            
            Scenario.NotPresentNodes.clear();
            for (int j = 1; j < NodeList.size(); j++ ) {
                if (NodeList[j].Service == false) {
                    Scenario.NotPresentNodes.push_back(j);
                }
            }
            
            for (int j = 1; j < NodeList.size(); j++ ) {
                //if (NodeList[j].Service == true) {
                    Scenario.Demands.push_back(NodeList[j].Demand);
                //}
            }
            
            Scenario.Probability = bprob;
            
            Scenario.CurCost = 0;
            
            Scenarios.push_back(Scenario);
        } else {
            result = GenerateScenarios(NodeList,Facility,pos+1,bprob);
        }
    }
    
    
    if (NodeList[pos].Probability < 0.999) {
        NodeList[pos].Service = false;
        NodeList[pos].Demand = 1;
        bprob = currentprobability;
        bprob = bprob * (1- NodeList[pos].Probability);
        if (pos == NodeList.size() - 1) {
            
            
            ScenarioClass Scenario;
            Scenario.PresentNodes.clear();
            for (int j = 1; j < NodeList.size(); j++ ) {
                if (NodeList[j].Service == true) {
                    Scenario.PresentNodes.push_back(j);
                }
            }
            
            Scenario.NotPresentNodes.clear();
            for (int j = 1; j < NodeList.size(); j++ ) {
                if (NodeList[j].Service == false) {
                    Scenario.NotPresentNodes.push_back(j);
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

float c_Exact::CostIntegerL(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> RouteList ) {
    float result;
    
    vector<vector<int>> t_rl;
    
    FacilityClass t_Facility = Facility;
    
    for (int i = 0; i < RouteList.size(); i++) {
        float sumdemands = 0;
        for ( int j = 0; j < RouteList[i].size(); j++) {
            sumdemands = sumdemands + NodeList[RouteList[i][j]].Demand;
        }
        
        if (sumdemands > Facility.Capacity) {
 
            vector<vector<int>> sp_rl;
            vector<int> rl;
            
            float sdem = 0;
            
            int w = 0;
            while (w < RouteList[i].size()) {
                sdem = sdem + NodeList[RouteList[i][w]].Demand;
                
                if (sdem <= Facility.Capacity) {
                    rl.push_back(RouteList[i][w]);
                } else {
                    sdem = NodeList[RouteList[i][w]].Demand;
                    sp_rl.push_back(rl);
                    rl.clear();
                    rl.push_back(RouteList[i][w]);
                }
                
                w = w + 1;
            }
            
            if (rl.size() > 0) {
                sp_rl.push_back(rl);
            }
            
            for (int j = 0; j < sp_rl.size(); j++) {
                t_rl.push_back(sp_rl[j]);
            }
        } else {
            t_rl.push_back(RouteList[i]);
        }
    }
    
    t_Facility.RouteList = t_rl;
    
    result = CalculateFacilityCosts(t_Facility, NodeList);
    
    return result;
}

bool c_Exact::BranchIntegerL(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility) {
    
    vector<ScenarioClass> t_Scenarios = i_Scenarios;

     for (int i = 0; i < t_Scenarios.size(); i++) {
         FacilityClass t_Facility = Facility;
         t_Facility.ServiceNodes = t_Scenarios[i].PresentNodes;
         for (int w = 0; w < t_Scenarios[i].Demands.size(); w++) {
             NodeList[w+1].Demand = t_Scenarios[i].Demands[w];
         }
         t_Facility = GurobiSol(NodeList, t_Facility);
         t_Scenarios[i].CurOpt = t_Facility.RouteList;
     }
    
    float SolutionCost = floatbigM;
    
    for (int i = 0; i < t_Scenarios.size(); i++) {
        float TotalCost = 0;
        for (int j = 0; j < t_Scenarios.size(); j++) {
            if (i != j) {
                FacilityClass t_Facility = Facility;
                t_Facility.RouteList = t_Scenarios[i].CurOpt;
                t_Facility.ServiceNodes = t_Scenarios[j].PresentNodes;
                
                for(int k = 0; k < t_Scenarios[j].NotPresentNodes.size(); k++) {
                    for ( int w = 0; w < t_Facility.RouteList.size(); w++) {
                        for ( int z = 0; z < t_Facility.RouteList[w].size(); z++) {
                            if ( t_Facility.RouteList[w][z] == t_Scenarios[j].NotPresentNodes[k]) {
                                t_Facility.RouteList[w].erase(t_Facility.RouteList[w].begin() + z);
                            }

                        }
                    }
                }
                
                for ( int w = 0; w < t_Facility.RouteList.size(); w++) {
                    if (t_Facility.RouteList[w].size() == 0) {
                        t_Facility.RouteList.erase(t_Facility.RouteList.begin() + w);
                    }
                }
                
                for (int w = 0; w < t_Scenarios[i].Demands.size(); w++) {
                    NodeList[w+1].Demand = t_Scenarios[i].Demands[w];
                }
                
                t_Scenarios[j].CurDet = GurobiIntegerL(NodeList, t_Facility);
                t_Scenarios[j].CurCost = CostIntegerL(NodeList, t_Facility, t_Scenarios[j].CurDet);
                TotalCost = TotalCost + (t_Scenarios[j].CurCost * t_Scenarios[j].Probability);
            } else {
                t_Scenarios[j].CurDet = t_Scenarios[j].CurOpt;
                FacilityClass t_Facility = Facility;
                t_Facility.RouteList = t_Scenarios[i].CurOpt;
                t_Scenarios[j].CurCost = CalculateFacilityCosts(t_Facility, NodeList);
                TotalCost = TotalCost + (t_Scenarios[j].CurCost * t_Scenarios[j].Probability);
            }
        }
        
        if (TotalCost < SolutionCost) {
            SolutionCost = TotalCost;
            Zstar = TotalCost;
            vector<vector<int>> t_Det = t_Scenarios[0].CurDet;
            for (int k = 0; k < Facility.SecondStageNodes.size(); k++) {
                for (int k2 = 0; k2 < t_Det.size(); k2++) {
                    for (int k3 = 0; k3 < t_Det[k2].size(); k3++) {
                        if( t_Det[k2][k3] == Facility.SecondStageNodes[k]) {
                            t_Det[k2].erase(t_Det[k2].begin() + k3);
                        }
                    }
                }
            }
            Xstar = t_Det;
        }
    }

    
    return true;
};

bool c_Exact::Branch(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints) {
    
    vector<ScenarioClass> t_Scenarios = i_Scenarios;
    
    float total_cost_node = 0;
    
    for (int i = 0; i < t_Scenarios.size(); i++) {
        
   //     cout << "Branch sc " << i << " calculated on thread: " << omp_get_thread_num() << endl;
        
        bool valid = true;
        // check for joint

        for (int a = 0; a < Constraints.size(); a++) {
            if (Constraints[a][0] == 0) { //joint
                int r1 = 0;
                int r2 = 0;
                for (int j = 0; j < t_Scenarios[i].CurDet.size(); j++) {
                    for (int k = 0; k < t_Scenarios[i].CurDet[j].size(); k++) {
                        if ( t_Scenarios[i].CurDet[j][k] == Constraints[a][1] ) {
                            r1 = j;
                        }
                        if ( t_Scenarios[i].CurDet[j][k] == Constraints[a][2] ) {
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
                for (int j = 0; j < t_Scenarios[i].CurDet.size(); j++) {
                    for (int k = 0; k < t_Scenarios[i].CurDet[j].size(); k++) {
                        if ( t_Scenarios[i].CurDet[j][k] == Constraints[a][1] ) {
                            r1 = j;
                        }
                        if ( t_Scenarios[i].CurDet[j][k] == Constraints[a][2] ) {
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
            
            FacilityClass DupF = Facility;
            vector<NodeClass> CopyNodeList = NodeList;
            DupF.ServiceNodes = t_Scenarios[i].PresentNodes;
            for (int j = 1; j < CopyNodeList.size(); j++) {
                CopyNodeList[j].Demand = t_Scenarios[i].Demands[j - 1];
            }
            
            vector<vector<int>> BfC = DetermineBaseFromConstraints(CopyNodeList, Facility, Constraints);
            int minva = BfC.size();
            //t_Scenarios[i].CurOpt = GurobiScenario(minva, t_Scenarios[i], CopyNodeList, DupF, &obj, Constraints);
            
            cout << "scenario " << i << endl;
            
            vector<vector<int>> t_CurDet;
            
            while (t_CurDet.size() == 0 && minva < DupF.ServiceNodes.size()) {
                t_Scenarios[i].CurOpt = GurobiAFSS(minva, t_Scenarios[i], CopyNodeList, DupF, &obj, Constraints);
                t_CurDet = t_Scenarios[i].CurOpt;
                minva = minva + 1;
            }
            
            if (minva >= DupF.ServiceNodes.size()) {
                t_Scenarios[i].CurOpt = GurobiAFSSLEQ(minva, t_Scenarios[i], CopyNodeList, DupF, &obj, Constraints);
                t_CurDet = t_Scenarios[i].CurOpt;
            }
            
            for (int iii= 0; iii < Facility.SecondStageNodes.size(); iii++){
                for (int j = 0; j < t_CurDet.size(); j++) {
                    for (int k = 0; k < t_CurDet[j].size(); k++) {
                        if (t_CurDet[j][k] == Facility.SecondStageNodes[iii]) {
                            t_CurDet[j].erase(t_CurDet[j].begin() + k);
                        }
                    }
                }
            }
            
            for ( int w = 0; w < t_CurDet.size(); w++) {
                if (t_CurDet[w].size() == 0) {
                    t_CurDet.erase(t_CurDet.begin() + w);
                }
            }
            
            t_Scenarios[i].CurDet = t_CurDet;
            
            if (t_Scenarios[i].CurDet.size() == 0) {
                cout << "ERROR ERROR ERROR ERROR!" << endl;
            }
            FacilityClass CopyFacility = Facility;
            CopyFacility.RouteList = t_Scenarios[i].CurOpt;
            t_Scenarios[i].CurCost = CalculateFacilityCosts(CopyFacility, NodeList);
            
            if ( t_Scenarios[i].CurCost == 0) {
                cout << "Kosten: 0" << endl;
            }
        }
    }
    
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
        for (int j = 0; j < t_Scenarios[i].CurDet.size(); j++) {
            if (t_Scenarios[i].CurDet[j].size() > 0) {
                for (int k = 0; k < t_Scenarios[i].CurDet[j].size() - 1; k++) {
                    for (int u = k+1; u < t_Scenarios[i].CurDet[j].size(); u++) {
                        OnSameRoute[t_Scenarios[i].CurDet[j][k] - 1][t_Scenarios[i].CurDet[j][u] - 1] += 1 ;
                        OnSameRoute[t_Scenarios[i].CurDet[j][u] - 1][t_Scenarios[i].CurDet[j][k] - 1] += 1 ;
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < t_Scenarios.size(); i++) {
        for (int j = 0; j < t_Scenarios[i].CurDet.size()-1; j++) {
            if (t_Scenarios[i].CurDet[j].size() > 0) {
                
                for (int k = 0; k < t_Scenarios[i].CurDet[j].size(); k++) {
                    
                    for (int w = j+1; w < t_Scenarios[i].CurDet.size(); w++) {
                        if (t_Scenarios[i].CurDet[w].size() > 0) {
                            for (int u = 0; u < t_Scenarios[i].CurDet[w].size(); u++) {
                                NotOnSameRoute[t_Scenarios[i].CurDet[j][k] - 1][t_Scenarios[i].CurDet[w][u] - 1] += 1;
                                NotOnSameRoute[t_Scenarios[i].CurDet[w][u] - 1][t_Scenarios[i].CurDet[j][k] - 1] += 1;
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
    
    float t_Z = 0;
    t_Z = 0;
    for (int i = 0; i < t_Scenarios.size(); i++) {
        t_Z = t_Z + (t_Scenarios[i].CurCost * t_Scenarios[i].Probability);
    }
    
    ofstream myfilen;
    stringstream ssn;
    ssn << "depth - " << Constraints.size() << " time " << time(NULL)  << ".txt";
    string Titlen = ssn.str();
    myfilen.open(Titlen);
    for (int i = 0; i < t_Scenarios.size(); i++) {
        myfilen << "s" << i << " nodes present: ";
        for (int j = 0; j < t_Scenarios[i].PresentNodes.size(); j++) {
            myfilen << t_Scenarios[i].PresentNodes[j] << ", ";
        }
        float demand = 0;
        for (int j = 0; j < t_Scenarios[i].Demands.size(); j++) {
            demand = demand + t_Scenarios[i].Demands[j];
        }
        myfilen << " demand " << demand;
        myfilen << " cost " << t_Scenarios[i].CurCost << " prob " << t_Scenarios[i].Probability << endl;
        total_cost_node = total_cost_node + (t_Scenarios[i].CurCost * t_Scenarios[i].Probability);
    }
    myfilen << "Total cost: " << total_cost_node << endl;
    
    myfilen << endl;
    myfilen << "Constraints: " << endl;
    for (int i = 0; i < Constraints.size(); i++) {
        myfilen << "(" << Constraints[i][0] << "," << Constraints[i][1] << "," << Constraints[i][2] << ")" << endl;
    }
    myfilen << endl;
    myfilen << "Branching possibilities: " << endl;
    for (int i = 0; i < BranchingPossibilities.size(); i++) {
        myfilen << "(" << Facility.FirstStageNodes[BranchingPossibilities[i][0]] << "," << Facility.FirstStageNodes[BranchingPossibilities[i][1]] << ")" << endl;
    }
    myfilen.close();
    
    cout << "Time: " << time(NULL) << " - Node Costs: " << total_cost_node << endl;

    
    if (ff == false) { // If no branching pair exists -> candidate found
        // Candidate Found -> update where necessary
        
        vector<float> savings;
        savings.resize(t_Scenarios.size());
            
        for (int i = 0; i < t_Scenarios.size(); i++) {
            float obj;
            FacilityClass DupF = Facility;
            DupF.RouteList= t_Scenarios[i].CurDet;
            FacilityClass CopyFacility = Facility;
            vector<NodeClass> CopyNodeList = NodeList;
            DupF.ServiceNodes.clear();
            for ( int x = 0; x < t_Scenarios[i].PresentNodes.size(); x++) {
                DupF.ServiceNodes.push_back(t_Scenarios[i].PresentNodes[x]);
            }
            for (int j = 1; j < CopyNodeList.size(); j++) {
                CopyNodeList[j].Demand = t_Scenarios[i].Demands[j - 1];
            }
            float objective;
            
            int vehicle_amount = -1;
            
            while (t_Scenarios[i].CurOpt.size() == 0) {
                
                if (vehicle_amount == -1) {
                    float sumdemands = 0;
                    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
                        sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
                    }
                    
                    vehicle_amount = ceil(sumdemands / Facility.Capacity);
                } else {
                    vehicle_amount = vehicle_amount + 1;
                }
                
                t_Scenarios[i].CurOpt = GurobiAltFormSecondStageK(CopyNodeList, DupF, &objective, vehicle_amount);
            }
            CopyFacility.RouteList = t_Scenarios[i].CurOpt;
            t_Scenarios[i].CurCost = CalculateFacilityCosts(CopyFacility, NodeList);
            savings[i] = t_Scenarios[i].Probability * t_Scenarios[i].CurCost;
        }
        
        float t_ST = 0;
    
        for (int i = 0; i < savings.size(); i++) {
            t_ST = t_ST + savings[i];
        }
        
        if (t_ST != t_Z) {
            cout << " Different " << endl;
        }

        if (t_ST < Zstar) {
            Zstar = t_Z;

            Xstar = t_Scenarios[0].CurDet;
            
            ofstream myfile;
            stringstream ss;
            ss << "Final - " << Zstar << ".txt";
            string Title = ss.str();
            myfile.open(Title);
            for ( int  k = 0; k < t_Scenarios.size(); k++) {
                myfile << t_Scenarios[k].PresentNodes.size();
                FacilityClass f_Facility = Facility;
                f_Facility.ServiceNodes = t_Scenarios[k].PresentNodes;
                f_Facility.RouteList = t_Scenarios[k].CurOpt;
                FacilityClass t_Facility = Facility;
                t_Facility.ServiceNodes = Scenarios[k].PresentNodes;
                t_Facility.RouteList = Scenarios[k].CurOpt;
                
                myfile << ";" << CalculateFacilityCosts(f_Facility, NodeList) -  CalculateFacilityCosts(t_Facility, NodeList) << endl;
            }
            
            myfile.close();

            cout << " CANDIDATE FOUND! Z: " << t_Z << endl;
        }
    } else {
        if (t_Z < Zstar && Zstar == floatbigM) {
             //Select Best Branching Possibility
            
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
            
            if( BestJoint >= BestDisjoint ) {
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

bool c_Exact::VectorCompare(vector<int> vector1, vector<int> vector2, bool inversedirection) {
    bool result = true;
    
    if (inversedirection) {
        reverse(vector2.begin(), vector2.end());
    }
    
    for (int i = 0; i < vector1.size(); i++) {
        if (vector1[i] != vector2[i]) {
            result = false;
        }
    }
    
    return result;
}

bool c_Exact::VectorContain(vector<int> vector1, vector<int> vector2) {
    bool result = true;
    
    for (int i = 0; i < vector1.size(); i++) {
        bool found = false;
        for ( int j = 0; j < vector2.size(); j++) {
            if (vector1[i] == vector2[j]) {
                found = true;
            }
        }
        if (found == false) {
            result = false;
        }
    }
    
    return result;
}


vector<ScenarioClass> c_Exact::DetermineBestOrder(vector<ScenarioClass> i_Scenarios, vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints) {
    
    vector<vector<vector<int>>> ScenarioOrders;
    
    vector<vector<vector<int>>> PotentialOrders;
    
    vector<vector<int>> scorder;
    vector<vector<int>> t_curopt;
    vector<vector<int>> t_order;
    
    vector<int> sorter;
    vector<int> sorter_opt;
    vector<int> pointer;
    vector<int> pointer_opt;

    scorder = i_Scenarios[ i_Scenarios.size() - 1 ].CurDet;
    
    PotentialOrders.push_back(i_Scenarios[0].CurDet);
    
    ScenarioOrders.push_back(i_Scenarios[0].CurDet);
    
    int PO_max = (int) PotentialOrders[0].size();
    
    bool Found;
    
    for (int sc_int = 1; sc_int < i_Scenarios.size(); sc_int++) {
        
        Found = false;
        
        if (i_Scenarios[sc_int].CurDet.size() < PO_max) {
            for (int i7 = 0; i7 < (PO_max - i_Scenarios[sc_int].CurDet.size()); i7++) {
                i_Scenarios[sc_int].CurDet.push_back(vector<int>());
            }
        }
        

        for (int so = 0; so < i_Scenarios[sc_int].CurDet.size(); so++) {
            for (int po_int = 0; po_int < PotentialOrders.size(); po_int++) {
                if (i_Scenarios[sc_int].CurDet[so] != PotentialOrders[po_int][so]) {
                    vector<int> invorder = i_Scenarios[sc_int].CurDet[so];
                    reverse(invorder.begin(), invorder.end());
                    if(invorder  == PotentialOrders[po_int][so]) {
                         i_Scenarios[sc_int].CurDet[so] = invorder;
                    }
                }
                if (i_Scenarios[sc_int].CurDet == PotentialOrders[po_int]) {
                    Found = true;
                }
            }
        }
        
        if (Found == false) {
            PotentialOrders.push_back(i_Scenarios[sc_int].CurDet);
        }
        
        ScenarioOrders.push_back(i_Scenarios[sc_int].CurDet);
        
    }
    
    // Now to find orders GAMMA should they exist
    
//    if (PotentialOrders.size() > 0) {
//        for (int sc_int = 0; sc_int < i_Scenarios.size(); sc_int++) {
//            for (int sc_int_j = sc_int + 1; sc_int_j < i_Scenarios.size(); sc_int_j++) {
//            
//                
//                // check if difference = 1 request
//                if(ScenarioOrders[sc_int] != ScenarioOrders[sc_int_j] && i_Scenarios[sc_int].PresentNodes.size() + 1 == i_Scenarios[sc_int_j].PresentNodes.size()) {
//                    
//                    int r_id;
//                    int additionalrequest;
//                    int Count_Same = 0;
//                    
//                    for (int pn = 0; pn < i_Scenarios[sc_int_j].PresentNodes.size(); pn++) {
//                        bool Found = false;
//                        for (int cn = 0; cn < i_Scenarios[sc_int].PresentNodes.size(); cn++) {
//                            if (i_Scenarios[sc_int_j].PresentNodes[pn] == i_Scenarios[sc_int].PresentNodes[cn]) {
//                                Count_Same = Count_Same + 1;
//                                Found = true;
//                            }
//                        }
//                        if (Found == false) {
//                            additionalrequest = i_Scenarios[sc_int_j].PresentNodes[pn];
//                        }
//                    }
//                    
//                    if (Count_Same == i_Scenarios[sc_int].PresentNodes.size()) {
//                        
//                        for ( int i = 0; i < i_Scenarios[sc_int].CurOpt.size(); i++) {
//                            for ( int j = 0; j < i_Scenarios[sc_int].CurOpt[i].size(); j++) {
//                                if (i_Scenarios[sc_int].CurOpt[i][j] == additionalrequest) {
//                                    r_id = i;
//                                }
//                            }
//                        }
//                        
//                        vector<int> route;
//                        
//                        vector<int> routeBETA = i_Scenarios[sc_int_j].CurOpt[r_id];
//                        
//                        vector<int> routeALPHA = i_Scenarios[sc_int].CurOpt[r_id];
//                        
//                        float betaincl = CalculateRL(NodeList, Facility, routeBETA);
//                        
//                        int q = 0;
//                        
//                        while (q < routeBETA.size()) {
//                            if (routeBETA[q] == additionalrequest) {
//                                routeBETA.erase(routeBETA.begin() + q);
//                            } else {
//                                q = q + 1;
//                            }
//                        }
//                        
//                        float betaexcl = CalculateRL(NodeList, Facility, routeBETA);
//                        
//                        float LB = betaincl - betaexcl;
//                        
//                        float alphaincl = DetermineUB(NodeList, Facility, routeALPHA, additionalrequest);
//                        
//                        float alphaexcl = CalculateRL(NodeList, Facility, routeALPHA);
//                        
//                        float UB = alphaincl - betaincl;
//                        
//                        for (int i = 0; i < i_Scenarios[sc_int].PresentNodes.size(); i++) {
//                            for (int j = 0; j < i_Scenarios[sc_int].PresentNodes.size(); j++) {
//                                
//                                float a = NodeDistance(NodeList[i_Scenarios[sc_int].PresentNodes[i]].LocationX, NodeList[i_Scenarios[sc_int].PresentNodes[i]].LocationY, NodeList[i_Scenarios[sc_int].PresentNodes[j]].LocationX,NodeList[i_Scenarios[sc_int].PresentNodes[j]].LocationX);
//                                float b = NodeDistance(NodeList[i_Scenarios[sc_int].PresentNodes[i]].LocationX, NodeList[i_Scenarios[sc_int].PresentNodes[i]].LocationY, NodeList[additionalrequest].LocationX,NodeList[additionalrequest].LocationX);
//                                float c = NodeDistance(NodeList[additionalrequest].LocationX, NodeList[additionalrequest].LocationY, NodeList[i_Scenarios[sc_int].PresentNodes[j]].LocationX,NodeList[i_Scenarios[sc_int].PresentNodes[j]].LocationX);
//                                float insertioncost = c + b - a;
//                                
//                                if(insertioncost > LB && insertioncost < UB) {
//                                    vector<int> newrouteexcl = determineNewSequence(NodeList, Facility, routeALPHA, i_Scenarios[sc_int].PresentNodes[i], i_Scenarios[sc_int].PresentNodes[j]);
//                                    
//                                    vector<int> newroutecopy = newrouteexcl;
//                                    
//                                    int requesta = i_Scenarios[sc_int].PresentNodes[i];
//                                    
//                                    
//                                    for (int w = 0; w < Facility.SecondStageNodes.size(); w++) {
//                                        int t = 0;
//                                        while (t < newroutecopy.size()) {
//                                            if (newroutecopy[t] == Facility.SecondStageNodes[w]) {
//                                                newroutecopy.erase(newroutecopy.begin() + t);
//                                            } else {
//                                                t  = t + 1;
//                                            }
//                                        }
//                                    }
//                                    
//                                    bool Continue = true;
//                                    
//                                    for (int u = 0; u < PotentialOrders.size(); u++) {
//                                        vector<int> rev = PotentialOrders[u][r_id];
//                                        reverse(rev.begin(), rev.end());
//                                        if (PotentialOrders[u][r_id] == newroutecopy || rev == newroutecopy) {
//                                            Continue = false;
//                                        }
//                                    }
//                                    
//                                    if (Continue == true) {
//                                        
//                                        int loc_a = 0;
//                                        for(int i = 0; i < newrouteexcl.size(); i++) {
//                                            if (newrouteexcl[i] == requesta) {
//                                                loc_a = i;
//                                            }
//                                        }
//                                        
//                                        vector<int> newrouteincl = newrouteexcl;
//                                        newrouteincl.insert(newrouteincl.begin() + loc_a, additionalrequest);
//                                        
//                                        float p1 = i_Scenarios[sc_int].Probability;
//                                        float p2 = i_Scenarios[sc_int_j].Probability;
//                                        
//                                        float costgamma = (p1 * CalculateRL(NodeList, Facility, newrouteexcl)) + (p2 * CalculateRL(NodeList, Facility, newrouteincl));
//                                        float costalpha = (p1 * alphaexcl) + (p2 * alphaincl);
//                                        float costbeta = (p1 * betaexcl) + (p2 * betaexcl);
//                                        
//                                        if (costgamma < costalpha && costgamma < costbeta) {
//                                            vector<vector<int>> OrderGamma = i_Scenarios[sc_int].CurDet;
//                                            
//                                            OrderGamma[r_id] = newroutecopy;
//                                            
//                                            PotentialOrders.push_back(OrderGamma);
//                                        }
//                                    }
//                                    
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
    
    int FinalP = 0;
    float TotalCost = floatbigM;
    
    vector<vector<float>> ScenarioCosts;
    ScenarioCosts.resize(PotentialOrders.size());
    
    for (int p = 0; p < ScenarioCosts.size(); p++) {
        ScenarioCosts[p].resize(i_Scenarios.size());
    }
        
    for (int p = 0; p < PotentialOrders.size(); p++) {
        vector<float> intermediatecost;
        intermediatecost.resize(i_Scenarios.size());
        for (int sc_int = 0; sc_int < i_Scenarios.size(); sc_int++) {
            FacilityClass CopyFacility = Facility;
            vector<NodeClass> CopyNodeList = NodeList;
            if (i_Scenarios[sc_int].CurDet != PotentialOrders[p]) {
                CopyFacility.RouteList = PotentialOrders[p];
                
                float objective;
                
                for (int i = 1; i < CopyNodeList.size(); i++) {
                    CopyNodeList[i].Demand = i_Scenarios[sc_int].Demands[i - 1];
                }
                
                vector<vector<int>> BfC = DetermineBaseFromConstraints(NodeList, Facility, Constraints);
                int minva = BfC.size();
                
                CopyFacility.RouteList = GurobiScenarioK(minva, i_Scenarios[sc_int], CopyNodeList, CopyFacility, &objective, Constraints);
                
            } else {
                CopyFacility.RouteList = i_Scenarios[sc_int].CurOpt;
            }
            
            ScenarioCosts[p][sc_int] = CalculateFacilityCosts(CopyFacility, CopyNodeList);
            intermediatecost[sc_int] = i_Scenarios[sc_int].Probability * CalculateFacilityCosts(CopyFacility, CopyNodeList);
        }
        
        float sumintermediatecost = 0;
        for (int tt = 0; tt < intermediatecost.size(); tt++) {
            sumintermediatecost = sumintermediatecost + intermediatecost[tt];
        }
        
        if (sumintermediatecost < TotalCost) {
            FinalP = p;
            TotalCost = sumintermediatecost;
        }
    }
    
    vector<ScenarioClass> o_Scenarios = i_Scenarios;
    
    for (int sc_int = 0; sc_int < o_Scenarios.size(); sc_int++) {
        o_Scenarios[sc_int].CurDet = PotentialOrders[FinalP];
        o_Scenarios[sc_int].CurCost = ScenarioCosts[FinalP][sc_int];
    }

    return o_Scenarios;
}

float c_Exact::DetermineUB(vector<NodeClass> NodeList, FacilityClass Facility, vector<int> Route, int additionalnode) {
//    GRBEnv env = GRBEnv();
//
//    GRBModel model = GRBModel(env);
//    //model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
//    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
//    model.getEnv().set(GRB_IntParam_Threads, 8);
//
//    int NodeAmount = (int)Route.size() + 1;
//    vector<vector<double>> distance;
//    vector<double> demands;
//    int i, j, k;
//
//    distance.resize(NodeAmount);
//    for (i = 0; i < NodeAmount; i++) {
//        distance[i].resize(NodeAmount);
//    };
//
//    vector<vector<double>> location;
//    for (i = 0; i < NodeAmount; i++) {
//        if (i == 0) {
//            location.push_back(vector<double>());
//            location[location.size() - 1].push_back(Facility.LocationX);
//            location[location.size() - 1].push_back(Facility.LocationY);
//            demands.push_back(0);
//        }
//        else {
//            location.push_back(vector<double>());
//            location[location.size() - 1].push_back(NodeList[Route[i - 1]].LocationX);
//            location[location.size() - 1].push_back(NodeList[Route[i - 1]].LocationY);
//            demands.push_back(NodeList[Route[i - 1]].Demand);
//        }
//    }
//
//    location[location.size() - 1].push_back(NodeList[additionalnode].LocationX);
//    location[location.size() - 1].push_back(NodeList[additionalnode].LocationY);
//    demands.push_back(NodeList[additionalnode].Demand);
//
//    for (int i = 0; i < location.size(); i++) {
//        for (int j = 0; j < location.size(); j++) {
//            float DistanceX = location[i][0] - location[j][0];
//            float DistanceY = location[i][1] - location[j][1];
//            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
//        }
//    }
//
//    NodeAmount = NodeAmount + 1;
//
//    vector<vector<double>> savings;
//    savings.resize(NodeAmount);
//    for (i = 0; i < NodeAmount; i++) {
//        savings[i].resize(NodeAmount);
//    };
//
//    for (int i = 0; i < location.size(); i++) {
//        for (int j = 0; j < location.size(); j++) {
//            savings[i][j] = 0;
//            if (i > 0 && j > 0 && i != j) {
//                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
//            }
//        }
//    }
//
//    vector<vector<GRBVar>> x;
//    x.resize(NodeAmount);
//    for (i = 0; i < NodeAmount; i++) {
//        x[i].resize(NodeAmount);
//    };
//
//
//    vector<GRBVar> y;
//    y.resize(NodeAmount);
//
//    // Create 3-D array of model variables
//
//    for (i = 0; i < NodeAmount; i++) {
//        for (j = 0; j < NodeAmount; j++) {
//            string s = "X_" + to_string(i) + "_" + to_string(j);
//            x[i][j] = model.addVar(0.0, 1.0, savings[i][j], GRB_BINARY, s); //from i to j
//        }
//    }
//
//    for (j = 1; j < NodeAmount; j++) {
//        string s = "Y_" + to_string(j);
//        y[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
//    };
//
//    //every customer should be visited by one vehicle (both entering and exiting
//    GRBLinExpr customervisit = 0;
//    for (i = 1; i < NodeAmount; i++) {
//        customervisit += x[0][i];
//    }
//    model.addConstr(customervisit, GRB_EQUAL, 1);
//
//    customervisit = 0;
//    for (i = 1; i < NodeAmount; i++) {
//        customervisit += x[0][i];
//    }
//    model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
//
//
//    for (int j = 1; j < NodeAmount; j++) {
//        GRBLinExpr customervisit = 0;
//        for (i = 0; i < NodeAmount; i++) {
//            if (j != i) {
//                customervisit += x[i][j];
//            }
//        }
//        model.addConstr(customervisit, GRB_EQUAL, 1.0);
//    }
//
//    //The arc to itself should be zero
//    for (i = 0; i < NodeAmount; i++) {
//        GRBLinExpr customervisit = 0;
//        customervisit += x[i][i];
//        model.addConstr(customervisit, GRB_EQUAL, 0.0);
//    }
//
//    for (i = 1; i < NodeAmount; i++) {
//        for (j = 1; j < NodeAmount; j++) {
//            if (i != j) {
//                GRBLinExpr Condv = 0;
//                Condv += y[i] + (demands[j] * x[i][j]) - (Facility.Capacity*(1-x[i][j]));
//                model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
//            }
//        }
//    }
//
//    for (i = 1; i < Route.size() - 1; i++) {
//        model.addConstr(y[i], GRB_LESS_EQUAL, y[i+1]);
//    }
//
//    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
//
//    model.optimize();
//
//    float result;
//
//    if (model.get(GRB_IntAttr_SolCount) > 0) {
//        result = model.get(GRB_DoubleAttr_ObjVal);
//    } else {
//        result = -1;
//    }
//
    float result = 0;
    return result;
}


vector<int> c_Exact::determineNewSequence(vector<NodeClass> NodeList, FacilityClass Facility, vector<int> Route, int requesta, int requestb) {
//    GRBEnv env = GRBEnv();
//
//    GRBModel model = GRBModel(env);
//    //model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
//    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
//    model.getEnv().set(GRB_IntParam_Threads, 1);
//
//    int NodeAmount = (int)Route.size() + 1;
//    vector<vector<double>> distance;
//    vector<double> demands;
//    int i, j, k;
//
//    distance.resize(NodeAmount);
//    for (i = 0; i < NodeAmount; i++) {
//        distance[i].resize(NodeAmount);
//    };
//
//    int loc_a;
//    int loc_b;
//    for(int i = 0; i < Route.size(); i++) {
//        if (Route[i] == requesta) {
//            loc_a = i + 1;
//        }
//        if (Route[i] == requestb) {
//            loc_b = i + 1;
//        }
//    }
//
//    vector<vector<double>> location;
//    for (i = 0; i < NodeAmount; i++) {
//        if (i == 0) {
//            location.push_back(vector<double>());
//            location[location.size() - 1].push_back(Facility.LocationX);
//            location[location.size() - 1].push_back(Facility.LocationY);
//            demands.push_back(0);
//        }
//        else {
//            location.push_back(vector<double>());
//            location[location.size() - 1].push_back(NodeList[Route[i - 1]].LocationX);
//            location[location.size() - 1].push_back(NodeList[Route[i - 1]].LocationY);
//            demands.push_back(NodeList[Route[i - 1]].Demand);
//        }
//    }
//
//    for (int i = 0; i < location.size(); i++) {
//        for (int j = 0; j < location.size(); j++) {
//            float DistanceX = location[i][0] - location[j][0];
//            float DistanceY = location[i][1] - location[j][1];
//            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
//        }
//    }
//
//    vector<vector<double>> savings;
//    savings.resize(NodeAmount);
//    for (i = 0; i < NodeAmount; i++) {
//        savings[i].resize(NodeAmount);
//    };
//
//    for (int i = 0; i < location.size(); i++) {
//        for (int j = 0; j < location.size(); j++) {
//            savings[i][j] = 0;
//            if (i > 0 && j > 0 && i != j) {
//                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
//            }
//        }
//    }
//
//    vector<vector<GRBVar>> x;
//    x.resize(NodeAmount);
//    for (i = 0; i < NodeAmount; i++) {
//        x[i].resize(NodeAmount);
//    };
//
//
//    vector<GRBVar> y;
//    y.resize(NodeAmount);
//
//    // Create 3-D array of model variables
//
//    for (i = 0; i < NodeAmount; i++) {
//        for (j = 0; j < NodeAmount; j++) {
//            string s = "X_" + to_string(i) + "_" + to_string(j);
//            x[i][j] = model.addVar(0.0, 1.0, savings[i][j], GRB_BINARY, s); //from i to j
//        }
//    }
//
//    for (j = 1; j < NodeAmount; j++) {
//        string s = "Y_" + to_string(j);
//        y[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
//    };
//
//    //every customer should be visited by one vehicle (both entering and exiting
//    GRBLinExpr customervisit = 0;
//    for (i = 1; i < NodeAmount; i++) {
//        customervisit += x[0][i];
//    }
//    model.addConstr(customervisit, GRB_EQUAL, 1);
//
//    customervisit = 0;
//    for (i = 1; i < NodeAmount; i++) {
//        customervisit += x[0][i];
//    }
//    model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
//
//
//    for (int j = 1; j < NodeAmount; j++) {
//        GRBLinExpr customervisit = 0;
//        for (i = 0; i < NodeAmount; i++) {
//            if (j != i) {
//                customervisit += x[i][j];
//            }
//        }
//        model.addConstr(customervisit, GRB_EQUAL, 1.0);
//    }
//
//    //The arc to itself should be zero
//    for (i = 0; i < NodeAmount; i++) {
//        GRBLinExpr customervisit = 0;
//        customervisit += x[i][i];
//        model.addConstr(customervisit, GRB_EQUAL, 0.0);
//    }
//
//    for (i = 1; i < NodeAmount; i++) {
//        for (j = 1; j < NodeAmount; j++) {
//            if (i != j) {
//                GRBLinExpr Condv = 0;
//                Condv += y[i] + (demands[j] * x[i][j]) - (Facility.Capacity*(1-x[i][j]));
//                model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
//            }
//        }
//    }
//
//    model.addConstr(x[loc_a][loc_b], GRB_EQUAL, 1.0);
//
//    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
//
//    model.optimize();
//
//    vector<int> result;
//
//    vector<int> RouteResult;
//    RouteResult.clear();
//    int t_I;
//    if (model.get(GRB_IntAttr_SolCount) > 0) {
//        for (int i = 0; i < NodeAmount; i++) {
//            if (x[0][i].get(GRB_DoubleAttr_X) > 0.5) {
//                bool done = false;
//                RouteResult.push_back(Route[i-1]);
//
//                t_I = i;
//                while (done == false) {
//                    done = true;
//                    for (int j = 1; j < NodeAmount; j++) {
//                        if (x[t_I][j].get(GRB_DoubleAttr_X) > 0.5) {
//                            RouteResult.push_back(Route[j-1]);
//                            done = false;
//                            t_I = j;
//                        }
//                    }
//                }
//            }
//        }
//        result = RouteResult;
//    } else {
//        result = RouteResult;
//    }
    vector<int> result;
    return result;
}


void c_Exact(){
    
    
}


vector<vector<int>> c_Exact::DetermineBaseFromConstraints(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> Constraints) {
//    vector<vector<int>> result;
//
//    vector<vector<bool>> PresentinRoute;
//
//    if (Constraints.size() > 0) {
//        GRBEnv env = GRBEnv();
//
//        GRBModel model = GRBModel(env);
//        model.getEnv().set(GRB_IntParam_OutputFlag, 0);
//        model.getEnv().set(GRB_IntParam_Threads, 8);
//
//        vector<GRBVar> x;
//        x.resize(Facility.FirstStageNodes.size());
//
//        GRBVar **a;
//        a = new GRBVar*[Facility.FirstStageNodes.size()];
//        for (int i = 0; i < Facility.FirstStageNodes.size(); i++)
//            a[i] = new GRBVar[Facility.FirstStageNodes.size()];
//
//        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
//            string s = "X_" + to_string(i);
//            x[i] = model.addVar(0.0, 1000, 1.0, GRB_INTEGER, s);
//        };
//
//        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
//            for (int j = 0; j < Facility.FirstStageNodes.size(); j++) {
//                string s = "A_" + to_string(i) + to_string(j);
//                a[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
//            }
//        }
//
//        for (int i = 0; i < Constraints.size(); i++) {
//            model.addConstr(x[Constraints[i][1]-1], GRB_GREATER_EQUAL, 1.0);
//            model.addConstr(x[Constraints[i][2]-1], GRB_GREATER_EQUAL, 1.0);
//            if (Constraints[i][0] == 0) {
//                model.addConstr(x[Constraints[i][2]-1], GRB_EQUAL, x[Constraints[i][1]-1]);
//            }
//            if (Constraints[i][0] == 1) {
//                model.addConstr( x[Constraints[i][1]-1] - x[Constraints[i][2]-1] + ( intbigM * a[Constraints[i][1]-1][Constraints[i][2]-1] ), GRB_GREATER_EQUAL, 1.0 );
//                model.addConstr( x[Constraints[i][1]-1] - x[Constraints[i][2]-1] + ( intbigM * a[Constraints[i][1]-1][Constraints[i][2]-1] ), GRB_LESS_EQUAL, intbigM - 1 );
//            }
//        }
//
//        model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
//
//        model.optimize();
//
//
//        vector<int> X_res;
//
//        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
//            X_res.push_back(x[i].get(GRB_DoubleAttr_X));
//        }
//
//        int maxnum = 0;
//        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
//            if (X_res[i] > maxnum) {
//                maxnum =X_res[i];
//            }
//        }
//
//        result.resize(maxnum);
//
//        for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
//            if (X_res[i] != 0) {
//                result[X_res[i]-1].push_back(i+1);
//            }
//        }
//    }
    vector<vector<int>> result;
    return result;
}

vector<ScenarioClass> c_Exact::GenerateSamplePresence(vector<NodeClass> NodeList, FacilityClass Facility, int sample_size, int lowerbound, int upperbound) {
    vector<ScenarioClass> sample;
        
    //int sample_size_corrected = sample_size;
    
    int width = -1;
    
    vector<int> distribution;
    
    vector<int> distamount;
    
    distribution.resize((upperbound - lowerbound + 1) );
    
    bool valid = false;
    
    vector<vector<int>> aggregated_distribution;
    
    int distribution_size = (int) distribution.size();
    
    aggregated_distribution.resize(sample_size);
    
    vector<int> present_nodes;
    vector<int> non_present_nodes;
    
    present_nodes.clear();
    non_present_nodes.clear();

    if (sample_size < distribution_size) { //THEN AGGREGATE
        
        //increase width until it matches exactly with distribution
        if ( distribution_size % sample_size == 0) {
            width = distribution_size / sample_size;
            valid = true;
        } else {
            //cout << "The sample size is invalid!";
            int s_size = sample_size;
            while ( distribution_size % s_size != 0) {
                //Here the size of the sample is decreased -> faster calculation time but lower precision
                s_size = s_size - 1;
            }
            
            width = distribution_size / s_size;
            
            sample_size = s_size;
            
            valid = true;
            
        }
        
        //int iterator = 0;
        
        if (valid == true ) { // width matches distribution.
            int high = 0 + width;
            int low = 0;
            
            //for each width sample distribution
            while ( high < distribution_size) {
                float p = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
                float cumulative = 0;
                for(int i = 0; i < width; i++) { // select individual category from width by probability p
                    float float_width = width;
                    float i_float = i;
                    float lb = i_float/float_width;
                    float ub = (i_float+1)/float_width;
                    if ( p >= lb && p < ub ) {
                        //Sample initialization
                        ScenarioClass t_Scenario;
                        present_nodes.clear();
                        non_present_nodes.clear();
                        for (int j = 0; j < Facility.FirstStageNodes.size(); j++) {
                            present_nodes.push_back(Facility.FirstStageNodes[j]);
                        }
                        for (int j = 0; j < Facility.SecondStageNodes.size(); j++) {
                            non_present_nodes.push_back(Facility.SecondStageNodes[j]);
                        }
                        //Add Nodes
                        
                        // Lowerbound = lowerbound of the distribution
                        // low = which "width" we are in the distribution
                        // i = individual category in the distribution
                        //START ADDING NODES UNTIL THE SCENARIO IS THE REQUIRED SIZE
                        while (present_nodes.size() < Facility.FirstStageNodes.size() + lowerbound + low + i) {
                            int selected;
                            if ( non_present_nodes.size() > 1) {
                                selected = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / ((int) non_present_nodes.size() - 1)));
                            } else {
                                selected = 0;
                            }
                            present_nodes.push_back(non_present_nodes[selected]);
                            non_present_nodes.erase(non_present_nodes.begin() + selected);
                        }
                        //Set demands for nodes in the scenario.
                        for (int j = 1; j < NodeList.size(); j++ ) {
                            if (NodeList[j].Stochastic == true  && NodeList[j].DiscreteDemand.size() > 1) {
                                int int_demand = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / ((int) NodeList[j].DiscreteDemand.size() - 1)));
                                float randomdemand = NodeList[j].DiscreteDemand[int_demand][0];
                                t_Scenario.Demands.push_back(randomdemand);
                            } else {
                                t_Scenario.Demands.push_back(NodeList[j].Demand);
                            }
                        }
                        t_Scenario.PresentNodes = present_nodes;
                        t_Scenario.NotPresentNodes = non_present_nodes;
                        sample.push_back(t_Scenario);
                    }
                }
                
                //iterator = iterator + 1;
                high = high + width;
                low = low + width;
            }
        }
        
    } else { //DISSAGGREGATE
        vector<int> sample_dist;
        
        sample_dist.resize(distribution.size()); // empty at first
        
        //intialize all at 1.
        for ( int i = 0; i < sample_dist.size(); i++) {
            sample_dist[i] = 1;
        }
        
        int sum_sample = 0;
        
        for ( int i = 0; i < sample_dist.size(); i++) {
            sum_sample = sample_dist[i] + sum_sample;
        }
        
        int upperlimit = (int) sample_dist.size() - 2;
        
        int half = ceil(( (int) sample_dist.size()/2 ));
        int opposite = ceil(( (int) sample_dist.size()/2 ) - 1);
        
        while (sum_sample < sample_size && opposite >= 0) { //start adding to sample_dist
            bool placed = false;
            
            if (half == upperlimit) {
                if( sample_dist[half - 1] != sample_dist[upperlimit]) {
                    sample_dist[half] = sample_dist[half] + 1;
                    
                    if(opposite != 0) {sample_dist[opposite] = sample_dist[opposite] + 1;};
                    
                    half = ceil(( (int) sample_dist.size()/2 ));
                    
                    opposite = ceil(( (int) sample_dist.size()/2 ) - 1);
                    
                    upperlimit = upperlimit - 1 ;
                    
                    if ( upperlimit <= ceil( ( (int) sample_dist.size()/2 )) + 1 ) { upperlimit = ceil( ( (int) sample_dist.size()/2 )) + 1; };
                    placed = true;
                } else {
                    sample_dist[half] = sample_dist[half] + 1;
                    if(opposite != 0) {sample_dist[opposite] = sample_dist[opposite] + 1;};
                    placed = true;
                }
            }
            
            if ( placed == false) {
                
                if (half != upperlimit) {
                    sample_dist[half] = sample_dist[half] + 1;
                    placed = true;
                }
                half = half + 1;

                if (opposite != 0) {
                    sample_dist[opposite] = sample_dist[opposite] + 1;
                    placed = true;
                }
                opposite = opposite - 1;
            }
            
            sum_sample = 0;
            
            for ( int i = 0; i < sample_dist.size(); i++) {
                sum_sample = sample_dist[i] + sum_sample;
            }
            
        }
        
        vector<int> mem_pos;
        vector<int> present_nodes;
        vector<int> non_present_nodes;
        
        for (int i = 0; i < sample_dist.size(); i++) {
            for (int j = 0; j < sample_dist[i]; j++) {
                ScenarioClass t_Scenario;
                present_nodes.clear();
                non_present_nodes.clear();
                for (int k = 0; k < Facility.FirstStageNodes.size(); k++) {
                    present_nodes.push_back(Facility.FirstStageNodes[k]);
                }
                for (int k = 0; k < Facility.SecondStageNodes.size(); k++) {
                    non_present_nodes.push_back(Facility.SecondStageNodes[k]);
                }
                //Set requests present
                while (present_nodes.size() < Facility.FirstStageNodes.size() + lowerbound + i) {
                    
                    int selected;
                    if ( non_present_nodes.size() > 1) {
                        selected = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / ((int) non_present_nodes.size() - 1)));
                    } else {
                        selected = 0;
                    }
                    present_nodes.push_back(non_present_nodes[selected]);
                    non_present_nodes.erase(non_present_nodes.begin() + selected);
                }
                //Set demands
                for (int j = 1; j < NodeList.size(); j++ ) {
                    if (NodeList[j].Stochastic == true  && NodeList[j].DiscreteDemand.size() > 1) {
                        int int_demand = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / ((int) NodeList[j].DiscreteDemand.size() - 1)));
                        float randomdemand = NodeList[j].DiscreteDemand[int_demand][0];
                        t_Scenario.Demands.push_back(randomdemand);
                    } else {
                        t_Scenario.Demands.push_back(NodeList[j].Demand);
                    }
                }
                t_Scenario.PresentNodes = present_nodes;
                t_Scenario.NotPresentNodes = non_present_nodes;
                sample.push_back(t_Scenario);
            }
        }
    }
    
    return sample;
}

vector<ScenarioClass> c_Exact::GenerateSampleDemand(vector<NodeClass> NodeList, FacilityClass Facility, int sample_size, int lowerbound, int upperbound, int presencelevel) {
        vector<ScenarioClass> sample;
        
        int width = -1;
        
        vector<int> distribution;
        
        vector<int> distamount;
        
        distribution.resize((upperbound - lowerbound));
        
        bool valid = false;
        
        vector<vector<int>> aggregated_distribution;
        
        int distribution_size = (int) distribution.size();
        
        aggregated_distribution.resize(sample_size);
        
        vector<int> present_nodes;
        vector<int> present_nodes_dem;
        vector<int> non_present_nodes;
        
        present_nodes.clear();
        non_present_nodes.clear();
        present_nodes_dem.clear();

        if (sample_size < distribution_size) { //THEN AGGREGATE
            
            //increase width until it matches exactly with distribution
            if ( distribution_size % sample_size == 0) {
                width = distribution_size / sample_size;
                valid = true;
            } else {
                //out << "The sample size is invalid!";
                int s_size = sample_size;
                while ( distribution_size % s_size != 0) {
                    s_size = s_size - 1;
                }
                
                width = distribution_size / s_size;
                
                sample_size = s_size;
                
                valid = true;
                
            }
            
            if (valid == true ) { // width matches distribution.
                int high = 0 + width;
                int low = 0;
                
                while ( high < distribution.size()) {
                    float p = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
                    float cumulative = 0;
                    for(int i = 0; i < width; i++) {
                        float float_width = width;
                        float i_float = i;
                        float lb = i_float/float_width;
                        float ub = (i_float+1)/float_width;
                        if ( p >= lb && p < ub ) {
                            // Nodes Present found
                            ScenarioClass t_Scenario;
                            present_nodes.clear();
                            non_present_nodes.clear();
                            present_nodes_dem.clear();
                            for (int j = 0; j < Facility.FirstStageNodes.size(); j++) {
                                present_nodes.push_back(Facility.FirstStageNodes[j]);
                            }
                            for (int j = 0; j < Facility.SecondStageNodes.size(); j++) {
                                non_present_nodes.push_back(Facility.SecondStageNodes[j]);
                            }
                            //Add Nodes
                            while (present_nodes.size() < presencelevel + Facility.FirstStageNodes.size()) {
                                int selected;
                                if ( non_present_nodes.size() > 1) {
                                    selected = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / ((int) non_present_nodes.size() - 1)));
                                } else {
                                    selected = 0;
                                }
                                present_nodes.push_back(non_present_nodes[selected]);
                                present_nodes_dem.push_back(non_present_nodes[selected]);
                                non_present_nodes.erase(non_present_nodes.begin() + selected);
                            }
                            //Set demands
                            int basedemand = 0;
                            for (int j = 0; j < Facility.FirstStageNodes.size(); j++) {
                                t_Scenario.Demands.push_back(NodeList[Facility.FirstStageNodes[j]].Demand);
                                basedemand = basedemand + NodeList[Facility.FirstStageNodes[j]].Demand;
                            }
                            for (int j = 0; j < present_nodes_dem.size(); j++) {
                                t_Scenario.Demands.push_back(NodeList[present_nodes_dem[j]].DiscreteDemand[0][0]);
                            }
                            float sumdemand = 0;
                            for (int i = 0; i < t_Scenario.Demands.size(); i++) {
                                sumdemand = sumdemand + t_Scenario.Demands[i];
                            }
                            
                            vector<int> counter;
                            counter.resize(present_nodes_dem.size());
                            for(int w = 0; w < counter.size(); w++) {
                                counter[w] = 0;
                            }
                            while (sumdemand < basedemand + lowerbound + low + i) {
                                int rand_stoch = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (int) present_nodes_dem.size())) - 1;
                                int dem_node = rand_stoch + Facility.FirstStageNodes.size() - 1;
                                
                                if ( counter[rand_stoch] <  NodeList[present_nodes_dem[rand_stoch]].DiscreteDemand.size()) {
                                    t_Scenario.Demands[dem_node] = t_Scenario.Demands[dem_node] + NodeList[present_nodes_dem[rand_stoch]].DiscreteDemand[counter[rand_stoch]][0];
                                    counter[rand_stoch] = counter[rand_stoch] + 1;
                                }
                                
                                sumdemand = 0;
                                for (int i = 0; i < t_Scenario.Demands.size(); i++) {
                                    sumdemand = sumdemand + t_Scenario.Demands[i];
                                }
                            }
                            t_Scenario.PresentNodes = present_nodes;
                            t_Scenario.NotPresentNodes = non_present_nodes;
                            sample.push_back(t_Scenario);
                        }
                    }
                    
                    //iterator = iterator + 1;
                    high = high + width;
                    low = low + width;
                }
            }
            
        } else { //DISSAGGREGATE
            vector<int> sample_dist;
            
            sample_dist.resize(distribution.size()); // empty at first
            
            for ( int i = 0; i < sample_dist.size(); i++) {
                sample_dist[i] = 1;
            }
            
            int sum_sample = 0;
            
            for ( int i = 0; i < sample_dist.size(); i++) {
                sum_sample = sample_dist[i] + sum_sample;
            }
            
            int minimumheight = 0;
            
            int upperlimit = (int) sample_dist.size() - 1;
            
            int half = ceil(( (int) sample_dist.size()/2 ));
            int opposite = ceil(( (int) sample_dist.size()/2 ) - 1);
            
            while (sum_sample < sample_size && opposite >= 0) { //start adding to sample_dist
                bool placed = false;
                
                if (half == upperlimit) {
                    if( sample_dist[half - 1] != sample_dist[upperlimit]) {
                        sample_dist[half] = sample_dist[half] + 1;
                        sample_dist[opposite] = sample_dist[opposite] + 1;
                        
                        half = ceil(( (int) sample_dist.size()/2 ));
                        
                        opposite = ceil(( (int) sample_dist.size()/2 ) - 1);
                        
                        upperlimit = upperlimit - 1 ;
                        
                        if ( upperlimit <= ceil( ( (int) sample_dist.size()/2 )) + 1 ) { upperlimit = ceil( ( (int) sample_dist.size()/2 )) + 1; };
                        placed = true;
                    } else {
                        sample_dist[half] = sample_dist[half] + 1;
                        sample_dist[opposite] = sample_dist[opposite] + 1;
                        placed = true;
                    }
                }
                
                if ( placed == false) {
                    sample_dist[half] = sample_dist[half] + 1;
                    half = half + 1;
                    
                    sample_dist[opposite] = sample_dist[opposite] + 1;
                    opposite = opposite - 1;

                    placed = true;
                }
                
                sum_sample = 0;
                
                for ( int i = 0; i < sample_dist.size(); i++) {
                    sum_sample = sample_dist[i] + sum_sample;
                }
            }
            
            for (int i = 0; i < sample_dist.size(); i++) {
                for (int j = 0; j < sample_dist[i]; j++) {
                    ScenarioClass t_Scenario;
                    present_nodes.clear();
                    non_present_nodes.clear();
                    present_nodes_dem.clear();
                    for (int j = 0; j < Facility.FirstStageNodes.size(); j++) {
                        present_nodes.push_back(Facility.FirstStageNodes[j]);
                    }
                    for (int j = 0; j < Facility.SecondStageNodes.size(); j++) {
                        non_present_nodes.push_back(Facility.SecondStageNodes[j]);
                    }
                    //Add Nodes
                    while (present_nodes.size() < presencelevel + Facility.FirstStageNodes.size()) {
                        int selected;
                        if ( non_present_nodes.size() > 1) {
                            selected = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / ((int) non_present_nodes.size() - 1)));
                        } else {
                            selected = 0;
                        }
                        present_nodes.push_back(non_present_nodes[selected]);
                        present_nodes_dem.push_back(non_present_nodes[selected]);
                        non_present_nodes.erase(non_present_nodes.begin() + selected);
                    }
                    //Set demands
                    
                    int basedemand = 0;
                    for (int j = 0; j < Facility.FirstStageNodes.size(); j++) {
                        t_Scenario.Demands.push_back(NodeList[Facility.FirstStageNodes[j]].Demand);
                        basedemand = basedemand + NodeList[Facility.FirstStageNodes[j]].Demand;
                    }
                    for (int j = 0; j < present_nodes_dem.size(); j++) {
                        t_Scenario.Demands.push_back(NodeList[present_nodes_dem[j]].DiscreteDemand[0][0]);
                    }
                    float sumdemand = 0;
                    for (int i = 0; i < t_Scenario.Demands.size(); i++) {
                        sumdemand = sumdemand + t_Scenario.Demands[i];
                    }
                    
                    vector<int> counter;
                    counter.resize(present_nodes_dem.size());
                    for(int w = 0; w < counter.size(); w++) {
                        counter[w] = 0;
                    }
                    while (sumdemand < basedemand + lowerbound + i) {
                        int rand_stoch = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (int) present_nodes_dem.size()));
                        int dem_node = rand_stoch + Facility.FirstStageNodes.size() - 1;
                        
                        if ( counter[rand_stoch] <  NodeList[present_nodes_dem[rand_stoch]].DiscreteDemand.size()) {
                            t_Scenario.Demands[dem_node] = t_Scenario.Demands[dem_node] + NodeList[present_nodes_dem[rand_stoch]].DiscreteDemand[counter[rand_stoch]][0];
                            counter[rand_stoch] = counter[rand_stoch] + 1;
                        }
                        
                        sumdemand = 0;
                        for (int i = 0; i < t_Scenario.Demands.size(); i++) {
                            sumdemand = sumdemand + t_Scenario.Demands[i];
                        }
                    }

                    t_Scenario.PresentNodes = present_nodes;
                    t_Scenario.NotPresentNodes = non_present_nodes;
                    sample.push_back(t_Scenario);
                }
            }
        }
        
        return sample;
}



float c_Exact::RunHeuristic(vector<NodeClass> NodeList, FacilityClass Facility, int initial_sample_size) {
    //PLEASE ONLY RUN WITH UNITARY DEMAND
    float result;
    
    float width = ceil((Facility.SecondStageNodes.size() + 1) * 0.5);
    int upperbound = Facility.SecondStageNodes.size();
    int lowerbound = 0;
    int minimum = Facility.SecondStageNodes.size() / 2;
    
    vector<ScenarioClass> CurrentSample;
    vector<float> DiffPerScenario;
    vector<ScenarioClass> TotalSample;
    TotalSample.clear();
    
    while (width > 1) {
        //Determine the sample
        CurrentSample = GenerateSamplePresence(NodeList, Facility, initial_sample_size, lowerbound, upperbound);
        DiffPerScenario.resize(CurrentSample.size());
    
        FacilityClass F1 = Facility;
        
        #pragma omp parallel for
        //Plan without second stage nodes (optimal first stage per scenario
        for (int i = 0; i < CurrentSample.size(); i++) {
            FacilityClass F_Temp = F1;
            F_Temp.ServiceNodes = CurrentSample[i].PresentNodes;
            F_Temp = RandomizedClarkeWright( NodeList, F_Temp);
            cout << "present " << CurrentSample[i].PresentNodes.size() << ", cost " << CalculateFacilityCosts(F_Temp, NodeList);
            for ( int w = 0; w < F_Temp.RouteList.size(); w++ ) {
                cout << " {";
                for (int k = 0; k < F_Temp.RouteList[w].size(); k++) {
                    cout << F_Temp.RouteList[w][k] << " ";
                }
                cout << "} ";
            }
            cout << endl;
            CurrentSample[i].CurOpt = F_Temp.RouteList;
            for ( int x = 0; x < F_Temp.SecondStageNodes.size(); x++) {
                for(int y = 0; y < F_Temp.RouteList.size(); y++) {
                    for( int z = 0; z < F_Temp.RouteList[y].size(); z++) {
                        if( F_Temp.RouteList[y][z] == F_Temp.SecondStageNodes[x]) {
                           F_Temp.RouteList[y].erase(F_Temp.RouteList[y].begin() + z);
                       }
                    }
                }
            }
            CurrentSample[i].CurDet = F_Temp.RouteList;
        }
        
        for (int i = 0; i < CurrentSample.size(); i++) {
            TotalSample.push_back(CurrentSample[i]);
        }
    
        #pragma omp parallel for
        //Calculate per Sample 
        for (int i = 0; i < CurrentSample.size(); i++) {
            
            float sumdifference = 0;
            FacilityClass F_Temp = F1;
            
            F_Temp.RouteList = CurrentSample[i].CurDet;
        
            for (int j = 0; j < TotalSample.size(); j++) {
                FacilityClass F2 = Facility;
                //F2.ServiceNodes = CurrentSample[j].PresentNodes;
                F_Temp.ServiceNodes = TotalSample[j].PresentNodes;
                F2 = VRPSS2( F_Temp, NodeList);
                
                FacilityClass F3 = Facility;
                F3.RouteList = TotalSample[j].CurOpt;
                
                float inject = CalculateFacilityCosts(F2, NodeList);
                float sample = CalculateFacilityCosts(F3, NodeList);
                
                if (inject < sample) {
                    sample = inject;
                }
                
                float diff =  (inject - sample);
                
                cout << "(" << i << "," << j << ") " << diff << " ";
                sumdifference = sumdifference + diff;
            }
            
            cout << endl;
            
            DiffPerScenario[i] = sumdifference;
        }
        
        float UMin = floatbigM;
        int rMin;
        
        cout << "List of current Sample: " << endl;
        
        for (int i = 0; i < DiffPerScenario.size(); i++) {
            cout << "Amount of requests: " << CurrentSample[i].PresentNodes.size() << " - Estimated Costs: " << DiffPerScenario[i] << endl;
            
            if( DiffPerScenario[i] < UMin) {
                UMin = DiffPerScenario[i];
                rMin = i;
            }
        }
        
        cout << "Best Found - Amount of Requests: " << CurrentSample[rMin].PresentNodes.size() << " - Estimated Cost: " << DiffPerScenario[rMin] << endl;
        cout << endl;
        
        width = floor(width * 0.5);
        
        vector<vector<int>> SampleDist;
        SampleDist.clear();
        
        for (int i = 0; i < CurrentSample.size(); i++) {
            bool foundpresence = false;
            for(int j = 0; j < SampleDist.size(); j++) {
                if(CurrentSample[i].PresentNodes.size() == CurrentSample[SampleDist[j][0]].PresentNodes.size()) {
                    SampleDist[j].push_back(i);
                    foundpresence = true;
                }
            }
            if (foundpresence == false ) {
                SampleDist.push_back(vector<int>());
                SampleDist[SampleDist.size()-1].push_back(i);
            }
        }
        
        vector<float> AdjDiffSce;
        AdjDiffSce.resize(SampleDist.size());
        for(int i = 0; i<AdjDiffSce.size(); i++) {
            float sum = 0;
            for (int j = 0; j < SampleDist[i].size(); j++) {
                sum = sum + DiffPerScenario[SampleDist[i][j]];
            }
            AdjDiffSce[i] = sum / (int) SampleDist[i].size();
        }
        
        float minimumscanner = floatbigM;
        
        if (DiffPerScenario.size() > SampleDist.size()) {
            for ( int i = 0; i <  AdjDiffSce.size(); i++) {
                if (AdjDiffSce[i] < minimumscanner) {
                    minimumscanner = AdjDiffSce[i];
                    minimum = (int) (Facility.SecondStageNodes.size() - CurrentSample[SampleDist[i][0]].NotPresentNodes.size());
                }
            }
        } else {
            for (int i = 0; i < DiffPerScenario.size(); i++) {
                float minX;
                if (width > 2 && i > 0 && i < DiffPerScenario.size() - 1) {
                    minX = (0.3 * DiffPerScenario[i-1]) + DiffPerScenario[i] + (0.3 *  DiffPerScenario[i+1]);
                }
                if ( width > 2 && i == 0 ) {
                    minX = DiffPerScenario[i] + (0.6 *  DiffPerScenario[i+1]);
                }
                if ( width > 2 && i == DiffPerScenario.size() - 1 ) {
                    minX = DiffPerScenario[i] + (0.6 *  DiffPerScenario[i-1]);
                }
                
                if ( width <= 2) {
                    minX =  DiffPerScenario[i];
                }
                
                if (minX < minimumscanner) {
                    minimumscanner = minX;
                    minimum = (int) (Facility.SecondStageNodes.size() - CurrentSample[i].NotPresentNodes.size());
                }
            }
        }
        
        if ( minimum + ceil(width) > Facility.SecondStageNodes.size()) {
            upperbound = Facility.SecondStageNodes.size();
        } else {
            upperbound = minimum + ceil(width);
        }
        
        if ( minimum - ceil(width) < 0) {
            lowerbound = 0;
        } else {
            lowerbound = minimum - ceil(width);
        }
        
        cout << "Width:" << width << endl;
        cout << "Upperbound:" << upperbound << endl;
        cout << "Lowerbound:" << lowerbound << endl;
        
        cout << "Resulting presence level: " << minimum << endl;
        
        cout << endl;
    }
    
    int PresenceLevel = minimum;
    
    int firststagenodes = Facility.FirstStageNodes.size();
    
    int maxdem = NodeList[firststagenodes + minimum].DiscreteDemand[NodeList[firststagenodes + minimum].DiscreteDemand.size()-1][0];
    int mindem = NodeList[firststagenodes + minimum].DiscreteDemand[0][0];
    
    upperbound = (maxdem * PresenceLevel);
    lowerbound = (mindem * PresenceLevel);  //minimum demand = 1;
    minimum = ((upperbound - lowerbound) / 2) + lowerbound;
    width = floor((upperbound - lowerbound) / 2);
    
    while (width > 1) {
        CurrentSample = GenerateSampleDemand(NodeList, Facility, initial_sample_size, lowerbound, upperbound, PresenceLevel);
        DiffPerScenario.resize(CurrentSample.size());
    
        FacilityClass F1 = Facility;
        
        #pragma omp parallel for
        for (int i = 0; i < CurrentSample.size(); i++) {
            FacilityClass F_Temp = F1;
            F_Temp.ServiceNodes = CurrentSample[i].PresentNodes;
            F_Temp = RandomizedClarkeWright( NodeList, F_Temp);
            CurrentSample[i].CurOpt = F_Temp.RouteList;
            for ( int x = 0; x < F_Temp.SecondStageNodes.size(); x++) {
                for(int y = 0; y < F_Temp.RouteList.size(); y++) {
                    for( int z = 0; z < F_Temp.RouteList[y].size(); z++) {
                        if( F_Temp.RouteList[y][z] == F_Temp.SecondStageNodes[x]) {
                           F_Temp.RouteList[y].erase(F_Temp.RouteList[y].begin() + z);
                       }
                    }
                }
            }
            CurrentSample[i].CurDet = F_Temp.RouteList;
        }
        
        for (int i = 0; i < CurrentSample.size(); i++) {
            TotalSample.push_back(CurrentSample[i]);
        }
    
        #pragma omp parallel for
        for (int i = 0; i < CurrentSample.size(); i++) {
            
            float sumdifference = 0;
            FacilityClass F_Temp = F1;
            
            F_Temp.RouteList = CurrentSample[i].CurDet;
        
            for (int j = 0; j < TotalSample.size(); j++) {
                FacilityClass F2 = Facility;
                //F2.ServiceNodes = CurrentSample[j].PresentNodes;
                F_Temp.ServiceNodes = TotalSample[j].PresentNodes;
                F2 = VRPSS2( F_Temp, NodeList);
                
                FacilityClass F3 = Facility;
                F3.RouteList = TotalSample[j].CurOpt;
                
                float inject = CalculateFacilityCosts(F2, NodeList);
                float sample = CalculateFacilityCosts(F3, NodeList);
                
                if (inject < sample) {
                    sample = inject;
                }
                
                float diff =  (/*CurrentSample[i].Probability */ inject) - (/*CurrentSample[i].Probability */ sample);
                sumdifference = sumdifference + diff;
            }
            
            DiffPerScenario[i] = sumdifference;
        }
        
        float UMin = floatbigM;
        int rMin;
        
        cout << "List of current Sample: " << endl;
        
        for (int i = 0; i < DiffPerScenario.size(); i++) {
            cout << "Amount of requests: " << CurrentSample[i].PresentNodes.size() << " - Estimated Costs: " << DiffPerScenario[i] << endl;
            
            if( DiffPerScenario[i] < UMin) {
                UMin = DiffPerScenario[i];
                rMin = i;
            }
        }
        
        cout << "Best Found - Amount of Requests: " << CurrentSample[rMin].PresentNodes.size() << " - Estimated Cost: " << DiffPerScenario[rMin] << endl;
        cout << endl;
        
        width = floor(width * 0.5);
        
        vector<vector<int>> SampleDist;
        SampleDist.clear();
        
        for (int i = 0; i < CurrentSample.size(); i++) {
            bool foundpresence = false;
            for(int j = 0; j < SampleDist.size(); j++) {
                if(CurrentSample[i].PresentNodes.size() == CurrentSample[SampleDist[j][0]].PresentNodes.size()) {
                    SampleDist[j].push_back(i);
                    foundpresence = true;
                }
            }
            if (foundpresence == false ) {
                SampleDist.push_back(vector<int>());
                SampleDist[SampleDist.size()-1].push_back(i);
            }
        }
        
        vector<float> AdjDiffSce;
        AdjDiffSce.resize(SampleDist.size());
        for(int i = 0; i<AdjDiffSce.size(); i++) {
            float sum = 0;
            for (int j = 0; j < SampleDist[i].size(); j++) {
                sum = sum + DiffPerScenario[SampleDist[i][j]];
            }
            AdjDiffSce[i] = sum / (int) SampleDist[i].size();
        }
        
        float minimumscanner = floatbigM;
        
        if (DiffPerScenario.size() > SampleDist.size()) {
            for ( int i = 0; i <  AdjDiffSce.size(); i++) {
                if (AdjDiffSce[i] < minimumscanner) {
                    minimumscanner = AdjDiffSce[i];
                    minimum = (int) (Facility.SecondStageNodes.size() - CurrentSample[SampleDist[i][0]].NotPresentNodes.size());
                }
            }
        } else {
            for (int i = 0; i < DiffPerScenario.size(); i++) {
                float minX;
                if (width > 2 && i > 0 && i < DiffPerScenario.size() - 1) {
                    minX = (0.3 * DiffPerScenario[i-1]) + DiffPerScenario[i] + (0.3 *  DiffPerScenario[i+1]);
                }
                if ( width > 2 && i == 0 ) {
                    minX = DiffPerScenario[i] + (0.6 *  DiffPerScenario[i+1]);
                }
                if ( width > 2 && i == DiffPerScenario.size() - 1 ) {
                    minX = DiffPerScenario[i] + (0.6 *  DiffPerScenario[i-1]);
                }
                
                if ( width <= 2) {
                    minX =  DiffPerScenario[i];
                }
                
                if (minX < minimumscanner) {
                    minimumscanner = minX;
                    minimum = (int) (Facility.SecondStageNodes.size() - CurrentSample[i].NotPresentNodes.size());
                }
            }
        }
        
        if ( minimum + ceil(width) > Facility.SecondStageNodes.size()) {
            upperbound = Facility.SecondStageNodes.size();
        } else {
            upperbound = minimum + ceil(width);
        }
        
        if ( minimum - ceil(width) < 0) {
            lowerbound = 0;
        } else {
            lowerbound = minimum - ceil(width);
        }
        
        cout << "Width:" << width << endl;
        cout << "Upperbound:" << upperbound << endl;
        cout << "Lowerbound:" << lowerbound << endl;
        
        cout << "Resulting presence level: " << minimum << endl;
        
        cout << endl;
    }
    
    
    
    
    return result;
}
