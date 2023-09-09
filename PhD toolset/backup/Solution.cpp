//#include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"
#include "Auction.h"
#include "LocalIterativeSearch.h"
#include "Solution.h"
#include "Exact.h"

c_SolutionSingle::c_SolutionSingle(int id, int Certain, int Uncertain, int vc, bool tw) {
    srand(id);
    int NodeAmount = Certain + Uncertain;
    VehicleCapacity = vc;
    floatbigM = 999999999;
    intbigM = 999999999;
    SolutionMethod = 1;
    GVIterations = 10000;
    TimeWindows = tw;
    Writefile = false;
    margin = 1;
    ValuationType = 1;
    
    FacilityList.clear();
    
    vector<float> TempDemand;
    vector<vector<float>> KD;
    TempDemand.push_back(1);
    TempDemand.push_back(1);
    TempDemand.push_back(1);
    KD.push_back(TempDemand);
    
    NodeList.push_back(NodeClass(0, 0, 0,1,0,0, true));
    for (int n = 1; n <= NodeAmount; n++) {
        //Randomnumber from 0 to 100
        float x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordX - 0)));
        float y = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordY - 0)));
        float demand = 1;//1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5 - 1)));
        float Probability = 1;
        if (n  > Certain) {
            Probability = 0.5;
        }
        NodeList.push_back(NodeClass(x, y, demand, Probability, 0, 0, true));
        NodeList[n].StartTW = 0;
        NodeList[n].DiscreteDemand = KD;
        NodeList[n].EndTW = floatbigM;
        NodeList[n].Service = true;
    };
    
    FacilityList.push_back(FacilityClass(50, 50));
    
    // Facility Colors
    FacilityList[0].red = 204;
    FacilityList[0].Color = "red";
    FacilityList[0].Number = 0;
    FacilityList[0].Capacity = VehicleCapacity;
    
    // Set nodes on facilities
    for (int n = 1; n <= NodeAmount; n++) {
        FacilityList[0].ServiceNodes.push_back(n);
    };
    
    for (int n = 1; n <= Certain; n++) {
        FacilityList[0].FirstStageNodes.push_back(n);
    };

    for (int n = Certain + 1; n <= Certain + Uncertain; n++) {
        FacilityList[0].SecondStageNodes.push_back(n);
    };
    
    DistanceMatrix = GenerateDM(NodeList, FacilityList);
    
    TotalNodes = 0;
    FathomedNodesInteger = 0;
    TimeNode =0;
    TimeSolution = 0;
    TimeLBF = 0;
    
};

c_Solution::c_Solution(int id, int NPF, int vc, bool tw) {
	srand(id);
	int NodeAmount = FacilityAmount * NPF;
	NodesPerFacility = NPF;
	VehicleCapacity = vc;
	floatbigM = 999999999;
	intbigM = 999999999;
	SolutionMethod = 3;
	GVIterations = 10000;
	TimeWindows = tw;
	Writefile = false;
	margin = 1;
	ValuationType = 1;

	FacilityList.clear();

	NodeList.push_back(NodeClass(0, 0, 0,1,0,0, true));
	for (int n = 1; n <= NodeAmount; n++) {
		//Randomnumber from 0 to 100
		float x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordX - 0)));
		float y = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordY - 0)));
		float demand = 1;//1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5 - 1)));
		NodeList.push_back(NodeClass(x, y, demand,1, 0, 0, true));
		NodeList[n].StartTW = 0;
		NodeList[n].EndTW = floatbigM;
        NodeList[n].Service = true;
	};

//	 Facility locations
	//FacilityList.push_back(FacilityClass(50, 50 + id));
	//FacilityList.push_back(FacilityClass(50 + (2 * 0.4472135955 * id), 50 - (0.4472135955 * id)));
	//FacilityList.push_back(FacilityClass(50 - (2 * 0.4472135955 * id), 50 - (0.4472135955 * id)));

	//float x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordX - 0)));
	//float y = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordY - 0)));
	//FacilityList.push_back(FacilityClass(x, y));
	//x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordX - 0)));
	//y = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordY - 0)));
	//FacilityList.push_back(FacilityClass(x, y));
	//x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordX - 0)));
	//y = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordY - 0)));
	//FacilityList.push_back(FacilityClass(x, y));

	FacilityList.push_back(FacilityClass(20, 20));
	FacilityList.push_back(FacilityClass(20, 80));
	FacilityList.push_back(FacilityClass(80, 50));

	// Facility Colors
	FacilityList[0].red = 204;
	FacilityList[0].Color = "red";
	FacilityList[0].Number = 0;
	FacilityList[0].Capacity = VehicleCapacity;
    FacilityList[1].green = 204;
    FacilityList[1].Color = "green";
    FacilityList[1].Number = 1;
    FacilityList[1].Capacity = VehicleCapacity;
    FacilityList[2].blue = 204;
    FacilityList[2].Color = "blue";
    FacilityList[2].Number = 2;
    FacilityList[2].Capacity = VehicleCapacity;

	// Set nodes on facilities
	for (int n = 1; n <= NPF; n++) {
		FacilityList[0].ServiceNodes.push_back(n);
	};

    for (int n = NPF + 1; n <= 2 * NPF; n++) {
        FacilityList[1].ServiceNodes.push_back(n);
    };

    for (int n = 2 * NPF + 1; n <= 3 * NPF; n++) {
        FacilityList[2].ServiceNodes.push_back(n);
    };

    CostTracker.resize(101);
    for (int i = 0; i < CostTracker.size(); i++) {
        CostTracker[i] = 0;
    };

