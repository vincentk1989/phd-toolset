//# include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"
#include "Auction.h"

c_Auction::c_Auction() {

};

std::vector< FacilityClass > c_Auction::OriginalSingleAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int FacilityAmount, ofstream& myfile) {
	std::vector< FacilityClass > CopyFacilityList;
	float RouteLength = 0;
	float RouteLengthCopy = 0;
	//start the Auction!
	std::array<float, 3> NodeCosts;
	std::vector<std::array<float, 3>> CostsList;
	float BidPrice = 0;
	float Lowestbid;
	int Lowestbidder;
	std::vector<float> BidList;
	int StopCriterium = 0;

	int ftuc = -1;

	while (StopCriterium < FacilityAmount) {
		ftuc = ftuc + 1;
		CostsList.clear();
		if (ftuc >= FacilityAmount) { ftuc = 0; };

		for (int n = 0; n < FacilityList[ftuc].ServiceNodes.size(); n++) {
			RouteLength = 0;
			RouteLengthCopy = 0;
			CopyFacilityList = FacilityList;
			CopyFacilityList[ftuc].ServiceNodes.erase(CopyFacilityList[ftuc].ServiceNodes.begin() + n);
			CopyFacilityList[ftuc] = ClarkeWright(NodeList, CopyFacilityList[ftuc]);
			for (int j = 0; j < CopyFacilityList[ftuc].RouteList.size(); j++) {
				RouteLength = RouteLength + CalculateRouteLength(NodeList, CopyFacilityList[ftuc], j);
			};
			for (int j = 0; j < FacilityList[ftuc].RouteList.size(); j++) {
				RouteLengthCopy = RouteLengthCopy + CalculateRouteLength(NodeList, FacilityList[ftuc], j);
			};
			NodeCosts[0] = n;
			NodeCosts[1] = RouteLengthCopy - RouteLength;
			NodeCosts[2] = FacilityList[ftuc].ServiceNodes[n];
			CostsList.push_back(NodeCosts);
		};

		//CostsList = VSumToOthers(FacilityList[ftuc], NodeList);

		struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[1] > j[1]; } } SortMarginalCosts;

		std::sort(CostsList.begin(), CostsList.end(), SortMarginalCosts);

		int Test = (int)CostsList[0][0];
        
        bool nonblocked;
        int i_req = 0;
        
        if(NodeList[FacilityList[ftuc].ServiceNodes[Test]].Service == true) {
            nonblocked = true;
        } else {
            nonblocked = false;
        }
        while(nonblocked == false ) {
            i_req = i_req + 1;
            int Test = (int)CostsList[i_req][0];
            if(NodeList[FacilityList[ftuc].ServiceNodes[Test]].Service == true) {
                nonblocked = true;
            } else {
                nonblocked = false;
            }
        }

		FacilityList[ftuc].ServiceNodes.erase(FacilityList[ftuc].ServiceNodes.begin() + Test);
		FacilityList[ftuc] = ClarkeWright(NodeList, FacilityList[ftuc]);

		BidList.clear();
		CopyFacilityList = FacilityList;

		for (int fts = 0; fts < FacilityAmount; fts++) {

			RouteLength = 0;
			RouteLengthCopy = 0;
			for (int j = 0; j < FacilityList[fts].RouteList.size(); j++) {
				RouteLength = RouteLength + CalculateRouteLength(NodeList, FacilityList[fts], j);
			};
			CopyFacilityList[fts].ServiceNodes.push_back(CostsList[0][2]);
			CopyFacilityList[fts] = ClarkeWright(NodeList, CopyFacilityList[fts]);
			for (int j = 0; j < CopyFacilityList[fts].RouteList.size(); j++) {
				RouteLengthCopy = RouteLengthCopy + CalculateRouteLength(NodeList, CopyFacilityList[fts], j);
			};
			if (fts != ftuc) {
				BidPrice = (RouteLengthCopy - RouteLength) * margin;
			}
			else {
				BidPrice = (RouteLengthCopy - RouteLength);
			};
			BidList.push_back(BidPrice);
		};
		Lowestbid = floatbigM;
		for (int Y = 0; Y < BidList.size(); Y++) {
			if (BidList[Y] < Lowestbid) {
				Lowestbid = BidList[Y];
				Lowestbidder = Y;
			}
		};
        
        if( Lowestbidder != ftuc ) {
            Tracker.push_back(vector<float>());
            Tracker[Tracker.size()-1].push_back(ftuc);
            Tracker[Tracker.size()-1].push_back(Lowestbidder);
            Tracker[Tracker.size()-1].push_back(BidList[ftuc]);
            Tracker[Tracker.size()-1].push_back(BidList[Lowestbidder]);
            ExchangedRequests.push_back(vector<int>());
            ExchangedRequests[ExchangedRequests.size()-1].push_back(CostsList[0][2]);
        }
        
		if (Lowestbidder == ftuc) {
			StopCriterium = StopCriterium + 1;
		}
		else
		{
			StopCriterium = 0;
		};

		bool TDS = false;
		for (int ss = 0; ss < Seen.size(); ss++) {
			if (Seen[ss] == CostsList[0][2]) {
				TDS = true;
			}
		};
		if (TDS == false) { Seen.push_back(CostsList[0][2]); };

		FacilityList[Lowestbidder].ServiceNodes.push_back(CostsList[0][2]);
		FacilityList[Lowestbidder] = ClarkeWright(NodeList, FacilityList[Lowestbidder]);

		if (Lowestbidder != ftuc && Writefile == true) {
			WriteGraphToFile(FacilityList, NodeList, myfile, "Node auction step " + to_string(GVstep) + " of node " + to_string((int)CostsList[0][2]));
			for (int Y = 0; Y < BidList.size(); Y++) {
				myfile << "The bid of facility " << Y + 1 << " is " << BidList[Y] << ".\\\\" << "\n";
			};
			myfile << "\\\\" << "\n";
			myfile << "Facility " << Lowestbidder + 1 << " is thus the lowest bidder in the auction.\\\\" << "\n";
			myfile << "\\\\" << "\n";
			WriteRouteToFile(FacilityList, NodeList, myfile);
			WriteNewPage(myfile);
			GVstep = GVstep + 1;
		}
	}
	return FacilityList;
};

