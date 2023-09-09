//# include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"

c_ClarkeWright::c_ClarkeWright() {

};

//bool c_ClarkeWright::SortMarginalCosts(std::array<float, 3> i, std::array<float, 3> j) { return i[1] > j[1]; }

//bool c_ClarkeWright::SortSavings(std::array<float, 3> i, std::array<float, 3> j) { return i[2] > j[2]; }

float c_ClarkeWright::Cross(std::array<float, 3> O, std::array<float, 3> A, std::array<float, 3> B) {
	return (float)(A[0] - O[0]) * (B[1] - O[1]) - (float)(A[1] - O[1]) * (B[0] - O[0]);
};

vector<int> c_ClarkeWright::DetermineTour(vector< NodeClass > NodeList, vector<int> RouteList, float LocationX, float LocationY) {
	int n = (int)RouteList.size() + 1, k = 0;
	std::vector<std::array<float, 3>> RL3;
	std::vector<std::array<float, 3>> CRL3(2 * n);
	std::array<float, 3> RN;
	std::vector<int> RLC;

	RN[0] = LocationX;
	RN[1] = LocationY;
	RN[2] = 0;
	RL3.push_back(RN);

	for (int i = 0; i < n - 1; i++) {
		RN[0] = NodeList[RouteList[i]].LocationX;
		RN[1] = NodeList[RouteList[i]].LocationY;
		RN[2] = RouteList[i];
		RL3.push_back(RN);
	};

	// Sort points lexicographically
	std::sort(RL3.begin(), RL3.end());

	// Build lower hull
	for (int i = 0; i < n; ++i) {
		while (k >= 2 && Cross(CRL3[k - 2], CRL3[k - 1], RL3[i]) <= 0) k--;
		CRL3[k++] = RL3[i];
	};

	// Build upper hull
	for (int i = n - 2, t = k + 1; i >= 0; i--) {
		while (k >= t && Cross(CRL3[k - 2], CRL3[k - 1], RL3[i]) <= 0) k--;
		CRL3[k++] = RL3[i];
	};

	CRL3.resize(k - 1);

	bool included;
	float ND;
    int Counter = 0, Acounter = 0;
	float MinimumDistance;
	int MemoryIt = 0;
	int sol = 0;
	std::array<float, 2> A, B, InsertPoint;

	while (CRL3.size() < RL3.size()) {
		ND = 100000000;
		//find node closest to solution
		for (int w = 0; w < RL3.size(); w++) {
			included = false;
			for (int i = 0; i < CRL3.size(); i++) {
				if (CRL3[i][2] == RL3[w][2]) {
					included = true;
				}
			};
			if (included == false) {
				for (int i = 0; i < CRL3.size(); i++) {
					if (NodeDistance(CRL3[i][0], CRL3[i][1], RL3[w][0], RL3[w][1]) < ND) {
						ND = NodeDistance(CRL3[i][0], CRL3[i][1], RL3[w][0], RL3[w][1]);
						sol = w;
					}
				}
			}
		};

		MinimumDistance = 100000000000;
		for (Counter = 0; Counter < CRL3.size(); Counter++) {

			B[0] = CRL3[Counter][0];
			B[1] = CRL3[Counter][1];
			if (Counter - 1 < 0) { Acounter = (int)CRL3.size() - 1; }
			else { Acounter = Counter - 1; };
			A[0] = CRL3[Acounter][0];
			A[1] = CRL3[Acounter][1];

			InsertPoint[0] = RL3[sol][0];
			InsertPoint[1] = RL3[sol][1];

			float CalculatedDistance = NodeDistance(A[0], A[1], InsertPoint[0], InsertPoint[1]) + NodeDistance(InsertPoint[0], InsertPoint[1], B[0], B[1]) - NodeDistance(A[0], A[1], B[0], B[1]);

			if (CalculatedDistance < MinimumDistance) {
				MinimumDistance = CalculatedDistance;
				MemoryIt = Acounter;
			};

		};
		CRL3.insert(CRL3.begin() + MemoryIt + 1, RL3[sol]);
	};

	bool pb = false;
	for (int j = 1; j < CRL3.size(); j++) {
		if (CRL3[j - 1][2] == 0) { pb = true; };
		if (pb == true) { RLC.push_back(CRL3[j][2]); };
	};
	pb = true;
	for (int j = 0; j < CRL3.size() - 1; j++) {
		if (CRL3[0][2] != 0) {
			if (RLC.size() > 0) {
				if (CRL3[j][2] != RLC[RLC.size() - 1] && pb == true) {
					RLC.push_back(CRL3[j][2]);
				}
			}
			else
			{
				RLC.push_back(CRL3[j][2]);
			};

			if (pb == true && CRL3[j + 1][2] == 0) { pb = false; };
		}
	};

	return RLC;
};

FacilityClass c_ClarkeWright::ClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility) {
	switch (SolutionMethod) {
		case 1: return RandomizedClarkeWright(NodeList, Facility);
		case 2: return TimeWindowsRandomizedClarkeWright(NodeList, Facility);
		case 3: return GurobiSol(NodeList, Facility);
		case 4: return ModifiedClarkeWright(NodeList, Facility);
        case 5: return WorstCaseClarkeWright(NodeList, Facility);
        case 6: return StageOneClarkeWright(NodeList, Facility);
        case 7: return GurobiSolAlt(NodeList, Facility);
        //case 8: return GurobiSolSecondStage(NodeList, Facility);
        case 9: return StageTwoAlgorithm(NodeList, Facility);
        default: return RandomizedClarkeWright(NodeList, Facility);
	}
}

FacilityClass c_ClarkeWright::ModifiedClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility) {
	std::array<float, 3> NodeSavings;
	std::vector<std::array<float, 3>> SavingsList;
	std::vector<std::array<float, 3>> RandomSavingsList;
	std::vector<std::array<float, 3>> CopySavingsList;
	FacilityClass SolutionFacility = Facility;
	int NodeAmount = (int)Facility.ServiceNodes.size();
	Facility.RouteList.clear();
	float CurCosts = 99999999999999999;

	//time_t tijd1 = time(0);

	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		int NodeTemp = Facility.ServiceNodes[i];
		Facility.RouteList.push_back({ NodeTemp });
	};

	//First determine the savings

	for (int n = 0; n < NodeAmount; n++) {
		for (int i = n + 1; i < NodeAmount; i++) {

			int nn = Facility.ServiceNodes[n];
			int nnn = Facility.ServiceNodes[i];
			int f = Facility.Number;

			//float Savings = NodeDistance(Facility.LocationX, Facility.LocationX, NodeList[nn].LocationX, NodeList[nn].LocationY) + NodeDistance(Facility.LocationX, Facility.LocationX, NodeList[nnn].LocationX, NodeList[nnn].LocationY) - NodeDistance(NodeList[nn].LocationX, NodeList[nn].LocationY, NodeList[nnn].LocationX, NodeList[nnn].LocationY);
			float Savings = NodeDistance(f, true, nn, false, (int)NodeList.size()) + NodeDistance(f, true, nnn, false, (int)NodeList.size()) - NodeDistance(nn, false, nnn, false, (int)NodeList.size());

			NodeSavings[0] = { (float)nn };
			NodeSavings[1] = { (float)nnn };
			NodeSavings[2] = { Savings };

			SavingsList.push_back(NodeSavings);
		}
	};

	Facility.RouteList.clear();

	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		int NodeTemp = Facility.ServiceNodes[i];
		Facility.RouteList.push_back({ NodeTemp });
	};

	RandomSavingsList.clear();


	struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[2] > j[2]; } } SortSavings;

	std::sort(SavingsList.begin(), SavingsList.end(), SortSavings);

	RandomSavingsList = SavingsList;

	//Now we start the Algorithm
	int RouteNr1 = -1;
	int RouteNr2 = -1;
	float RouteDemand = 0;
	bool Done = false;
	std::vector<int> TempRoute;
	for (int n = 0; n < RandomSavingsList.size(); n++) {
		RouteNr1 = -1;
		RouteNr2 = -1;
		// check if already in route
		for (int i = 0; i < Facility.RouteList.size(); i++) {
			for (int j = 0; j < Facility.RouteList[i].size(); j++) {
				if (Facility.RouteList[i][j] == (int)RandomSavingsList[n][0]) {
					RouteNr1 = i;
				};
				if (Facility.RouteList[i][j] == (int)RandomSavingsList[n][1]) {
					RouteNr2 = i;
				}
			}
		};
		// Merge Routes
		Done = false;
		if (RouteNr1 == RouteNr2) {
			Done = true;
		};
		if (Done == false) {
			TempRoute = Facility.RouteList[RouteNr1];
			TempRoute.insert(TempRoute.begin(), Facility.RouteList[RouteNr2].begin(), Facility.RouteList[RouteNr2].end());
			RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
			if (RouteDemand <= Facility.Capacity) {
				Facility.RouteList[RouteNr1] = TempRoute;
				Facility.RouteList.erase(Facility.RouteList.begin() + RouteNr2);
			}
		};
	};
	for (int i = 0; i < Facility.RouteList.size(); i++) {
		Facility.RouteList[i] = DetermineTour(NodeList, Facility.RouteList[i], Facility.LocationX, Facility.LocationY);
	};

	if (CalculateFacilityCosts(Facility, NodeList) < CurCosts) {
		SolutionFacility = Facility;
		CurCosts = CalculateFacilityCosts(Facility, NodeList);
	};

	//time_t tijd2 = time(0);
	//GVCount = GVCount + 1;

	//cout << GVCount << endl;
	//cout << difftime(tijd2, tijd1) << " seconden" << endl;

	return SolutionFacility;
};