//    if (TimeWindows == true) {
//        SolutionMethod = 2;
//
//        for (int n = 1; n <= 60; n++) {
//            NodeList[n].StartTW = 0;
//            NodeList[n].EndTW = 240;
//        };
//
//        for (int n = 1; n <= 5; n++) {
//            NodeList[n].StartTW = 0;
//            NodeList[n].EndTW = 90;
//        };
//
//        for (int n = 6; n <= 10; n++) {
//            NodeList[n].StartTW = 60;
//            NodeList[n].EndTW = 120;
//        };
//
//        for (int n = 21; n <= 25; n++) {
//            NodeList[n].StartTW = 0;
//            NodeList[n].EndTW = 90;
//        };
//
//        for (int n = 26; n <= 30; n++) {
//            NodeList[n].StartTW = 60;
//            NodeList[n].EndTW = 120;
//        };
//
//        for (int n = 41; n <= 45; n++) {
//            NodeList[n].StartTW = 0;
//            NodeList[n].EndTW = 90;
//        };
//
//        for (int n = 46; n <= 50; n++) {
//            NodeList[n].StartTW = 60;
//            NodeList[n].EndTW = 120;
//        };
//    };

	DistanceMatrix = GenerateDM(NodeList, FacilityList);
};

c_SolomonSolution::c_SolomonSolution(int id, int NPF, int vc, bool tw, const char* filename) {
    srand(id);
    NodesPerFacility = NPF;

    floatbigM = 999999999;
    intbigM = 999999999;
    SolutionMethod = 1;
    GVIterations = 40000;
    TimeWindows = tw;
    Writefile = false;
    margin = 1;
    ValuationType = 1;
    
    problem input;
    input.loadSolomon(filename);
    
    FacilityList.clear();
    
    NodeList.push_back(NodeClass(0, 0, 0,1,0,0, true));
    for (int n = 0; n <= input.allCustomer.size(); n++) {
        //Randomnumber from 0 to 100
        float x = input.allCustomer[n].x_pos;
        float y = input.allCustomer[n].y_pos;
        float demand = input.allCustomer[n].demand;
        NodeList.push_back(NodeClass(x, y, demand,1, 0, 0, true));
        NodeList[n].ServiceTime = input.allCustomer[n].unload;
        NodeList[n].StartTW = input.allCustomer[n].start;
        NodeList[n].EndTW = input.allCustomer[n].end;
    };
    
    for (int f = 0; f < input.allFacilities.size(); f++) {
        FacilityList.push_back(FacilityClass(input.allFacilities[f].x_pos, input.allFacilities[f].y_pos));
        FacilityList[f].red = 204;
        FacilityList[f].Color = "red";
        FacilityList[f].Number = 0;
        FacilityList[f].Capacity = input.capacity;
    }
    
    FacilityAmount = (int)input.allFacilities.size();
    
    CostTracker.resize(101);
    for (int i = 0; i < CostTracker.size(); i++) {
        CostTracker[i] = 0;
    };
    
    VehicleCapacity = input.capacity;
    
    // Set nodes on facilities
    for (int n = 1; n < NodeList.size(); n++) {
        FacilityList[0].ServiceNodes.push_back(n);
    };
    
    DistanceMatrix = GenerateDM(NodeList, FacilityList);
};

c_CordeauSolution::c_CordeauSolution(int id, int NPF, int vc, bool tw, const char* filename) {
    srand(id);
    NodesPerFacility = NPF;
    floatbigM = 999999999;
    intbigM = 999999999;
    SolutionMethod = 1;
    GVIterations = 12000;
    TimeWindows = tw;
    Writefile = false;
    margin = 1;
    ValuationType = 1;
    
    problem input;
    input.loadCordeau(filename);
    
    FacilityList.clear();
    
    NodeList.push_back(NodeClass(0, 0, 0,1,0,0, true));
    for (int n = 0; n < input.allCustomer.size(); n++) {
        //Randomnumber from 0 to 100
        float x = input.allCustomer[n].x_pos;
        float y = input.allCustomer[n].y_pos;
        float demand = input.allCustomer[n].demand;
        NodeList.push_back(NodeClass(x, y, demand,1, 0, 0, true));
        NodeList[n+1].StartTW = 0;
        NodeList[n+1].Service = true;
        NodeList[n+1].EndTW = floatbigM;
    };
    
    for (int f = 0; f < input.allFacilities.size(); f++) {
        FacilityList.push_back(FacilityClass(input.allFacilities[f].x_pos, input.allFacilities[f].y_pos));
        FacilityList[f].Capacity = input.Capacities[f];
    }
    
    FacilityList[0].red = 204;
    FacilityList[0].Color = "red";
    FacilityList[0].Number = 0;
    FacilityList[1].green = 204;
    FacilityList[1].Color = "green";
    FacilityList[1].Number = 1;
//    FacilityList[2].blue = 204;
//    FacilityList[2].Color = "blue";
//    FacilityList[2].Number = 2;
//    FacilityList[3].red = 255;
//    FacilityList[3].green = 255;
//    FacilityList[3].Color = "yellow";
//    FacilityList[3].Number = 3;
    
    FacilityAmount = (int)input.allFacilities.size();
    
    CostTracker.resize(101);
    for (int i = 0; i < CostTracker.size(); i++) {
        CostTracker[i] = 0;
    };
    
    // Set nodes on facilities
    for (int n = 1; n < NodeList.size(); n++) {
        int selfac = static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / (FacilityList.size())));
        FacilityList[selfac].ServiceNodes.push_back(n);
    };
    
    VehicleCapacity = input.Capacities[0];
    
    DistanceMatrix = GenerateDM(NodeList, FacilityList);
};