std::vector< FacilityClass > c_Auction::BlackListSingleAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int FacilityAmount, ofstream& myfile) {
	std::vector< FacilityClass > ReferenceFacilityList = FacilityList;
	std::vector< FacilityClass > CopyFacilityList;
	std::vector< FacilityClass > PrevFacilityList;
	float RouteLength = 0;
	float RouteLengthCopy = 0;
    margin = 1;
	//start the Auction!
	std::array<float, 3> NodeCosts;
	std::vector<int> BlackList;
	std::vector<std::array<float, 3>> CostsList;
	float BidPrice = 0;
	float Lowestbid;
	int Lowestbidder;
	std::vector<float> BidList;
	std::vector<float> T_List;
	int StopCriterium = 0;
    int mc = 0;
	int ftuc = -1;
    
    for (int i = 0; i < NodeList.size(); i++) {
        if (NodeList[i].Service == false) {
            BlackList.push_back(i);
        }
    };

	while (BlackList.size() < NodeList.size()) {
		mc = mc + 1;
		ftuc = ftuc + 1;
        
		CostsList.clear();
		if (ftuc > FacilityAmount - 1) { ftuc = 0; };

		while (FacilityList[ftuc].ServiceNodes.size() == 0) {
			ftuc = ftuc + 1;
			if (ftuc > FacilityAmount - 1) { ftuc = 0; };
		};
		

//        switch (ValuationType) {
//        case 0: //ordinary
			for (int n = 0; n < FacilityList[ftuc].ServiceNodes.size(); n++) {

				CopyFacilityList = FacilityList;
				CopyFacilityList[ftuc].ServiceNodes.erase(CopyFacilityList[ftuc].ServiceNodes.begin() + n);
				CopyFacilityList[ftuc] = ClarkeWright(NodeList, CopyFacilityList[ftuc]);


				RouteLength = 0;
				for (int j = 0; j < CopyFacilityList[ftuc].RouteList.size(); j++) {
					RouteLength = RouteLength + CalculateRouteLength(NodeList, CopyFacilityList[ftuc], j);
				};

				RouteLengthCopy = 0;
				for (int j = 0; j < FacilityList[ftuc].RouteList.size(); j++) {
					RouteLengthCopy = RouteLengthCopy + CalculateRouteLength(NodeList, FacilityList[ftuc], j);
				};

				NodeCosts[0] = n;
				NodeCosts[1] = RouteLengthCopy - RouteLength;
				NodeCosts[2] = FacilityList[ftuc].ServiceNodes[n];
				CostsList.push_back(NodeCosts);
            };
//            break;
//        case 1:
//            CostsList = VFacilityToNode(FacilityList[ftuc], NodeList);
//            break;
//        case 2:
//            CostsList = VNextAndPrevious(FacilityList[ftuc], NodeList);
//            break;
//        case 3:
//            CostsList = VSumToOthers(FacilityList[ftuc], NodeList);
//            break;
//        case 4:
//            CostsList = VClosestNode(FacilityList[ftuc], NodeList);
//            break;
//        case 5:
//            CostsList = VDaganzo(FacilityList[ftuc], NodeList);
//            break;
//        case 6:
//            CostsList = VCentroid(FacilityList[ftuc], NodeList);
//            break;
//        default:
//            cout << "Error, bad input, quitting\n";
//            break;
//        };

		struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[1] > j[1]; } } SortMarginalCosts;

		std::sort(CostsList.begin(), CostsList.end(), SortMarginalCosts);

		bool Done = false;
		int Counter = -1;
		while (Done == false) {
			Counter = Counter + 1;
			Done = true;
			for (int g = 0; g < BlackList.size(); g++) {
				if (CostsList[Counter][2] == BlackList[g]) {
					Done = false;
				}
			};
			if (Counter + 1 >= CostsList.size()) {
				Done = true;
				Counter = Counter + 1;
			};
		};
        
        int Test = (int)CostsList[Counter][0];

//        if (Counter < CostsList.size()) {
//
//            bool nonblocked;
//            int i_req = Counter;
//
//            if(NodeList[FacilityList[ftuc].ServiceNodes[Test]].Service == true) {
//                nonblocked = true;
//            } else {
//                nonblocked = false;
//            }
//            while ( nonblocked == false ) {
//                i_req = i_req + 1;
//                if (i_req < FacilityList[ftuc].ServiceNodes.size()) {
//                    int Test = (int)CostsList[i_req][0];
//                    if(NodeList[FacilityList[ftuc].ServiceNodes[Test]].Service == true) {
//                        nonblocked = true;
//                    } else {
//                        nonblocked = false;
//                    }
//                } else {
//                    Counter = (int)CostsList.size() + 1;
//                }
//            }
//        }

		PrevFacilityList = FacilityList;

		if (Counter < CostsList.size()) {
			FacilityList[ftuc].ServiceNodes.erase(FacilityList[ftuc].ServiceNodes.begin() + Test);
			FacilityList[ftuc] = ClarkeWright(NodeList, FacilityList[ftuc]);

			BidList.clear();
			T_List.clear();
			CopyFacilityList = FacilityList;

			for (int fts = 0; fts < FacilityAmount; fts++) {
				RouteLength = 0;
				RouteLengthCopy = 0;
				for (int j = 0; j < FacilityList[fts].RouteList.size(); j++) {
					RouteLength = RouteLength + CalculateRouteLength(NodeList, FacilityList[fts], j);
				};
				CopyFacilityList[fts].ServiceNodes.push_back(CostsList[Counter][2]);

				CopyFacilityList[fts] = ClarkeWright(NodeList, CopyFacilityList[fts]);
				for (int j = 0; j < CopyFacilityList[fts].RouteList.size(); j++) {
					RouteLengthCopy = RouteLengthCopy + CalculateRouteLength(NodeList, CopyFacilityList[fts], j);
				};

				if (fts != ftuc) {
					BidPrice = (RouteLengthCopy - RouteLength) * margin;
				}
				else {
					BidPrice = (RouteLengthCopy - RouteLength);
				};
				BidList.push_back(BidPrice);

				BidPrice = (RouteLengthCopy - RouteLength);
				T_List.push_back(BidPrice);
			};
			Lowestbid = 1000000000000;
			for (int Y = 0; Y < BidList.size(); Y++) {
				if (BidList[Y] < Lowestbid) {
					Lowestbid = BidList[Y];
					Lowestbidder = Y;
				}
			};
            
//            if( Lowestbidder != ftuc ) {
//                Tracker.push_back(vector<float>());
//                Tracker[Tracker.size()-1].push_back(ftuc);
//                Tracker[Tracker.size()-1].push_back(Lowestbidder);
//                Tracker[Tracker.size()-1].push_back(BidList[ftuc]);
//                Tracker[Tracker.size()-1].push_back(BidList[Lowestbidder]);
//                ExchangedRequests.push_back(vector<int>());
//                ExchangedRequests[ExchangedRequests.size()-1].push_back(CostsList[Counter][2]);
//            }

//            bool TDS = false;
//            for (int ss = 0; ss < Seen.size(); ss++) {
//                if (Seen[ss] == CostsList[Counter][2]) {
//                    TDS = true;
//                }
//            };
//            if (TDS == false) { Seen.push_back(CostsList[Counter][2]); };

			if (Lowestbidder == ftuc) {
				BlackList.push_back(CostsList[Counter][2]);
			}
			else
			{   
				BlackList.clear();
                for (int i = 0; i < NodeList.size(); i++) {
                    if (NodeList[i].Service == false) {
                        BlackList.push_back(i);
                    }
                };
			};

			FacilityList[Lowestbidder].ServiceNodes.push_back(CostsList[Counter][2]);
			FacilityList[Lowestbidder] = ClarkeWright(NodeList, FacilityList[Lowestbidder]);
		}
        
        cout << CalculateTotalCosts(FacilityList, NodeList) << endl;
	}
	return FacilityList;
};