FacilityClass c_ClarkeWright::RandomizedClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility) {
	std::array<float, 3> NodeSavings;
	std::vector<std::array<float, 3>> SavingsList;
	std::vector<std::array<float, 3>> RandomSavingsList;
	std::vector<std::array<float, 3>> CopySavingsList;
	FacilityClass SolutionFacility = Facility;
	int NodeAmount = (int)Facility.ServiceNodes.size();
	Facility.RouteList.clear();
	int CSet = 1;
	float Kans;
	float TotaalKans;
	float LKans, HKans;
	float CurCosts = floatbigM;
	bool Done;

	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		int NodeTemp = Facility.ServiceNodes[i];
		Facility.RouteList.push_back({ NodeTemp });
	};

	//First determine the savings

	for (int n = 0; n < NodeAmount; n++) {
		for (int i = n + 1; i < NodeAmount; i++) {

			int nn = Facility.ServiceNodes[n];
			int nnn = Facility.ServiceNodes[i];
			int f = Facility.Number;
			float Savings = NodeDistance(f, true, nn, false, (int)NodeList.size()) + NodeDistance(f, true, nnn, false, (int)NodeList.size()) - NodeDistance(nn, false, nnn, false, (int)NodeList.size());

			NodeSavings[0] = { (float)nn };
			NodeSavings[1] = { (float)nnn };
			NodeSavings[2] = { Savings };

			SavingsList.push_back(NodeSavings);
		}
	};

	struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[2] > j[2]; } } SortSavings;

	std::sort(SavingsList.begin(), SavingsList.end(), SortSavings);

	for (int iter = 0; iter < GVIterations; iter++) {

		Facility.RouteList.clear();

		for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
			int NodeTemp = Facility.ServiceNodes[i];
			Facility.RouteList.push_back({ NodeTemp });
		};

		RandomSavingsList.clear();

		CopySavingsList = SavingsList;

		while (CopySavingsList.size() != 0) {
			//MAAK HIER DE RANDOMWIEL!
			CSet = 1 + static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / (7 - 1)));

			if (CSet > CopySavingsList.size()) {
				CSet = (int)CopySavingsList.size();
			};

			TotaalKans = 0;

			for (int p = 0; p < CSet; p++) {
				TotaalKans = TotaalKans + CopySavingsList[p][2];
			};

			Kans = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
			LKans = 0;
			Done = false;

			for (int p = 0; p < CSet; p++) {
				if (Done == false) {
					HKans = LKans + (CopySavingsList[p][2] / TotaalKans);
					if (p == CSet - 1) { HKans = 1.1; };
					if (Kans >= LKans && Kans < HKans) {
						RandomSavingsList.push_back(CopySavingsList[p]);
						CopySavingsList.erase(CopySavingsList.begin() + p);
						Done = true;
					};
					LKans = HKans;
				}
			};
		};

		if(GVIterations == 1) {
			RandomSavingsList = SavingsList;
		};

		//Now we start the Algorithm
		int end = 0;
		std::vector<int> TempRoute;
        std::vector<int> t_route;
		float RouteDemand = 0;
		std::vector<int> DoneList;

		for (int n = 0; n < RandomSavingsList.size(); n++) {
			int optie1 = -1;
			int optie2 = -1;
			int optie3 = -1;
			int optie4 = -1;
			Done = false;
			for (int i = 0; i < Facility.RouteList.size(); i++)
			{
				end = (int)Facility.RouteList[i].size() - 1;
				if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][0]) {
					optie1 = i;
				};
				if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][end]) {
					optie2 = i;
				};
				if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][0]) {
					optie3 = i;
				};
				if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][end]) {
					optie4 = i;
				}

			};
			if ((optie1 == optie3 && optie1 != -1) || (optie1 == optie4 && optie1 != -1) || (optie2 == optie3 && optie2 != -1) || (optie2 == optie4 && optie2 != -1)) {
				Done = true;
			};

			//Now compose new route.

			if ((optie1 > -1) && (optie4 > -1) && (Done == false)) {
				TempRoute = Facility.RouteList[optie4];
				TempRoute.insert(TempRoute.end(), Facility.RouteList[optie1].begin(), Facility.RouteList[optie1].end());
				RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
				if (RouteDemand <= Facility.Capacity) {
                    t_route = DetermineTour(NodeList, TempRoute, Facility.LocationX, Facility.LocationY);
                    if(AltRouteLength(NodeList, Facility, t_route) < AltRouteLength(NodeList, Facility, TempRoute) ) {
                        TempRoute = t_route;
                    }
					Facility.RouteList[optie1] = TempRoute;
					Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
				}
				Done = true;
			};
			if ((optie2 > -1) && (optie3 > -1) && (Done == false)) {
				TempRoute = Facility.RouteList[optie2];
				TempRoute.insert(TempRoute.end(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
				RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
				if (RouteDemand <= Facility.Capacity) {
                    t_route = DetermineTour(NodeList, TempRoute, Facility.LocationX, Facility.LocationY);
                    if(AltRouteLength(NodeList, Facility, t_route) < AltRouteLength(NodeList, Facility, TempRoute) ) {
                        TempRoute = t_route;
                    }
					Facility.RouteList[optie2] = TempRoute;
					Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
				}
				Done = true;
			};
			if ((optie2 > -1) && (optie4 > -1) && (Done == false)) {
				std::reverse(Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
				TempRoute = Facility.RouteList[optie2];
				TempRoute.insert(TempRoute.end(), Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
				RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
				if (RouteDemand <= Facility.Capacity) {
                    t_route = DetermineTour(NodeList, TempRoute, Facility.LocationX, Facility.LocationY);
                    if(AltRouteLength(NodeList, Facility, t_route) < AltRouteLength(NodeList, Facility, TempRoute) ) {
                        TempRoute = t_route;
                    }
					Facility.RouteList[optie2] = TempRoute;
					Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
				}
				Done = true;
			};
			if ((optie1 > -1) && (optie3 > -1) && (Done == false)) {
				std::reverse(Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
				TempRoute = Facility.RouteList[optie1];
				TempRoute.insert(TempRoute.begin(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
				RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
				if (RouteDemand <= Facility.Capacity) {
                    t_route = DetermineTour(NodeList, TempRoute, Facility.LocationX, Facility.LocationY);
                    if(AltRouteLength(NodeList, Facility, t_route) < AltRouteLength(NodeList, Facility, TempRoute) ) {
                        TempRoute = t_route;
                    }
					Facility.RouteList[optie1] = TempRoute;
					Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
				}
				Done = true;
			}
		};

		if (CalculateFacilityCosts(Facility, NodeList) < CurCosts) {
			SolutionFacility = Facility;
			CurCosts = CalculateFacilityCosts(Facility, NodeList);
		};
	};

	return SolutionFacility;
};



FacilityClass c_ClarkeWright::WorstCaseClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    std::array<float, 3> NodeSavings;
    std::vector<std::array<float, 3>> SavingsList;
    std::vector<std::array<float, 3>> RandomSavingsList;
    std::vector<std::array<float, 3>> CopySavingsList;
    FacilityClass SolutionFacility = Facility;
    int NodeAmount = (int)Facility.ServiceNodes.size();
    Facility.RouteList.clear();
    int CSet = 1;
    float Kans;
    float TotaalKans;
    float LKans, HKans;
    float CurCosts = floatbigM;
    bool Done;
    
    //GVstep = GVstep + 1;
    //cout << GVstep << " ";
    
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        int NodeTemp = Facility.ServiceNodes[i];
        Facility.RouteList.push_back({ NodeTemp });
    };
    
    //First determine the savings
    
    for (int n = 0; n < NodeAmount; n++) {
        for (int i = n + 1; i < NodeAmount; i++) {
            
            int nn = Facility.ServiceNodes[n];
            int nnn = Facility.ServiceNodes[i];
            int f = Facility.Number;
            float Savings = NodeDistance(f, true, nn, false, (int)NodeList.size()) + NodeDistance(f, true, nnn, false, (int)NodeList.size()) - NodeDistance(nn, false, nnn, false, (int)NodeList.size());
            
            NodeSavings[0] = { (float)nn };
            NodeSavings[1] = { (float)nnn };
            NodeSavings[2] = { Savings };
            
            SavingsList.push_back(NodeSavings);
        }
    };
    
    struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[2] > j[2]; } } SortSavings;
    
    std::sort(SavingsList.begin(), SavingsList.end(), SortSavings);
    
    for (int iter = 0; iter < GVIterations; iter++) {
        
        Facility.RouteList.clear();
        
        for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
            int NodeTemp = Facility.ServiceNodes[i];
            Facility.RouteList.push_back({ NodeTemp });
        };
        
        RandomSavingsList.clear();
        
        CopySavingsList = SavingsList;
        
        while (CopySavingsList.size() != 0) {
            //MAAK HIER DE RANDOMWIEL!
            CSet = 1 + static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / (6 - 1)));
            
            if (CSet > CopySavingsList.size()) {
                CSet = (int)CopySavingsList.size();
            };
            
            TotaalKans = 0;
            
            for (int p = 0; p < CSet; p++) {
                TotaalKans = TotaalKans + CopySavingsList[p][2];
            };
            
            Kans = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
            LKans = 0;
            Done = false;
            
            for (int p = 0; p < CSet; p++) {
                if (Done == false) {
                    HKans = LKans + (CopySavingsList[p][2] / TotaalKans);
                    if (p == CSet - 1) { HKans = 1.1; };
                    if (Kans >= LKans && Kans < HKans) {
                        RandomSavingsList.push_back(CopySavingsList[p]);
                        CopySavingsList.erase(CopySavingsList.begin() + p);
                        Done = true;
                    };
                    LKans = HKans;
                }
            };
        };
        
        if(GVIterations == 1) {
            RandomSavingsList = SavingsList;
        };
        
        //Now we start the Algorithm
        int end = 0;
        std::vector<int> TempRoute;
        float RouteDemand = 0;
        std::vector<int> DoneList;
        
        for (int n = 0; n < RandomSavingsList.size(); n++) {
            int optie1 = -1;
            int optie2 = -1;
            int optie3 = -1;
            int optie4 = -1;
            Done = false;
            for (int i = 0; i < Facility.RouteList.size(); i++)
            {
                end = (int)Facility.RouteList[i].size() - 1;
                if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][0]) {
                    optie1 = i;
                };
                if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][end]) {
                    optie2 = i;
                };
                if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][0]) {
                    optie3 = i;
                };
                if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][end]) {
                    optie4 = i;
                }
                
            };
            if ((optie1 == optie3 && optie1 != -1) || (optie1 == optie4 && optie1 != -1) || (optie2 == optie3 && optie2 != -1) || (optie2 == optie4 && optie2 != -1)) {
                Done = true;
            };
            
            //Now compose new route.
            
            if ((optie1 > -1) && (optie4 > -1) && (Done == false)) {
                TempRoute = Facility.RouteList[optie4];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie1].begin(), Facility.RouteList[optie1].end());
                RouteDemand = WorstCaseCalculateRouteDemand(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie1] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
                }
                Done = true;
            };
            if ((optie2 > -1) && (optie3 > -1) && (Done == false)) {
                TempRoute = Facility.RouteList[optie2];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                RouteDemand = WorstCaseCalculateRouteDemand(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie2] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
                }
                Done = true;
            };
            if ((optie2 > -1) && (optie4 > -1) && (Done == false)) {
                std::reverse(Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
                TempRoute = Facility.RouteList[optie2];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
                RouteDemand = WorstCaseCalculateRouteDemand(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie2] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
                }
                Done = true;
            };
            if ((optie1 > -1) && (optie3 > -1) && (Done == false)) {
                std::reverse(Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                TempRoute = Facility.RouteList[optie1];
                TempRoute.insert(TempRoute.begin(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                RouteDemand = WorstCaseCalculateRouteDemand(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie1] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
                }
                Done = true;
            }
        };
        
        if (CalculateFacilityCosts(Facility, NodeList) < CurCosts) {
            SolutionFacility = Facility;
            CurCosts = CalculateFacilityCosts(Facility, NodeList);
        };
    };
    
    return SolutionFacility;
};


FacilityClass c_ClarkeWright::StageOneClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    std::array<float, 3> NodeSavings;
    std::vector<std::array<float, 3>> SavingsList;
    std::vector<std::array<float, 3>> RandomSavingsList;
    std::vector<std::array<float, 3>> CopySavingsList;
    FacilityClass SolutionFacility = Facility;
    FacilityClass PrevFacility = Facility;
    int NodeAmount = (int)Facility.ServiceNodes.size();
    Facility.RouteList.clear();
    int CSet = 1;
    float Kans;
    float TotaalKans;
    float LKans, HKans;
    float CurCosts = floatbigM;
    bool Done;
    
    //Calculate Amount of Vehicles
//    for (int i = 1; i < NodeList.size(); i++) {
//        int which = 2;
//        double p = confidence;
//        double q = 1-p;
//        double X;
//        double mean = NodeList[i].Mean;
//        double stdev = NodeList[i].Stdev;
//        int status;
//        double bound;
//
//        cdfnor(&which,&p,&q,&X,&mean,&stdev,&status,&bound);
//
//        sumdemands = sumdemands + (mean * NodeList[i].Probability);
//    }
//
//    int VehicleAmount = ceil(sumdemands/VehicleCapacity);
    
    //First determine the savings
    
    for (int n = 0; n < NodeAmount; n++) {
        for (int i = n + 1; i < NodeAmount; i++) {
            
            int nn = Facility.ServiceNodes[n];
            int nnn = Facility.ServiceNodes[i];
            int f = Facility.Number;
            
            //NodeList[nn].Probability * NodeList[nnn].Probability * (
            
            float Savings =  (NodeDistance(f, true, nn, false, (int)NodeList.size()) + NodeDistance(f, true, nnn, false, (int)NodeList.size()) - NodeDistance(nn, false, nnn, false, (int)NodeList.size()) );
            
            NodeSavings[0] = { (float)nn };
            NodeSavings[1] = { (float)nnn };
            NodeSavings[2] = { Savings };
            
            SavingsList.push_back(NodeSavings);
        }
    };
    
    struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[2] > j[2]; } } SortSavings;
    std::sort(SavingsList.begin(), SavingsList.end(), SortSavings);
    
    for (int iter = 0; iter < GVIterations; iter++) {
        
        Facility.RouteList.clear();
        
        for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
            int NodeTemp = Facility.ServiceNodes[i];
            Facility.RouteList.push_back({ NodeTemp });
        };
        
        RandomSavingsList.clear();
        
        CopySavingsList = SavingsList;
        
        while (CopySavingsList.size() != 0) {
            //MAAK HIER DE RANDOMWIEL!
            CSet = 1 + static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / (6 - 1)));
            
            if (CSet > CopySavingsList.size()) {
                CSet = (int)CopySavingsList.size();
            };
            
            TotaalKans = 0;
            
            for (int p = 0; p < CSet; p++) {
                TotaalKans = TotaalKans + CopySavingsList[p][2];
            };
            
            Kans = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
            LKans = 0;
            Done = false;
            
            for (int p = 0; p < CSet; p++) {
                if (Done == false) {
                    HKans = LKans + (CopySavingsList[p][2] / TotaalKans);
                    if (p == CSet - 1) { HKans = 1.1; };
                    if (Kans >= LKans && Kans < HKans) {
                        RandomSavingsList.push_back(CopySavingsList[p]);
                        CopySavingsList.erase(CopySavingsList.begin() + p);
                        Done = true;
                    };
                    LKans = HKans;
                }
            };
        };

        //Now we start the Algorithm
        int end = 0;
        std::vector<int> TempRoute;
        float RouteDemand = 0;
        std::vector<int> DoneList;
        
        for (int n = 0; n < RandomSavingsList.size(); n++) {
                
            int optie1 = -1;
            int optie2 = -1;
            int optie3 = -1;
            int optie4 = -1;
            Done = false;
            for (int i = 0; i < Facility.RouteList.size(); i++)
            {
                end = (int) Facility.RouteList[i].size() - 1;
                if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][0]) {
                    optie1 = i;
                };
                if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][end]) {
                    optie2 = i;
                };
                if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][0]) {
                    optie3 = i;
                };
                if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][end]) {
                    optie4 = i;
                }
                
            };
            if ((optie1 == optie3 && optie1 != -1) || (optie1 == optie4 && optie1 != -1) || (optie2 == optie3 && optie2 != -1) || (optie2 == optie4 && optie2 != -1))
            {
                Done = true;
            };
            
            //Now compose new route.
            
            if ((optie1 > -1) && (optie4 > -1) && (Done == false)) {
                TempRoute = Facility.RouteList[optie4];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie1].begin(), Facility.RouteList[optie1].end());
                RouteDemand = CalculateRouteDemandwP(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie1] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
                }
                Done = true;
            };
            if ((optie2 > -1) && (optie3 > -1) && (Done == false)) {
                TempRoute = Facility.RouteList[optie2];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                RouteDemand = CalculateRouteDemandwP(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie2] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
                }
                Done = true;
            };
            if ((optie2 > -1) && (optie4 > -1) && (Done == false)) {
                std::reverse(Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
                TempRoute = Facility.RouteList[optie2];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
                RouteDemand = CalculateRouteDemandwP(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie2] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
                }
                Done = true;
            };
            if ((optie1 > -1) && (optie3 > -1) && (Done == false)) {
                std::reverse(Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                TempRoute = Facility.RouteList[optie1];
                TempRoute.insert(TempRoute.begin(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                RouteDemand = CalculateRouteDemandwP(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie1] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
                }
                Done = true;
            }
        };
        
        if (CalculateFacilityCosts(Facility, NodeList) < CurCosts) {
            SolutionFacility = Facility;
            CurCosts = CalculateFacilityCosts(Facility, NodeList);
        };
    };
    
    return SolutionFacility;
};

bool c_ClarkeWright::SatisfiesConstraints(std::vector< NodeClass > NodeList, FacilityClass ConstrainedFacility, FacilityClass Facility) {
    bool result = true;
    
    // PositionList remembers positions
    std::vector< std::vector<int>> PositionList;
    std::vector< bool > Found;
    Found.resize(ConstrainedFacility.RouteList.size());
    for( int i = 0; i < Found.size(); i++) {
        Found[i] = false;
    }
    PositionList.resize(ConstrainedFacility.RouteList.size());
    for (int i = 0; i < ConstrainedFacility.RouteList.size(); i++) {
        PositionList[i].resize(ConstrainedFacility.RouteList[i].size());
    };
    
    //When positionlist is 0, no position is known
    for (int i=0; i < PositionList.size(); i++) {
        for (int j = 0; j < PositionList[i].size(); j++) {
            PositionList[i][j] = -1;
        }
    }
    
    //find position and write
    for (int i = 0; i < ConstrainedFacility.RouteList.size(); i++) {

        for (int j = 0; j < Facility.RouteList.size(); j++) {
            for (int t = 0; t < PositionList[i].size(); t++) {
                PositionList[i][t] = -1;
            }
            //Find Positions
            for (int w = 0; w < ConstrainedFacility.RouteList[i].size(); w++) {
                for (int k = 0; k < Facility.RouteList[j].size(); k++) {
                    if (ConstrainedFacility.RouteList[i][w] == Facility.RouteList[j][k]) {
                        PositionList[i][w] = k;
                    }
                }
            };
            
            int count = 0;
            
            for (int w = 1; w < PositionList[i].size(); w++) {
                if (PositionList[i][w-1] < PositionList[i][w] && PositionList[i][w-1] >= 0 && PositionList[i][w] >= 0) {
                    count = count + 1;
                }
            }
            
            int test = (int)PositionList[i].size() - 1;
            
            if (count >= test) {
                Found[i] = true;
            }
            
            //other direction
            count = 0;
            
            for (int w = 1; w < PositionList[i].size(); w++) {
                if (PositionList[i][w-1] > PositionList[i][w] && PositionList[i][w-1] >= 0 && PositionList[i][w] >= 0) {
                    count = count + 1;
                }
            }
            
            test = (int)PositionList[i].size() - 1;
            
            if (count >= test) {
                Found[i] = true;
            }
        }
    }
    
    result = true;
    
    for (int i = 0; i < Found.size(); i++) {
        if( Found[i] == false ) {
            result = false;
        }
    }
    
    return result;
};

