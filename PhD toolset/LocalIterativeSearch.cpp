//# include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"
#include "Auction.h"
#include "LocalIterativeSearch.h"

c_LocalIterativeSearch::c_LocalIterativeSearch() {

};



bool c_LocalIterativeSearch::BranchAndCut(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<vector<int>> Constraints) {
    
    //Check Integrality
    //not integer:
        //Calculate z(x)
            //if z(x) < Z* -> stop;
            //if z(x) infeasible -> stop;
        //-> branch on first non-integer value 0 and 1,
        //Generate constraints x() <= 0 and x() >= 1 and add to constraints
        //Solve X to Xnew
        //i.e. BranchAndCut(Xnew, ConstraintsNew);
    //Else
        //Determine actual second stage cost
        //If second stage Cost < Z* -> update
    
    int FirstStageNodes =  (int)Facility.FirstStageNodes.size() + 1;
    
    float obj;
    
    TotalNodes = TotalNodes + 1;
    
    time_t start = time(NULL);
    
    vector<vector<float>> X = GurobiAltForm(NodeList, Facility, &obj);
    
    time_t end = time(NULL);
    
    TimeNode = TimeNode + difftime(end,start);
    
    if (X[0][0] != -1) {
    
        cout << endl;
        
        for (int i = 0; i < FirstStageNodes; i++) {
            for (int j = 0; j < FirstStageNodes; j++) {
                cout << X[i][j] << " ";
            }
            cout << endl;
        }
        
        cout << endl;
        
        float no_int_X = 0;
        float no_int_Y = 0;
        
        
        bool integral = true;
        for (int i = 0; i < X.size(); i++) {
            for (int j = 0; j < X[i].size(); j++) {
                if (X[i][j] != floor(X[i][j])) {
                    if( integral != false) {
                        no_int_X = i;
                        no_int_Y = j;
                    }
                    integral = false;
                }
            }
        }
        
        if (integral == false) {
            vector<vector<int>> relationships;
        
            for (int i = 0; i < FirstStageNodes; i++) {
                for( int j = 0; j < FirstStageNodes; j++) {
                    float value = X[i][j];
                    if (value > 0.0) {
                        relationships.push_back(vector<int>());
                        relationships[relationships.size()-1].push_back(i);
                        relationships[relationships.size()-1].push_back(j);
                    }
                }
            }
//            start = time(NULL);
//            float Z = AE_secondstage(NodeList, Facility, relationships);
//            end = time(NULL);
//            TimeLBF = TimeLBF + difftime(end,start);
//            
//            bool infeasible = false;
//            if (Z == -1) {
//                infeasible = true;
//            }
////
////            if (infeasible == false) {
//            //Z = 0.5*(Z - obj) + obj;
//            FoundInTime.push_back(Z);
//            Lowerbounds.push_back(Z);
//            cout << "Current Z: " << Z << endl;
            //if (Z > Zstar) {
                vector<vector<int>> t_Constraints = Constraints;
                t_Constraints.push_back(vector<int>());
                t_Constraints[t_Constraints.size()-1].push_back(no_int_X);
                t_Constraints[t_Constraints.size()-1].push_back(no_int_Y);
                t_Constraints[t_Constraints.size()-1].push_back(1);
                BranchAndCut(NodeList, Facility, t_Constraints);
                t_Constraints = Constraints;
                t_Constraints.push_back(vector<int>());
                t_Constraints[t_Constraints.size()-1].push_back(no_int_X);
                t_Constraints[t_Constraints.size()-1].push_back(no_int_Y);
                t_Constraints[t_Constraints.size()-1].push_back(0);
                BranchAndCut(NodeList, Facility, t_Constraints);
            //} else {
            //    FathomedNodesNonInteger = FathomedNodesNonInteger + 1;
            //}
//            }
        } else {
            Facility.RouteList.clear();
            int t_I;
            for (int i = 0; i < X[0].size(); i++) {
                if (X[0][i] == 1) {
                    bool done = false;
                    Facility.RouteList.push_back(vector<int>());
                    Facility.RouteList[ Facility.RouteList.size()-1].push_back(i);
                    
                    t_I = i;
                    while (done == false) {
                        done = true;
                        for (int j = 1; j < X[t_I].size(); j++) {
                            if (X[t_I][j] == 1) {
                                Facility.RouteList[ Facility.RouteList.size()-1].push_back(j);
                                done = false;
                                t_I = j;
                            }
                        }
                    }
                }
            }
            start = time(NULL);
            float Zbar = E_secondstage(NodeList, Facility);
            end = time(NULL);
            TimeSolution = TimeSolution + difftime(end,start);
            
            ActualCosts.push_back(Zbar);
            FoundInTime.push_back(Zbar);
            cout << endl;
            
            for (int i = 0; i < FirstStageNodes; i++) {
                for (int j = 0; j < FirstStageNodes; j++) {
                    cout << X[i][j] << " ";
                }
                cout << endl;
            }
            
            cout << endl;
            cout << "New solution found with value: " << Zbar << endl;
            if (Zbar > Zstar) {
                cout << "New incumbent found!" << endl;
                Zstar = Zbar;
                Xstar.clear();
                Xstar.resize(FirstStageNodes);
                for (int i = 0; i < Xstar.size(); i++) {
                    Xstar[i].resize(FirstStageNodes);
                }
                for (int i = 0; i < X.size(); i++) {
                    for (int j = 0; j < X[i].size(); j++) {
                        Xstar[i][j] = (int) X[i][j];
                    }
                }
            }
        }
    }
    
    
    return true;
}