std::vector< FacilityClass > c_Auction::RouteAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int FacilityAmount, ofstream& myfile) {
	std::vector< FacilityClass > CopyFacilityList;
	std::vector< FacilityClass > PrevFacilityList;
	bool Complete = false;
	int route;
	int fcuc = 0;
	float MaxRouteCost = 0;
	vector<int> c_Route;
	int solfac;
	float testcost = floatbigM;
	int noex = 0;

	while (Complete == false) {
        
		PrevFacilityList = FacilityList;

        //Find Most expensive route
		MaxRouteCost = 0;
		for (int rtest = 0; rtest < FacilityList[fcuc].RouteList.size(); rtest++) {
			if (CalculateRouteLength(NodeList, FacilityList[fcuc], rtest) > MaxRouteCost) {
				MaxRouteCost = CalculateRouteLength(NodeList, FacilityList[fcuc], rtest);
				route = rtest;
			}
		};

		if (FacilityList[fcuc].ServiceNodes.size() > 0) {
			c_Route = FacilityList[fcuc].RouteList[route];
		}
		else { c_Route.clear(); };

		//add to Seen
		for (int q = 0; q < c_Route.size(); q++) {
			bool TDS = false;
			for (int ss = 0; ss < Seen.size(); ss++) {
				if (Seen[ss] == c_Route[q]) {
					TDS = true;
				}
			};
			if (TDS == false) { Seen.push_back(c_Route[q]); };
		}
        
        bool control = false;
        int it = 0;
        
        while (control == false) {
            if (NodeList[c_Route[it]].Service == false) {
                c_Route.erase(c_Route.begin() + it);
            } else {
                it = it + 1;
            }
            if ( it >= c_Route.size()) {
                control = true;
            }
        }
        
		// Start Auctual Auction;
		for (int j = 0; j < c_Route.size(); j++) {
			for (int w = 0; w < FacilityList.size(); w++) {
                int k = 0;
                bool done = false;
                while (done == false && FacilityList[w].ServiceNodes.size() > 0) {
					if (FacilityList[w].ServiceNodes[k] == c_Route[j]) {
						FacilityList[w].ServiceNodes.erase(FacilityList[w].ServiceNodes.begin() + k);
                    } else {
                        k = k + 1;
                    }
                    if (k >= FacilityList[w].ServiceNodes.size() ) {
                        done = true;
                    }
                }
			}
		};

		for (int w = 0; w < FacilityList.size(); w++) {
			FacilityList[w] = ClarkeWright(NodeList, FacilityList[w]);
		};
        
        float t_bid;
        testcost = floatbigM;

		for (int fcoc = 0; fcoc < FacilityList.size(); fcoc++) {
			CopyFacilityList = FacilityList;
			for (int rc = 0; rc < c_Route.size(); rc++) {
				CopyFacilityList[fcoc].ServiceNodes.push_back(c_Route[rc]);
			};
			CopyFacilityList[fcoc] = ClarkeWright(NodeList, CopyFacilityList[fcoc]);
			float bid;
			if (fcoc == fcuc) {
				bid = CalculateTotalCosts(CopyFacilityList, NodeList) - CalculateTotalCosts(FacilityList, NodeList);
                t_bid = bid;
			}
			else {
				bid = (CalculateTotalCosts(CopyFacilityList, NodeList) - CalculateTotalCosts(FacilityList, NodeList)) * margin;
			}
				
			if (bid < testcost) {
				solfac = fcoc;
				testcost = bid;
			}
		};
        
		for (int rc = 0; rc < c_Route.size(); rc++) {
			FacilityList[solfac].ServiceNodes.push_back(c_Route[rc]);
		};

		if (solfac == fcuc) {
			noex = noex + 1;
		}

		if (noex > 2*FacilityList.size()) {
			Complete = true;
		};

		for (int w = 0; w < FacilityList.size(); w++) {
			FacilityList[w] = ClarkeWright(NodeList, FacilityList[w]);
		};
        


		if (CalculateTotalCosts(FacilityList, NodeList) > CalculateTotalCosts(PrevFacilityList, NodeList)) 
		{
			FacilityList = PrevFacilityList;
        } else {
            if( solfac != fcuc && c_Route.size() > 0) {
                Tracker.push_back(vector<float>());
                Tracker[Tracker.size()-1].push_back(fcuc);
                Tracker[Tracker.size()-1].push_back(solfac);
                Tracker[Tracker.size()-1].push_back(t_bid);
                Tracker[Tracker.size()-1].push_back(testcost);
                ExchangedRequests.push_back(c_Route);
            }
        }

//        for (int ct = 1; ct <= 100; ct++) {
//            float c = ct;
//            float p = ct - 1;
//            float countf = Seen.size();
//            float nls = NodeList.size() - 1;
//            float percseen = countf / nls;
//            float curperc = p / nls;
//            float prevperc = c / nls;
//            if (percseen <= c / 100 && percseen > p / 100) {
//                CostTracker[ct] = CalculateTotalCosts(FacilityList, NodeList);
//            }
//        }
        
		fcuc = fcuc + 1;
		if (fcuc > FacilityList.size() - 1) { fcuc = 0; };
        
        cout << CalculateTotalCosts(FacilityList, NodeList) << endl;

	};

	return FacilityList;
}

bool InBL(vector<int> route, vector<vector<int>> blacklist) {
	bool result = false;
	for (int i = 0; i < blacklist.size(); i++) {
		if (route == blacklist[i]) {
			result = true;
		}
	};
	return result;
}

std::vector< FacilityClass > c_Auction::RecursiveAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile) {
	vector<int> c_Route;
	vector<vector<int>> solution;
	vector<vector<int>> blacklist;
	int erasenode;
	bool Complete = false;
	int NoImprovement = 0;
	int fcuc = 0;
	int route = 0;
	float MaxRouteCost = 0;
	int iteration = 0;
	int routesize =  0;
	std::vector< FacilityClass > SolutionFacilityList;
	std::vector< FacilityClass > CopyFacilityList;
	float curcosts = CalculateTotalCosts(FacilityList, NodeList);
    
    vector<float> o_costs;
    


	while (Complete == false) {
		o_costs.clear();
		for (int i = 0; i < FacilityList.size(); i++) {
			o_costs.push_back(CalculateFacilityCosts(FacilityList[i], NodeList));
		}

		iteration += 1;
		//cout << "Iteration: " << iteration << endl;
		c_Route.clear();
		MaxRouteCost = 0;
		for (int rtest = 0; rtest < FacilityList[fcuc].RouteList.size(); rtest++) {
			if (CalculateRouteLength(NodeList, FacilityList[fcuc], rtest) > MaxRouteCost && !InBL(FacilityList[fcuc].RouteList[rtest], blacklist)) {
				MaxRouteCost = CalculateRouteLength(NodeList, FacilityList[fcuc], rtest);
				route = rtest;
			}
		};
		if (MaxRouteCost == 0) {
			fcuc = fcuc + 1;
			if (fcuc > FacilityList.size() - 1) { fcuc = 0; };
		};
		if (MaxRouteCost != 0) {
			//add to Seen
			for (int q = 0; q < FacilityList[fcuc].RouteList[route].size(); q++) {
				bool TDS = false;
				for (int ss = 0; ss < Seen.size(); ss++) {
					if (Seen[ss] == FacilityList[fcuc].RouteList[route][q]) {
						TDS = true;
					}
				};
				if (TDS == false) { Seen.push_back(FacilityList[fcuc].RouteList[route][q]); };
			};
			for (int w = 0; w < FacilityList[fcuc].RouteList[route].size(); w++) {
				c_Route.push_back(FacilityList[fcuc].RouteList[route][w]);
			}
			//End add to seen
            
            bool control = false;
            int it = 0;
            
            while (control == false) {
                if (NodeList[c_Route[it]].Service == false) {
                    c_Route.erase(c_Route.begin() + it);
                } else {
                    it = it + 1;
                }
                if ( it >= c_Route.size()) {
                    control = true;
                }
            }

			CopyFacilityList = FacilityList;

			for (int j = 0; j < c_Route.size(); j++) {
				for (int w = 0; w < FacilityList.size(); w++) {
					for (int k = 0; k < FacilityList[w].ServiceNodes.size(); k++) {
						if (FacilityList[w].ServiceNodes[k] == c_Route[j]) {
							erasenode = k;
							FacilityList[w].ServiceNodes.erase(FacilityList[w].ServiceNodes.begin() + erasenode);
						}
					}
				}
			};

			for (int w = 0; w < FacilityList.size(); w++) {
				FacilityList[w] = ClarkeWright(NodeList, FacilityList[w]);
			};

			r_Check.clear();

			solution = r_Auction(FacilityList, NodeList, fcuc, c_Route);

			for (int w = 0; w < FacilityList.size(); w++) {
				for (int j = 0; j < solution[w].size(); j++) {
					FacilityList[w].ServiceNodes.push_back(solution[w][j]);
				};
				FacilityList[w] = ClarkeWright(NodeList, FacilityList[w]);
			};
            
            
            for (int w = 0; w < FacilityList.size(); w++) {
                if ( w != fcuc ) {
                    if(c_Route.size() > 0) {
                        Tracker.push_back(vector<float>());
                        Tracker[Tracker.size()-1].push_back(fcuc);
                        Tracker[Tracker.size()-1].push_back(w);
                        Tracker[Tracker.size()-1].push_back(o_costs[w]);
                        Tracker[Tracker.size()-1].push_back(CalculateFacilityCosts(FacilityList[w], NodeList));
                        ExchangedRequests.push_back(solution[w]);
                    }
                }
            }

			if (CalculateTotalCosts(FacilityList, NodeList) < curcosts - 0.0001) {
				curcosts = CalculateTotalCosts(FacilityList, NodeList);
				SolutionFacilityList = FacilityList;
				blacklist.clear();
			}
			else {
				blacklist.push_back(c_Route);
				FacilityList = CopyFacilityList;
			};

			//cout << " costs this iteration " << CalculateTotalCosts(FacilityList, NodeList) << endl;

			//if (solution[fcuc].size() == c_Route.size()) {
			//	NoImprovement += 1;
			//	blacklist.push_back(c_Route);
			//}
			//else {
			//	blacklist.clear();
			//}

//            for (int ct = 1; ct <= 100; ct++) {
//                float c = ct;
//                float p = ct - 1;
//                float countf = Seen.size();
//                float nls = NodeList.size() - 1;
//                float percseen = countf / nls;
//                float curperc = p / nls;
//                float prevperc = c / nls;
//                if (percseen <= c / 100 && percseen > p / 100) {
//                    CostTracker[ct] = CalculateTotalCosts(FacilityList, NodeList);
//                }
//            }

			cout << blacklist.size()<< endl;
			//cout << "No improvement " << NoImprovement << endl;

			if (blacklist.size() >= 9 || iteration > 36) {
				Complete = true;
			}

			cout << "Total Costs: " << CalculateTotalCosts(FacilityList, NodeList) << endl;

			if (Writefile == true) {
				//FacilityList[i].RouteList.erase(FacilityList[i].RouteList.begin() + j);
				stringstream Titel;
				Titel << "Combi veiling van route stap " << GVstep << " ";
				Titel << c_Route[0];
				for (int rtitel = 1; rtitel < c_Route.size(); rtitel++) {
					Titel << "-" << c_Route[rtitel];
				}
				WriteGraphToFile(FacilityList, NodeList, myfile, Titel.str());
				WriteRouteToFile(FacilityList, NodeList, myfile);
				WriteNewPage(myfile);
				GVstep = GVstep + 1;
			}

			fcuc = fcuc + 1;
			if (fcuc > FacilityList.size() - 1) { fcuc = 0; };
		}
	};

	return SolutionFacilityList;
}