FacilityClass c_ClarkeWright::StageTwoAlgorithm(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    GVstep = GVstep + 1;
    cout << GVstep << endl;
    FacilityClass ConstrainedFacility = Facility;
    vector<int> RemainingNodes = Facility.ServiceNodes;
    
    for (int i = 0; i < ConstrainedFacility.RouteList.size(); i++) {
        for (int j = 0; j < ConstrainedFacility.RouteList[i].size(); j++) {
            for (int w = 0; w < RemainingNodes.size(); w++) {
                if (ConstrainedFacility.RouteList[i][j] == RemainingNodes[w]) {
                    RemainingNodes.erase(RemainingNodes.begin() + w);
                }
            }
        }
    }
    
    //bestaande restricties opvullen
    
    bool done = false;
    
    while (done == false) {
        int insertroute = -1;
        int insertpos = -1;
        int insertnode = -1;
        
        int w = 0;
        
        float CurrentCost = 99999999999;
        
        for (int w = 0; w < RemainingNodes.size(); w++) {
            for (int i = 0; i < Facility.RouteList.size(); i++) {
                if (Facility.RouteList[i].size() > 0) {
                    for (int j = 0; j < Facility.RouteList[i].size(); j++) {
                        FacilityClass CopyFacility = Facility;
                        CopyFacility.RouteList[i].insert(CopyFacility.RouteList[i].begin() + j, RemainingNodes[w]);
                        if (CalculateRouteLength(NodeList, CopyFacility, i) < CurrentCost && CalculateRouteDemand(NodeList, CopyFacility.RouteList[i]) < VehicleCapacity) {
                            CurrentCost = CalculateRouteLength(NodeList, CopyFacility, i);
                            insertroute = i;
                            insertpos = j;
                            insertnode = RemainingNodes[w];
                        }
                    }
                }
            }
        }
        
        if (insertpos > - 1 && insertroute > -1 && insertnode > -1) {
            Facility.RouteList[insertroute].insert(Facility.RouteList[insertroute].begin() + insertpos, RemainingNodes[w]);
            RemainingNodes.erase(RemainingNodes.begin() + w);
        } else {
            done = true;
        }
    }
    
//    for (int i = 0; i < Facility.RouteList.size(); i++) {
//        if (Facility.RouteList[i].size() > 0) {
//            Facility.RouteList[i] = DetermineTour(NodeList, Facility.RouteList[i], Facility.LocationX, Facility.LocationY);
//        }
//    }
    
    FacilityClass BaseFacility = Facility;
    BaseFacility.ServiceNodes = RemainingNodes;
    BaseFacility.RouteList.clear();
    BaseFacility = RandomizedClarkeWright(NodeList, BaseFacility);
    for (int i = 0; i < BaseFacility.RouteList.size(); i++) {
        Facility.RouteList.push_back(BaseFacility.RouteList[i]);
    }
    return Facility;
}

FacilityClass c_ClarkeWright::StageTwoClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    FacilityClass ConstrainedFacility = Facility;

//    Facility.RouteList.clear();
//    Facility.RouteList.resize(ConstrainedFacility.RouteList.size());
//    Facility.RouteList[0].push_back(1);
//    Facility.RouteList[0].push_back(7);
//    Facility.RouteList[0].push_back(2);
//    Facility.RouteList[0].push_back(3);
//    Facility.RouteList[0].push_back(8);
//    Facility.RouteList[1].push_back(4);
//    Facility.RouteList[1].push_back(9);
//    Facility.RouteList[1].push_back(10);
//    Facility.RouteList[1].push_back(5);
//    Facility.RouteList[1].push_back(6);
//
//    bool constrained = SatisfiesConstraints(NodeList,ConstrainedFacility, Facility);
    
    
    
    std::array<float, 3> NodeSavings;
    std::vector<std::array<float, 3>> SavingsList;
    std::vector<std::array<float, 3>> RandomSavingsList;
    std::vector<std::array<float, 3>> CopySavingsList;
    FacilityClass SolutionFacility = Facility;
    //FacilityClass ConstrainedFacility = Facility;
    int NodeAmount = (int)Facility.ServiceNodes.size();
    Facility.RouteList.clear();
    int CSet = 1;
    float Kans;
    float TotaalKans;
    float LKans, HKans;
    float CurCosts = floatbigM;
    bool Done;
    
    //First determine the savings
    
    for (int n = 0; n < NodeAmount; n++) {
        for (int i = n + 1; i < NodeAmount; i++) {
            
            int nn = Facility.ServiceNodes[n];
            int nnn = Facility.ServiceNodes[i];
            int f = Facility.Number;
            
            //NodeList[nn].Probability * NodeList[nnn].Probability * (
            
            float Savings = (NodeDistance(f, true, nn, false, (int)NodeList.size()) + NodeDistance(f, true, nnn, false, (int)NodeList.size()) - NodeDistance(nn, false, nnn, false, (int)NodeList.size()) );
            
            NodeSavings[0] = { (float)nn };
            NodeSavings[1] = { (float)nnn };
            NodeSavings[2] = { Savings };
            
            SavingsList.push_back(NodeSavings);
        }
    };
    
    struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[2] > j[2]; } } SortSavings;
    std::sort(SavingsList.begin(), SavingsList.end(), SortSavings);
    
    for (int iter = 0; iter < 10000; iter++) {
        
        Facility.RouteList.clear();
        
        for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
            int NodeTemp = Facility.ServiceNodes[i];
            Facility.RouteList.push_back({ NodeTemp });
        };
        
        RandomSavingsList.clear();
        
        CopySavingsList = SavingsList;
        
        while (CopySavingsList.size() != 0) {
            //MAAK HIER DE RANDOMWIEL!
            CSet = 1 + static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / (SavingsList.size() - 1)));
            
            if (CSet > CopySavingsList.size()) {
                CSet = (int)CopySavingsList.size();
            };
            
            TotaalKans = 0;
            
            for (int p = 0; p < CSet; p++) {
                TotaalKans = TotaalKans + CopySavingsList[p][2];
            };
            
            Kans = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
            LKans = 0;
            Done = false;
            
            for (int p = 0; p < CSet; p++) {
                if (Done == false) {
                    HKans = LKans + (CopySavingsList[p][2] / TotaalKans);
                    if (p == CSet - 1) { HKans = 1.1; };
                    if (Kans >= LKans && Kans < HKans) {
                        RandomSavingsList.push_back(CopySavingsList[p]);
                        CopySavingsList.erase(CopySavingsList.begin() + p);
                        Done = true;
                    };
                    LKans = HKans;
                }
            };
        };
        
        //Now we start the Algorithm
        int end = 0;
        std::vector<int> TempRoute;
        float RouteDemand = 0;
        std::vector<int> DoneList;
        
        for (int n = 0; n < RandomSavingsList.size(); n++) {
            
            int optie1 = -1;
            int optie2 = -1;
            int optie3 = -1;
            int optie4 = -1;
            Done = false;
            for (int i = 0; i < Facility.RouteList.size(); i++)
            {
                end = (int) Facility.RouteList[i].size() - 1;
                if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][0]) {
                    optie1 = i;
                };
                if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][end]) {
                    optie2 = i;
                };
                if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][0]) {
                    optie3 = i;
                };
                if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][end]) {
                    optie4 = i;
                }
                
            };
            if ((optie1 == optie3 && optie1 != -1) || (optie1 == optie4 && optie1 != -1) || (optie2 == optie3 && optie2 != -1) || (optie2 == optie4 && optie2 != -1))
            {
                Done = true;
            };
            
            //Now compose new route.
            
            if ((optie1 > -1) && (optie4 > -1) && (Done == false)) {
                TempRoute = Facility.RouteList[optie4];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie1].begin(), Facility.RouteList[optie1].end());
                RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie1] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
                }
                Done = true;
            };
            if ((optie2 > -1) && (optie3 > -1) && (Done == false)) {
                TempRoute = Facility.RouteList[optie2];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie2] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
                }
                Done = true;
            };
            if ((optie2 > -1) && (optie4 > -1) && (Done == false)) {
                std::reverse(Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
                TempRoute = Facility.RouteList[optie2];
                TempRoute.insert(TempRoute.end(), Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
                RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie2] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
                }
                Done = true;
            };
            if ((optie1 > -1) && (optie3 > -1) && (Done == false)) {
                std::reverse(Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                TempRoute = Facility.RouteList[optie1];
                TempRoute.insert(TempRoute.begin(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
                RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
                if (RouteDemand <= Facility.Capacity) {
                    Facility.RouteList[optie1] = TempRoute;
                    Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
                }
                Done = true;
            }
        };
        
        if (CalculateFacilityCosts(Facility, NodeList) < CurCosts && SatisfiesConstraints(NodeList,ConstrainedFacility, Facility)) {
            SolutionFacility = Facility;
            CurCosts = CalculateFacilityCosts(Facility, NodeList);
        };
    };
    
    return SolutionFacility;
};