//class expectedcost: public GRBCallback
//{
//public:
//    GRBVar** x;
//    GRBVar Th;
//    FacilityClass Facility;
//    vector<NodeClass> NodeList;
//    int NodeAmount;
//    vector<vector<float>> DistanceMatrix;
//    expectedcost(GRBVar** xvars, GRBVar Theta, vector<NodeClass> NL,  FacilityClass F, vector<vector<float>> D, int n) {
//        x = xvars;
//        Th = Theta;
//        Facility = F;
//        NodeList = NL;
//        DistanceMatrix = D;
//        NodeAmount = n;
//    }
//protected:
//    void callback() {
//        try {
//            if (where == GRB_CB_PRESOLVE) {
//                vector<vector<float>> x_temp;
//
//                for (int i = 0; i < NodeAmount; i++) {
//                    x_temp.push_back(vector<float>());
//                    for (int j = 0; j < NodeAmount; j++) {
//                        x_temp[i].push_back(getSolution(x[i][j]));
//                    }
//                }
//
//                cout << endl;
//
//                for (int i = 0; i < NodeAmount; i++) {
//                    for (int j = 0; j < NodeAmount; j++) {
//                        cout << x_temp[i][j] << " ";
//                    }
//                    cout << endl;
//                }
//
//                cout << endl;
//            }
//
//            if (where == GRB_CB_MIPSOL) {
//                vector<vector<float>> x_temp;
//
//                for (int i = 0; i < NodeAmount; i++) {
//                    x_temp.push_back(vector<float>());
//                    for (int j = 0; j < NodeAmount; j++) {
//                        x_temp[i].push_back(getSolution(x[i][j]));
//                    }
//                }
//
//
//                int NextNode;
//                int NN = 0;
//                Facility.RouteList.clear();
//                for (int j = 1; j < NodeAmount; j++) {
//                    if (getSolution(x[0][j]) > 0.5) {
//                        NextNode = j;
//                        Facility.RouteList.push_back(vector<int>());
//                        while(NextNode != 0) {
//                            Facility.RouteList[Facility.RouteList.size() - 1].push_back(Facility.ServiceNodes[NextNode-1]);
//                            for (int w = 0; w < NodeAmount; w++) {
//                                if (getSolution(x[NextNode][w]) > 0.5) {
//                                    NN = w;
//                                }
//                            }
//                            NextNode = NN;
//                        }
//                    }
//                }
//
//
//                c_LocalIterativeSearch clis;
//                clis.DistanceMatrix = DistanceMatrix;
//
//                float total = clis.E_secondstage(NodeList, Facility);
//
//                float current = clis.CalculateFacilityCosts(Facility, NodeList);
//
//                float Lowerbound = total - current;
//
//                int variableamount = 0;
//
//                GRBLinExpr expr = 0;
//
//                for (int i = 0; i < NodeAmount; i++) {
//                    for (int j = 0; j < NodeAmount; j++) {
//                        if (getSolution(x[i][j]) > 0.5) {
//                            expr += x[i][j];
//                            variableamount += 1;
//                        }
//                    }
//                }
//
//                expr = expr - (variableamount - 1);
//
//                addLazy(Th >= Lowerbound * expr);
//
//
//
//            }
//        } catch (GRBException e) {
//            cout << "Error number: " << e.getErrorCode() << endl;
//            cout << e.getMessage() << endl;
//        } catch (...) {
//            cout << "Error during callback" << endl;
//        }
//    }
//};
//
//FacilityClass c_LocalIterativeSearch::GurobiSolFirstStage(std::vector< NodeClass > NodeList, FacilityClass Facility) {
//    GRBEnv env = GRBEnv();
//
//    Facility.RouteList.clear();
//
//    GRBModel model = GRBModel(env);
//    model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
//    //model.getEnv().set(GRB_IntParam_OutputFlag, 0);
//    model.getEnv().set(GRB_IntParam_Threads, 8);
//
//    int NodeAmount = (int)Facility.FirstStageNodes.size() + 1;
//    vector<vector<double>> distance;
//    vector<double> demands;
//    int i, j;
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
//            location[location.size() - 1].push_back(NodeList[Facility.FirstStageNodes[i - 1]].LocationX);
//            location[location.size() - 1].push_back(NodeList[Facility.FirstStageNodes[i - 1]].LocationY);
//            demands.push_back(NodeList[Facility.FirstStageNodes[i - 1]].Demand);
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
//
//
//    GRBVar **x = NULL;
//
//    x = new GRBVar*[NodeAmount];
//    for (i = 0; i < NodeAmount; i++)
//        x[i] = new GRBVar[NodeAmount];
//
//    vector<GRBVar> u;
//    u.resize(NodeAmount);
//
//    GRBVar Theta;
//    Theta = model.addVar(0.0, floatbigM, 1, GRB_CONTINUOUS);
//
//    // Create 3-D array of model variables
//
//    for (i = 0; i < NodeAmount; i++) {
//        for (j = 0; j < NodeAmount; j++) {
//            string s = "X_" + to_string(i) + "_" + to_string(j);
//            x[i][j] = model.addVar(0.0, 1.0, distance[i][j], GRB_BINARY, s); //from i to j with vehicle k
//        }
//    };
//
//    for (j = 1; j < NodeAmount; j++) {
//        string s = "U_" + to_string(j);
//        u[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
//    };
//
//    //every customer should be visited by one vehicle (IN)
//    for (j = 1; j < NodeAmount; j++) {
//        GRBLinExpr customervisit = 0;
//        for (i = 0; i < NodeAmount; i++) {
//            if (j != i) {
//                customervisit += x[i][j];
//            }
//        }
//        model.addConstr(customervisit, GRB_EQUAL, 1.0);
//    };
//
//    //every customer should be visited by one vehicle (OUT)
//    for (j = 1; j < NodeAmount; j++) {
//        GRBLinExpr customervisit = 0;
//        for (i = 0; i < NodeAmount; i++) {
//            if (j != i) {
//                customervisit += x[j][i];
//            }
//        }
//        model.addConstr(customervisit, GRB_EQUAL, 1.0);
//    };
//
//    //Set to itself equal to 0
//    for (i = 0; i < NodeAmount; i++) {
//        GRBLinExpr customervisit = 0;
//        customervisit += x[i][i];
//        model.addConstr(customervisit, GRB_EQUAL, 0.0);
//    };
//
//
//    for (i = 1; i < NodeAmount; i++) {
//        GRBLinExpr Condf = 0;
//        Condf += u[i];
//        model.addConstr(Condf, GRB_LESS_EQUAL, Facility.Capacity + ((demands[i] - Facility.Capacity) * x[0][i]));
//    };
//
//    for (i = 1; i < NodeAmount; i++) {
//        for (j = 1; j < NodeAmount; j++) {
//            if (i != j) {
//                GRBLinExpr Condv = 0;
//                Condv += u[i] - u[j] + (Facility.Capacity * x[i][j]);
//                model.addConstr(Condv, GRB_LESS_EQUAL, Facility.Capacity - demands[j]);
//            }
//        }
//    }
//
//    // Set Callback
//
//    expectedcost cb = expectedcost(x, Theta, NodeList, Facility, DistanceMatrix, NodeAmount);
//    model.setCallback(&cb);
//
//    //
//
//    GRBLinExpr obj = 0.0;
//
//    for (i = 0; i < NodeAmount; i++) {
//        for (j = 0; j < NodeAmount; j++) {
//            if ( i != j) {
//                obj += x[i][j] * distance[i][j];
//            }
//        }
//    };
//
//    obj += Theta;
//
//    model.setObjective(obj, GRB_MINIMIZE);
//
//    model.optimize();
//
//    int NextNode;
//    int NN = 0;
//
//    for (j = 1; j < NodeAmount; j++) {
//        if (x[0][j].get(GRB_DoubleAttr_X) > 0) {
//            NextNode = j;
//            Facility.RouteList.push_back(vector<int>());
//            cout << "route " << j << ": ";
//            while(NextNode != 0) {
//                Facility.RouteList[Facility.RouteList.size() - 1].push_back(Facility.FirstStageNodes[NextNode-1]);
//                cout << Facility.FirstStageNodes[NextNode-1] << " ";
//                for (int w = 0; w < NodeAmount; w++) {
//                    if (x[NextNode][w].get(GRB_DoubleAttr_X) > 0) {
//                        NN = w;
//                    }
//                }
//                NextNode = NN;
//            }
//            cout << endl;
//        }
//    };
//
//    float Qx = Theta.get(GRB_DoubleAttr_X);
//
//
//
//    cout << "Dit is Qx: " << Qx << endl;
//
//
//
//    return Facility;
//
//
//}
//
//
float c_LocalIterativeSearch::EstimateSecondStageCost(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    srand(0);
    float result = 0;
    int amount = 100;
    float sum = 0;
    
    for (int i = 0; i < amount; i++) {
        BenchmarkFacility = Facility;
        BenchmarkFacility.ServiceNodes.clear();
        
        for (int j = 1; j < NodeList.size(); j++ ) {
            float prob = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
            if (prob <= NodeList[j].Probability) {
                NodeList[j].Service = true;
                BenchmarkFacility.ServiceNodes.push_back(j);
            } else {
                NodeList[j].Service = false;
            }
            if (NodeList[j].Service == true) {
                prob = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
                for (int w = 0; w < NodeList[j].DiscreteDemand.size(); w++) {
                    if (prob < NodeList[j].DiscreteDemand[w][1]) {
                        NodeList[j].Demand = NodeList[j].DiscreteDemand[w][0];
                    }
                }
            }
        }
        
        
        BenchmarkFacility = GurobiSol(NodeList, BenchmarkFacility);
        sum = sum + CalculateFacilityCosts(BenchmarkFacility, NodeList);
    };
    
    result = sum / amount;
    
    return result;
}

