//# include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"
#include <curl/curl.h>
#include "tinyxml2.h"

using namespace tinyxml2;

c_DistanceMatrix::c_DistanceMatrix() {

};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size* nmemb);
    return size * nmemb;
}


float c_DistanceMatrix::NodeDistance(float OriginX, float OriginY, float DestinationX, float DestinationY) {
	float DistanceX = DestinationX - OriginX;
	float DistanceY = DestinationY - OriginY;
	float Solution = sqrt((DistanceX * DistanceX) + (DistanceY * DistanceY));
	return Solution;
};

float c_DistanceMatrix::RealNodeDistance(float x, float y, float x2, float y2, listCoordinates &drawRoute) {
    float distance = 0;
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    string url = "http://localhost:8989/route?point=" + std::to_string(x) + "%2C" + std::to_string(y) + "&point=" + std::to_string(x2) + "%2C" + std::to_string(y2) + "&type=gpx";
    string text;
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        text = readBuffer;
        
        //cout << text << endl;

        readBuffer.clear();
    }

    if (text != "") {

        XMLDocument doc;
        doc.Parse(text.c_str());

        XMLNode * xNode = doc.FirstChildElement("gpx")->FirstChildElement("metadata")->FirstChildElement();
        string test = xNode->Value();
        if (test != "extensions") {
            XMLNode * xNode = doc.FirstChildElement("gpx")->FirstChildElement("rte");
            XMLElement * TrackSegment = xNode->FirstChildElement("rtept");
            while (TrackSegment != NULL) {
                XMLElement * dist = TrackSegment->FirstChildElement("extensions")->FirstChildElement("gh:distance");
                distance = distance + stof(dist->GetText());
                TrackSegment = TrackSegment->NextSiblingElement();
            }
            
            bool done = false;
            
            XMLNode * trk = doc.FirstChildElement("gpx")->FirstChildElement("trk");
            XMLElement * trkseg = trk->FirstChildElement("trkseg");
            while (trkseg != NULL && done == false) {
                XMLElement * trkpt = trkseg->FirstChildElement("trkpt");
                while (trkpt != NULL) {
                    const char * lattext;
                    const char * lontext;
                    lattext = trkpt->Attribute("lat");
                    lontext = trkpt->Attribute("lon");
                    float latX = stof(lattext);
                    float lonY = stof(lontext);
                    coordinates partCoords;
                    partCoords.coordinateX = latX;
                    partCoords.coordinateY = lonY;
                    drawRoute.listOfCoordinates.push_back(partCoords);
                    trkpt = trkpt->NextSiblingElement();
                }
                done = true;
            }
        }
    }
    else {
        cout << "Niet gevonden bij coordinaat " << x << " " << y << " " << x2 << " " << y2 << endl;
    }
    
    if (distance < 100) {
        distance = 9999999999999999;
    }

    distance = distance / 1000;

    return distance;
};

void c_DistanceMatrix::exportCoordinates(vector<NodeClass> NodeList, FacilityClass Facility) {
    float distance = 0;
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    string url = "http://localhost:8989/route?point=";// + std::to_string(x) + "%2C" + std::to_string(y) + "&point=" + std::to_string(x2) + "%2C" + std::to_string(y2) + "&type=gpx";
    string text;
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        text = readBuffer;
        
        //cout << text << endl;

        readBuffer.clear();
    }

    if (text != "") {

        XMLDocument doc;
        doc.Parse(text.c_str());

        XMLNode * xNode = doc.FirstChildElement("gpx")->FirstChildElement("metadata")->FirstChildElement();
        string test = xNode->Value();
        if (test != "extensions") {
            XMLNode * xNode = doc.FirstChildElement("gpx")->FirstChildElement("rte");
            XMLElement * TrackSegment = xNode->FirstChildElement("rtept");
            while (TrackSegment != NULL) {
                XMLElement * dist = TrackSegment->FirstChildElement("extensions")->FirstChildElement("gh:distance");
                distance = distance + stof(dist->GetText());
                TrackSegment = TrackSegment->NextSiblingElement();
            }
        }
    }
    else {
        //cout << "Niet gevonden bij coordinaat " << x << " " << y << " " << x2 << " " << y2 << endl;
    }

    distance = distance / 1000;
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

float c_DistanceMatrix::CalculateRL(std::vector< NodeClass > NodeList, FacilityClass Facility, vector<int> route) {
    float RouteLength = 0;
    if (route.size() > 0) {
        int NodeNumber = route[0];
        RouteLength = NodeDistance(Facility.Number, true, route[0], false, NodeList.size());
        int PrevNode = route[0];
        int Size = route.size() - 1;
        for (int i = 0; i < Size; i++) {
            NodeNumber = route[i + 1];
            PrevNode = route[i];
            RouteLength = RouteLength + NodeDistance(PrevNode, false, NodeNumber, false, NodeList.size());
        }
        NodeNumber = route[Size];
        RouteLength = RouteLength + NodeDistance(Facility.Number, true, route[Size], false, NodeList.size());
        
    }
    return RouteLength;
};