std::vector<vector<int>> c_Auction::r_Auction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int orgf, vector<int> route) {
	vector<vector<vector<int>>> CheckRoute;
	vector<vector<float>> CheckCosts;
	vector<vector<int>> tempsolution;
	vector<vector<int>> solution;
	vector<vector<int>> orgsolution;
	std::vector< FacilityClass >  CopyFacilityList = FacilityList;

	CheckRoute.resize(FacilityList.size());
	CheckCosts.resize(FacilityList.size());
	tempsolution.resize(FacilityList.size());
	orgsolution.resize(FacilityList.size());
	for (int rc = 0; rc < route.size(); rc++) {
		orgsolution[orgf].push_back(route[rc]);
	};

	vector<int> Node;
	float addedcost = 0;
	float standardcosts = CalculateTotalCosts(FacilityList, NodeList);
	float newcosts = 0;
	float Maxcosts = floatbigM;
	float testcosts;
	bool Done = false;
	bool Found = false;
	bool Added = false;
	for (int i = 0; i < route.size(); i++) {
		Node.push_back(0);
	}
	Node[0] = -1;

	while (Done == false) {
		tempsolution.clear();
		tempsolution.resize(FacilityList.size());
		Node[0] = Node [0] + 1;
		for (int i = 0; i < route.size() - 1; i++) {
			if (Node[i] > FacilityList.size() - 1) {
				Node[i] = 0;
				Node[i + 1] = Node[i + 1] + 1;
			}
		};

		if (Node[route.size() - 1] == FacilityList.size() - 1) {
			Done = true;
		};

		for (int i = 0; i < route.size(); i++) {
			tempsolution[Node[i]].push_back(route[i]);
		};

		addedcost = 0;

		for (int fl = 0; fl < FacilityList.size(); fl++) {
			Found = false;
			for (int cr = 0; cr < CheckRoute[fl].size(); cr++) {
				if (CheckRoute[fl][cr] == tempsolution[fl]) {
					addedcost = addedcost + CheckCosts[fl][cr];
					Found = true;
				}
			}
			if (Found == false) {
				CopyFacilityList = FacilityList;
				for (int j = 0; j < tempsolution[fl].size(); j++) {
					CopyFacilityList[fl].ServiceNodes.push_back(tempsolution[fl][j]);
				}
				CopyFacilityList[fl] = ClarkeWright(NodeList, CopyFacilityList[fl]);
				newcosts = CalculateTotalCosts(CopyFacilityList, NodeList);
				addedcost = addedcost + (newcosts - standardcosts);
				CheckRoute[fl].push_back(tempsolution[fl]);
				CheckCosts[fl].push_back(newcosts - standardcosts);
			}
		};

		if (addedcost < Maxcosts) {
			Maxcosts = addedcost;
			solution = tempsolution;
		}
	};

	CopyFacilityList = FacilityList;

	for (int rc = 0; rc < route.size(); rc++) {
		CopyFacilityList[orgf].ServiceNodes.push_back(route[rc]);
	};

	for (int w = 0; w < CopyFacilityList.size(); w++) {
		CopyFacilityList[w] = ClarkeWright(NodeList, CopyFacilityList[w]);
	};

	testcosts = CalculateTotalCosts(CopyFacilityList, NodeList);

	CopyFacilityList = FacilityList;

	for (int w = 0; w < CopyFacilityList.size(); w++) {
		for (int j = 0; j < solution[w].size(); j++) {
			CopyFacilityList[w].ServiceNodes.push_back(solution[w][j]);
		};
		CopyFacilityList[w] = ClarkeWright(NodeList, CopyFacilityList[w]);
	};

	if (testcosts < CalculateTotalCosts(CopyFacilityList, NodeList)) {
		solution = orgsolution;
	}

	return solution;
}

std::vector< FacilityClass > c_Auction::ConcurrentAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList) {
	


	return FacilityList;
}

std::vector< FacilityClass > c_Auction::OptimalSolution(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList) {

    FacilityList = GurobiMDVRP(NodeList, FacilityList);
    
    cout << CalculateTotalCosts(FacilityList, NodeList) << endl;

	return FacilityList;
};


std::vector<std::array<float, 3>> c_Auction::VFacilityToNode(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	std::array<float, 3> NodeCosts;
	std::vector<std::array<float, 3>> CostN;
	float TempDist;
	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		TempDist = NodeDistance(Facility.LocationX, Facility.LocationY, NodeList[Facility.ServiceNodes[i]].LocationX, NodeList[Facility.ServiceNodes[i]].LocationY);
		NodeCosts[0] = i;
		NodeCosts[1] = TempDist;
		NodeCosts[2] = Facility.ServiceNodes[i];
		CostN.push_back(NodeCosts);
	};
	return CostN;
}

std::vector<std::array<float, 3>> c_Auction::VNextAndPrevious(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	std::array<float, 3> NodeCosts;
	std::vector<std::array<float, 3>> CostN;
	float TempDist;

	Facility = ClarkeWright(NodeList, Facility);

	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		for (int j = 0; j < Facility.RouteList.size(); j++) {
			for (int r = 0; r < Facility.RouteList[j].size(); r++) {
				if (Facility.RouteList[j][r] == Facility.ServiceNodes[i]) {
					if (r == 0) {
						TempDist = NodeDistance(Facility.LocationX, Facility.LocationY, NodeList[Facility.RouteList[j][r]].LocationX, NodeList[Facility.RouteList[j][r]].LocationY) + NodeDistance(NodeList[Facility.RouteList[j][r]].LocationX, NodeList[Facility.RouteList[j][r]].LocationY, NodeList[Facility.RouteList[j][r + 1]].LocationX, NodeList[Facility.RouteList[j][r + 1]].LocationY);
					}
					if (r > 0 && r < Facility.RouteList[j].size() - 1) {
						TempDist = NodeDistance(NodeList[Facility.RouteList[j][r]].LocationX, NodeList[Facility.RouteList[j][r]].LocationY, NodeList[Facility.RouteList[j][r - 1]].LocationX, NodeList[Facility.RouteList[j][r - 1]].LocationY) + NodeDistance(NodeList[Facility.RouteList[j][r]].LocationX, NodeList[Facility.RouteList[j][r]].LocationY, NodeList[Facility.RouteList[j][r + 1]].LocationX, NodeList[Facility.RouteList[j][r + 1]].LocationY);
					}
					if (r == Facility.RouteList[j].size() - 1) {
						TempDist = NodeDistance(NodeList[Facility.RouteList[j][r]].LocationX, NodeList[Facility.RouteList[j][r]].LocationY, NodeList[Facility.RouteList[j][r - 1]].LocationX, NodeList[Facility.RouteList[j][r - 1]].LocationY) + NodeDistance(NodeList[Facility.RouteList[j][r]].LocationX, NodeList[Facility.RouteList[j][r]].LocationY, Facility.LocationX, Facility.LocationY);
					}
				}

			}
		};
		NodeCosts[0] = i;
		NodeCosts[1] = TempDist;
		NodeCosts[2] = Facility.ServiceNodes[i];
		CostN.push_back(NodeCosts);
	};
	return CostN;
}