float c_LocalIterativeSearch::E_secondstage(vector<NodeClass> NodeList, FacilityClass Facility) {
    vector<vector<float>> ScenarioCostList;
    float avg = 0;
    //Enumerate Scenarios
    int pos = (int)Facility.FirstStageNodes.size();
    ScenarioCostList = RecursiveScenarios(NodeList, Facility, pos, 1.0, ScenarioCostList);
    
    for (int i = 0; i < ScenarioCostList.size(); i++ ) {
        avg = avg + (ScenarioCostList[i][0] * ScenarioCostList[i][1]);
    }
    return avg;
}

float c_LocalIterativeSearch::AE_secondstage(vector<NodeClass> NodeList, FacilityClass Facility, vector<vector<int>> relationships) {
    vector<vector<float>> ScenarioCostList;
    float avg = 0;
    //Enumerate Scenarios
    int pos = (int)Facility.FirstStageNodes.size();
    ScenarioCostList = AltRecursiveScenarios(NodeList, Facility, pos, 1.0, ScenarioCostList, relationships);
    
    for (int i = 0; i < ScenarioCostList.size(); i++ ) {
        avg = avg + (ScenarioCostList[i][0] * ScenarioCostList[i][1]);
    }
    return avg;
}



vector<FacilityClass> c_LocalIterativeSearch::RecursivePossibilities(vector<NodeClass> NodeList, FacilityClass Facility, vector<int> KnownNodesRemaining, vector<FacilityClass> Possibilities) {
    FacilityClass CurrentFacility = Facility;
    if (KnownNodesRemaining.size() == 0 ) {
        
        bool feasanduniq = true;
        
        //Check for feasibility
        for (int i = 0; i < CurrentFacility.RouteList.size(); i++) {
            if (CalculateRouteDemand(NodeList, CurrentFacility.RouteList[i]) > VehicleCapacity) {
                feasanduniq = false;
            }
        }
        
        //Must be unique
        if (feasanduniq == true) {
            vector<bool> found;
            bool posfound = true;
            for (int i = 0; i < Possibilities.size(); i++) {
                posfound = true;
                found.resize(CurrentFacility.RouteList.size());
                for (int j = 0; j < CurrentFacility.RouteList.size(); j++) {
                    found[j] = false;
                    for (int w = 0; w < Possibilities[i].RouteList.size(); w++ ) {
                        if (CurrentFacility.RouteList[j] == Possibilities[i].RouteList[w]) {
                            found[j] = true;
                        }
                    }
                }
                for (int j = 0; j < found.size(); j++) {
                    if (found[j] == false) {
                        posfound = false;
                    }
                }
                if (posfound == true) {
                    feasanduniq = false;
                }
            }
        }
        
        
        if (feasanduniq == true) {
            vector<vector<float>> ScenarioCostList;
            float avg = 0;
            //Enumerate Scenarios
            ScenarioCostList = RecursiveScenarios(NodeList, CurrentFacility, 1, 1, ScenarioCostList);
            
            for (int i = 0; i < ScenarioCostList.size(); i++ ) {
                avg = avg + (ScenarioCostList[i][0] * ScenarioCostList[i][1]);
            }
            CurrentFacility.Cost = avg;
            if (ScenarioCostList.size() > 0) {
                Possibilities.push_back(CurrentFacility);
            }
        }
    } else {
        for (int i = 0; i < KnownNodesRemaining.size(); i++ ) {
            vector<int> t_KNR = KnownNodesRemaining;
            t_KNR.erase(t_KNR.begin() + i);
            for (int j = 0; j < CurrentFacility.RouteList.size(); j++) {
                CurrentFacility = Facility;
                CurrentFacility.RouteList[j].push_back(KnownNodesRemaining[i]);
                Possibilities = RecursivePossibilities(NodeList,CurrentFacility,t_KNR,Possibilities);
            }
            
        }
    }
    return Possibilities;
};