FacilityClass c_ClarkeWright::TimeWindowsRandomizedClarkeWright(std::vector< NodeClass > NodeList, FacilityClass Facility) {
	std::array<float, 3> NodeSavings;
	std::vector<std::array<float, 3>> SavingsList;
	std::vector<std::array<float, 3>> RandomSavingsList;
	std::vector<std::array<float, 3>> CopySavingsList;
	FacilityClass SolutionFacility = Facility;
	int NodeAmount = (int)Facility.ServiceNodes.size();
	Facility.RouteList.clear();
	int CSet = 1;
	float Kans;
	float TotaalKans;
	float LKans, HKans;
	float CurCosts = floatbigM;
	bool Done;

	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		int NodeTemp = Facility.ServiceNodes[i];
		Facility.RouteList.push_back({ NodeTemp });
	};

	//First determine the savings

	for (int n = 0; n < NodeAmount; n++) {
		for (int i = n + 1; i < NodeAmount; i++) {

			int nn = Facility.ServiceNodes[n];
			int nnn = Facility.ServiceNodes[i];
			int f = Facility.Number;

			//float Savings = NodeDistance(Facility.LocationX, Facility.LocationX, NodeList[nn].LocationX, NodeList[nn].LocationY) + NodeDistance(Facility.LocationX, Facility.LocationX, NodeList[nnn].LocationX, NodeList[nnn].LocationY) - NodeDistance(NodeList[nn].LocationX, NodeList[nn].LocationY, NodeList[nnn].LocationX, NodeList[nnn].LocationY);
			float Savings = NodeDistance(f, true, nn, false, (int)NodeList.size()) + NodeDistance(f, true, nnn, false, (int)NodeList.size()) - NodeDistance(nn, false, nnn, false, (int)NodeList.size());

			NodeSavings[0] = { (float)nn };
			NodeSavings[1] = { (float)nnn };
			NodeSavings[2] = { Savings };

			SavingsList.push_back(NodeSavings);
		}
	};

	struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[2] > j[2]; } } SortSavings;

	std::sort(SavingsList.begin(), SavingsList.end(), SortSavings);

	for (int iter = 0; iter < GVIterations; iter++) {

		Facility.RouteList.clear();

		for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
			int NodeTemp = Facility.ServiceNodes[i];
			Facility.RouteList.push_back({ NodeTemp });
		};

		RandomSavingsList.clear();

		CopySavingsList = SavingsList;

		while (CopySavingsList.size() != 0) {
			//MAAK HIER DE RANDOMWIEL!
			CSet = 1 + static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / (6 - 1)));

			if (CSet > CopySavingsList.size()) {
				CSet = (int)CopySavingsList.size();
			};

			TotaalKans = 0;

			for (int p = 0; p < CSet; p++) {
				TotaalKans = TotaalKans + CopySavingsList[p][2];
			};

			Kans = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));
			LKans = 0;
			Done = false;

			for (int p = 0; p < CSet; p++) {
				if (Done == false) {
					HKans = LKans + (CopySavingsList[p][2] / TotaalKans);
					if (p == CSet - 1) { HKans = 1.1; };
					if (Kans >= LKans && Kans < HKans) {
						RandomSavingsList.push_back(CopySavingsList[p]);
						CopySavingsList.erase(CopySavingsList.begin() + p);
						Done = true;
					};
					LKans = HKans;
				}
			};
		};

		//Now we start the Algorithm
		int end = 0;
		std::vector<int> TempRoute;
		float RouteDemand = 0;
		std::vector<int> DoneList;

		for (int n = 0; n < RandomSavingsList.size(); n++) {
			int optie1 = -1;
			int optie2 = -1;
			int optie3 = -1;
			int optie4 = -1;
			Done = false;
			for (int i = 0; i < Facility.RouteList.size(); i++)
			{
				end = (int)Facility.RouteList[i].size() - 1;
				if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][0]) {
					optie1 = i;
				};
				if ((int)RandomSavingsList[n][0] == Facility.RouteList[i][end]) {
					optie2 = i;
				};
				if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][0]) {
					optie3 = i;
				};
				if ((int)RandomSavingsList[n][1] == Facility.RouteList[i][end]) {
					optie4 = i;
				}

			};
			if ((optie1 == optie3 && optie1 != -1) || (optie1 == optie4 && optie1 != -1) || (optie2 == optie3 && optie2 != -1) || (optie2 == optie4 && optie2 != -1)) {
				Done = true;
			};

			//Now compose new route.

			if ((optie1 > -1) && (optie4 > -1) && (Done == false)) {
				TempRoute = Facility.RouteList[optie4];
				TempRoute.insert(TempRoute.end(), Facility.RouteList[optie1].begin(), Facility.RouteList[optie1].end());
				RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
				if (RouteDemand <= Facility.Capacity && TimeWindowsCheck(Facility,NodeList,TempRoute)) {
					Facility.RouteList[optie1] = TempRoute;
					Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
				}
				Done = true;
			};
			if ((optie2 > -1) && (optie3 > -1) && (Done == false)) {
				TempRoute = Facility.RouteList[optie2];
				TempRoute.insert(TempRoute.end(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
				RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
				if (RouteDemand <= Facility.Capacity && TimeWindowsCheck(Facility, NodeList, TempRoute)) {
					Facility.RouteList[optie2] = TempRoute;
					Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
				}
				Done = true;
			};
			if ((optie2 > -1) && (optie4 > -1) && (Done == false)) {
				std::reverse(Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
				TempRoute = Facility.RouteList[optie2];
				TempRoute.insert(TempRoute.end(), Facility.RouteList[optie4].begin(), Facility.RouteList[optie4].end());
				RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
				if (RouteDemand <= Facility.Capacity && TimeWindowsCheck(Facility, NodeList, TempRoute)) {
					Facility.RouteList[optie2] = TempRoute;
					Facility.RouteList.erase(Facility.RouteList.begin() + optie4);
				}
				Done = true;
			};
			if ((optie1 > -1) && (optie3 > -1) && (Done == false)) {
				std::reverse(Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
				TempRoute = Facility.RouteList[optie1];
				TempRoute.insert(TempRoute.begin(), Facility.RouteList[optie3].begin(), Facility.RouteList[optie3].end());
				RouteDemand = CalculateRouteDemand(NodeList, TempRoute);
				if (RouteDemand <= Facility.Capacity && TimeWindowsCheck(Facility, NodeList, TempRoute)) {
					Facility.RouteList[optie1] = TempRoute;
					Facility.RouteList.erase(Facility.RouteList.begin() + optie3);
				}
				Done = true;
			}
		};

		if (CalculateFacilityCosts(Facility, NodeList) < CurCosts) {
			SolutionFacility = Facility;
			CurCosts = CalculateFacilityCosts(Facility, NodeList);
		};
	};

	return SolutionFacility;
};

FacilityClass c_ClarkeWright::GurobiSolAlt(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    GVstep = GVstep + 1;
    cout << GVstep << endl;
    GRBEnv env = GRBEnv();

    Facility.RouteList.clear();

    
    GRBModel model = GRBModel(env);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 1);

    int NodeAmount = (int)Facility.ServiceNodes.size() + 1;
    vector<vector<double>> distance;
    vector<double> servicetimes;
    vector<double> startingtimes;
    vector<double> endingtimes;
    vector<vector<double>> traveltime;
    vector<double> demands;
    int i, j, k;
    
    servicetimes.resize(NodeAmount);
    servicetimes[0] = 0;
    for (i = 1; i < NodeAmount; i++) {
        servicetimes[i] = NodeList[Facility.ServiceNodes[i-1]].ServiceTime;
    };
    
    startingtimes.resize(NodeAmount);
    startingtimes[0] = 0;
    for (i = 1; i < NodeAmount; i++) {
        startingtimes[i] = NodeList[Facility.ServiceNodes[i-1]].StartTW;
    };
    
    endingtimes.resize(NodeAmount);
    endingtimes[0] = 900;
    for (i = 1; i < NodeAmount; i++) {
        endingtimes[i] = NodeList[Facility.ServiceNodes[i-1]].EndTW;
    };
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    traveltime.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        traveltime[i].resize(NodeAmount);
    };

    vector<vector<double>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.ServiceNodes[i - 1]].Demand);
        }
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
            traveltime[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    int VehicleAmount = 6;
    
    vector<vector<vector<double>>> x_res;

    vector<vector<vector<GRBVar>>> x;
    x.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        x[i].resize(NodeAmount);
    };
    for (i = 0; i < NodeAmount; i++) {
        for(j = 0; j < NodeAmount; j++) {
            x[i][j].resize(VehicleAmount);
        }
    };

    vector<vector<GRBVar >> b;
    b.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        b[i].resize(VehicleAmount);
    };
    

    // Create 3-D array of model variables

    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            for (k = 0; k < VehicleAmount; k++) {
                string name = "X_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k);
                x[i][j][k] = model.addVar(0.0, 1.0, distance[i][j], GRB_BINARY, name); //from i to j with vehicle k
            }
        }
    };

    for (j = 0; j < NodeAmount; j++) {
        for (k = 0; k < VehicleAmount; k++) {
            string name = "b_" + to_string(j) + "_" + to_string(k);
            b[j][k] = model.addVar(0.0,900,0.0,GRB_CONTINUOUS,name); //starting time at requests
        }
    };

    //every customer should be visited by one vehicle
    for (i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (int j = 0; j < NodeAmount; j++) {
            for (int k = 0; k < VehicleAmount; k++) {
                if (j != i) {
                    customervisit += x[i][j][k];
                }
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    };

    for (i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 0; j < NodeAmount; j++) {
            for (int k = 0; k < VehicleAmount; k++) {
                if (j != i) {
                    customervisit += x[j][i][k];
                }
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    };
    
    for (int k = 0; k < VehicleAmount; k++) {
        GRBLinExpr customervisit = 0;
        for (j = 0; j < NodeAmount; j++) {
            customervisit += x[0][j][k];
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    for (int k = 0; k < VehicleAmount; k++) {
        GRBLinExpr customervisit = 0;
        for (j = 0; j < NodeAmount; j++) {
            customervisit += x[j][0][k];
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    // flow constraint
    for (i = 0; i < NodeAmount; i++) {
        for (k = 0; k < VehicleAmount; k++) {
            GRBLinExpr NodeEnter = 0;
            for (j = 0; j < NodeAmount; j++) {
                NodeEnter += x[j][i][k];
            };
            GRBLinExpr NodeExit = 0;
            for (j = 0; j < NodeAmount; j++) {
                NodeExit += x[i][j][k];
            }
            model.addConstr(NodeEnter, GRB_EQUAL, NodeExit);
        };
    };

    for (k = 0; k < VehicleAmount; k++) {
        GRBLinExpr RouteDemand = 0;
        for (i = 0; i < NodeAmount; i++) {
            for (j = 1; j < NodeAmount; j++) {
                RouteDemand += demands[j] * x[i][j][k];
            }
        };
        model.addConstr(RouteDemand, GRB_LESS_EQUAL, Facility.Capacity);
    };

    //restrictions on starting times.
    for (i = 0; i < NodeAmount; i++) {
        for (j = 1; j < NodeAmount; j++) {
            for ( k = 0; k < VehicleAmount; k++) {
                if (i != j) {
                    GRBLinExpr CorrectTime = 0;
                    CorrectTime += b[i][k] + servicetimes[i] + traveltime[i][j] - floatbigM * (1 - x[i][j][k]);
                    model.addConstr(CorrectTime, GRB_LESS_EQUAL, b[j][k]);
                }
            }
        }
    };
    
    //Must satisfy time windows
    for (i = 1; i < NodeAmount; i++) {
        for ( k = 0; k < VehicleAmount; k++) {
            GRBLinExpr SatisfiesTW = 0;
            SatisfiesTW += b[i][k];
            model.addConstr(SatisfiesTW, GRB_LESS_EQUAL, endingtimes[i]);
        }
    };
    
    for (i = 1; i < NodeAmount; i++) {
        for ( k = 0; k < VehicleAmount; k++) {
            GRBLinExpr SatisfiesTW = 0;
            SatisfiesTW += b[i][k];
            model.addConstr(SatisfiesTW, GRB_GREATER_EQUAL, startingtimes[i]);
        }
    };
    

    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

    model.optimize();
    
    int NextNode = 0;
    int NN = 0;

    for (k = 0; k < VehicleAmount; k++) {
        Facility.RouteList.push_back(vector<int>());
        for (j = 1; j < NodeAmount; j++) {
            if (x[0][j][k].get(GRB_DoubleAttr_X) > 0) {
                NextNode = j;
                while(NextNode != 0) {
                    Facility.RouteList[Facility.RouteList.size() - 1].push_back(Facility.ServiceNodes[NextNode-1]);
                    for (int w = 0; w < NodeAmount; w++) {
                        if (x[NextNode][w][k].get(GRB_DoubleAttr_X) > 0) {
                            NN = w;
                        }
                    }
                    NextNode = NN;
                }
            }
        }
    };

	return Facility;
}

vector<vector<int>> c_ClarkeWright::GurobiSolSecondStage(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    FacilityClass ConstrainedFacility = Facility;
    

    GRBEnv env = GRBEnv();
    
    Facility.RouteList.clear();
    
    GRBModel model = GRBModel(env);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 1);
    
    float sumdemands = 0;
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
    }
    
    int p = ceil(sumdemands / Facility.Capacity);
    
    int NodeAmount = (int)Facility.ServiceNodes.size() + 1;
    vector<vector<double>> distance;
    vector<double> servicetimes;
    vector<double> startingtimes;
    vector<double> endingtimes;
    vector<vector<double>> traveltime;
    vector<double> demands;
    int i, j, k;
    
    servicetimes.resize(NodeAmount);
    servicetimes[0] = 0;
    for (i = 1; i < NodeAmount; i++) {
        servicetimes[i] = NodeList[Facility.ServiceNodes[i-1]].ServiceTime;
    };
    
    startingtimes.resize(NodeAmount);
    startingtimes[0] = 0;
    for (i = 1; i < NodeAmount; i++) {
        startingtimes[i] = NodeList[Facility.ServiceNodes[i-1]].StartTW;
    };
    
    endingtimes.resize(NodeAmount);
    endingtimes[0] = 900;
    for (i = 1; i < NodeAmount; i++) {
        endingtimes[i] = NodeList[Facility.ServiceNodes[i-1]].EndTW;
    };
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    traveltime.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        traveltime[i].resize(NodeAmount);
    };
    
    vector<vector<double>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.ServiceNodes[i - 1]].Demand);
        }
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
            traveltime[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    vector<vector<double>> savings;
    savings.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        savings[i].resize(NodeAmount);
    };
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            savings[i][j] = 0;
            if (i > 0 && j > 0) {
                savings[i][j] = distance[i][0] + distance[0][j] - distance[i][j];
            }
        }
    }

    
    vector<vector<vector<double>>> x_res;
    
    vector<vector<vector<GRBVar>>> x;
    x.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        x[i].resize(NodeAmount);
    };
    for (i = 0; i < NodeAmount; i++) {
        for(j = 0; j < NodeAmount; j++) {
            x[i][j].resize(p);
        }
    };
    
    vector<vector<GRBVar >> b;
    b.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        b[i].resize(p);
    };
    
    
    // Create 3-D array of model variables
    
    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            for (k = 0; k < p; k++) {
                string name = "X_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k);
                x[i][j][k] = model.addVar(0.0, 1.0, distance[i][j], GRB_BINARY, name); //from i to j with vehicle k
            }
        }
    };
    
    for (j = 0; j < NodeAmount; j++) {
        for (k = 0; k < p; k++) {
            string name = "b_" + to_string(j) + "_" + to_string(k);
            b[j][k] = model.addVar(0.0,900,0.0,GRB_CONTINUOUS,name); //starting time at requests
        }
    };
    
    //every customer should be visited by one vehicle
    for (i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 0; j < NodeAmount; j++) {
            for (k = 0; k < p; k++) {
                if (j != i) {
                    customervisit += x[i][j][k];
                }
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    };
    
    for (i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 0; j < NodeAmount; j++) {
            for (int k = 0; k < p; k++) {
                if (j != i) {
                    customervisit += x[j][i][k];
                }
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    };
    
    for (k = 0; k < p; k++) {
        GRBLinExpr customervisit = 0;
        for (j = 0; j < NodeAmount; j++) {
            customervisit += x[0][j][k];
        }
        model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
    }
    
    for (k = 0; k < p; k++) {
        GRBLinExpr customervisit = 0;
        for (j = 0; j < NodeAmount; j++) {
            customervisit += x[j][0][k];
        }
        model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
    }
    
    for ( i = 0; i < NodeAmount; i++) {
        for (k = 0; k < p; k++) {
            GRBLinExpr customervisit = 0;
            customervisit += x[i][i][k];
            model.addConstr(customervisit, GRB_LESS_EQUAL, 0.0);
        }
    }
    
    // flow constraint
    for (i = 0; i < NodeAmount; i++) {
        for (k = 0; k < p; k++) {
            GRBLinExpr NodeEnter = 0;
            for (j = 0; j < NodeAmount; j++) {
                NodeEnter += x[j][i][k];
            };
            GRBLinExpr NodeExit = 0;
            for (j = 0; j < NodeAmount; j++) {
                NodeExit += x[i][j][k];
            }
            model.addConstr(NodeEnter, GRB_EQUAL, NodeExit);
        };
    };
    
    for (k = 0; k < p; k++) {
        GRBLinExpr RouteDemand = 0;
        for (i = 0; i < NodeAmount; i++) {
            for (j = 1; j < NodeAmount; j++) {
                RouteDemand += demands[j] * x[i][j][k];
            }
        };
        model.addConstr(RouteDemand, GRB_LESS_EQUAL, Facility.Capacity);
    };
    
    //restrictions on starting times.
    for (i = 0; i < NodeAmount; i++) {
        for (j = 1; j < NodeAmount; j++) {
            for ( k = 0; k < p; k++) {
                if (i != j) {
                    GRBLinExpr CorrectTime = 0;
                    CorrectTime += b[i][k] + servicetimes[i] + traveltime[i][j] - floatbigM * (1 - x[i][j][k]);
                    model.addConstr(CorrectTime, GRB_LESS_EQUAL, b[j][k]);
                }
            }
        }
    };
    
    //Must satisfy time windows
    for (i = 1; i < NodeAmount; i++) {
        for ( k = 0; k < p; k++) {
            GRBLinExpr SatisfiesTW = 0;
            SatisfiesTW += b[i][k];
            model.addConstr(SatisfiesTW, GRB_LESS_EQUAL, endingtimes[i]);
        }
    };
    
    for (i = 1; i < NodeAmount; i++) {
        for ( k = 0; k < p; k++) {
            GRBLinExpr SatisfiesTW = 0;
            SatisfiesTW += b[i][k];
            model.addConstr(SatisfiesTW, GRB_GREATER_EQUAL, startingtimes[i]);
        }
    };
    
    //Additional Constraints
    for (k = 0; k < p; k++) {
        for (i = 0; i < ConstrainedFacility.RouteList.size(); i++) {
            if (ConstrainedFacility.RouteList[i].size() > 1) {
                for (int w = 0; w < ConstrainedFacility.RouteList[i].size() - 1; w++) {
                    for (k = 0; k < p; k++) {
                        int r1 = ConstrainedFacility.RouteList[i][w];
                        GRBLinExpr NodeEnter = 0;
                        for (j = 0; j < NodeAmount; j++) {
                            NodeEnter += x[j][r1][k];
                        };
                        int r2 = ConstrainedFacility.RouteList[i][w+1];
                        GRBLinExpr NodeEnterAlso = 0;
                        for (j = 0; j < NodeAmount; j++) {
                            NodeEnterAlso += x[j][r2][k];
                        }
                        model.addConstr(NodeEnter, GRB_EQUAL, NodeEnterAlso);
                        
                        GRBLinExpr firststart = 0;
                        firststart += b[r1][k];
                        GRBLinExpr nextstart = 0;
                        nextstart += b[r2][k];
                        model.addConstr(nextstart, GRB_GREATER_EQUAL, firststart);
                    }
                }
            }
        }
    };
    
    
    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
    
    model.optimize();
    

    vector<vector<int>> result;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        
        vector<vector<int>> RouteList;
        RouteList.clear();
        int NextNode = 0;
        int NN = 0;
        
        for (k = 0; k < p; k++) {
            RouteList.push_back(vector<int>());
            for (j = 1; j < NodeAmount; j++) {
                if (x[0][j][k].get(GRB_DoubleAttr_X) > 0) {
                    NextNode = j;
                    while(NextNode != 0) {
                        RouteList[RouteList.size() - 1].push_back(Facility.ServiceNodes[NextNode-1]);
                        for (int w = 0; w < NodeAmount; w++) {
                            if (x[NextNode][w][k].get(GRB_DoubleAttr_X) > 0) {
                                NN = w;
                            }
                        }
                        NextNode = NN;
                    }
                }
            }
        };
        
        //        for (int i= 0; i < Facility.SecondStageNodes.size(); i++){
        //            for (int j = 0; j < RouteList.size(); j++) {
        //                for (int k = 0; k < RouteList[j].size(); k++) {
        //                    if (RouteList[j][k] == Facility.SecondStageNodes[i]) {
        //                        RouteList[j].erase(RouteList[j].begin() + k);
        //                    }
        //                }
        //            }
        //        }
        
        bool dd = false;
        
        int q = 0;
        
        while (dd == false) {
            if (RouteList[q].size() == 0) {
                RouteList.erase(RouteList.begin() + q);
                q = 0;
            }
            q = q + 1;
            if( q >= RouteList.size() ) {
                dd = true;
            }
        }
        
        
        result = RouteList;
    } else {
        vector<vector<int>> RouteList;
        result = RouteList;
    }
    
        
    return result;
}