std::vector<std::array<float, 3>> c_Auction::VSumToOthers(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	std::array<float, 3> NodeCosts;
	std::vector<std::array<float, 3>> CostN;
	float TempDist;
	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		TempDist = 0;
		for (int j = 0; j < Facility.ServiceNodes.size(); j++) {
			if (i != j) {
				TempDist = TempDist + NodeDistance(NodeList[Facility.ServiceNodes[j]].LocationX, NodeList[Facility.ServiceNodes[j]].LocationY, NodeList[Facility.ServiceNodes[i]].LocationX, NodeList[Facility.ServiceNodes[i]].LocationY);
			}
		};
		NodeCosts[0] = i;
		NodeCosts[1] = TempDist;
		NodeCosts[2] = Facility.ServiceNodes[i];
		CostN.push_back(NodeCosts);
	};
	return CostN;
}

std::vector<std::array<float, 3>> c_Auction::VClosestNode(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	std::array<float, 3> NodeCosts;
	std::vector<std::array<float, 3>> CostN;
	float TempDist = 999999;
	float TDist = 0;
	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		TempDist = 9999999999999999;
		for (int j = 0; j < Facility.ServiceNodes.size(); j++) {
			if (i != j) {
				TDist = NodeDistance(NodeList[Facility.ServiceNodes[j]].LocationX, NodeList[Facility.ServiceNodes[j]].LocationY, NodeList[Facility.ServiceNodes[i]].LocationX, NodeList[Facility.ServiceNodes[i]].LocationY);
			}
			if (TDist < TempDist) { TempDist = TDist; };
		};
		NodeCosts[0] = i;
		NodeCosts[1] = TempDist;
		NodeCosts[2] = Facility.ServiceNodes[i];
		CostN.push_back(NodeCosts);
	};
	return CostN;
}

std::vector<std::array<float, 3>> c_Auction::VDaganzo(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	std::array<float, 3> NodeCosts;
	std::vector<std::array<float, 3>> CostN;
	float TempDist = 0;
	float Daganzo1;
	float Daganzo2;
	float rho1;
	float rho2;

	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		for (int j = 0; j < Facility.ServiceNodes.size(); j++) {
			TempDist = TempDist + NodeDistance(Facility.LocationX, Facility.LocationY, NodeList[Facility.ServiceNodes[j]].LocationX, NodeList[Facility.ServiceNodes[j]].LocationY);
		};
		rho1 = TempDist / Facility.ServiceNodes.size();
		TempDist = 0;
		for (int j = 0; j < Facility.ServiceNodes.size(); j++) {
			if (i != j) {
				TempDist = TempDist + NodeDistance(Facility.LocationX, Facility.LocationY, NodeList[Facility.ServiceNodes[j]].LocationX, NodeList[Facility.ServiceNodes[j]].LocationY);
			}
		};
		rho2 = TempDist / Facility.ServiceNodes.size() - 1;

		Daganzo1 = 2 * rho1 * ((1.1 / (Facility.ServiceNodes.size() / 5)) + 0.83 * sqrt(Facility.ServiceNodes.size())) * Facility.ServiceNodes.size();
		Daganzo2 = 2 * rho2 * ((1.1 / (Facility.ServiceNodes.size() - 1 / 5)) + 0.83 * sqrt(Facility.ServiceNodes.size() - 1)) * Facility.ServiceNodes.size() - 1;


		NodeCosts[0] = i;
		NodeCosts[1] = Daganzo1 - Daganzo2;
		NodeCosts[2] = Facility.ServiceNodes[i];
		CostN.push_back(NodeCosts);
	};
	return CostN;
}

std::vector<std::array<float, 3>> c_Auction::VCentroid(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	std::array<float, 3> NodeCosts;
	std::vector<std::array<float, 3>> CostN;

	float area;

	float Cx = 0;
	float Cy = 0;

	float Xi;
	float Xi1;
	float Yi1;
	float Yi;

	float TempDist = 0;

	Facility = ClarkeWright(NodeList, Facility);

	for (int r = 0; r < Facility.RouteList.size(); r++) {
		//Determine Centroid
		area = 0;
		for (int i = 0; i < Facility.RouteList[r].size(); i++) {
			Xi = NodeList[Facility.RouteList[r][i]].LocationX;
			Yi = NodeList[Facility.RouteList[r][i]].LocationY;
			if (i + 1 > Facility.RouteList[r].size() - 1) {
				Xi1 = Facility.LocationX;
				Yi1 = Facility.LocationY;
			}
			else {
				Xi1 = NodeList[Facility.RouteList[r][i + 1]].LocationX;
				Yi1 = NodeList[Facility.RouteList[r][i + 1]].LocationY;
			}
			area = area + 0.5 * ((Xi * Yi1) - (Xi1 * Yi));
		};

		for (int i = 0; i < Facility.RouteList[r].size(); i++) {
			Xi = NodeList[Facility.RouteList[r][i]].LocationX;
			Yi = NodeList[Facility.RouteList[r][i]].LocationY;
			if (i + 1 > Facility.RouteList[r].size() - 1) {
				Xi1 = Facility.LocationX;
				Yi1 = Facility.LocationY;
			}
			else {
				Xi1 = NodeList[Facility.RouteList[r][i + 1]].LocationX;
				Yi1 = NodeList[Facility.RouteList[r][i + 1]].LocationY;
			}
			Cx = Cx + ((Xi + Xi1) * ((Xi * Yi1) - (Xi1 * Yi)));
		};

		for (int i = 0; i < Facility.RouteList[r].size(); i++) {
			Xi = NodeList[Facility.RouteList[r][i]].LocationX;
			Yi = NodeList[Facility.RouteList[r][i]].LocationY;
			if (i + 1 > Facility.RouteList[r].size() - 1) {
				Xi1 = Facility.LocationX;
				Yi1 = Facility.LocationY;
			}
			else {
				Xi1 = NodeList[Facility.RouteList[r][i + 1]].LocationX;
				Yi1 = NodeList[Facility.RouteList[r][i + 1]].LocationY;
			}
			Cy = Cy + ((Yi + Yi1) * ((Xi * Yi1) - (Xi1 * Yi)));
		};

		Cx = Cx / (6 * area);
		Cy = Cy / (6 * area);

		for (int i = 0; i < Facility.RouteList[r].size(); i++) {
			TempDist = NodeDistance(Cx, Cy, NodeList[Facility.RouteList[r][i]].LocationX, NodeList[Facility.RouteList[r][i]].LocationY);
			NodeCosts[0] = i;
			NodeCosts[1] = TempDist;
			NodeCosts[2] = Facility.RouteList[r][i];
			CostN.push_back(NodeCosts);
		}
	}

	return CostN;
}