vector<vector<float>> c_LocalIterativeSearch::AltRecursiveScenarios(std::vector< NodeClass > NodeList, FacilityClass Facility, int pos, float currentprobability, vector<vector<float>> ScenarioCostList, vector<vector<int>> relationships) {
//    BenchmarkFacility = Facility;
//    float bprob = currentprobability;
//
//    //Calculate Cost of current scenario
//    NodeList[pos].Service = true;
//    for (int i = 0; i < NodeList[pos].DiscreteDemand.size(); i++) {
//        bprob = currentprobability;
//        NodeList[pos].Demand = NodeList[pos].DiscreteDemand[i][0];
//        bprob = bprob * NodeList[pos].Probability * NodeList[pos].DiscreteDemand[i][2];
//        if (pos == NodeList.size() - 1) {
//            BenchmarkFacility.ServiceNodes.clear();
//            for (int j = 1; j < NodeList.size(); j++ ) {
//                if (NodeList[j].Service == true) {
//                    BenchmarkFacility.ServiceNodes.push_back(j);
//                }
//            }
//            BenchmarkFacility.RouteList = Facility.RouteList;
//            float costs = GurobiAFSS(NodeList, BenchmarkFacility, relationships);
//
//            if (costs != -1) {
//                vector<float> ScenarioCost;
//                ScenarioCost.push_back(costs);
//                ScenarioCost.push_back(bprob);
//                ScenarioCostList.push_back(ScenarioCost);
//            } else { cout << "ERROR!!! ERROR!!! ERROR!!!" << endl; }
//        } else {
//            ScenarioCostList = AltRecursiveScenarios(NodeList,Facility,pos+1,bprob, ScenarioCostList, relationships);
//        }
//    }
//
//
//    if (NodeList[pos].Probability < 1.0) {
//        NodeList[pos].Service = false;
//        bprob = currentprobability;
//        bprob = bprob * NodeList[pos].Probability;
//        if (pos == NodeList.size() - 1) {
//
//            BenchmarkFacility.ServiceNodes.clear();
//            for (int j = 1; j < NodeList.size(); j++ ) {
//                if (NodeList[j].Service == true) {
//                    BenchmarkFacility.ServiceNodes.push_back(j);
//                }
//            }
//            BenchmarkFacility.RouteList = Facility.RouteList;
//            float costs = GurobiAFSS(NodeList, BenchmarkFacility, relationships);
//
//            if (costs != -1) {
//                vector<float> ScenarioCost;
//                ScenarioCost.push_back(costs);
//                ScenarioCost.push_back(bprob);
//                ScenarioCostList.push_back(ScenarioCost);
//            } else { cout << "ERROR!!! ERROR!!! ERROR!!!" << endl; }
//        } else {
//            ScenarioCostList = AltRecursiveScenarios(NodeList,Facility,pos+1,bprob, ScenarioCostList,relationships);
//        }
//    }
    
    return ScenarioCostList;
}