vector<vector<float>> c_ClarkeWright::GurobiAltForm(std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj) {
    GRBEnv env = GRBEnv();
    
    Facility.RouteList.clear();
    
    GRBModel model = GRBModel(env);
    //model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
    model.getEnv().set(GRB_IntParam_OutputFlag, 1);
    model.getEnv().set(GRB_IntParam_Threads, 8);
    
//    float sumdemands = 0;
//    for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
//        sumdemands += NodeList[Facility.FirstStageNodes[i]].Demand;
//    }
//
//    int p = ceil(sumdemands / Facility.Capacity);
    
    float sumdemands = 0;
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
    }
    
    int p = ceil(sumdemands / Facility.Capacity);
    
    int NodeAmount = (int)Facility.FirstStageNodes.size() + 1;
    vector<vector<double>> distance;
    vector<double> demands;
    int i, j;
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    vector<vector<double>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(NodeList[Facility.FirstStageNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.FirstStageNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.FirstStageNodes[i - 1]].Demand);
        }
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    vector<vector<double>> savings;
    savings.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        savings[i].resize(NodeAmount);
    };
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            savings[i][j] = 0;
            if (i > 0 && j > 0 && i != j) {
                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
            }
        }
    }
    
    vector<vector<GRBVar>> x;
    x.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        x[i].resize(NodeAmount);
    };
    
    vector<GRBVar> y;
    y.resize(NodeAmount);
    
    float test =  Facility.Capacity;
    
    // Create 3-D array of model variables
    
    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            string s = "X_" + to_string(i) + "_" + to_string(j);
            x[i][j] = model.addVar(0.0, 1.0, savings[i][j], GRB_BINARY, s); //from i to j with vehicle k
        }
    };
    
    for (j = 1; j < NodeAmount; j++) {
        string s = "Y_" + to_string(j);
        y[j] = model.addVar(demands[j], test, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
    };
    
    //every customer should be visited by one vehicle (both entering and exiting
    GRBLinExpr customervisit = 0;
    for (i = 1; i < NodeAmount; i++) {
        customervisit += x[0][i];
    }
    model.addConstr(customervisit, GRB_EQUAL, p);
    
    
    for (int j = 1; j < NodeAmount; j++) {
        GRBLinExpr customervisit = 0;
        for (i = 0; i < NodeAmount; i++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    for (int i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 1; j < NodeAmount; j++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
    }

    
    //The arc to itself should be zero
    for (i = 0; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        customervisit += x[i][i];
        model.addConstr(customervisit, GRB_EQUAL, 0.0);
    };
    
    
    for (i = 1; i < NodeAmount; i++) {
        for (j = 1; j < NodeAmount; j++) {
            if (i != j) {
                GRBLinExpr Condv = 0;
                Condv += y[i] + (demands[j] * x[i][j]) - (Facility.Capacity*(1-x[i][j]));
                model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
            }
        }
    }
    
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    model.optimize();
    
//    int NextNode;
//    int NN = 0;
    
    vector<vector<float>> X_Sol;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        X_Sol.resize(NodeAmount);
        for (i = 0; i < NodeAmount; i++) {
            X_Sol[i].resize(NodeAmount);
        };
        
        for (int i = 0; i < NodeAmount; i++) {
            for (int j = 0; j < NodeAmount; j++) {
                X_Sol[i][j] = x[i][j].get(GRB_DoubleAttr_X);
            }
        }
        
        *obj = model.get(GRB_DoubleAttr_ObjVal);
    } else {
        X_Sol.push_back(vector<float>());
        X_Sol[X_Sol.size()-1].push_back(-1);
    }
    
    return X_Sol;
    
    
}


float c_ClarkeWright::GurobiAFSS(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<vector<int>> relationships) {
    GRBEnv env = GRBEnv();
    
    FacilityClass ConstrainedFacility = Facility;
    
    
    GRBModel model = GRBModel(env);
    model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 8);
    
    float sumdemands = 0;
    for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
        sumdemands += NodeList[Facility.FirstStageNodes[i]].Demand;
    }
    for (int i = 0; i < Facility.SecondStageNodes.size(); i++) {
        sumdemands += NodeList[Facility.SecondStageNodes[i]].Demand;
    }
    
    int p = ceil(sumdemands / Facility.Capacity);
    
    int NodeAmount = (int)Facility.ServiceNodes.size() + 1;
    vector<vector<double>> distance;
    vector<double> demands;
    int i, j, k;
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    vector<vector<double>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.ServiceNodes[i - 1]].Demand);
        }
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    vector<vector<double>> savings;
    savings.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        savings[i].resize(NodeAmount);
    };
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            savings[i][j] = 0;
            if (i > 0 && j > 0 && i != j) {
                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
            }
        }
    }
    
    GRBVar **x;
    x = new GRBVar*[NodeAmount];
    for (i = 0; i < NodeAmount; i++)
        x[i] = new GRBVar[NodeAmount];
    
    
    vector<GRBVar> y;
    y.resize(NodeAmount);
    
    // Create 3-D array of model variables
    
    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            string s = "X_" + to_string(i) + "_" + to_string(j);
            x[i][j] = model.addVar(0.0, 1.0, savings[i][j], GRB_CONTINUOUS, s); //from i to j
        }
    };
    
    for (j = 1; j < NodeAmount; j++) {
        string s = "Y_" + to_string(j);
        y[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
    };
    
    //every customer should be visited by one vehicle (both entering and exiting
    GRBLinExpr customervisit = 0;
    for (i = 1; i < NodeAmount; i++) {
        customervisit += x[0][i];
    }
    model.addConstr(customervisit, GRB_EQUAL, p);
    
    
    for (int j = 1; j < NodeAmount; j++) {
        GRBLinExpr customervisit = 0;
        for (i = 0; i < NodeAmount; i++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    for (int i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 1; j < NodeAmount; j++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
    }
    
    
    //The arc to itself should be zero
    for (i = 0; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        customervisit += x[i][i];
        model.addConstr(customervisit, GRB_EQUAL, 0.0);
    };
    
    
    
    for (i = 1; i < NodeAmount; i++) {
        for (j = 1; j < NodeAmount; j++) {
            if (i != j) {
                GRBLinExpr Condv = 0;
                Condv += y[i] + (demands[j] * x[i][j]) - (Facility.Capacity*(1-x[i][j]));
                model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
            }
        }
    }
    
    
    for (i = 0; i< relationships.size(); i++) {
        int i1 = relationships[i][0];
        int i2 = relationships[i][1];
        if (i1 != 0 && i2 != 0) {
            model.addConstr(y[i1] + demands[i2]*x[i1][i2] , GRB_LESS_EQUAL, y[i2]);
        }
    }
    
    
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    model.optimize();
    
    float result;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        result = model.get(GRB_DoubleAttr_ObjVal);
    } else {
        result = -1;
    }
    
    return result;
}

float c_ClarkeWright::GurobiAltFormSecondStage(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<vector<int>> relationships, vector<vector<int>> Constraints) {
    GRBEnv env = GRBEnv();
    
    Facility.RouteList.clear();
    
    GRBModel model = GRBModel(env);
    //model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 8);
    
    float sumdemands = 0;
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
    }
    
    int p = ceil(sumdemands / Facility.Capacity);
    
    int NodeAmount = (int)Facility.ServiceNodes.size() + 1;
    vector<vector<float>> distance;
    vector<float> demands;
    int i, j;
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    vector<vector<float>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<float>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<float>());
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.ServiceNodes[i - 1]].Demand);
        }
    }
    
    for (i = 0; i < Facility.SecondStageNodes.size(); i++) {
        demands[Facility.SecondStageNodes[i]] = 0.0;
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    vector<vector<float>> savings;
    savings.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        savings[i].resize(NodeAmount);
    };
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            savings[i][j] = 0;
            if (i > 0 && j > 0 && i != j) {
                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
            }
        }
    }
    
    vector<vector<GRBVar>> x;
    x.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        x[i].resize(NodeAmount);
    };
    
    vector<GRBVar> y;
    y.resize(NodeAmount);
    
    // Create 3-D array of model variables
    
    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            string s = "X_" + to_string(i) + "_" + to_string(j);
            x[i][j] = model.addVar(0.0, 1.0, savings[i][j], GRB_CONTINUOUS, s); //from i to j with vehicle k
        }
    };
    
    for (j = 1; j < NodeAmount; j++) {
        string s = "Y_" + to_string(j);
        y[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
    };
    
    //every customer should be visited by one vehicle (both entering and exiting
    GRBLinExpr customervisit = 0;
    for (i = 1; i < NodeAmount; i++) {
        customervisit += x[0][i];
    }
    model.addConstr(customervisit, GRB_LESS_EQUAL, p);
    
    
    for (int j = 1; j < NodeAmount; j++) {
        GRBLinExpr customervisit = 0;
        for (i = 0; i < NodeAmount; i++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    for (int i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 1; j < NodeAmount; j++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
    }
    
    
    //The arc to itself should be zero
    for (i = 0; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        customervisit += x[i][i];
        model.addConstr(customervisit, GRB_EQUAL, 0.0);
    };
    

    
    for (i = 1; i < NodeAmount; i++) {
        for (j = 1; j < NodeAmount; j++) {
            if (i != j) {
                GRBLinExpr Condv = 0;
                Condv += y[i] + (demands[j] * x[i][j]) - (Facility.Capacity*(1-x[i][j]));
                model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
            }
        }
    }
    
    for (i = 0; i< relationships.size(); i++) {
        int i1 = relationships[i][0];
        int i2 = relationships[i][1];
        if (i1 != 0 && i2 != 0) {
            model.addConstr(y[i1], GRB_LESS_EQUAL, y[i2]);
        }
    }
    
   //  + demands[i2]*x[i1][i2]
//
//    for (i = 0; i < Constraints.size(); i++) {
//        if(Constraints[i][2] == 1) {
//            model.addConstr(x[Constraints[i][0]][Constraints[i][1]], GRB_GREATER_EQUAL, 1.0);
//        } else {
//            model.addConstr(x[Constraints[i][0]][Constraints[i][1]], GRB_LESS_EQUAL, 0.0);
//        }
//    }
    
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    model.optimize();
    
    float result;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        result = model.get(GRB_DoubleAttr_ObjVal);
        
//        for (int i = 0; i < NodeAmount; i++) {
//            for (int j = 0; j < NodeAmount; j++) {
//                cout << round(x[i][j].get(GRB_DoubleAttr_X)) << " ";
//            }
//            cout << endl;
//        }
//
    } else {
        result = -1;
    }

    return result;
}

class validroutes: public GRBCallback
{
public:
    GRBVar** x;
    FacilityClass Facility;
    vector<NodeClass> NodeList;
    int NodeAmount;
    vector<vector<float>> DistanceMatrix;
    validroutes(GRBVar** xvars, vector<NodeClass> NL,  FacilityClass F, vector<vector<float>> D, int n) {
        x = xvars;
        Facility = F;
        NodeList = NL;
        DistanceMatrix = D;
        NodeAmount = n;
    }
protected:
    void callback() {
        try {
            if (where == GRB_CB_MIPSOL) {
                vector<int> Destroy;
                
                Destroy.clear();
                
                vector<vector<float>> x_temp;
                
                for (int i = 0; i < NodeAmount; i++) {
                    x_temp.push_back(vector<float>());
                    for (int j = 0; j < NodeAmount; j++) {
                        x_temp[i].push_back(getSolution(x[i][j]));
                    }
                }
                
                vector<vector<int>> RL;
                vector<vector<int>> RL2;
                
                int t_I;
                for (int i = 1; i < x_temp[0].size(); i++) {
                    if (x_temp[0][i] == 1) {
                        bool done = false;
                        RL.push_back(vector<int>());
                        RL2.push_back(vector<int>());
                        RL[ RL.size()-1].push_back(Facility.ServiceNodes[i-1]);
                        RL2[ RL2.size()-1].push_back(i);
                        
                        t_I = i;
                        while (done == false) {
                            done = true;
                            for (int j = 1; j < x_temp[t_I].size(); j++) {
                                if (x_temp[t_I][j] == 1) {
                                    RL[ RL.size()-1].push_back(Facility.ServiceNodes[j-1]);
                                    RL2[ RL2.size()-1].push_back(j);
                                    done = false;
                                    t_I = j;
                                }
                            }
                        }
                    }
                }
                
                int FN, NN;
                
                vector<vector<int>>  T_rn = RL;
                
                for (int l = 0; l < RL.size(); l++) {
                    for (int e = 0; e < RL[l].size(); e++) {
                        T_rn[l][e] = -1;
                    }
                }
                
                for (int i = 0; i < Facility.RouteList.size(); i++){
                    if (Facility.RouteList[i].size() > 1) {
                        for (int j = 0; j < Facility.RouteList[i].size() - 1; j++) {
                            FN = Facility.RouteList[i][j];
                            NN = Facility.RouteList[i][j+1];
                            
                            for (int l = 0; l < RL.size(); l++) {
                                for (int e = 0; e < RL[l].size(); e++) {
                                    if (RL[l][e] == FN) {
                                        T_rn[l][e] = i;
                                    }
                                    if (RL[l][e] == NN) {
                                        T_rn[l][e] = i;
                                    }
                                }
                            }
                        }
                    } else {
                        for (int l = 0; l < RL.size(); l++) {
                            for (int e = 0; e < RL[l].size(); e++) {
                                if (RL[l][e] == Facility.RouteList[i][0]) {
                                    T_rn[l][e] = i;
                                }
                            }
                        }
                    }
                    
                }
                
                bool don2 = false;
                
                while (don2 == false) {
                    don2 = true;
                    for (int l = 0; l < T_rn.size(); l++) {
                        bool cont = false;
                        for (int e = 0; e < T_rn[l].size(); e++) {
                            if (T_rn[l][e] != -1) {
                                cont = true;
                            }
                        }
                        if (cont == true) {
                            for (int e = 0; e < T_rn[l].size(); e++) {
                                if ( T_rn[l][e] < 0 && e != 0 && e != T_rn[l].size()-1) {
                                    if (T_rn[l][e - 1] >= 0) {
                                        T_rn[l][e] = T_rn[l][e - 1];
                                        don2 = false;
                                    }
                                    if (T_rn[l][e + 1] >= 0) {
                                        T_rn[l][e] = T_rn[l][e + 1];
                                        don2 = false;
                                    }
                                }
                                if ( T_rn[l][e] < 0 && e == T_rn[l].size()-1) {
                                    if (T_rn[l][e - 1] >= 0) {
                                        T_rn[l][e] = T_rn[l][e - 1];
                                        don2 = false;
                                    }
                                }
                                if ( T_rn[l][e] < 0 && e == 0) {
                                    if (T_rn[l][e + 1] >= 0) {
                                        T_rn[l][e] = T_rn[l][e + 1];
                                        don2 = false;
                                    }
                                } 
                            }
                        }
                    }
                }
                
                for (int l = 0; l < T_rn.size(); l++) {
                    for (int e = 1; e < T_rn[l].size(); e++) {
                        if (T_rn[l][e] != T_rn[l][e-1]) {
                            bool found = false;
                            for (int w = 0; w < Destroy.size(); w++) {
                                if (Destroy[w] == l) {
                                    found = true;
                                }
                            }
                            if (found == false) { Destroy.push_back(l); }
                        }
                    }
                }
                
                for (int l = 0; l < T_rn.size(); l++) {
                    for (int e = 0; e < T_rn.size(); e++) {
                        if ( l != e) {
                            if (T_rn[l][0] == T_rn[e][0]) {
                                bool found = false;
                                for (int w = 0; w < Destroy.size(); w++) {
                                    if (Destroy[w] == l) {
                                        found = true;
                                    }
                                }
                                if (found == false) { Destroy.push_back(l); }
                                found = false;
                                for (int w = 0; w < Destroy.size(); w++) {
                                    if (Destroy[w] == e) {
                                        found = true;
                                    }
                                }
                                if (found == false) { Destroy.push_back(e); }
                            }
                        }
                    }
                }

                

                if (Destroy.size() > 0) {
                    for (int i = 0; i < Destroy.size(); i++) {
                        int variableamount = 0;
                        GRBLinExpr expr = 0;
                        expr += x[0][RL2[Destroy[i]][0]];
                        variableamount += 1;
                        for (int j = 0; j < RL[Destroy[i]].size() -1; j++) {
                            int x1 = RL2[Destroy[i]][j];
                            int x2 = RL2[Destroy[i]][j+1];
                            expr += x[x1][x2];
                            variableamount += 1;
                        }
                        addLazy(expr <= variableamount - 1);
                    }
                }
                
            }
        } catch (GRBException e) {
            cout << "Error number: " << e.getErrorCode() << endl;
            cout << e.getMessage() << endl;
        } catch (...) {
            cout << "Error during callback" << endl;
        }
    }
};


float c_ClarkeWright::GurobiAltFormSecondStageK(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    GRBEnv env = GRBEnv();
    
    FacilityClass ConstrainedFacility = Facility;
    
    Facility.RouteList.clear();
    
    GRBModel model = GRBModel(env);
    //model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 8);
    
    float sumdemands = 0;
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
    }
    
    int p = ceil(sumdemands / Facility.Capacity);
    
    int NodeAmount = (int)Facility.ServiceNodes.size() + 1;
    vector<vector<double>> distance;
    vector<double> demands;
    int i, j, k;
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    vector<vector<double>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.ServiceNodes[i - 1]].Demand);
        }
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    vector<vector<double>> savings;
    savings.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        savings[i].resize(NodeAmount);
    };
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            savings[i][j] = 0;
            if (i > 0 && j > 0 && i != j) {
                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
            }
        }
    }