FacilityClass c_VRPSCDinstance::ExactSolution(vector< NodeClass > NodeList, FacilityClass Facility) {
    FacilityClass result;
    result.Cost = floatbigM;
    vector<int> KnownNodesRemaining;
    
    for (int i = 1; i < NodeList.size(); i++) {
        if (NodeList[i].Probability == 1) {
            KnownNodesRemaining.push_back(i);
        }
    }
    
    vector<FacilityClass> Possibilities;
    
    Facility.RouteList.resize(5);
    
    Possibilities = RecursivePossibilities(NodeList, Facility, KnownNodesRemaining, Possibilities);
    
    for (int i = 0; i < Possibilities.size(); i++) {
        if(Possibilities[i].Cost < result.Cost) {
            result = Possibilities[i];
        }
    }
    
    return result;
}

c_VRPSCDinstance::c_VRPSCDinstance(int id, int R, int U, bool tw) {
    srand(id);
    int NodeAmount = R + U;
    VehicleCapacity = 15;
    Certain = R;
    Uncertain = U;
    floatbigM = 999999999;
    intbigM = 999999999;
    SolutionMethod = 7;
    GVIterations = 500;
    TimeWindows = tw;
    Writefile = false;
    ValuationType = 1;
    
    FacilityList.clear();
    
    //Set disrete demand
    vector<vector<float>> DD;
    vector<float> TempDemand;
    TempDemand.push_back(2.5);
    TempDemand.push_back(0.333333333333333);
    TempDemand.push_back(0.333333333333333);
    DD.push_back(TempDemand);
    TempDemand.clear();
    TempDemand.push_back(5);
    TempDemand.push_back(0.666666666666667);
    TempDemand.push_back(0.333333333333333);
    DD.push_back(TempDemand);
    TempDemand.clear();
    TempDemand.push_back(7.5);
    TempDemand.push_back(1);
    TempDemand.push_back(0.333333333333333);
    DD.push_back(TempDemand);
    TempDemand.clear();
    
    vector<vector<float>> KD;
    TempDemand.push_back(5);
    TempDemand.push_back(1);
    TempDemand.push_back(1);
    KD.push_back(TempDemand);
    
    
    NodeList.push_back(NodeClass(0, 0, 0, 0, 0, 0, true));
    for (int n = 1; n <= NodeAmount; n++) {
        //Randomnumber from 0 to 100
        float x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordX - 0)));
        float y = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (GVMaxCoordY - 0)));
        float Mean = 6;
        float Stdev = 1;
        
        //Demand
        std::default_random_engine generator;
        std::normal_distribution<double> distribution(Mean,Stdev);
        double number = distribution(generator);
        
        float demand = number;
        if (n <= R) {
            demand = 5;
        }
        float probability = 1;
        if (n > R) {
            probability = 0.5;
        };
        bool Needservice = true;
        
        NodeList.push_back(NodeClass(x, y, demand, probability, Mean, Stdev, Needservice));
        if (n <= R) {
            NodeList[n].DiscreteDemand = KD;
        }
        if (n > R) {
            NodeList[n].DiscreteDemand = DD;
        };
        NodeList[n].StartTW = 0;
        NodeList[n].EndTW = 480;
        NodeList[n].ServiceTime = 15;
    };
    
    //calculate necessary vehicles
    
    FacilityList.push_back(FacilityClass(50, 50));
    
    // Facility Colors
    FacilityList[0].red = 204;
    FacilityList[0].Color = "black";
    FacilityList[0].Number = 0;
    FacilityList[0].Capacity = VehicleCapacity;

    // Set nodes on facilities
    for (int n = 1; n <= NodeAmount; n++) {
        FacilityList[0].ServiceNodes.push_back(n);
    };
    
    DistanceMatrix = GenerateDM(NodeList, FacilityList);
};

FacilityClass c_VRPSCDinstance::CreateStageOneSolution(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    for (int i = 0; i < Facility.RouteList.size(); i++ ) {
        bool done = false;
        int j = 0;
        while (done == false) {
            if(NodeList[Facility.RouteList[i][j]].Probability < 1) {
                Facility.RouteList[i].erase(Facility.RouteList[i].begin() + j);
                j = -1;
            }
            j += 1;
            if (j >= Facility.RouteList[i].size()) {
                done = true;
            }
            
        }
    };
    
    return Facility;
};