vector<vector<float>> c_LocalIterativeSearch::RecursiveScenarios(std::vector< NodeClass > NodeList, FacilityClass Facility, int pos, float currentprobability, vector<vector<float>> ScenarioCostList) {
    BenchmarkFacility = Facility;
    float bprob = currentprobability;
    
    //Calculate Cost of current scenario
    NodeList[pos].Service = true;
    for (int i = 0; i < NodeList[pos].DiscreteDemand.size(); i++) {
        bprob = currentprobability;
        NodeList[pos].Demand = NodeList[pos].DiscreteDemand[i][0];
        bprob = bprob * NodeList[pos].Probability * NodeList[pos].DiscreteDemand[i][2];
        if (pos == NodeList.size() - 1) {
            BenchmarkFacility.ServiceNodes.clear();
            for (int j = 1; j < NodeList.size(); j++ ) {
                if (NodeList[j].Service == true) {
                    BenchmarkFacility.ServiceNodes.push_back(j);
                }
            }
            BenchmarkFacility.RouteList = Facility.RouteList;
            float costs;
            vector<vector<int>> rid = GurobiAltFormSecondStageKC(NodeList, BenchmarkFacility, &costs);
            
            if (costs != -1) {
                vector<float> ScenarioCost;
                ScenarioCost.push_back(costs);
                ScenarioCost.push_back(bprob);
                ScenarioCostList.push_back(ScenarioCost);
            } else { cout << "ERROR!!! ERROR!!! ERROR!!!" << endl; }
        } else {
            ScenarioCostList = RecursiveScenarios(NodeList,Facility,pos+1,bprob, ScenarioCostList);
        }
    }
    
    
    if (NodeList[pos].Probability < 1.0) {
        NodeList[pos].Service = false;
        bprob = currentprobability;
        bprob = bprob * NodeList[pos].Probability;
        if (pos == NodeList.size() - 1) {
            
            BenchmarkFacility.ServiceNodes.clear();
            for (int j = 1; j < NodeList.size(); j++ ) {
                if (NodeList[j].Service == true) {
                    BenchmarkFacility.ServiceNodes.push_back(j);
                }
            }
            BenchmarkFacility.RouteList = Facility.RouteList;
            float costs;
            vector<vector<int>> rid = GurobiAltFormSecondStageKC(NodeList, BenchmarkFacility, &costs);
            
            if (costs != -1) {
                vector<float> ScenarioCost;
                ScenarioCost.push_back(costs);
                ScenarioCost.push_back(bprob);
                ScenarioCostList.push_back(ScenarioCost);
            } else { cout << "ERROR!!! ERROR!!! ERROR!!!" << endl; }
        } else {
            ScenarioCostList = RecursiveScenarios(NodeList,Facility,pos+1,bprob, ScenarioCostList);
        }
    }
    
    return ScenarioCostList;
}