//    GRBVar **x;
//    x = new GRBVar*[NodeAmount];
//    for (i = 0; i < NodeAmount; i++)
//        x[i] = new GRBVar[NodeAmount];
    
    vector<vector<GRBVar>> R;
    R.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        R[i].resize(NodeAmount);
    };
    
    vector<vector<vector<GRBVar>>> x;
    x.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        x[i].resize(NodeAmount);
        for (j = 0; j < NodeAmount; j++) {
            x[i][j].resize(p);
        }
    };

    
    vector<GRBVar> y;
    y.resize(NodeAmount);
    
    // Create 3-D array of model variables
    
    for (k = 0; k < p; k++) {
        for (i = 0; i < NodeAmount; i++) {
            for (j = 0; j < NodeAmount; j++) {
                string s = "X_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k);
                x[i][j][k] = model.addVar(0.0, 1.0, savings[i][j], GRB_BINARY, s); //from i to j
            }
        }
    }
    
    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            string s = "R_" + to_string(i) + "_" + to_string(j);
            R[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, s); //from i to j
        }
    };
    
    for (j = 1; j < NodeAmount; j++) {
        string s = "Y_" + to_string(j);
        y[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
    };
    
    //every customer should be visited by one vehicle (both entering and exiting
    GRBLinExpr customervisit = 0;
    for (k = 0; k < p; k++) {
        for (i = 1; i < NodeAmount; i++) {
            customervisit += x[0][i][k];
        }
    }
    model.addConstr(customervisit, GRB_LESS_EQUAL, p);

    for (k = 0; k < p; k++) {
        GRBLinExpr customervisit = 0;
        for (i = 1; i < NodeAmount; i++) {
            customervisit += x[0][i][k];
        }
        model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
    }
    
    
    for (int j = 1; j < NodeAmount; j++) {
        GRBLinExpr customervisit = 0;
        for (k = 0; k < p; k++) {
            for (i = 0; i < NodeAmount; i++) {
                if (j != i) {
                    customervisit += x[i][j][k];
                }
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    for (k = 0; k < p; k++) {
        for (int j = 1; j < NodeAmount; j++) {
            GRBLinExpr customerenter = 0;
            for (i = 0; i < NodeAmount; i++) {
                if (j != i) {
                    customerenter += x[i][j][k];
                }
            }
            GRBLinExpr customerexit = 0;
            for (i = 0; i < NodeAmount; i++) {
                if (j != i) {
                    customerexit += x[j][i][k];
                }
            }
            model.addConstr(customerenter, GRB_EQUAL, customerexit);
        }
    }
    
    //The arc to itself should be zero
    for (k = 0; k < p; k++) {
        for (i = 0; i < NodeAmount; i++) {
            GRBLinExpr customervisit = 0;
            customervisit += x[i][i][k];
            model.addConstr(customervisit, GRB_EQUAL, 0.0);
        }
    }
    
    
    for (int k = 0; k < p; k++) {
        for (i = 1; i < NodeAmount; i++) {
            for (j = 1; j < NodeAmount; j++) {
                if (i != j) {
                    GRBLinExpr Condv = 0;
                    Condv += y[i] + (demands[j] * x[i][j][k]) - (Facility.Capacity*(1-x[i][j][k]));
                    model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
                }
            }
        }
    }
    
    
    for (i = 0; i < ConstrainedFacility.RouteList.size(); i++) {
        if (ConstrainedFacility.RouteList[i].size() > 1) {
            for (int w = 0; w < ConstrainedFacility.RouteList[i].size() - 1; w++) {
                int index1;
                int r1 = ConstrainedFacility.RouteList[i][w];
                for (int e = 0; e < ConstrainedFacility.ServiceNodes.size(); e++) {
                    if (r1 == ConstrainedFacility.ServiceNodes[e]) {
                        index1 = e + 1;
                    }
                }
                int index2;
                int r2 = ConstrainedFacility.RouteList[i][w+1];
                for (int e = 0; e < ConstrainedFacility.ServiceNodes.size(); e++) {
                    if (r2 == ConstrainedFacility.ServiceNodes[e]) {
                        index2 = e + 1;
                    }
                }
                model.addConstr(y[index1] + demands[index2], GRB_LESS_EQUAL, y[index2]);
                
                
                for (k = 0; k < p; k++) {
                    GRBLinExpr customerenter = 0;
                    for (int q = 0; q < NodeAmount; q++) {
                        if (q != index1) {
                            customerenter += x[q][index1][k];
                        }
                    }
                    GRBLinExpr customerenteralso = 0;
                    for (int q = 0; q < NodeAmount; q++) {
                        if (q != index2) {
                            customerenteralso += x[q][index2][k];
                        }
                    }
                    model.addConstr(customerenter, GRB_EQUAL, customerenteralso);
                }
            }
        }
    }


//    validroutes cb = validroutes(x, NodeList, ConstrainedFacility, DistanceMatrix, NodeAmount);
//    model.setCallback(&cb);
//
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    model.optimize();
    
    float result;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        result = model.get(GRB_DoubleAttr_ObjVal);
        
//        for (int k = 0; k < p; k++) {
//            for (int i = 0; i < NodeAmount; i++) {
//                for (int j = 0; j < NodeAmount; j++) {
//                    if ( x[i][j][k].get(GRB_DoubleAttr_X) < 0.5) {
//                        cout << "0 ";
//                    } else {
//                        cout << "1 ";
//                    }
//                }
//                cout << endl;
//            }
//            cout << endl;
//        }
//
//        cout << endl;
//        
//        for (int i = 0; i < NodeAmount; i++) {
//            for (int j = 0; j < NodeAmount; j++) {
//                cout << savings[i][j] << " ";
//            }
//            cout << endl;
//        }
    
//        Facility.RouteList.clear();
//        for (k = 0; k < p; k++) {
//            int t_I;
//            for (int i = 0; i < NodeAmount; i++) {
//                if (x[0][i][k].get(GRB_DoubleAttr_X) > 0.5) {
//                    bool done = false;
//                    Facility.RouteList.push_back(vector<int>());
//                    Facility.RouteList[ Facility.RouteList.size()-1].push_back(Facility.ServiceNodes[i-1]);
//
//                    t_I = i;
//                    while (done == false) {
//                        done = true;
//                        for (int j = 1; j < NodeAmount; j++) {
//                            if (x[t_I][j][k].get(GRB_DoubleAttr_X) > 0.5) {
//                                Facility.RouteList[ Facility.RouteList.size()-1].push_back(Facility.ServiceNodes[j-1]);
//                                done = false;
//                                t_I = j;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//
//        WriteGraphToFile_C(Facility, ConstrainedFacility, NodeList, file);
        
    } else {
        result = -1;
    }
    
    return result;
}

vector<vector<int>> c_ClarkeWright::GurobiAltFormSecondStageKC(std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj) {
    GRBEnv env = GRBEnv();
    
    FacilityClass ConstrainedFacility = Facility;
    
    
    GRBModel model = GRBModel(env);
    model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 8);
    
    float sumdemands = 0;
    for (int i = 0; i < Facility.FirstStageNodes.size(); i++) {
        sumdemands += NodeList[Facility.FirstStageNodes[i]].Demand;
    }
    for (int i = 0; i < Facility.SecondStageNodes.size(); i++) {
        sumdemands += NodeList[Facility.SecondStageNodes[i]].Demand;
    }
    
    int p = ceil(sumdemands / Facility.Capacity);
    
    int NodeAmount = (int)Facility.ServiceNodes.size() + 1;
    vector<vector<double>> distance;
    vector<double> demands;
    int i, j, k;
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    vector<vector<double>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.ServiceNodes[i - 1]].Demand);
        }
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    vector<vector<double>> savings;
    savings.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        savings[i].resize(NodeAmount);
    };
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            savings[i][j] = 0;
            if (i > 0 && j > 0 && i != j) {
                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
            }
        }
    }
    
    GRBVar **x;
    x = new GRBVar*[NodeAmount];
    for (i = 0; i < NodeAmount; i++)
        x[i] = new GRBVar[NodeAmount];
    
    
    vector<GRBVar> y;
    y.resize(NodeAmount);
    
    // Create 3-D array of model variables
    
    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            string s = "X_" + to_string(i) + "_" + to_string(j);
            x[i][j] = model.addVar(0.0, 1.0, savings[i][j], GRB_BINARY, s); //from i to j
        }
    };
    
    for (j = 1; j < NodeAmount; j++) {
        string s = "Y_" + to_string(j);
        y[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
    };
    
    //every customer should be visited by one vehicle (both entering and exiting
    GRBLinExpr customervisit = 0;
    for (i = 1; i < NodeAmount; i++) {
        customervisit += x[0][i];
    }
    model.addConstr(customervisit, GRB_EQUAL, p);
    
    
    for (int j = 1; j < NodeAmount; j++) {
        GRBLinExpr customervisit = 0;
        for (i = 0; i < NodeAmount; i++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    for (int i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 1; j < NodeAmount; j++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
    }
    
    
    //The arc to itself should be zero
    for (i = 0; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        customervisit += x[i][i];
        model.addConstr(customervisit, GRB_EQUAL, 0.0);
    };
    
    
    
    for (i = 1; i < NodeAmount; i++) {
        for (j = 1; j < NodeAmount; j++) {
            if (i != j) {
                GRBLinExpr Condv = 0;
                Condv += y[i] + (demands[j] * x[i][j]) - (Facility.Capacity*(1-x[i][j]));
                model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
            }
        }
    }
    
    
    for (i = 0; i < ConstrainedFacility.RouteList.size(); i++) {
        if (ConstrainedFacility.RouteList[i].size() > 1) {
            for (int w = 0; w < ConstrainedFacility.RouteList[i].size() - 1; w++) {
                int r1 = ConstrainedFacility.RouteList[i][w];
                int r2 = ConstrainedFacility.RouteList[i][w+1];
                model.addConstr(y[r1] + demands[r2], GRB_LESS_EQUAL, y[r2]);
            }
        }
    };
    
    validroutes cb = validroutes(x, NodeList, ConstrainedFacility, DistanceMatrix, NodeAmount);
    model.setCallback(&cb);
    
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    model.optimize();
    
    vector<vector<int>> result;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        *obj = model.get(GRB_DoubleAttr_ObjVal);
        
        vector<vector<int>> RouteList;
        RouteList.clear();
        int t_I;
        for (int i = 0; i < NodeAmount; i++) {
            if (x[0][i].get(GRB_DoubleAttr_X) > 0.5) {
                bool done = false;
                RouteList.push_back(vector<int>());
                RouteList[ RouteList.size()-1].push_back(Facility.ServiceNodes[i-1]);
                
                t_I = i;
                while (done == false) {
                    done = true;
                    for (int j = 1; j < NodeAmount; j++) {
                        if (x[t_I][j].get(GRB_DoubleAttr_X) > 0.5) {
                            RouteList[ RouteList.size()-1].push_back(Facility.ServiceNodes[j-1]);
                            done = false;
                            t_I = j;
                        }
                    }
                }
            }
        }
        
//        for (int i= 0; i < Facility.SecondStageNodes.size(); i++){
//            for (int j = 0; j < RouteList.size(); j++) {
//                for (int k = 0; k < RouteList[j].size(); k++) {
//                    if (RouteList[j][k] == Facility.SecondStageNodes[i]) {
//                        RouteList[j].erase(RouteList[j].begin() + k);
//                    }
//                }
//            }
//        }
        
        bool dd = false;
        
        int q = 0;
        
        while (dd == false) {
            if (RouteList[q].size() == 0) {
                RouteList.erase(RouteList.begin() + q);
                q = 0;
            }
            q = q + 1;
            if( q >= RouteList.size() ) {
                dd = true;
            }
        }
        
        
        result = RouteList;
    } else {
        vector<vector<int>> RouteList;
        result = RouteList;
    }
    
    return result;
}

FacilityClass c_ClarkeWright::GurobiSol(std::vector< NodeClass > NodeList, FacilityClass Facility) {
    FacilityClass ResultFacility = Facility;
    GRBEnv env = GRBEnv();
    
    GRBModel model = GRBModel(env);
    //model.getEnv().set(GRB_IntParam_LazyConstraints, 0);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 8);
    
    float sumdemands = 0;
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
    }
    int p = ceil(sumdemands / Facility.Capacity);
    
    int NodeAmount = (int)Facility.ServiceNodes.size() + 1;
    vector<vector<double>> distance;
    vector<double> demands;
    int i, j;
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    vector<vector<double>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.ServiceNodes[i - 1]].Demand);
        }
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    vector<vector<double>> savings;
    savings.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        savings[i].resize(NodeAmount);
    };
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            savings[i][j] = 0;
            if (i > 0 && j > 0 && i != j) {
                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
            }
        }
    }
    
    GRBVar **x;
    x = new GRBVar*[NodeAmount];
    for (i = 0; i < NodeAmount; i++)
        x[i] = new GRBVar[NodeAmount];
    
    
    vector<GRBVar> y;
    y.resize(NodeAmount);
    
    // Create 3-D array of model variables
    
    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            string s = "X_" + to_string(i) + "_" + to_string(j);
            x[i][j] = model.addVar(0.0, 1.0, savings[i][j], GRB_BINARY, s); //from i to j
        }
    };
    
    for (j = 1; j < NodeAmount; j++) {
        string s = "Y_" + to_string(j);
        y[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
    };
    
    //every customer should be visited by one vehicle (both entering and exiting
    GRBLinExpr customervisit = 0;
    for (i = 1; i < NodeAmount; i++) {
        customervisit += x[0][i];
    }
    model.addConstr(customervisit, GRB_EQUAL, p);
    
    
    for (int j = 1; j < NodeAmount; j++) {
        GRBLinExpr customervisit = 0;
        for (i = 0; i < NodeAmount; i++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    for (int i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 1; j < NodeAmount; j++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
    }
    
    
    //The arc to itself should be zero
    for (i = 0; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        customervisit += x[i][i];
        model.addConstr(customervisit, GRB_EQUAL, 0.0);
    };
    
    
    
    for (i = 1; i < NodeAmount; i++) {
        for (j = 1; j < NodeAmount; j++) {
            if (i != j) {
                GRBLinExpr Condv = 0;
                Condv += y[i] + (demands[j] * x[i][j]) - (Facility.Capacity*(1-x[i][j]));
                model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
            }
        }
    }
    
    //model.set(GRB_DoubleParam_MIPGap, 0.1);
    
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    model.optimize();
    
    vector<vector<int>> result;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        //*obj = model.get(GRB_DoubleAttr_ObjVal);
        
        vector<vector<int>> RouteList;
        RouteList.clear();
        int t_I;
        for (int i = 0; i < NodeAmount; i++) {
            if (x[0][i].get(GRB_DoubleAttr_X) > 0.5) {
                bool done = false;
                RouteList.push_back(vector<int>());
                RouteList[ RouteList.size()-1].push_back(Facility.ServiceNodes[i-1]);
                
                t_I = i;
                while (done == false) {
                    done = true;
                    for (int j = 1; j < NodeAmount; j++) {
                        if (x[t_I][j].get(GRB_DoubleAttr_X) > 0.5) {
                            RouteList[ RouteList.size()-1].push_back(Facility.ServiceNodes[j-1]);
                            done = false;
                            t_I = j;
                        }
                    }
                }
            }
        }
        
        for (int i= 0; i < Facility.SecondStageNodes.size(); i++){
            for (int j = 0; j < RouteList.size(); j++) {
                for (int k = 0; k < RouteList[j].size(); k++) {
                    if (RouteList[j][k] == Facility.SecondStageNodes[i]) {
                        RouteList[j].erase(RouteList[j].begin() + k);
                    }
                }
            }
        }
        
        bool dd = false;
        
        int q = 0;
        
        while (dd == false) {
            if (RouteList[q].size() == 0) {
                RouteList.erase(RouteList.begin() + q);
                q = 0;
            }
            q = q + 1;
            if( q >= RouteList.size() ) {
                dd = true;
            }
        }
        
        
        ResultFacility.RouteList = RouteList;
    } else {
        vector<vector<int>> RouteList;
        ResultFacility.RouteList = RouteList;
    }
    
    return ResultFacility;
    
    
}


bool c_ClarkeWright::TimeWindowsCheck(FacilityClass Facility, std::vector< NodeClass > NodeList, vector<int> route) {
	bool valid;
	bool validinverse;
	bool result = false;
	valid = TimeWindowsDirection(Facility, NodeList, route);
	std::reverse(route.begin(), route.end());
	validinverse = TimeWindowsDirection(Facility, NodeList, route);
	if (validinverse == true || valid == true) {
		result = true;
	}
	return result;
}


bool c_ClarkeWright::TimeWindowsDirection(FacilityClass Facility, std::vector< NodeClass > NodeList, vector<int> route) {
	int NodeNumber = route[0];
	bool Valid = true;
	float CurTime = 0;
	CurTime = CurTime + NodeDistance(Facility.Number, true, route[0], false, (int)NodeList.size());
	if (CurTime < NodeList[route[0]].StartTW) {
		CurTime = NodeList[route[0]].EndTW;
	};
	if (CurTime > NodeList[route[0]].EndTW) {
		Valid = false;
	};
	int PrevNode = route[0];
	int Size = (int)route.size() - 1;
	for (int i = 0; i < Size; i++) {
		NodeNumber = route[i + 1];
		PrevNode = route[i];
		CurTime = CurTime + NodeDistance(PrevNode, false, NodeNumber, false, (int)NodeList.size());
		if (CurTime < NodeList[NodeNumber].StartTW) {
			CurTime = NodeList[NodeNumber].StartTW;
		};
		if (CurTime > NodeList[NodeNumber].EndTW) {
			Valid = false;
		};
	}
	NodeNumber = route[Size];
	CurTime = CurTime + NodeDistance(Facility.Number, true, route[Size], false, (int)NodeList.size());
	if (CurTime < NodeList[NodeNumber].StartTW) {
		CurTime = NodeList[NodeNumber].StartTW;
	};
	if (CurTime > NodeList[NodeNumber].EndTW) {
		Valid = false;
	};
	return Valid;
}


class validconstraints: public GRBCallback
{
public:
    GRBVar** x;
    GRBVar* y;
    FacilityClass Facility;
    vector<NodeClass> NodeList;
    int NodeAmount;
    vector<vector<int>> constr;
    vector<vector<float>> DistanceMatrix;
    validconstraints(GRBVar** xvars, GRBVar* yvars, vector<NodeClass> NL,  FacilityClass F, vector<vector<float>> D, int n, vector<vector<int>> Constraints) {
        x = xvars;
        y = yvars;
        Facility = F;
        NodeList = NL;
        DistanceMatrix = D;
        NodeAmount = n;
        constr = Constraints;
    }
protected:
    void callback() {
        try {
            if (where == GRB_CB_MIPSOL) {
                vector<int> Destroy;
                
                Destroy.clear();
                
                vector<vector<float>> x_temp;
                
                for (int i = 0; i < NodeAmount; i++) {
                    x_temp.push_back(vector<float>());
                    for (int j = 0; j < NodeAmount; j++) {
                        //x_temp[i].push_back(getSolution(x[i][j]));
                        int t_X;
                        if (getSolution(x[i][j]) < 0.5) {
                            t_X = 0;
                        } else {
                           t_X = 1;
                        }
                        //cout << t_X << " ";
                        x_temp[i].push_back(t_X);
                    }
                    //cout << endl;
                }
                //cout << endl;
                
                vector<vector<int>> RL;
                vector<vector<int>> RL2;
                
                int t_I;
                for (int i = 1; i < x_temp[0].size(); i++) {
                    if (x_temp[0][i] == 1) { // if arc is used
                        bool done = false;
                        RL.push_back(vector<int>());
                        RL2.push_back(vector<int>());
                        // push back id
                        RL[ RL.size()-1].push_back(Facility.ServiceNodes[i-1]);
                        // push back loc
                        RL2[ RL2.size()-1].push_back(i);
                        
                        t_I = i;
                        while (done == false) { //Construct routes
                            done = true;
                            for (int j = 1; j < x_temp[t_I].size(); j++) {
                                if (x_temp[t_I][j] == 1) {
                                    RL[ RL.size()-1].push_back(Facility.ServiceNodes[j-1]);
                                    RL2[ RL2.size()-1].push_back(j);
                                    done = false;
                                    t_I = j;
                                }
                            }
                        }
                    }
                }
                
                for (int a = 0; a < constr.size(); a++) {
                    if (constr[a][0] == 0) { //joint
                        int r1 = 0;
                        int r2 = 0;
                        int t1 = 0;
                        int t2 = 0;
                        for (int j = 0; j < RL.size(); j++) {
                            for (int k = 0; k < RL[j].size(); k++) {
                                if ( RL[j][k] == constr[a][1] ) {
                                    r1 = j;
                                    t1 = k;
                                }
                                if ( RL[j][k] == constr[a][2] ) {
                                    r2 = j;
                                    t2 = k;
                                }
                            }
                        }
                        if (r2 != r1) {
                            //if ( getSolution(y[RL2[r1][t1]]) > getSolution(y[RL2[r2][t2]])) {
                            bool found = false;
                            for (int w = 0; w < Destroy.size(); w++) {
                                if (Destroy[w] == r1) {
                                    found = true;
                                }
                            }
                            if (found == false) { Destroy.push_back(r1); }
                            
                            found = false;
                            for (int w = 0; w < Destroy.size(); w++) {
                                if (Destroy[w] == r2) {
                                    found = true;
                                }
                            }
                            if (found == false) { Destroy.push_back(r2); }
//                                Destroy.push_back(r1);
//                            //} else {
//                                Destroy.push_back(r2);
//                            //}
                        }
                    }
                    if (constr[a][0] == 1) { //disjoint
                        int r1 = 0;
                        int r2 = 0;
                        for (int j = 0; j < RL.size(); j++) {
                            for (int k = 0; k < RL[j].size(); k++) {
                                if ( RL[j][k] == constr[a][1] ) {
                                    r1 = j;
                                }
                                if ( RL[j][k] == constr[a][2] ) {
                                    r2 = j;
                                }
                            }
                        }
                        if (r2 == r1) {
                            bool found = false;
                            for (int w = 0; w < Destroy.size(); w++) {
                                if (Destroy[w] == r1) {
                                    found = true;
                                }
                            }
                            if (found == false) { Destroy.push_back(r1); }
                            //Destroy.push_back(r1);
                        }
                    }
                }
                
                if (Destroy.size() > 0) {
                    //cout << "Destroy: " << endl;
                    for (int i = 0; i < Destroy.size(); i++) {
                        int variableamount = 0;
                        GRBLinExpr expr = 0;
                        expr += x[0][RL2[Destroy[i]][0]];
                        //cout << " x[" << 0 << "," << RL2[Destroy[i]][0] << "] -> ";
                        variableamount += 1;
                        int x1;
                        int x2;
                        for (int j = 0; j < RL[Destroy[i]].size() -1; j++) {
                            x1 = RL2[Destroy[i]][j];
                            x2 = RL2[Destroy[i]][j+1];
                            //cout << " x[" << x1 << "," << x2 << "] -> ";
                            expr += x[x1][x2];
                            variableamount += 1;
                        }
                        if (RL[Destroy[i]].size() == 1) {
                            x2 = RL2[Destroy[i]][0];
                        }
                        //cout << " x[" << x2 << "," << 0 << "]" << endl;
                        variableamount += 1;
                        expr += x[x2][0];
                        addLazy(expr <= variableamount - 1);
                    }
                }
                
            }
        } catch (GRBException e) {
            cout << "Error number: " << e.getErrorCode() << endl;
            cout << e.getMessage() << endl;
        } catch (...) {
            cout << "Error during callback" << endl;
        }
    }
};



vector<vector<int>> c_ClarkeWright::GurobiScenario(int minva, ScenarioClass sc, std::vector< NodeClass > NodeList, FacilityClass Facility, float *obj, vector<vector<int>> Constraints) {
    GRBEnv env = GRBEnv();
    
    FacilityClass ConstrainedFacility = Facility;
    
    //Facility.ServiceNodes = sc.PresentNodes;
    for (int i = 1; i < NodeList.size(); i++) {
        NodeList[i].Demand = sc.Demands[i - 1];
    }
    
    GRBModel model = GRBModel(env);
    model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 8);
    
    float sumdemands = 0;
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        sumdemands += NodeList[Facility.ServiceNodes[i]].Demand;
    }
    int p = ceil(sumdemands / Facility.Capacity);
    
    if (minva > p) {
        p = minva;
    };
    
    Facility.ServiceNodes = sc.PresentNodes;
    
    int NodeAmount = (int)Facility.ServiceNodes.size() + 1;
    vector<vector<float>> distance;
    vector<float> demands;
    int i, j;
    
    distance.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        distance[i].resize(NodeAmount);
    };
    
    vector<vector<float>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<float>());
            location[location.size() - 1].push_back(Facility.LocationX);
            location[location.size() - 1].push_back(Facility.LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<float>());
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationX);
            location[location.size() - 1].push_back(NodeList[Facility.ServiceNodes[i - 1]].LocationY);
            demands.push_back(NodeList[Facility.ServiceNodes[i - 1]].Demand);
        }
    }
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            float DistanceX = location[i][0] - location[j][0];
            float DistanceY = location[i][1] - location[j][1];
            distance[i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
        }
    }
    
    vector<vector<float>> savings;
    savings.resize(NodeAmount);
    for (i = 0; i < NodeAmount; i++) {
        savings[i].resize(NodeAmount);
    };
    
    for (int i = 0; i < location.size(); i++) {
        for (int j = 0; j < location.size(); j++) {
            savings[i][j] = 0;
            if (i > 0 && j > 0 && i != j) {
                savings[i][j] = distance[0][i] + distance[0][j] - distance[i][j];
            }
        }
    }
    
    GRBVar **x;
    x = new GRBVar*[NodeAmount];
    for (i = 0; i < NodeAmount; i++)
        x[i] = new GRBVar[NodeAmount];
    
    GRBVar *y;
    y = new GRBVar[NodeAmount];
    
    