FacilityClass c_VRPSCDinstance::Repair(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    FacilityClass SolFacility;
    FacilityClass PrevFacility;
    
    //first remove non-present requests;
    for (int i = 0; i < Facility.RouteList.size(); i++ ) {
        for (int j = 0; j < Facility.RouteList[i].size(); j++) {
            if(NodeList[Facility.RouteList[i][j]].Service == false) {
                Facility.RouteList[i].erase(Facility.RouteList[i].begin() + j);
            }
        }
    };
    
    //Cheapest insertion
    
    for (int n = 0; n < NodeList.size(); n++) {
        if (NodeList[n].Service == true) {
            bool Found = false;
            for (int i = 0; i < Facility.RouteList.size(); i++ ) {
                for (int j = 0; j < Facility.RouteList[i].size(); j++) {
                    if (Facility.RouteList[i][j] == n) {
                        Found = true;
                    }
                }
            };
            
            for (int i = 0; i < FacilityList[0].RouteList.size(); i++ ) {
                if (FacilityList[0].RouteList[i].size() == 0) {
                    FacilityList[0].RouteList.erase(FacilityList[0].RouteList.begin() + i);
                }
            };

            
            
            if (Found == false) {
                float FacilityCost = 999999999999;
                PrevFacility = Facility;
                for (int i = 0; i < Facility.RouteList.size(); i++ ) {
                    for (int j = 0; j < Facility.RouteList[i].size(); j++) {
                        Facility = PrevFacility;
                        Facility.RouteList[i].insert(Facility.RouteList[i].begin() + j, n);
                        if (CalculateFacilityCosts(Facility, NodeList) < FacilityCost && CalculateRouteDemand(NodeList, Facility.RouteList[i]) < VehicleCapacity) {
                            FacilityCost = CalculateFacilityCosts(Facility, NodeList);
                            SolFacility = Facility;
                        }
                    }
                }
            }
        }
    }
    
    SolFacility.ServiceNodes.clear();
    for (int i = 0; i < Facility.RouteList.size(); i++ ) {
        for (int j = 0; j < Facility.RouteList[i].size(); j++) {
            SolFacility.ServiceNodes.push_back(SolFacility.RouteList[i][j]);
        }
    }
    
    return SolFacility;
};



//FacilityClass c_VRPSCDinstance::FirstStage(std::vector< NodeClass > NodeList, FacilityClass Facility) {
//    FacilityClass ResultFacility = Facility;
//    
//    
//    //Generate initial solution
//    ResultFacility = CreateStageOneSolution(NodeList, ResultFacility);
//    
//    FacilityClass CopyFacility = Facility;
//    FacilityClass BetweenSolutionFacility = Facility;
//    
//    float CurrentCost = CalculateTotalCosts(FacilityList, NodeList);
//    float EstimateCost = 0;
//    
//    //Select Node for removal
//    for (int i = 0; i < CopyFacility.RouteList.size(); i++) {
//        for (int j = 0; j < CopyFacility.RouteList[i].size(); j++) {
//            
//            // remove nodes from being serviced.
//            for (int w = 0; w < CopyFacility.ServiceNodes.size(); w++) {
//                if (CopyFacility.ServiceNodes[w] == CopyFacility.RouteList[i][j]) {
//                    CopyFacility.ServiceNodes.erase(CopyFacility.ServiceNodes.begin() + w);
//                }
//            };
//            
//            CopyFacility.RouteList[i].erase(CopyFacility.RouteList[i].begin() + j);
//            
//            EstimateCost = EstimateSecondStageCost(NodeList, Facility);
//            
//            if(EstimateCost < CurrentCost) {
//                BetweenSolutionFacility = CopyFacility;
//                CurrentCost = EstimateCost;
//            }
//            
//            CopyFacility = Facility;
//        }
//    };
//
//    //Insert Node
//    return ResultFacility;
//}

float c_VRPSCDinstance::Runinstances(int amount) {
    float result=0;
    //*******  TWO STAGE METHOD ********
    FacilityClass StageTwoFacility = StageTwoClarkeWright(NodeList, BenchmarkFacility);
    float tscost = CalculateFacilityCosts(StageTwoFacility, NodeList);
    
    //*******  OWN METHOD  ********
    FacilityClass StageTwoFacilityCW = StageTwoClarkeWright(NodeList, BenchmarkFacility);
    float stcwcost = CalculateFacilityCosts(StageTwoFacilityCW, NodeList);
    
    //*******  BENCHMARK (REOPTIMIZE)  ********
    SolutionMethod = 1; //ordinary randomized version
    BenchmarkFacility = ClarkeWright(NodeList,BenchmarkFacility);
    float reopt = CalculateFacilityCosts(BenchmarkFacility, NodeList);
    
    cout << "Skip: " << tscost << " - Own method: " << stcwcost << "  -  Reoptimize: " << reopt << endl;

    return result;
};