std::vector<std::array<float, 3>> c_Auction::VRandomizedCW(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	float RouteLength = 0;
	float RouteLengthCopy = 0;
	FacilityClass CopyFacility = Facility;
	std::array<float, 3> NodeCosts;
	std::vector<std::array<float, 3>> CostsList;

	Facility = ClarkeWright(NodeList, Facility);

	for (int n = 0; n < Facility.ServiceNodes.size(); n++) {
		RouteLength = 0;
		RouteLengthCopy = 0;
		CopyFacility = Facility;
		CopyFacility.ServiceNodes.erase(CopyFacility.ServiceNodes.begin() + n);
		CopyFacility = ClarkeWright(NodeList, CopyFacility);
		for (int j = 0; j < CopyFacility.RouteList.size(); j++) {
			RouteLength = RouteLength + CalculateRouteLength(NodeList, CopyFacility, j);
		};
		for (int j = 0; j < Facility.RouteList.size(); j++) {
			RouteLengthCopy = RouteLengthCopy + CalculateRouteLength(NodeList, Facility, j);
		};
		NodeCosts[0] = n;
		NodeCosts[1] = RouteLengthCopy - RouteLength;
		NodeCosts[2] = Facility.ServiceNodes[n];
		CostsList.push_back(NodeCosts);
	};

	return CostsList;
}

bool In(int node, vector<int> subset) {
	bool s = false;
	for (int i = 0; i < subset.size(); i++) {
		if (node == subset[i]) { s = true; };
	}
	return s;
}

double avg(vector<double> mc, vector<int> subset) {
	double sum = 0;
	double solution;
	int amount = subset.size();
	for (int i = 0; i < subset.size(); i++) {
		sum += mc[i];
	};
	solution = sum / amount;
	return solution;
}

std::vector< FacilityClass > c_Auction::ClusterAuction(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList) {
	vector< FacilityClass > CopyFacilityList = FacilityList;
	vector< FacilityClass > PrevFacilityList = FacilityList;
	vector<double> mc;
	vector<int> route;
	vector<int> t_cluster;
	vector<vector<int>> clusters;
	double Sol = 0;
	double SolmC = 0;
	int nn, nm, n;
	int tr_it;
	int f_it;
	int ex_c = 0;
	int end;
	int Lowestbidder;
	bool Done;
	vector<int> subsetU;
	vector<int> subsetS;
	double BidPrice, Lowestbid;
	vector<double> BidList;
	double bigM = 999999999999999999;

	bool Complete = false;

	int ctr = 0;


	f_it = 0;
	while (Complete == false) {
		PrevFacilityList = FacilityList;
		clusters.clear();
		ctr = ctr + 1;
		for (int rl_it = 0; rl_it < FacilityList[f_it].RouteList.size(); rl_it++) { //for all routes
			//Determine marginal costs for all requests on the route.
			route = FacilityList[f_it].RouteList[rl_it];
			for (int r_it = 0; r_it < FacilityList[f_it].RouteList[rl_it].size(); r_it++) { // define marginal costs
				mc.resize(FacilityList[f_it].RouteList[rl_it].size());
				end = (int)FacilityList[f_it].RouteList[rl_it].size() - 1;
				if (r_it == 0)
				{
					n = FacilityList[f_it].RouteList[rl_it][r_it];
					nn = FacilityList[f_it].RouteList[rl_it][r_it + 1];
                    mc[r_it] = NodeDistance(f_it, true, n, false, (int)NodeList.size()) + NodeDistance(n, false, nn, false, (int)NodeList.size()) - NodeDistance(f_it, true, nn, false, (int)NodeList.size());
				}
				else if (r_it == end)
				{
					nm = FacilityList[f_it].RouteList[rl_it][r_it - 1];
					n = FacilityList[f_it].RouteList[rl_it][r_it];
					mc[r_it] = NodeDistance(n, false, f_it, true, (int)NodeList.size()) + NodeDistance(nm, false, n, false, (int)NodeList.size()) - NodeDistance(nn, false, f_it, true, (int)NodeList.size());
				}
				else
				{
					nm = FacilityList[f_it].RouteList[rl_it][r_it - 1];
					n = FacilityList[f_it].RouteList[rl_it][r_it];
					nn = FacilityList[f_it].RouteList[rl_it][r_it + 1];
					mc[r_it] = NodeDistance(n, false, nn, false, (int)NodeList.size()) + NodeDistance(nm, false, n, false, (int)NodeList.size()) - NodeDistance(nm, false, nn, false, (int)NodeList.size());
				}
			};

			subsetS.clear();
			tr_it = 0;
			Done = false;

			while (tr_it < route.size() && Done == false) {
				subsetS.push_back(tr_it);
				subsetU.clear();
				for (int w = 0; w < route.size(); w++) {
					if (!In(w, subsetS)) { subsetU.push_back(w); };
				};
				if (avg(mc, subsetS) / avg(mc, subsetU)  > 0.20) {
					if (subsetS.size() >= 2) { //Cluster Identified
						bool remove = false;
						for (int s = 0; s < subsetS.size() - 1; s++) {
							if (NodeList[FacilityList[f_it].RouteList[rl_it][subsetS[s]]].Service == false) { remove = true; }
							t_cluster.push_back(FacilityList[f_it].RouteList[rl_it][subsetS[s]]);
						}
						if (remove == false) { clusters.push_back(t_cluster); };
						t_cluster.clear();
					};
					//if (subsetS.size() > 5) { Done = true; }
					subsetS.clear();
				}
				tr_it += 1;
			}
		}

		if (clusters.size() > 0) {

			int d_cl = 0;

			float CurrentCosts = floatbigM;

			for (int c_it = 0; c_it < clusters.size(); c_it++) { //start auction
				CopyFacilityList = FacilityList;
				for (int tc_it = 0; tc_it < clusters[c_it].size(); tc_it++) { // erase nodes in the clusters from the serviced nodes;
					for (int f_it = 0; f_it < FacilityList.size(); f_it++) {
						int t_end = (int)FacilityList[f_it].ServiceNodes.size() - 1;
						for (int sn_it = 0; sn_it < t_end; sn_it++) {
							int asn_it = t_end - sn_it;
							if (CopyFacilityList[f_it].ServiceNodes[asn_it] == clusters[c_it][tc_it]) {
								CopyFacilityList[f_it].ServiceNodes.erase(CopyFacilityList[f_it].ServiceNodes.begin() + asn_it);
							}
						}
					}
				};
				if (CalculateFacilityCosts(CopyFacilityList[f_it], NodeList) < CurrentCosts) {
					d_cl = c_it;
					CurrentCosts = CalculateFacilityCosts(CopyFacilityList[f_it], NodeList);
				}
			}

			CopyFacilityList = FacilityList;
			for (int ff_it = 0; ff_it < CopyFacilityList.size(); ff_it++) { // determine the bidprice
				Sol = 0;
				SolmC = 0;
				for (int j = 0; j < CopyFacilityList[ff_it].RouteList.size(); j++) {
					Sol = Sol + CalculateRouteLength(NodeList, CopyFacilityList[ff_it], j);
				};
				for (int tc_it = 0; tc_it < clusters[d_cl].size(); tc_it++) {
					CopyFacilityList[f_it].ServiceNodes.push_back(clusters[d_cl][tc_it]);
				};
				CopyFacilityList[ff_it] = ClarkeWright(NodeList, CopyFacilityList[f_it]);
				for (int j = 0; j < CopyFacilityList[ff_it].RouteList.size(); j++) {
					SolmC = SolmC + CalculateRouteLength(NodeList, CopyFacilityList[ff_it], j);
				};
				BidPrice = SolmC - Sol;
				BidList.push_back(BidPrice);
			};
			Lowestbid = bigM;
			for (int Y = 0; Y < BidList.size(); Y++) {
				if (BidList[Y] < Lowestbid) {
					Lowestbid = BidList[Y];
					Lowestbidder = Y;
				}
			};

			ctr = 0;

			if (Lowestbidder != f_it && clusters[d_cl].size() > 0) {
				Tracker.push_back(vector<float>());
				Tracker[Tracker.size() - 1].push_back(f_it);
				Tracker[Tracker.size() - 1].push_back(Lowestbidder);
				Tracker[Tracker.size() - 1].push_back(BidList[f_it]);
				Tracker[Tracker.size() - 1].push_back(BidList[Lowestbidder]);
				ExchangedRequests.push_back(clusters[d_cl]);
			}

			BidList.clear();

			for (int tc_it = 0; tc_it < clusters[d_cl].size(); tc_it++) {
				bool TDS = false;
				for (int ss = 0; ss < Seen.size(); ss++) {
					if (Seen[ss] == clusters[d_cl][tc_it]) {
						TDS = true;
					}
				};
				if (TDS == false) { Seen.push_back(clusters[d_cl][tc_it]); };
				FacilityList[Lowestbidder].ServiceNodes.push_back(clusters[d_cl][tc_it]);
			};
			FacilityList[Lowestbidder] = ClarkeWright(NodeList, FacilityList[Lowestbidder]);

			for (int ct = 1; ct <= 100; ct++) {
				float c = ct;
				float p = ct - 1;
				float countf = Seen.size();
				float nls = NodeList.size() - 1;
				float percseen = countf / nls;
				float curperc = p / nls;
				float prevperc = c / nls;
				if (percseen <= c / 100 && percseen > p / 100) {
					CostTracker[ct] = CalculateTotalCosts(FacilityList, NodeList);
				}
			}

			if (CalculateTotalCosts(FacilityList, NodeList) > CalculateTotalCosts(PrevFacilityList, NodeList)) {
				Complete = true;
				FacilityList = PrevFacilityList;
			}
		}

		if (ctr > FacilityList.size()) {
			Complete = true;
		}

		f_it = f_it + 1;
		if (f_it > FacilityList.size() - 1) { f_it = 0; };
        
        cout << CalculateTotalCosts(FacilityList, NodeList) << endl;
	};



	return FacilityList;
}


