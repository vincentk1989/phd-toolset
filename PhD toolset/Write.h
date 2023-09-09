#pragma once


#include "bClass.h"
#include "DistanceMatrix.h"
//#include "ClarkeWright.h"

class c_Write : public c_DistanceMatrix {
public:
    float MinimumCoordY;
    float MinimumCoordX;
    float avgX;
    float avgY;
	bool Writefile = false;
	c_Write();
	bool SaveFile(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int Counter);
	bool BeginWriteFile(ofstream& myfile);
	bool WriteGraphToFile(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile, string Title);
    bool WriteFacilityToFile(FacilityClass Facility, std::vector< NodeClass > NodeList, ofstream& myfile);
	bool WriteRouteToFile(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile);
	bool WriteNewPage(ofstream& myfile);
	bool CloseFile(ofstream& myfile);
	bool SaveSVRP(FacilityClass Facility, std::vector< NodeClass > NodeList);
	bool WriteInfo(vector<float> CostTracker, ofstream& myfile);
	bool WriteCoords(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, string name);
	std::vector<std::array<float, 3>> WriteChanges(std::vector< FacilityClass > OFacilityList, std::vector< FacilityClass > NFacilityList, std::vector< NodeClass > NodeList, string Title);
    bool WriteGraphToFile_C( FacilityClass Facility, FacilityClass FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile);
    string WriteFacilityToXML(std::vector< FacilityClass > OriginalFacilityList, std::vector< FacilityClass > OptimalFacilityList, std::vector< FacilityClass > AuctionFacilityList, std::vector< NodeClass > NodeList, string type, int iterator);
    void DrawMAP(vector<FacilityClass> FacilityList, int argc, char **argv, vector<vector<vector<double>>> coordinates);
    
    bool WriteGraphToFile_Real(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile);
};

bool writeXML();