c_RealSolution::c_RealSolution(int id, int NPF, int vc) {
	srand(id);
	NodesPerFacility = NPF;
	VehicleCapacity = vc;
	floatbigM = 999999999;
	intbigM = 999999999;
	SolutionMethod = 4;
	GVIterations = 10000;
	TimeWindows = false;
	Writefile = false;
	margin = 1;
	ValuationType = 1;

	FacilityList.clear();

	// Facility locations
	FacilityList.push_back(FacilityClass(52.709138, 6.191286));
	FacilityList.push_back(FacilityClass(52.045975, 4.529032));
	FacilityList.push_back(FacilityClass(51.511100, 5.376871));

	// Facility Colors
	FacilityList[0].red = 204;
	FacilityList[0].Color = "red";
	FacilityList[0].Number = 0;
	FacilityList[0].pcc6 = "7942HZ";
	FacilityList[1].green = 204;
	FacilityList[1].Color = "green";
	FacilityList[1].Number = 1;
	FacilityList[1].pcc6 = "2665NT";
	FacilityList[2].blue = 204;
	FacilityList[2].Color = "blue";
	FacilityList[2].Number = 2;
	FacilityList[2].pcc6 = "5684NV";

	int it = 0;
	int it2 = 0;

	string pcc6;
	double xc = 0;
	double yc = 0;
	int demand;

	NodeList.push_back(NodeClass(0, 0, 0, 1, 0, 0, true));

	string line;
	ifstream myfile("Nodes.txt");
	while (it2 < 3000)
	{
		it += 1;

		if (it == 1) {
			getline(myfile, line, ' ');
			pcc6 = line;

		}
		if (it == 2) {
			getline(myfile, line, ' ');
			xc = stod(line);

		}
		if (it == 3) {
			getline(myfile, line, ' ');
			yc = stod(line);
		}
		if (it == 4) {
			it = 0;
			it2 += 1;
			getline(myfile, line, '\n');
			demand = stod(line);
			NodeList.push_back(NodeClass(0, 0, 0, 1, 0, 0, true));
			NodeList[NodeList.size() - 1].LocationX = xc;
			NodeList[NodeList.size() - 1].LocationY = yc;
			NodeList[NodeList.size() - 1].pcc6 = pcc6;
			NodeList[NodeList.size() - 1].Demand = demand;
		}
	}
	myfile.close();

	CostTracker.resize(101);
	for (int i = 0; i < CostTracker.size(); i++) {
		CostTracker[i] = 0;
	};

	// Set nodes on facilities
	for (int n = 1; n <= NPF; n++) {
		FacilityList[0].ServiceNodes.push_back(n);
	};

	for (int n = NPF + 1; n <= 2 * NPF; n++) {
		FacilityList[1].ServiceNodes.push_back(n);
	};

	for (int n = 2 * NPF + 1; n <= 3 * NPF; n++) {
		FacilityList[2].ServiceNodes.push_back(n);
	};

	DistanceMatrix = GenerateDM(NodeList, FacilityList);
};