std::vector< FacilityClass > c_Auction::BRRA(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile) {
    vector<int> c_Route;
    vector<vector<int>> solution;
    vector<vector<int>> sol_no_change;
	vector<vector<int>> sol_no_change_prev;
    vector<vector<int>> blacklist;
    bool Complete = false;
    int fcuc = 0;
    std::vector<std::array<float, 3>> CostsList;
	std::vector< FacilityClass > PrevFacilityList;
    std::vector< FacilityClass > SolutionFacilityList;
    std::vector< FacilityClass > CopyFacilityList;
    float curcosts = CalculateTotalCosts(FacilityList, NodeList);
    std::array<float, 3> NodeCosts;
    float RouteLength = 0;
    float RouteLengthCopy = 0;


	vector<float> o_costs;

    
    while (Complete == false) {

		PrevFacilityList = FacilityList;

		o_costs.clear();
		for (int i = 0; i < FacilityList.size(); i++) {
			o_costs.push_back(CalculateFacilityCosts(FacilityList[i], NodeList));
		}

		c_Route.clear();
        
        for(int fc = 0; fc < FacilityList.size(); fc++) {
            CostsList.clear();
            for (int n = 0; n < FacilityList[fc].ServiceNodes.size(); n++) {
                RouteLength = 0;
                RouteLengthCopy = 0;
                CopyFacilityList = FacilityList;
                CopyFacilityList[fc].ServiceNodes.erase(CopyFacilityList[fc].ServiceNodes.begin() + n);
                CopyFacilityList[fc] = ClarkeWright(NodeList, CopyFacilityList[fc]);
                for (int j = 0; j < CopyFacilityList[fc].RouteList.size(); j++) {
                    RouteLength = RouteLength + CalculateRouteLength(NodeList, CopyFacilityList[fc], j);
                };
                for (int j = 0; j < FacilityList[fc].RouteList.size(); j++) {
                    RouteLengthCopy = RouteLengthCopy + CalculateRouteLength(NodeList, FacilityList[fc], j);
                };
                NodeCosts[0] = n;
                NodeCosts[1] = RouteLengthCopy - RouteLength;
                NodeCosts[2] = FacilityList[fc].ServiceNodes[n];
                CostsList.push_back(NodeCosts);
            };

			int i_req = 0;
			bool nonblocked;

			if (NodeList[FacilityList[fc].ServiceNodes[CostsList[0][0]]].Service == true) {
				nonblocked = true;
			}
			else {
				nonblocked = false;
			}
			while (nonblocked == false) {
				i_req = i_req + 1;
				int Test = (int)CostsList[i_req][0];
				if (NodeList[FacilityList[fc].ServiceNodes[Test]].Service == true) {
					nonblocked = true;
				}
				else {
					nonblocked = false;
				}
			}
            
            struct { bool operator()(std::array<float, 3> i, std::array<float, 3> j) { return i[1] > j[1]; } } SortMarginalCosts;
            
            std::sort(CostsList.begin(), CostsList.end(), SortMarginalCosts);
            
            c_Route.push_back(CostsList[i_req][2]);
            
            FacilityList[fc].ServiceNodes.erase(FacilityList[fc].ServiceNodes.begin() + CostsList[i_req][0]);
            FacilityList[fc] = ClarkeWright(NodeList, FacilityList[fc]);
        };
        
        r_Check.clear();
        
        solution = r_Auction(FacilityList, NodeList, fcuc, c_Route);
        
        for (int w = 0; w < FacilityList.size(); w++) {
            for (int j = 0; j < solution[w].size(); j++) {
                FacilityList[w].ServiceNodes.push_back(solution[w][j]);
            };
            FacilityList[w] = ClarkeWright(NodeList, FacilityList[w]);
        };
        
        if (CalculateTotalCosts(FacilityList, NodeList) < curcosts - 0.0001) {
            curcosts = CalculateTotalCosts(FacilityList, NodeList);
            SolutionFacilityList = FacilityList;
        }

		for (int w = 0; w < FacilityList.size(); w++) {
			if (w != fcuc) {
				if (c_Route.size() > 0) {
					Tracker.push_back(vector<float>());
					Tracker[Tracker.size() - 1].push_back(fcuc);
					Tracker[Tracker.size() - 1].push_back(w);
					Tracker[Tracker.size() - 1].push_back(o_costs[w]);
					Tracker[Tracker.size() - 1].push_back(CalculateFacilityCosts(FacilityList[w], NodeList));
					ExchangedRequests.push_back(solution[w]);
				}
			}
		}

		if (CalculateTotalCosts(FacilityList, NodeList) > CalculateTotalCosts(PrevFacilityList, NodeList)) {
			Complete = true;
			SolutionFacilityList = PrevFacilityList;
		}
    };
    
    return SolutionFacilityList;
}