//    vector<GRBVar> y;
//    y.resize(NodeAmount);
    
    // Create 3-D array of model variables
    
    for (i = 0; i < NodeAmount; i++) {
        for (j = 0; j < NodeAmount; j++) {
            string s = "X_" + to_string(i) + "_" + to_string(j);
            x[i][j] = model.addVar(0.0, 1.0, savings[i][j], GRB_BINARY, s); //from i to j
        }
    };
    
    for (j = 1; j < NodeAmount; j++) {
        string s = "Y_" + to_string(j);
        y[j] = model.addVar(demands[j], Facility.Capacity, 0.0, GRB_CONTINUOUS, s); //from i to j with vehicle k
    };
    
    //every customer should be visited by one vehicle (both entering and exiting
    GRBLinExpr customervisit = 0;
    for (i = 1; i < NodeAmount; i++) {
        customervisit += x[0][i];
    }
    model.addConstr(customervisit, GRB_LESS_EQUAL, p);
    
    
    for (int j = 1; j < NodeAmount; j++) {
        GRBLinExpr customervisit = 0;
        for (i = 0; i < NodeAmount; i++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    for (int i = 1; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        for (j = 0; j < NodeAmount; j++) {
            if (j != i) {
                customervisit += x[i][j];
            }
        }
        model.addConstr(customervisit, GRB_EQUAL, 1.0);
    }
    
    
    //The arc to itself should be zero
    for (i = 0; i < NodeAmount; i++) {
        GRBLinExpr customervisit = 0;
        customervisit += x[i][i];
        model.addConstr(customervisit, GRB_EQUAL, 0.0);
    };
    
    
    
    for (i = 1; i < NodeAmount; i++) {
        for (j = 1; j < NodeAmount; j++) {
            if (i != j) {
                GRBLinExpr Condv = 0;
                Condv += y[i] + (demands[j] * x[i][j]) - (Facility.Capacity*(1-x[i][j]));
                model.addConstr(Condv, GRB_LESS_EQUAL, y[j]);
            }
        }
    }
 
//    for (i = 0; i < ConstrainedFacility.RouteList.size(); i++) {
//        if (ConstrainedFacility.RouteList[i].size() > 1) {
//            for (int w = 0; w < ConstrainedFacility.RouteList[i].size() - 1; w++) {
//                int r1 = ConstrainedFacility.RouteList[i][w];
//                int r2 = ConstrainedFacility.RouteList[i][w+1];
//                model.addConstr(y[r1] + demands[r2], GRB_LESS_EQUAL, y[r2]);
//            }
//        }
//    };
    
    validconstraints cb = validconstraints(x, y, NodeList, ConstrainedFacility, distance, NodeAmount, Constraints);
    model.setCallback(&cb);

    
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    model.optimize();
    
    vector<vector<int>> result;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        *obj = model.get(GRB_DoubleAttr_ObjVal);
        
//        cout << endl;
//
//        for (int i = 0; i < NodeAmount; i++) {
//            for (int j = 0; j < NodeAmount; j++) {
//                cout << x[i][j].get(GRB_DoubleAttr_X) << " ";
//            }
//            cout << endl;
//        }
//
//        cout << endl;
//        
//        cout << model.get(GRB_DoubleAttr_ObjVal) << endl;
        
        vector<vector<int>> RouteList;
        RouteList.clear();
        int t_I;
        for (int i = 0; i < NodeAmount; i++) {
            if (x[0][i].get(GRB_DoubleAttr_X) > 0.5) {
                bool done = false;
                RouteList.push_back(vector<int>());
                RouteList[ RouteList.size()-1].push_back(Facility.ServiceNodes[i-1]);

                t_I = i;
                while (done == false) {
                    done = true;
                    for (int j = 1; j < NodeAmount; j++) {
                        if (x[t_I][j].get(GRB_DoubleAttr_X) > 0.5) {
                            RouteList[ RouteList.size()-1].push_back(Facility.ServiceNodes[j-1]);
                            done = false;
                            t_I = j;
                        }
                    }
                }
            }
        }
        
        for (int i= 0; i < Facility.SecondStageNodes.size(); i++){
            for (int j = 0; j < RouteList.size(); j++) {
                for (int k = 0; k < RouteList[j].size(); k++) {
                    if (RouteList[j][k] == Facility.SecondStageNodes[i]) {
                        RouteList[j].erase(RouteList[j].begin() + k);
                    }
                }
            }
        }
        
        bool dd = false;
        
        int q = 0;
        
        while (dd == false) {
            if (RouteList[q].size() == 0) {
                RouteList.erase(RouteList.begin() + q);
                q = 0;
            }
            q = q + 1;
            if( q >= RouteList.size() ) {
                dd = true;
            }
        }
        
        
        result = RouteList;
    } else {
        vector<vector<int>> RouteList;
        result = RouteList;
    }
    
    return result;
}