//vector<vector<float>> c_LocalIterativeSearch::AltRecursiveScenarios(std::vector< NodeClass > NodeList, FacilityClass Facility, int pos, float currentprobability, vector<vector<float>> ScenarioCostList, NodeList, Facility, relationships, Constraints) {
//    BenchmarkFacility = Facility;
//    float bprob = currentprobability;
//    
//    //Calculate Cost of current scenario
//    NodeList[pos].Service = true;
//    for (int i = 0; i < NodeList[pos].DiscreteDemand.size(); i++) {
//        bprob = currentprobability;
//        NodeList[pos].Demand = NodeList[pos].DiscreteDemand[i][0];
//        bprob = bprob * NodeList[pos].Probability * NodeList[pos].DiscreteDemand[i][2];
//        if (pos == NodeList.size() - 1) {
//            BenchmarkFacility.ServiceNodes.clear();
//            for (int j = 1; j < NodeList.size(); j++ ) {
//                if (NodeList[j].Service == true) {
//                    BenchmarkFacility.ServiceNodes.push_back(j);
//                }
//            }
//            BenchmarkFacility.RouteList = Facility.RouteList;
//            float costs = GurobiAltFormSecondStageKC(NodeList, BenchmarkFacility);
//            
//            if (costs != -1) {
//                vector<float> ScenarioCost;
//                ScenarioCost.push_back(costs);
//                ScenarioCost.push_back(bprob);
//                ScenarioCostList.push_back(ScenarioCost);
//            } else { cout << "ERROR!!! ERROR!!! ERROR!!!" << endl; }
//        } else {
//            ScenarioCostList = RecursiveScenarios(NodeList,Facility,pos+1,bprob, ScenarioCostList);
//        }
//    }
//    
//    
//    if (NodeList[pos].Probability < 1.0) {
//        NodeList[pos].Service = false;
//        bprob = currentprobability;
//        bprob = bprob * NodeList[pos].Probability;
//        if (pos == NodeList.size() - 1) {
//            
//            BenchmarkFacility.ServiceNodes.clear();
//            for (int j = 1; j < NodeList.size(); j++ ) {
//                if (NodeList[j].Service == true) {
//                    BenchmarkFacility.ServiceNodes.push_back(j);
//                }
//            }
//            BenchmarkFacility.RouteList = Facility.RouteList;
//            float costs = GurobiAltFormSecondStageKC(NodeList, BenchmarkFacility);
//            
//            if (costs != -1) {
//                vector<float> ScenarioCost;
//                ScenarioCost.push_back(costs);
//                ScenarioCost.push_back(bprob);
//                ScenarioCostList.push_back(ScenarioCost);
//            } else { cout << "ERROR!!! ERROR!!! ERROR!!!" << endl; }
//        } else {
//            ScenarioCostList = RecursiveScenarios(NodeList,Facility,pos+1,bprob, ScenarioCostList);
//        }
//    }
//    
//    return ScenarioCostList;
//}

FacilityClass c_LocalIterativeSearch::LocalSearch(vector<NodeClass> NodeList, FacilityClass Facility) {
    int RemoveVersion = 2;
    int InsertVersion = 3;
    int TimeWindowVersion = 1;
    
    FacilityClass OriginalFacility = Facility;
    
//    do {
//        OriginalFacility = Facility;
//        Facility = RemoveNode(NodeList, Facility, RemoveVersion);
//        Facility = InsertNode(NodeList, Facility, InsertVersion);
//        Facility = AllocateTimeWindows(NodeList, Facility, TimeWindowVersion);
//    } while (Improvement(NodeList, OriginalFacility, Facility));
    
    int maximprovementcount = 6;
    int i_count = 0;
    FacilityClass BestFacility = Facility;
    FacilityClass OriginalBestFacility;
    
    while ( OriginalBestFacility.RouteList != BestFacility.RouteList ) {
        OriginalBestFacility = BestFacility;
        do {
            i_count = i_count + 1;
            OriginalFacility = Facility;
            Facility = RemoveNode(NodeList, Facility, RemoveVersion);
            Facility = InsertNode(NodeList, Facility, InsertVersion);
            Facility = AllocateTimeWindows(NodeList, Facility, TimeWindowVersion);
            if (Improvement(NodeList, OriginalFacility, Facility)){
                BestFacility = Facility;
            }
        } while (i_count <= maximprovementcount);
    }
    
    Facility = BestFacility;
    
    return Facility;
}