string c_Auction::WriteFacilityToXML(std::vector< FacilityClass > OriginalFacilityList, std::vector< FacilityClass > OptimalFacilityList, std::vector< FacilityClass > AuctionFacilityList, std::vector< NodeClass > NodeList, string type, int iterator) {
    
    string temp;
    
    vector<string> writelist;
    writelist.reserve(10000);
    
    rapidxml::xml_document<> doc;


    
    rapidxml::xml_node<> * root;
    // Read the xml file into a vector
    ifstream theFile ("test.xml");

	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);

	root = doc.first_node("Results");
    
    rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
    decl->append_attribute(doc.allocate_attribute("version", "1.0"));
    decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
    doc.append_node(decl);
    
    if (root == NULL) {
        root = doc.allocate_node(rapidxml::node_element, "Results");
        root->append_attribute(doc.allocate_attribute("type", type.c_str()));
        doc.append_node(root);
    }
    
    string it = to_string(iterator);
    rapidxml::xml_node<>* instance = doc.allocate_node(rapidxml::node_element, "Instance");
    instance->append_attribute(doc.allocate_attribute("id", it.c_str()));
    root->append_node(instance);
    
    rapidxml::xml_node<>* facilities = doc.allocate_node(rapidxml::node_element, "Facilities");
    instance->append_node(facilities);
    
    for (int i = 0; i < OriginalFacilityList.size(); i++) {
        rapidxml::xml_node<>* facility = doc.allocate_node(rapidxml::node_element, "Facility");
        writelist.push_back(to_string(i));
        facility->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(OriginalFacilityList[i].LocationX));
        facility->append_attribute(doc.allocate_attribute("x", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(OriginalFacilityList[i].LocationY));
        facility->append_attribute(doc.allocate_attribute("y", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(VehicleCapacity));
        facility->append_attribute(doc.allocate_attribute("cap", writelist[writelist.size()-1].c_str()));
        facilities->append_node(facility);
    }
    
    rapidxml::xml_node<>* nodes = doc.allocate_node(rapidxml::node_element, "Requests");
    instance->append_node(nodes);
    
    for (int i = 1; i < NodeList.size(); i++) {
        rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "Request");
        writelist.push_back(to_string(i));
        node->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(NodeList[i].LocationX));
        node->append_attribute(doc.allocate_attribute("x", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(NodeList[i].LocationY));
        node->append_attribute(doc.allocate_attribute("y", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(NodeList[i].Demand));
        node->append_attribute(doc.allocate_attribute("demand", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(NodeList[i].StartTW));
        node->append_attribute(doc.allocate_attribute("twstart", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(NodeList[i].EndTW));
        node->append_attribute(doc.allocate_attribute("twend", writelist[writelist.size()-1].c_str()));
        nodes->append_node(node);
    }
    
    rapidxml::xml_node<>* originalprice = doc.allocate_node(rapidxml::node_element, "Costs");
    instance->append_node(originalprice);
    writelist.push_back(to_string(CalculateTotalCosts(OriginalFacilityList, NodeList)));
    originalprice->append_attribute(doc.allocate_attribute("originalcost", writelist[writelist.size()-1].c_str()));
    writelist.push_back(to_string(CalculateTotalCosts(OptimalFacilityList, NodeList)));
    originalprice->append_attribute(doc.allocate_attribute("optimalcost", writelist[writelist.size()-1].c_str()));
    writelist.push_back(to_string(CalculateTotalCosts(AuctionFacilityList, NodeList)));
    originalprice->append_attribute(doc.allocate_attribute("postauction", writelist[writelist.size()-1].c_str()));
    writelist.push_back(to_string(Seen.size()));
    originalprice->append_attribute(doc.allocate_attribute("numberseen", writelist[writelist.size()-1].c_str()));
    
    rapidxml::xml_node<>* routes = doc.allocate_node(rapidxml::node_element, "PriorRoutes");
    instance->append_node(routes);
    
    for (int i = 0; i < OriginalFacilityList.size(); i++) {
        rapidxml::xml_node<>* facility = doc.allocate_node(rapidxml::node_element, "Facility");
        writelist.push_back(to_string(i));
        facility->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
        for (int j = 0; j < OriginalFacilityList[i].RouteList.size(); j++) {
            rapidxml::xml_node<>* route = doc.allocate_node(rapidxml::node_element, "Route");
            for (int k = 0; k < OriginalFacilityList[i].RouteList[j].size();k++) {
                rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "Node");
                writelist.push_back(to_string(OriginalFacilityList[i].RouteList[j][k]));
                node->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
                route->append_node(node);
            }
            facility->append_node(route);
        }
        routes->append_node(facility);
    }
    
    rapidxml::xml_node<>* optroutes = doc.allocate_node(rapidxml::node_element, "OptRoutes");
    instance->append_node(optroutes);
    
    for (int i = 0; i < OptimalFacilityList.size(); i++) {
        rapidxml::xml_node<>* facility = doc.allocate_node(rapidxml::node_element, "Facility");
        writelist.push_back(to_string(i));
        facility->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
        for (int j = 0; j < OptimalFacilityList[i].RouteList.size(); j++) {
            rapidxml::xml_node<>* route = doc.allocate_node(rapidxml::node_element, "Route");
            for (int k = 0; k < OptimalFacilityList[i].RouteList[j].size();k++) {
                rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "Node");
                writelist.push_back(to_string(OptimalFacilityList[i].RouteList[j][k]));
                node->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
                route->append_node(node);
            }
            facility->append_node(route);
        }
        optroutes->append_node(facility);
    }
    
    rapidxml::xml_node<>* transfers = doc.allocate_node(rapidxml::node_element, "Transfers");
    instance->append_node(transfers);
    
    for (int i = 0; i < Tracker.size(); i++) {
        rapidxml::xml_node<>* transfer = doc.allocate_node(rapidxml::node_element, "Transfer");
        writelist.push_back(to_string((int)Tracker[i][0]));
        transfer->append_attribute(doc.allocate_attribute("from", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string((int)Tracker[i][1]));
        transfer->append_attribute(doc.allocate_attribute("to", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(Tracker[i][2]));
        transfer->append_attribute(doc.allocate_attribute("cost", writelist[writelist.size()-1].c_str()));
        writelist.push_back(to_string(Tracker[i][3]));
        transfer->append_attribute(doc.allocate_attribute("bid", writelist[writelist.size()-1].c_str()));
        transfers->append_node(transfer);
        for (int w = 0; w < ExchangedRequests[i].size(); w++) {
            rapidxml::xml_node<>* child = doc.allocate_node(rapidxml::node_element, "Request");
            writelist.push_back(to_string(ExchangedRequests[i][w]));
            child->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
            transfer->append_node(child);
        }
    }
    
    rapidxml::xml_node<>* exchange = doc.allocate_node(rapidxml::node_element, "Exchanges");
    instance->append_node(exchange);
    
    for (int i = 0; i < OriginalFacilityList.size(); i++) {
        for (int j = 0; j < OriginalFacilityList[i].ServiceNodes.size(); j++) {
            for (int w = 0; w < AuctionFacilityList.size(); w++) {
                for (int k = 0; k < AuctionFacilityList[w].ServiceNodes.size(); k++) {
                    if (OriginalFacilityList[i].ServiceNodes[j] == AuctionFacilityList[w].ServiceNodes[k] && i != w) {
                        rapidxml::xml_node<>* child = doc.allocate_node(rapidxml::node_element, "Exchange");
                        writelist.push_back(to_string(i));
                        child->append_attribute(doc.allocate_attribute("from", writelist[writelist.size()-1].c_str()));
                        writelist.push_back(to_string(w));
                        child->append_attribute(doc.allocate_attribute("to", writelist[writelist.size()-1].c_str()));
                        writelist.push_back(to_string(OriginalFacilityList[i].ServiceNodes[j]));
                        child->append_attribute(doc.allocate_attribute("request", writelist[writelist.size()-1].c_str()));
                        exchange->append_node(child);
                    }
                }
            }
        }
    }
    
    rapidxml::xml_node<>* postroutes = doc.allocate_node(rapidxml::node_element, "PostRoutes");
    instance->append_node(postroutes);
    
    for (int i = 0; i < AuctionFacilityList.size(); i++) {
        rapidxml::xml_node<>* facility = doc.allocate_node(rapidxml::node_element, "Facility");
        writelist.push_back(to_string(i));
        facility->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
        for (int j = 0; j < AuctionFacilityList[i].RouteList.size(); j++) {
            rapidxml::xml_node<>* route = doc.allocate_node(rapidxml::node_element, "Route");
            for (int k = 0; k < AuctionFacilityList[i].RouteList[j].size();k++) {
                rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "Node");
                writelist.push_back(to_string(AuctionFacilityList[i].RouteList[j][k]));
                node->append_attribute(doc.allocate_attribute("id", writelist[writelist.size()-1].c_str()));
                route->append_node(node);
            }
            facility->append_node(route);
        }
        postroutes->append_node(facility);
    }
    
    ofstream myXML;
    stringstream ss;
    ss << "test.xml";
    string Title = ss.str();
    myXML.open(Title);
    
    myXML << doc;
    
    myXML.close();
    
    stringstream t_s;
    t_s << doc;
    
    //std::cout << doc;
    return t_s.str();
}