bool Paper1Runsolution() {
      int ilow = 1;
      int iterations = 10;

      for (int n = ilow; n <= iterations; n++)
      {
          /*int margup = 20;
           int marglow = 1;*/
          //Settings
          bool Netwerk = true;
          bool Registratie = true;
          bool NetwGraph = true;
          bool Information = true;
          
          bool Optimal = false;
          bool SingleItemNoTABU = true;
          bool SingleItemTABU = true;
          bool RouteAuction = true;
          bool RecAuction = true;
          bool ClustAuction = true;
          
          bool RealLife = false;
          bool GurSol = false;
          bool RCWSol = true;
          
          bool TimeWindows = false;
          
          ofstream myfile;
          
          stringstream ss;
          ss << "result" << n << ".txt";
          string Title = ss.str();
          myfile.open(Title);
          
          ofstream myfilep;
          stringstream sp;
          sp << "picture" << n << ".txt";
          string Titlep = sp.str();
          myfilep.open(Titlep);
          
          ofstream seen;
          stringstream se;
          se << "seen" << n << ".txt";
          string TSeen = se.str();
          seen.open(TSeen);
          
          ofstream info;
          stringstream nf;
          nf << "info" << n << ".txt";
          string Tinfo = nf.str();
          info.open(Tinfo);
          
          
          cout << "n: " << n << endl;
          
          if (GurSol == true) {
              
              c_Solution Sol = c_Solution(n, 16, 4, TimeWindows);
              
              Sol.GVstep = 0;
              
              if (NetwGraph == true) { Sol.Writefile = true; };
              
              Sol.SolutionMethod = 3;
              for (int i = 0; i < Sol.FacilityList.size(); i++) {
                  Sol.FacilityList[i] = Sol.ClarkeWright(Sol.NodeList, Sol.FacilityList[i]);
              };
              Sol.CostTracker[0] = Sol.CalculateTotalCosts(Sol.FacilityList, Sol.NodeList);
              
              myfile << Sol.CalculateTotalCosts(Sol.FacilityList, Sol.NodeList) << endl;
              if (NetwGraph == true) { Sol.WriteGraphToFile(Sol.FacilityList, Sol.NodeList, myfilep, "rcw"); };
              
              //Sol.SolutionMethod = 3;
              //for (int i = 0; i < Sol.FacilityList.size(); i++) {
              //    Sol.FacilityList[i] = Sol.ClarkeWright(Sol.NodeList, Sol.FacilityList[i]);
              //};
              //if (NetwGraph == true) { Sol.WriteGraphToFile(Sol.FacilityList, Sol.NodeList, myfilep, "gur"); };
              
              //myfile << Sol.CalculateTotalCosts(Sol.FacilityList, Sol.NodeList) << endl;
              
              //Sol.SolutionMethod = 4;
              //for (int i = 0; i < Sol.FacilityList.size(); i++) {
              //    Sol.FacilityList[i] = Sol.ClarkeWright(Sol.NodeList, Sol.FacilityList[i]);
              //};
              
              myfile << Sol.CalculateTotalCosts(Sol.FacilityList, Sol.NodeList) << endl;
              if (NetwGraph == true) { Sol.WriteGraphToFile(Sol.FacilityList, Sol.NodeList, myfilep, "mcw"); };
              
              if (NetwGraph == true) { Sol.WriteGraphToFile(Sol.FacilityList, Sol.NodeList, myfile, "ordinary"); };
              
              if (Netwerk == true) { myfile << Sol.CalculateTotalCosts(Sol.FacilityList, Sol.NodeList) << endl; };
              
              //cout << Sol.CalculateTotalCosts(Sol.FacilityList, Sol.NodeList) << endl;
              
              Sol.GVOriginalCosts = Sol.CalculateTotalCosts(Sol.FacilityList, Sol.NodeList);
              
              //cout << "nrml" << endl;
              
              //if (NetwGraph == true) { Sol.BeginWriteFile(myfile); };
              
              
              
              if (Optimal == true) {
                  Sol.GVstep = 0;
                  Sol.Seen.clear();
                  std::vector< FacilityClass >  OptFacilityList = Sol.OptimalSolution(Sol.FacilityList, Sol.NodeList);
                  if (NetwGraph == true) { Sol.WriteGraphToFile(OptFacilityList, Sol.NodeList, myfilep, "optimal"); };
                  if (Registratie == true) { seen << Sol.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << Sol.CalculateTotalCosts(OptFacilityList, Sol.NodeList) << endl; };
                  if (Information == true) { Sol.WriteInfo(Sol.CostTracker, info); };
                  cout << "opt" << endl;
                  Sol.GVOptimalCosts = Sol.CalculateTotalCosts(OptFacilityList, Sol.NodeList);
              };
              
              if (SingleItemNoTABU == true) {
                  Sol.GVstep = 0;
                  Sol.Seen.clear();
                  std::vector< FacilityClass >  OSAFacilityList = Sol.OriginalSingleAuction(Sol.FacilityList, Sol.NodeList, (int)Sol.FacilityList.size(), myfilep);
                  if (NetwGraph == true) { Sol.WriteGraphToFile(OSAFacilityList, Sol.NodeList, myfilep, "osa"); };
                  if (Registratie == true) { seen << Sol.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << Sol.CalculateTotalCosts(OSAFacilityList, Sol.NodeList) << endl; };
                  if (Information == true) { Sol.WriteInfo(Sol.CostTracker, info); };
                  cout << "osa" << endl;
              };
              
              if (SingleItemTABU == true) {
                  Sol.GVstep = 0;
                  Sol.Seen.clear();
                  std::vector< FacilityClass >  BLFacilityList = Sol.BlackListSingleAuction(Sol.FacilityList, Sol.NodeList, Sol.FacilityList.size(), myfile);
                  if (NetwGraph == true) { Sol.WriteGraphToFile(BLFacilityList, Sol.NodeList, myfilep, "blacklist"); };
                  if (Registratie == true) { seen << Sol.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << Sol.CalculateTotalCosts(BLFacilityList, Sol.NodeList) << endl; };
                  if (Information == true) { Sol.WriteInfo(Sol.CostTracker, info); };
                  cout << "bl" << endl;
              };
              
              if (RouteAuction == true) {
                  Sol.GVstep = 0;
                  Sol.Seen.clear();
                  std::vector< FacilityClass >  RBLFacilityList = Sol.RouteAuction(Sol.FacilityList, Sol.NodeList, Sol.FacilityList.size(), myfile);
                  RBLFacilityList = Sol.BlackListSingleAuction(RBLFacilityList, Sol.NodeList, Sol.FacilityList.size(), myfile);
                  if (NetwGraph == true) { Sol.WriteGraphToFile(RBLFacilityList, Sol.NodeList, myfilep, "rbl"); };
                  if (Registratie == true) { seen << Sol.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << Sol.CalculateTotalCosts(RBLFacilityList, Sol.NodeList) << endl; };
                  if (Information == true) { Sol.WriteInfo(Sol.CostTracker, info); };
                  cout << "rbl" << endl;
              };
              
              if (RecAuction == true) {
                  Sol.GVstep = 0;
                  Sol.Seen.clear();
                  std::vector< FacilityClass >  RecFacilityList = Sol.RecursiveAuction(Sol.FacilityList, Sol.NodeList, myfile);
                  if (NetwGraph == true) { Sol.WriteGraphToFile(RecFacilityList, Sol.NodeList, myfilep, "combinatorial"); };
                  if (Registratie == true) { seen << Sol.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << Sol.CalculateTotalCosts(RecFacilityList, Sol.NodeList) << endl; };
                  if (Information == true) { Sol.WriteInfo(Sol.CostTracker, info); };
                  cout << "rec" << endl;
              };
          };
          
          
          
          if (RCWSol == true) {
              
              c_Solution SolRCW = c_Solution(n, 16, 4, TimeWindows);
              
              /* for circle */
              //SolRCW.FacilityList = OriginalFacilityList;
              
              //SolRCW.FacilityList[0].LocationX = 50;
              //SolRCW.FacilityList[0].LocationY = 50  + n;
              //SolRCW.FacilityList[1].LocationX = 50 + (2 * 0.4472135955 * n);
              //SolRCW.FacilityList[1].LocationY = 50 - (0.4472135955 * n);
              //SolRCW.FacilityList[2].LocationX = 50 - (2 * 0.4472135955 * n);
              //SolRCW.FacilityList[2].LocationY = 50 - (0.4472135955 * n);
              //SolRCW.DistanceMatrix = SolRCW.GenerateDM(SolRCW.NodeList, SolRCW.FacilityList);
              
              SolRCW.SolutionMethod = 3;
              
              for (int i = 0; i < SolRCW.FacilityList.size(); i++) {
                  SolRCW.FacilityList[i] = SolRCW.ClarkeWright(SolRCW.NodeList, SolRCW.FacilityList[i]);
              };
              SolRCW.CostTracker[0] = SolRCW.CalculateTotalCosts(SolRCW.FacilityList, SolRCW.NodeList);
              
              //cout << SolRCW.CalculateTotalCosts(SolRCW.FacilityList, SolRCW.NodeList) << endl;
              //myfile << SolRCW.CalculateTotalCosts(SolRCW.FacilityList, SolRCW.NodeList) << endl;
              
              
              //SolRCW.SolutionMethod = 3;
              
              //for (int i = 0; i < SolRCW.FacilityList.size(); i++) {
              //    SolRCW.FacilityList[i] = SolRCW.ClarkeWright(SolRCW.NodeList, SolRCW.FacilityList[i]);
              //};
              //myfile << SolRCW.CalculateTotalCosts(SolRCW.FacilityList, SolRCW.NodeList) << endl;
              
              //cout << SolRCW.CalculateTotalCosts(SolRCW.FacilityList, SolRCW.NodeList) << endl;
              if (Netwerk == true) { myfile << SolRCW.CalculateTotalCosts(SolRCW.FacilityList, SolRCW.NodeList) << endl; };
              if (NetwGraph == true) { SolRCW.WriteGraphToFile(SolRCW.FacilityList, SolRCW.NodeList, myfilep, "org"); };
              
              if (Optimal == true) {
                  SolRCW.GVstep = 0;
                  SolRCW.Seen.clear();
                  std::vector< FacilityClass >  OptFacilityList = SolRCW.OptimalSolution(SolRCW.FacilityList, SolRCW.NodeList);
                  if (NetwGraph == true) { SolRCW.WriteGraphToFile(OptFacilityList, SolRCW.NodeList, myfilep, "optimal"); };
                  if (Registratie == true) { seen << SolRCW.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << SolRCW.CalculateTotalCosts(OptFacilityList, SolRCW.NodeList) << endl; };
                  //if (Information == true) { SolRCW.WriteInfo(SolRCW.CostTracker, info); };
                  cout << "opt" << endl;
                  //cout << SolRCW.CalculateTotalCosts(OptFacilityList, SolRCW.NodeList);
                  SolRCW.GVOptimalCosts = SolRCW.CalculateTotalCosts(OptFacilityList, SolRCW.NodeList);
              };
              
              if (SingleItemNoTABU == true) {
                  SolRCW.GVstep = 0;
                  SolRCW.Seen.clear();
                  std::vector< FacilityClass >  OSAFacilityList = SolRCW.OriginalSingleAuction(SolRCW.FacilityList, SolRCW.NodeList, SolRCW.FacilityList.size(), myfile);
                  if (NetwGraph == true) { SolRCW.WriteGraphToFile(OSAFacilityList, SolRCW.NodeList, myfilep, "osa"); };
                  if (Registratie == true) { seen << SolRCW.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << SolRCW.CalculateTotalCosts(OSAFacilityList, SolRCW.NodeList) << endl; };
                  if (Information == true) { SolRCW.WriteInfo(SolRCW.CostTracker, info); };
                  //cout << "osa" << endl;
              };
              
              if (SingleItemTABU == true) {
                  SolRCW.GVstep = 0;
                  SolRCW.Seen.clear();
                  std::vector< FacilityClass >  BLFacilityList = SolRCW.BlackListSingleAuction(SolRCW.FacilityList, SolRCW.NodeList, SolRCW.FacilityList.size(), myfile);
                  cout << SolRCW.CalculateTotalCosts(BLFacilityList, SolRCW.NodeList) << endl;
                  if (NetwGraph == true) { SolRCW.WriteGraphToFile(BLFacilityList, SolRCW.NodeList, myfilep, "blacklist"); };
                  if (Registratie == true) { seen << SolRCW.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << SolRCW.CalculateTotalCosts(BLFacilityList, SolRCW.NodeList) << endl; };
                  if (Information == true) { SolRCW.WriteInfo(SolRCW.CostTracker, info); };
                  //cout << "bl" << endl;
              };
              
              
              if (RouteAuction == true) {
                  SolRCW.GVstep = 0;
                  SolRCW.Seen.clear();
                  std::vector< FacilityClass >  RBLFacilityList = SolRCW.RouteAuction(SolRCW.FacilityList, SolRCW.NodeList, SolRCW.FacilityList.size(), myfile);
                  cout << SolRCW.CalculateTotalCosts(RBLFacilityList, SolRCW.NodeList) << endl;
                  RBLFacilityList = SolRCW.BlackListSingleAuction(RBLFacilityList, SolRCW.NodeList, SolRCW.FacilityList.size(), myfile);
                  cout << SolRCW.CalculateTotalCosts(RBLFacilityList, SolRCW.NodeList) << endl;
                  if (NetwGraph == true) { SolRCW.WriteGraphToFile(RBLFacilityList, SolRCW.NodeList, myfilep, "rbl"); };
                  if (Registratie == true) { seen << SolRCW.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << SolRCW.CalculateTotalCosts(RBLFacilityList, SolRCW.NodeList) << endl; };
                  if (Information == true) { SolRCW.WriteInfo(SolRCW.CostTracker, info); };
                  //cout << "rbl" << endl;
              };
              
              if (RecAuction == true) {
                  SolRCW.GVstep = 0;
                  SolRCW.Seen.clear();
                  std::vector< FacilityClass >  RecFacilityList = SolRCW.RecursiveAuction(SolRCW.FacilityList, SolRCW.NodeList, myfile);
                  if (NetwGraph == true) { SolRCW.WriteGraphToFile(RecFacilityList, SolRCW.NodeList, myfilep, "combinatorial"); };
                  if (Registratie == true) { seen << SolRCW.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << SolRCW.CalculateTotalCosts(RecFacilityList, SolRCW.NodeList) << endl; };
                  if (Information == true) { SolRCW.WriteInfo(SolRCW.CostTracker, info); };
                  //cout << "rec" << endl;
              };
              
              if (ClustAuction == true) {
                  SolRCW.GVstep = 0;
                  SolRCW.Seen.clear();
                  std::vector< FacilityClass >  CAFacilityList = SolRCW.RouteAuction(SolRCW.FacilityList, SolRCW.NodeList, SolRCW.FacilityList.size(), myfile);
                  CAFacilityList = SolRCW.ClusterAuction(CAFacilityList, SolRCW.NodeList);
                  cout << SolRCW.CalculateTotalCosts(CAFacilityList, SolRCW.NodeList);
                  CAFacilityList = SolRCW.BlackListSingleAuction(CAFacilityList, SolRCW.NodeList, SolRCW.FacilityList.size(), myfile);
                  cout << SolRCW.CalculateTotalCosts(CAFacilityList, SolRCW.NodeList);
                  if (NetwGraph == true) { SolRCW.WriteGraphToFile(CAFacilityList, SolRCW.NodeList, myfilep, "combinatorial"); };
                  if (Registratie == true) { seen << SolRCW.Seen.size() << endl; };
                  if (Netwerk == true) { myfile << SolRCW.CalculateTotalCosts(CAFacilityList, SolRCW.NodeList) << endl; };
                  if (Information == true) { SolRCW.WriteInfo(SolRCW.CostTracker, info); };
                  //cout << "ca" << endl;
              };
              //}
          }
          
          if (RealLife == true) {
              c_RealSolution SolReal = c_RealSolution(n, 1000, 30);
              for (int i = 0; i < SolReal.FacilityList.size(); i++) {
                  SolReal.FacilityList[i] = SolReal.ClarkeWright(SolReal.NodeList, SolReal.FacilityList[i]);
              };
              SolReal.WriteCoords(SolReal.FacilityList, SolReal.NodeList, "original");
              SolReal.CostTracker[0] = SolReal.CalculateTotalCosts(SolReal.FacilityList, SolReal.NodeList);
              std::vector< FacilityClass >  OptFacilityList = SolReal.OptimalSolution(SolReal.FacilityList, SolReal.NodeList);
              SolReal.WriteCoords(OptFacilityList, SolReal.NodeList, "optimal");
              if (Netwerk == true) { myfile << SolReal.CalculateTotalCosts(SolReal.FacilityList, SolReal.NodeList) << endl; };
              SolReal.GVstep = 0;
              SolReal.Seen.clear();
              std::vector< FacilityClass >  CAFacilityList = SolReal.RouteAuction(SolReal.FacilityList, SolReal.NodeList, SolReal.FacilityList.size(), myfile);
              if (Information == true) { SolReal.WriteInfo(SolReal.CostTracker, info); };
              SolReal.Seen.clear();
              CAFacilityList = SolReal.ClusterAuction(CAFacilityList, SolReal.NodeList);
              if (Information == true) { SolReal.WriteInfo(SolReal.CostTracker, info); };
              SolReal.Seen.clear();
              CAFacilityList = SolReal.BlackListSingleAuction(CAFacilityList, SolReal.NodeList, SolReal.FacilityList.size(), myfile);
              if (NetwGraph == true) { SolReal.WriteGraphToFile(CAFacilityList, SolReal.NodeList, myfilep, "combinatorial"); };
              if (Registratie == true) { seen << SolReal.Seen.size() << endl; };
              if (Netwerk == true) { myfile << SolReal.CalculateTotalCosts(CAFacilityList, SolReal.NodeList) << endl; };
              if (Information == true) { SolReal.WriteInfo(SolReal.CostTracker, info); };
              SolReal.WriteCoords(SolReal.FacilityList, SolReal.NodeList, "auction");
          }
          
          myfile.close();
          seen.close();
          myfilep.close();
          info.close();
          
          if (Netwerk == false) {
              remove(Title.c_str());
          };
          if (Registratie == false) {
              remove(TSeen.c_str());
          };
          if (NetwGraph == false) {
              remove(Titlep.c_str());
          };
          if (Information == false) {
              remove(Tinfo.c_str());
          };
      };
    
    return true;
}
                  
