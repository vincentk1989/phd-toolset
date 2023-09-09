//# include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"

c_DistanceMatrix::c_DistanceMatrix() {

};


float c_DistanceMatrix::NodeDistance(float OriginX, float OriginY, float DestinationX, float DestinationY) {
	float DistanceX = DestinationX - OriginX;
	float DistanceY = DestinationY - OriginY;
	float Solution = sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
	return Solution;
};

float c_DistanceMatrix::NodeDistance(int i, bool fi, int j, bool fii, int ListCount) {
	int x;
	int y;

	if (fi == true) {
		x = i + ListCount;
	}
	else {
		x = i;
	}

	if (fii == true) {
		y = j + ListCount;
	}
	else {
		y = j;
	}

	return DistanceMatrix[x][y];
}

float c_DistanceMatrix::CalculateRouteLength(std::vector< NodeClass > NodeList, FacilityClass Facility, int RouteNr) {
    float RouteLength = 0;
    if (Facility.RouteList[RouteNr].size() > 0) {
        int NodeNumber = Facility.RouteList[RouteNr][0];
        RouteLength = NodeDistance(Facility.Number, true, Facility.RouteList[RouteNr][0], false, NodeList.size());
        int PrevNode = Facility.RouteList[RouteNr][0];
        int Size = Facility.RouteList[RouteNr].size() - 1;
        for (int i = 0; i < Size; i++) {
            NodeNumber = Facility.RouteList[RouteNr][i + 1];
            PrevNode = Facility.RouteList[RouteNr][i];
            RouteLength = RouteLength + NodeDistance(PrevNode, false, NodeNumber, false, NodeList.size());
        }
        NodeNumber = Facility.RouteList[RouteNr][Size];
        RouteLength = RouteLength + NodeDistance(Facility.Number, true, Facility.RouteList[RouteNr][Size], false, NodeList.size());
        
    }
    return RouteLength;
};

float c_DistanceMatrix::AltRouteLength(std::vector< NodeClass > NodeList, FacilityClass Facility, std::vector<int> Route) {
    float RouteLength = 0;
    if (Route.size() > 0) {
        int NodeNumber = Route[0];
        RouteLength = NodeDistance(Facility.Number, true, Route[0], false, NodeList.size());
        int PrevNode = Route[0];
        int Size = Route.size() - 1;
        for (int i = 0; i < Size; i++) {
            NodeNumber = Route[i + 1];
            PrevNode = Route[i];
            RouteLength = RouteLength + NodeDistance(PrevNode, false, NodeNumber, false, NodeList.size());
        }
        NodeNumber = Route[Size];
        RouteLength = RouteLength + NodeDistance(Facility.Number, true, Route[Size], false, NodeList.size());
        
    }
    return RouteLength;
};

float c_DistanceMatrix::CalculateRouteDemand(std::vector< NodeClass > NodeList, std::vector<int> RouteList) {
	float Td = 0;
	int Size = RouteList.size();
	for (int i = 0; i < Size; i++) {
		int NodeNumber = RouteList[i];
		Td = Td + NodeList[NodeNumber].Demand;
	}
	return Td;
};

float c_DistanceMatrix::CalculateRouteDemandwP(std::vector< NodeClass > NodeList, std::vector<int> RouteList) {
    float Td = 0;
    int Size = RouteList.size();
    for (int i = 0; i < Size; i++) {
        int NodeNumber = RouteList[i];
        Td = Td + (NodeList[NodeNumber].Demand * NodeList[NodeNumber].Probability);
    }
    return Td;
};

float c_DistanceMatrix::WorstCaseCalculateRouteDemand(std::vector< NodeClass > NodeList, std::vector<int> RouteList) {
    float Td = 0;
    int Size = RouteList.size();
    for (int i = 0; i < Size; i++) {
        int NodeNumber = RouteList[i];
        
        int which = 2;
        double p = confidence;
        double q = 1-p;
        double X;
        double mean = NodeList[NodeNumber].Mean;
        double stdev = NodeList[NodeNumber].Stdev;
        int status;
        double bound;
        
        //cdfnor(&which,&p,&q,&X,&mean,&stdev,&status,&bound);
        
        if ( NodeNumber < Certain) {
            Td = Td + NodeList[NodeNumber].Mean;
        } else {
            Td = Td + X;
        }
    }
    return Td;
};