FacilityClass c_LocalIterativeSearch::AllocateTimeWindows(vector<NodeClass> NodeList, FacilityClass Facility, int TimeWindowVersion) {
    return Facility;
};



bool c_LocalIterativeSearch::Improvement(vector<NodeClass> NodeList, FacilityClass OriginalFacility, FacilityClass ResultFacility) {
    if ( EstimateSecondStageCost(NodeList, OriginalFacility) <=  EstimateSecondStageCost(NodeList, ResultFacility)) {
        return false;
    } else {
        return true;
    }
}

FacilityClass c_LocalIterativeSearch::RemoveNode(vector<NodeClass> NodeList, FacilityClass Facility, int RemoveVersion) {
    switch (RemoveVersion) {
        case 1: return BestRemoval(NodeList, Facility);
        case 2: return RandomRemoval(NodeList, Facility);
        default: return RandomRemoval(NodeList, Facility);
    }
}

FacilityClass c_LocalIterativeSearch::InsertNode(vector<NodeClass> NodeList, FacilityClass Facility, int InsertVersion) {
    switch (InsertVersion) {
        case 1: return BestInsertion(NodeList, Facility);
        case 2: return FirstInsertion(NodeList, Facility);
        case 3: return RandomInsertion(NodeList, Facility);
        default: return FirstInsertion(NodeList, Facility);
    }
}

FacilityClass c_LocalIterativeSearch::BestRemoval(vector<NodeClass> NodeList, FacilityClass Facility) {
    Facility.Cost = EstimateSecondStageCost(NodeList, Facility);
    float  CurrentCost = floatbigM;
    int routepos = -1;
    int requestpos = -1;
    
    for (int i = 0; i < Facility.RouteList.size(); i++) {
        for (int j = 0; j < Facility.RouteList[i].size(); j++) {
            if (Facility.RouteList[i].size() > 0) {
                FacilityClass CopyFacility = Facility;
                CopyFacility.RouteList[i].erase(CopyFacility.RouteList[i].begin() + j);
                float t_prob;
                t_prob = NodeList[CopyFacility.RouteList[i][j]].Probability;
                NodeList[CopyFacility.RouteList[i][j]].Probability = 0;
                float newcost = EstimateSecondStageCost(NodeList, CopyFacility);
                if (newcost < CurrentCost) {
                    CurrentCost = newcost;
                    routepos = i;
                    requestpos = j;
                }
                NodeList[CopyFacility.RouteList[i][j]].Probability = 1;
            }
        }
    }
    
    if (requestpos > -1 && routepos > -1) {
        Facility.RouteList[routepos].erase(Facility.RouteList[routepos].begin() + requestpos);
    }
    
    Facility.RemovedNode = Facility.RouteList[routepos][requestpos];
    return Facility;
}

FacilityClass c_LocalIterativeSearch::RandomRemoval(vector<NodeClass> NodeList, FacilityClass Facility) {
    Facility.Cost = EstimateSecondStageCost(NodeList, Facility);
    vector<int> CertainRequests;
    rand();
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        if (NodeList[Facility.ServiceNodes[i]].Probability == 1) {
            CertainRequests.push_back(Facility.ServiceNodes[i]);
        }
    }
    int request = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / (CertainRequests.size())));
    request = CertainRequests[request];
    
    for (int i = 0; i < Facility.RouteList.size(); i++) {
        for (int j = 0; j < Facility.RouteList[i].size(); j++) {
            if (request == Facility.RouteList[i][j]) {
                Facility.RouteList[i].erase(Facility.RouteList[i].begin() + j);
            }
        }
    }
    
    Facility.RemovedNode = request;
    return Facility;
}