float c_DistanceMatrix::AltRouteLength(std::vector< NodeClass > NodeList, FacilityClass Facility, std::vector<int> Route) {
    float RouteLength = 0;
    if (Route.size() > 0) {
        int NodeNumber = Route[0];
        RouteLength = RouteLength + NodeDistance(Facility.LocationX, Facility.LocationY, NodeList[Route[0]].LocationX, NodeList[Route[0]].LocationY);
        int PrevNode = Route[0];
        int Size = Route.size() - 1;
        for (int i = 0; i < Size; i++) {
            NodeNumber = Route[i + 1];
            PrevNode = Route[i];
            RouteLength = RouteLength + NodeDistance(NodeList[PrevNode].LocationX, NodeList[PrevNode].LocationY, NodeList[NodeNumber].LocationX, NodeList[NodeNumber].LocationY);
        }
        NodeNumber = Route[Size];
        RouteLength = RouteLength + NodeDistance(NodeList[NodeNumber].LocationX, NodeList[NodeNumber].LocationY, Facility.LocationX, Facility.LocationY);
        
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
			TotalCosts = TotalCosts + AltRouteLength(NodeList, FacilityList[i], FacilityList[i].RouteList[j]);
		}
	};
	return TotalCosts;
};

float c_DistanceMatrix::CalculateFacilityCosts(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	float TotalCosts = 0;
	for (int j = 0; j < Facility.RouteList.size(); j++) {
		TotalCosts = TotalCosts + AltRouteLength(NodeList, Facility, Facility.RouteList[j]);
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

vector<vector<float>> c_DistanceMatrix::GenerateRealDM(std::vector< NodeClass > NodeList, std::vector< FacilityClass > FacilityList) {
    int test = NodeList.size() + FacilityList.size();
    int f;
    int fi;
    DrawMatrix.resize(test);
    for (int i = 0; i < DrawMatrix.size(); i++) {
        DrawMatrix[i].resize(test);
    }
    vector<vector<float>> DM(test, vector<float>(test));
    for (int i = 0; i < DM.size(); i++) {
        for ( int j = 0; j < DM[i].size(); j++) {
            DM[i][j] = 9999999999999;
        }
    }

    for (int i = 1; i < NodeList.size(); i++) {
        for (int j = i; j < NodeList.size(); j++) {
            if (j == i) {
                DM[i][j] = 9999999999999;
            }
            else {
                listCoordinates drawRoute;
                DM[i][j] = RealNodeDistance(NodeList[i].LocationX, NodeList[i].LocationY, NodeList[j].LocationX, NodeList[j].LocationY, drawRoute);
                //DM[i][j] = RealNodeDistance(NodeList[i].LocationY, NodeList[i].LocationX, NodeList[j].LocationY, NodeList[j].LocationX, drawRoute);
                DrawMatrix[i][j] = drawRoute;
                DM[j][i] = DM[i][j];
                DrawMatrix[j][i] = DrawMatrix[i][j];
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
                listCoordinates drawRoute;
                DM[i][j] = RealNodeDistance(FacilityList[f].LocationX, FacilityList[f].LocationY, NodeList[j].LocationX, NodeList[j].LocationY, drawRoute);
                //DM[i][j] = RealNodeDistance(FacilityList[f].LocationY, FacilityList[f].LocationX, NodeList[j].LocationY, NodeList[j].LocationX, drawRoute);
                DrawMatrix[i][j] = drawRoute;
                DM[j][i] = DM[i][j];
                DrawMatrix[j][i] = DrawMatrix[i][j];
            };
        }
    };

    for (int i = NodeList.size(); i < test; i++) {
        for (int j = NodeList.size(); j < test; j++) {
            if (j == i) {
                DM[i][j] = 9999999999999;
            }
            else {
                f = i - NodeList.size();
                fi = j - NodeList.size();
                listCoordinates drawRoute;
                DM[i][j] = RealNodeDistance(FacilityList[f].LocationX, FacilityList[f].LocationY, FacilityList[fi].LocationX, FacilityList[fi].LocationY, drawRoute);
                //DM[i][j] = RealNodeDistance(FacilityList[f].LocationY, FacilityList[f].LocationX, FacilityList[fi].LocationY, FacilityList[fi].LocationX, drawRoute);
                DrawMatrix[i][j] = drawRoute;
                DM[j][i] = DM[i][j];
                DrawMatrix[j][i] = DrawMatrix[i][j];
            };
        }
    };

    return DM;
};

vector<vector<vector<double>>>  c_DistanceMatrix::GenerateCoordinates(vector<FacilityClass> FacilityList, vector<NodeClass> NodeList) {
    vector<vector<vector<double>>> coordinates;
    for (int f_iter =0; f_iter < FacilityList.size(); f_iter++) {
        coordinates.push_back(vector<vector<double>>());
        coordinates[f_iter].push_back(vector<double>());
        coordinates[f_iter][coordinates[f_iter].size()-1].push_back(FacilityList[f_iter].LocationX);
        coordinates[f_iter][coordinates[f_iter].size()-1].push_back(FacilityList[f_iter].LocationY);
        
        for (int rl_iter = 0; rl_iter < FacilityList[f_iter].RouteList.size(); rl_iter++) {
            
            for (int r_iter = 0; r_iter < FacilityList[f_iter].RouteList[rl_iter].size(); r_iter++) {
                
                coordinates[f_iter].push_back(vector<double>());
                coordinates[f_iter][coordinates[f_iter].size()-1].push_back(NodeList[FacilityList[f_iter].RouteList[rl_iter][r_iter]].LocationX);
                coordinates[f_iter][coordinates[f_iter].size()-1].push_back(NodeList[FacilityList[f_iter].RouteList[rl_iter][r_iter]].LocationY);
            };
            
            coordinates[f_iter].push_back(vector<double>());
            coordinates[f_iter][coordinates[f_iter].size()-1].push_back(FacilityList[f_iter].LocationX);
            coordinates[f_iter][coordinates[f_iter].size()-1].push_back(FacilityList[f_iter].LocationY);
        }
    }
    return coordinates;
}

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