vector<FacilityClass> c_ClarkeWright::GurobiMDVRP(std::vector< NodeClass > NodeList, vector<FacilityClass> FacilityList) {
    GRBEnv env = GRBEnv();
    
    for (int i = 0; i < FacilityList.size(); i++) {
        FacilityList[i].RouteList.clear();
    }
    
    
    GRBModel model = GRBModel(env);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    model.getEnv().set(GRB_IntParam_Threads, 8);
    
    float sumdemands = 0;
    for (int i = 0; i < FacilityList.size(); i++) {
        for (int j = 0; j < FacilityList[i].ServiceNodes.size(); j++) {
            sumdemands += NodeList[FacilityList[i].ServiceNodes[j]].Demand;
        }
    }
    
    vector<int> t_ServiceNodes;
    for (int i = 0; i < FacilityList.size(); i++) {
        for (int j = 0; j < FacilityList[i].ServiceNodes.size(); j++) {
            t_ServiceNodes.push_back(FacilityList[i].ServiceNodes[j]);
        }
    }
    
    int p = ceil(ceil(sumdemands / FacilityList[0].Capacity)/FacilityList.size());
    
    int NodeAmount = (int)NodeList.size();
    int FacilityAmount = (int)FacilityList.size();
    vector<vector<vector<double>>> distance;
    vector<double> demands;
    int i, j, k;
    
    
    distance.resize(FacilityAmount);
    for (i = 0; i < FacilityAmount; i++) {
        distance[i].resize(NodeAmount);
        for (int j = 0; j < NodeAmount; j++) {
            distance[i][j].resize(NodeAmount);
        }
    };
    
    vector<vector<double>> location;
    for (i = 0; i < NodeAmount; i++) {
        if (i == 0) {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(FacilityList[0].LocationX);
            location[location.size() - 1].push_back(FacilityList[0].LocationY);
            demands.push_back(0);
        }
        else {
            location.push_back(vector<double>());
            location[location.size() - 1].push_back(NodeList[i].LocationX);
            location[location.size() - 1].push_back(NodeList[i].LocationY);
            demands.push_back(NodeList[i].Demand);
        }
    }
    
    for (int k = 0; k < FacilityAmount; k++) {
        location[0][0] = FacilityList[k].LocationX;
        location[0][1] = FacilityList[k].LocationY;
        for (int i = 0; i < location.size(); i++) {
            for (int j = 0; j < location.size(); j++) {
                float DistanceX = location[i][0] - location[j][0];
                float DistanceY = location[i][1] - location[j][1];
                distance[k][i][j] = std::sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
            }
        }
    }
    
    vector<vector<vector<double>>> savings;
    
    savings.resize(FacilityAmount);
    for (i = 0; i < FacilityAmount; i++) {
        savings[i].resize(NodeAmount);
        for (int j = 0; j < NodeAmount; j++) {
            savings[i][j].resize(NodeAmount);
        }
    };
  
    for (int k = 0; k < FacilityAmount; k++) {
        for (int i = 0; i < location.size(); i++) {
            for (int j = 0; j < location.size(); j++) {
                savings[k][i][j] = 0;
                if (i > 0 && j > 0 && i != j) {
                    savings[k][i][j] = distance[k][0][i] + distance[k][0][j] - distance[k][i][j];
                }
            }
        }
    }
    
    vector<vector<vector<GRBVar>>> x;
    x.resize(FacilityAmount);
    for (k = 0; k < FacilityAmount; k++) {
        x[k].resize(NodeAmount);
        for (i = 0; i < NodeAmount; i++) {
            x[k][i].resize(NodeAmount);
        };
    };
    
    vector<vector<GRBVar>> y;
    y.resize(FacilityAmount);
    for (k = 0; k < FacilityAmount; k++) {
        y[k].resize(NodeAmount);
    };
    
    float test =  FacilityList[0].Capacity;
    
    // Create 3-D array of model variables
    
    for (k = 0; k < FacilityAmount; k++) {
        for (i = 0; i < NodeAmount; i++) {
            for (j = 0; j < NodeAmount; j++) {
                string s = "X_" + to_string(i) + "_" + to_string(j);
                x[k][i][j] = model.addVar(0.0, 1.0, savings[k][i][j], GRB_BINARY, s);
            }
        }
    }
    
    for (k = 0; k < FacilityAmount; k++) {
        for (j = 1; j < NodeAmount; j++) {
            string s = "Y_" + to_string(j);
            y[k][j] = model.addVar(demands[j], test, 0.0, GRB_CONTINUOUS, s);
        }
    }
    
    //every customer should be visited by one vehicle (both entering and exiting
    GRBLinExpr customervisit = 0;
    for (k = 0; k < FacilityAmount; k++) {
        for (i = 1; i < NodeAmount; i++) {
            customervisit += x[k][0][i];
        }
    }
    model.addConstr(customervisit, GRB_EQUAL, p);
    
    for (k = 0; k < FacilityAmount; k++) {
        for (int j = 1; j < NodeAmount; j++) {
            GRBLinExpr customervisit = 0;
            for (i = 0; i < NodeAmount; i++) {
                if (j != i) {
                    customervisit += x[k][i][j];
                }
            }
            model.addConstr(customervisit, GRB_EQUAL, 1.0);
        }
    }
    
    for (k = 0; k < FacilityAmount; k++) {
        for (int j = 1; j < NodeAmount; j++) {
            GRBLinExpr customerenter = 0;
            for (i = 0; i < NodeAmount; i++) {
                if (j != i) {
                    customerenter += x[k][i][j];
                }
            }
            GRBLinExpr customerexit = 0;
            for (i = 0; i < NodeAmount; i++) {
                if (j != i) {
                    customerexit += x[k][j][i];
                }
            }
            
            model.addConstr(customerenter, GRB_EQUAL, customerexit);
        }
    }
    
//    for (k = 0; k < FacilityAmount; k++) {
//        for (int i = 1; i < NodeAmount; i++) {
//            GRBLinExpr customervisit = 0;
//            for (j = 1; j < NodeAmount; j++) {
//                if (j != i) {
//                    customervisit += x[k][i][j];
//                }
//            }
//            model.addConstr(customervisit, GRB_LESS_EQUAL, 1.0);
//        }
//    }
    
    
    //The arc to itself should be zero
    for (int k = 0; k < FacilityAmount; k++) {
        for (i = 0; i < NodeAmount; i++) {
            GRBLinExpr customervisit = 0;
            customervisit += x[k][i][i];
            model.addConstr(customervisit, GRB_EQUAL, 0.0);
        }
    }
    
    for (k = 0; k < FacilityAmount; k++) {
        for (i = 1; i < NodeAmount; i++) {
            for (j = 1; j < NodeAmount; j++) {
                if (i != j) {
                    GRBLinExpr Condv = 0;
                    Condv += y[k][i] + (demands[j] * x[k][i][j]) - (FacilityList[k].Capacity*(1-x[k][i][j]));
                    model.addConstr(Condv, GRB_LESS_EQUAL, y[k][j]);
                }
            }
        }
    }
    
    model.set(GRB_DoubleParam_MIPGap, 0.1);
    
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    model.optimize();
    
    vector<vector<vector<float>>> X_Sol;
    
    if (model.get(GRB_IntAttr_SolCount) > 0) {
        
        vector<int> t_ServiceNodes;
        for (int i = 0; i < FacilityList.size(); i++) {
            FacilityList[i].ServiceNodes.clear();
            FacilityList[i].RouteList.clear();
        }
        
        for (int k = 0; k < FacilityAmount; k++) {
            vector<vector<int>> RouteList;
            RouteList.clear();
            int t_I;
            for (int i = 0; i < NodeAmount; i++) {
                if (x[0][i][k].get(GRB_DoubleAttr_X) > 0.5) {
                    bool done = false;
                    RouteList.push_back(vector<int>());
                    RouteList[ RouteList.size()-1].push_back(t_ServiceNodes[i-1]);
                    FacilityList[k].ServiceNodes.push_back(t_ServiceNodes[i-1]);
                    
                    t_I = i;
                    while (done == false) {
                        done = true;
                        for (int j = 1; j < NodeAmount; j++) {
                            if (x[t_I][j][k].get(GRB_DoubleAttr_X) > 0.5) {
                                RouteList[ RouteList.size()-1].push_back(t_ServiceNodes[j-1]);
                                FacilityList[k].ServiceNodes.push_back(t_ServiceNodes[j-1]);
                                done = false;
                                t_I = j;
                            }
                        }
                    }
                }
            }
        }
        
        return FacilityList;
    } else {
        vector<int> t_ServiceNodes;
        for (int i = 0; i < FacilityList.size(); i++) {
            FacilityList[i].ServiceNodes.clear();
            FacilityList[i].RouteList.clear();
        }
        return FacilityList;
    }
    
    
}