float c_DistanceMatrix::CalculateMinimumDistances(int n, FacilityClass Facility, std::vector< NodeClass > NodeList) {
	float SumDistances = 0;
	float Dist = 0;
	for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
		if (i != n) {
			Dist = NodeDistance(NodeList[Facility.ServiceNodes[i]].LocationX, NodeList[Facility.ServiceNodes[i]].LocationY, NodeList[Facility.ServiceNodes[n]].LocationX, NodeList[Facility.ServiceNodes[n]].LocationY);
			if (Dist < SumDistances) {
				SumDistances = Dist;
			}
		}
	};
	return SumDistances;
};

float c_DistanceMatrix::CalculateTotalCosts(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList) {
	float TotalCosts = 0;
	for (int i = 0; i < FacilityList.size(); i++) {
		for (int j = 0; j < FacilityList[i].RouteList.size(); j++) {
			TotalCosts = TotalCosts + CalculateRouteLength(NodeList, FacilityList[i], j);
		}
	};
	return TotalCosts;
};

float c_DistanceMatrix::CalculateFacilityCosts(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	float TotalCosts = 0;
	for (int j = 0; j < Facility.RouteList.size(); j++) {
		TotalCosts = TotalCosts + CalculateRouteLength(NodeList, Facility, j);
	};
	return TotalCosts;
};

vector<vector<float>> c_DistanceMatrix::GenerateDM(std::vector< NodeClass > NodeList, std::vector< FacilityClass > FacilityList) {
	int test = NodeList.size() + FacilityList.size();
	int f;
	int fi;
	vector<vector<float>> DM(test, vector<float>(test));

	for (int i = 0; i < NodeList.size(); i++) {
		for (int j = i; j < NodeList.size(); j++) {
			if (j == i) {
				DM[i][j] = 0;
			}
			else {
				DM[i][j] = NodeDistance(NodeList[i].LocationX, NodeList[i].LocationY, NodeList[j].LocationX, NodeList[j].LocationY);
				DM[j][i] = DM[i][j];
			};
		}
	};

	for (int i = NodeList.size(); i < test; i++) {
		for (int j = 0; j < NodeList.size(); j++) {
			if (j == i) {
				//DM[i][j] = 0;
			}
			else {
				f = i - NodeList.size();
				DM[i][j] = NodeDistance(FacilityList[f].LocationX, FacilityList[f].LocationY, NodeList[j].LocationX, NodeList[j].LocationY);
				DM[j][i] = DM[i][j];
			};
		}
	};

	for (int i = NodeList.size(); i < test; i++) {
		for (int j = NodeList.size(); j < test; j++) {
			if (j == i) {
				DM[i][j] = 0;
			}
			else {
				f = i - NodeList.size();
				fi = j - NodeList.size();
				DM[i][j] = NodeDistance(FacilityList[f].LocationX, FacilityList[f].LocationY, FacilityList[fi].LocationX, FacilityList[fi].LocationY);
				DM[j][i] = DM[i][j];
			};
		}
	};

	return DM;
};

//bool Check_capacity_spdp(float Capacity, vector<NodeClass> NodeList, vector<int> route) {
//    bool result = false;
//    float total_pickup = 0;
//    for (int i = 0; i < route.size(); i++) {
//        total_pickup = total_pickup + NodeList[route[i]].Pickup;
//    };
//    float total_deliver = 0;
//    for (int i = 0; i < route.size(); i++) {
//        total_deliver = total_deliver + NodeList[route[i]].Demand;
//    };
//    if (total_pickup <= Capacity && total_deliver <= Capacity) {
//        for (int i = 0; i < route.size() - 1; i++) {
//            float total_pickup_alt = 0;
//            for (int j = 0; j < i; j++) {
//                total_pickup_alt = total_pickup_alt + NodeList[route[j]].Pickup;
//            };
//            float total_deliver_alt = 0;
//            for (int j = i; j < route.size(); j++) {
//                total_deliver_alt = total_deliver_alt + NodeList[route[j]].Pickup;
//            };
//            if (total_pickup_alt + total_deliver_alt <= Capacity) {
//                result = true;
//            }
//        }
//    }
//    return result;
//}