FacilityClass c_LocalIterativeSearch::BestInsertion(vector<NodeClass> NodeList, FacilityClass Facility) {
    int request = Facility.RemovedNode;
    float CurrentCost = floatbigM;
    int routepos;
    int requestpos;
    float cost;
    
    for (int i = 0; i < Facility.RouteList.size(); i++) {
        for (int j = 0; j < Facility.RouteList[i].size(); j++) {
            FacilityClass CopyFacility = Facility;
            CopyFacility.RouteList[i].insert(CopyFacility.RouteList[i].begin() + j, request);
            if (CalculateRouteDemand(NodeList,Facility.RouteList[i]) < VehicleCapacity) {
                cost = EstimateSecondStageCost(NodeList, CopyFacility);
                if (cost < CurrentCost) {
                    CurrentCost = cost;
                    routepos = i;
                    requestpos = j;
                }
            }
        }
    }
    
    for (int i = 0; i < Facility.RouteList.size(); i++) {
        FacilityClass CopyFacility = Facility;
        CopyFacility.RouteList[i].push_back(request);
        if (CalculateRouteDemand(NodeList,Facility.RouteList[i]) < VehicleCapacity) {
            cost = EstimateSecondStageCost(NodeList, CopyFacility);
            if (cost < CurrentCost) {
                CurrentCost = cost;
                routepos = i;
                requestpos = static_cast <int> (Facility.RouteList[i].size());
            }
        }
    }
    
    if (requestpos >= Facility.RouteList[routepos].size()) {
        Facility.RouteList[routepos].push_back(request);
    } else {
        Facility.RouteList[routepos].insert(Facility.RouteList[routepos].begin() + requestpos, request);
    }
    Facility.Cost = CurrentCost;
    
    return Facility;
}

FacilityClass c_LocalIterativeSearch::FirstInsertion(vector<NodeClass> NodeList, FacilityClass Facility) {
    FacilityClass CopyFacility = Facility;
    bool inserted = false;
    
    while (inserted == false) {
        int routesize = static_cast <int> (Facility.RouteList.size());
        int routepos = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / routesize));
    
        if (routepos >= Facility.RouteList.size()) {
            CopyFacility.RouteList.push_back(vector<int> ());
            CopyFacility.RouteList[routepos].push_back(Facility.RemovedNode);
            if (CalculateRouteDemand(NodeList,Facility.RouteList[routepos]) < VehicleCapacity) {
                float cost = EstimateSecondStageCost(NodeList, CopyFacility);
                if (cost < Facility.Cost) {
                    Facility = CopyFacility;
                    Facility.Cost = cost;
                    inserted = true;
                }
            }
        } else {
            int requestpos = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / Facility.RouteList[routepos].size()));
            if (requestpos >= Facility.RouteList[routepos].size()) {
                CopyFacility.RouteList[routepos].push_back(Facility.RemovedNode);
                if (CalculateRouteDemand(NodeList,Facility.RouteList[routepos]) < VehicleCapacity) {
                    float cost = EstimateSecondStageCost(NodeList, CopyFacility);
                    if (cost < Facility.Cost) {
                        Facility = CopyFacility;
                        Facility.Cost = cost;
                        inserted = true;
                    }
                }
            } else {
                CopyFacility.RouteList[routepos].insert(CopyFacility.RouteList[routepos].begin() + requestpos, Facility.RemovedNode);
                if (CalculateRouteDemand(NodeList,Facility.RouteList[routepos]) < VehicleCapacity) {
                    float cost = EstimateSecondStageCost(NodeList, CopyFacility);
                    if (cost < Facility.Cost) {
                        Facility = CopyFacility;
                        Facility.Cost = cost;
                        inserted = true;
                    }
                }
            }
        }
    
    }
    
    return Facility;
}

FacilityClass c_LocalIterativeSearch::RandomInsertion(vector<NodeClass> NodeList, FacilityClass Facility) {
    FacilityClass CopyFacility = Facility;
    bool inserted = false;
    
    while (inserted == false) {
        int routesize = static_cast <int> (Facility.RouteList.size());
        int routepos = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / routesize));
        
        if (routepos >= Facility.RouteList.size()) {
            CopyFacility.RouteList.push_back(vector<int> ());
            CopyFacility.RouteList[routepos].push_back(Facility.RemovedNode);
            if (CalculateRouteDemand(NodeList,Facility.RouteList[routepos]) < VehicleCapacity) {
                Facility = CopyFacility;
                inserted = true;
            }
        } else {
            int requestpos = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / Facility.RouteList[routepos].size()));
            if (requestpos >= Facility.RouteList[routepos].size()) {
                CopyFacility.RouteList[routepos].push_back(Facility.RemovedNode);
                if (CalculateRouteDemand(NodeList,Facility.RouteList[routepos]) < VehicleCapacity) {
                    Facility = CopyFacility;
                    inserted = true;
                }
            } else {
                CopyFacility.RouteList[routepos].insert(CopyFacility.RouteList[routepos].begin() + requestpos, Facility.RemovedNode);
                if (CalculateRouteDemand(NodeList,Facility.RouteList[routepos]) < VehicleCapacity) {
                    Facility = CopyFacility;
                    inserted = true;
                }
            }
        }
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return Facility;
}



