//# include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"

c_Write::c_Write() {

};

bool c_Write::SaveFile(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, int Counter) {
	ofstream myfile;
	string Title = string("Evolutie") + to_string(Counter) + string(".txt");
	myfile.open(Title);

	for (int i = 0; i < FacilityList.size(); i++) {

		myfile << "Facility " << i << ":" << "\n";
		myfile << FacilityList[i].LocationX << "\t" << FacilityList[i].LocationX << "\n";
		for (int r = 0; r < FacilityList[i].ServiceNodes.size(); r++) {
			myfile << NodeList[FacilityList[i].ServiceNodes[r]].LocationX << "\t" << NodeList[FacilityList[i].ServiceNodes[r]].LocationY << "\t" << NodeList[FacilityList[i].ServiceNodes[r]].Demand << "\n";
		};
		for (int r = 0; r < FacilityList[i].RouteList.size(); r++) {
			myfile << "Route " << r + 1 << ": ";
			for (int y = 0; y < FacilityList[i].RouteList[r].size(); y++) {
				myfile << FacilityList[i].RouteList[r][y] << " ";
			};
			myfile << "\n";
		};
	};
	myfile << "\n";
	myfile << "De totale kosten zijn: " << CalculateTotalCosts(FacilityList, NodeList);
	myfile << "\n";
	myfile << "***********************************************************************************************************************";
	myfile << "\n";


	myfile.close();
	return true;
};

bool c_Write::BeginWriteFile(ofstream& myfile) {
	myfile << "\\documentclass{article}" << "\n";
	myfile << "\\usepackage[right=2.54cm,left=2.54cm,top=2.54cm,bottom=2.54cm]{geometry}" << "\n";
	myfile << "\\usepackage{tikz}" << "\n";
	myfile << "\\usepackage{progressbar}" << "\n";
	myfile << "\\usetikzlibrary{arrows}" << "\n";
	myfile << "\\begin{document}" << "\n";
	return true;
};

bool c_Write::WriteGraphToFile(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile, string Title) {
	float CSC = GVMaxCoordX / 15;

	myfile << "\\centerline{ \\textbf{ \\huge{test} } }" << "\n";;
	myfile << "\\centerline{}" << "\n";;

	//DrawNodes
	myfile << "\\begin{tikzpicture}[scale=0.5]" << "\n";

	for (int i = 1; i < NodeList.size(); i++) {
		myfile << "\\filldraw[black](" << NodeList[i].LocationX / CSC << "," << NodeList[i].LocationY / CSC << ") circle(2pt) node[anchor = west]{" << i << "};" << "\n";
	};

	for (int i = 0; i < FacilityList.size(); i++) {
		myfile << "\\filldraw[" << FacilityList[i].Color << "](" << FacilityList[i].LocationX / CSC << "," << FacilityList[i].LocationY / CSC << ") circle(3pt) node[anchor = west]{ Facility " << i + 1 << " };" << "\n";
		for (int j = 0; j < FacilityList[i].RouteList.size(); j++) {
            if (FacilityList[i].RouteList[j].size() > 0) {
                myfile << "\\draw[" << FacilityList[i].Color << ", thick,dotted,->](" << FacilityList[i].LocationX / CSC << "," << FacilityList[i].LocationY / CSC << ") -- (" << NodeList[FacilityList[i].RouteList[j][0]].LocationX / CSC << "," << NodeList[FacilityList[i].RouteList[j][0]].LocationY / CSC << ");" << "\n";
                for (int w = 0; w < FacilityList[i].RouteList[j].size() - 1; w++) {
                    myfile << "\\draw[" << FacilityList[i].Color << ", thick,dotted,->](" << NodeList[FacilityList[i].RouteList[j][w]].LocationX / CSC << "," << NodeList[FacilityList[i].RouteList[j][w]].LocationY / CSC << ") -- (" << NodeList[FacilityList[i].RouteList[j][w + 1]].LocationX / CSC << "," << NodeList[FacilityList[i].RouteList[j][w + 1]].LocationY / CSC << ");" << "\n";
                };
                myfile << "\\draw[" << FacilityList[i].Color << ", thick,dotted,->](" << NodeList[FacilityList[i].RouteList[j][FacilityList[i].RouteList[j].size() - 1]].LocationX / CSC << "," << NodeList[FacilityList[i].RouteList[j][FacilityList[i].RouteList[j].size() - 1]].LocationY / CSC << ") -- (" << FacilityList[i].LocationX / CSC << "," << FacilityList[i].LocationY / CSC << ");" << "\n";
            }
		}
	};

	myfile << "\\end{tikzpicture}" << "\n";
    myfile << "\\\\" << "\n";

	return true;
};

bool c_Write::WriteRouteToFile(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile) {
	for (int i = 0; i < FacilityList.size(); i++) {
		myfile << "\\textbf{ Routes for Facility " << i + 1 << ": } \\\\" << "\n";
		for (int j = 0; j < FacilityList[i].RouteList.size(); j++) {
            if (FacilityList[i].RouteList[j].size() > 0) {
                myfile << "Route " << j + 1 << ": ";
                for (int w = 0; w < FacilityList[i].RouteList[j].size() - 1; w++) {
                    myfile << FacilityList[i].RouteList[j][w] << " $\\to$ ";
                };
                myfile << FacilityList[i].RouteList[j][FacilityList[i].RouteList[j].size() - 1] << "\\\\ \n";
            }
		};
		myfile << "\\\\" << "\n";
	};
//    myfile << "\\\\" << "\n";
//    myfile << "\\\\" << "\n";
//    for (int i = 0; i < FacilityList.size(); i++) {
//        myfile << "The costs for facility " << i + 1 << " are " << CalculateFacilityCosts(FacilityList[i], NodeList) << "\\\\" << "\n";
//    };
//    myfile << "\\\\" << "\n";
//    myfile << "The total costs thus amount to: " << CalculateTotalCosts(FacilityList, NodeList) << "\\\\" << "\n";
//    myfile << "\\\\" << "\n";
	return true;
};


bool c_Write::WriteGraphToFile_C(FacilityClass Facility, FacilityClass ConstrainedFacility, std::vector< NodeClass > NodeList, ofstream& myfile) {
    float CSC = GVMaxCoordX / 15;
    
    vector<vector<int>> edges;
    
    for (int i = 0; i < ConstrainedFacility.RouteList.size(); i++) {
        for (int j = 0; j < ConstrainedFacility.RouteList[i].size(); j++) {
            if ( j == 0) {
                edges.push_back(vector<int>());
                edges[edges.size()-1].push_back(0);
                edges[edges.size()-1].push_back(ConstrainedFacility.RouteList[i][j]);
            }
            if ( j > 0 && j < ConstrainedFacility.RouteList[i].size()) {
                edges.push_back(vector<int>());
                edges[edges.size()-1].push_back(ConstrainedFacility.RouteList[i][j-1]);
                edges[edges.size()-1].push_back(ConstrainedFacility.RouteList[i][j]);
            }
            if ( j == ConstrainedFacility.RouteList[i].size() - 1) {
                edges.push_back(vector<int>());
                edges[edges.size()-1].push_back(ConstrainedFacility.RouteList[i][j]);
                edges[edges.size()-1].push_back(0);
            }
        }
    }
    
    //myfile << "\\centerline{ \\textbf{ \\huge{test} } }" << "\n";;
    myfile << "\\centerline{}" << "\n";;
    
    //DrawNodes
    myfile << "\\begin{tikzpicture}[scale=0.5]" << "\n";
    
    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
            myfile << "\\filldraw[black](" << NodeList[Facility.ServiceNodes[i]].LocationX / CSC << "," << NodeList[Facility.ServiceNodes[i]].LocationY / CSC << ") circle(2pt) node[anchor = west]{" << Facility.ServiceNodes[i] << "};" << "\n";
    };
    
    myfile << "\\filldraw[" << Facility.Color << "](" << Facility.LocationX / CSC << "," << Facility.LocationY / CSC << ") circle(3pt) node[anchor = west]{ Facility };" << "\n";
    for (int j = 0; j < Facility.RouteList.size(); j++) {
        if (Facility.RouteList[j].size() > 0) {
            string Color = "blue";
            
            for (int l = 0; l < edges.size(); l++) {
                if (edges[l][0] == 0 && edges[l][1] == Facility.RouteList[j][0]){
                    Color = "black";
                }
            }
            
            myfile << "\\draw[" << Color << ", thick,->](" << Facility.LocationX / CSC << "," << Facility.LocationY / CSC << ") -- (" << NodeList[Facility.RouteList[j][0]].LocationX / CSC << "," << NodeList[Facility.RouteList[j][0]].LocationY / CSC << ");" << "\n";
            
            for (int w = 0; w < Facility.RouteList[j].size() - 1; w++) {
                Color = "blue";
                
                for (int l = 0; l < edges.size(); l++) {
                    if (edges[l][0] == Facility.RouteList[j][w] && edges[l][1] == Facility.RouteList[j][w+1]){
                        Color = "black";
                    }
                }
                
                myfile << "\\draw[" << Color << ", thick,->](" << NodeList[Facility.RouteList[j][w]].LocationX / CSC << "," << NodeList[Facility.RouteList[j][w]].LocationY / CSC << ") -- (" << NodeList[Facility.RouteList[j][w + 1]].LocationX / CSC << "," << NodeList[Facility.RouteList[j][w + 1]].LocationY / CSC << ");" << "\n";
            };
            
            Color = "blue";
            
            for (int l = 0; l < edges.size(); l++) {
                if (edges[l][0] == Facility.RouteList[j][Facility.RouteList[j].size() - 1] && edges[l][1] == 0){
                    Color = "black";
                }
            }
            
            myfile << "\\draw[" << Color << ", thick,->](" << NodeList[Facility.RouteList[j][Facility.RouteList[j].size() - 1]].LocationX / CSC << "," << NodeList[Facility.RouteList[j][Facility.RouteList[j].size() - 1]].LocationY / CSC << ") -- (" << Facility.LocationX / CSC << "," << Facility.LocationY / CSC << ");" << "\n";
        }
    }
    
    myfile << "\\end{tikzpicture}" << "\n";
    myfile << "\\\\" << "\n";
    
    return true;
};


bool c_Write::WriteNewPage(ofstream& myfile) {
	myfile << "\\newpage" << "\n";
	return true;
};

bool c_Write::CloseFile(ofstream& myfile) {
	myfile << "\\end{document}" << "\n";
	return true;
};

std::vector<std::array<float, 3>> c_Write::WriteChanges(std::vector< FacilityClass > OFacilityList, std::vector< FacilityClass > NFacilityList, std::vector< NodeClass > NodeList, string Title) {
	int FacilityAmount = OFacilityList.size();
	std::vector<std::array<float, 3>> Result;
	std::array<float, 3> Change;
	ofstream myfile;
	myfile.open(Title);
	for (int i = 0; i < FacilityAmount; i++) {
		for (int j = 0; j < OFacilityList[i].ServiceNodes.size(); j++) {
			for (int ii = 0; ii < FacilityAmount; ii++) {
				for (int jj = 0; jj < NFacilityList[ii].ServiceNodes.size(); jj++) {
					if (OFacilityList[i].ServiceNodes[j] == NFacilityList[ii].ServiceNodes[jj]) {
						if (ii != i) {
							myfile << OFacilityList[i].Number << " " << NFacilityList[ii].Number << " " << NodeList[OFacilityList[i].ServiceNodes[j]].Demand << "\n";
							Change[0] = OFacilityList[i].Number;
							Change[1] = NFacilityList[ii].Number;
							Change[2] = NodeList[OFacilityList[i].ServiceNodes[j]].Demand;
							Result.push_back(Change);
						}
					}
				}
			}
		}
	};
	return Result;
}

bool c_Write::SaveSVRP(FacilityClass Facility, std::vector< NodeClass > NodeList) {
	ofstream myfile;
	string Title = "vrp.txt";
	myfile.open(Title);

	myfile << Facility.LocationX << "\t" << Facility.LocationY << "\n";

	for (int r = 0; r < Facility.ServiceNodes.size(); r++) {
		myfile << NodeList[Facility.ServiceNodes[r]].LocationX << "\t" << NodeList[Facility.ServiceNodes[r]].LocationY << "\t" << NodeList[Facility.ServiceNodes[r]].Demand << "\n";
	};

	myfile.close();
	return true;
};

bool c_Write::WriteInfo(vector<float> CostTracker, ofstream& myfile) {
	for (int i = 1; i < CostTracker.size(); i++) {
		if (CostTracker[i] == 0) {
			CostTracker[i] = CostTracker[i - 1];
		}
	};

	for (int i = 0; i < CostTracker.size(); i++) {
		myfile << i+1 << " " << CostTracker[i] << "\n";
	}

	return true;
}

bool c_Write::WriteCoords(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, string name) {
	vector<vector<double>> f_coordinates;
	for (int f_iter = 0; f_iter < FacilityList.size(); f_iter++) {
		f_coordinates.clear();
		f_coordinates.push_back(vector<double>());
		f_coordinates[f_coordinates.size() - 1].push_back(FacilityList[f_iter].LocationX);
		f_coordinates[f_coordinates.size() - 1].push_back(FacilityList[f_iter].LocationY);

		for (int rl_iter = 0; rl_iter < FacilityList[f_iter].RouteList.size(); rl_iter++) {
			for (int r_iter = 0; r_iter < FacilityList[f_iter].RouteList[rl_iter].size(); r_iter++) {
				f_coordinates.push_back(vector<double>());
				f_coordinates[f_coordinates.size() - 1].push_back(NodeList[FacilityList[f_iter].RouteList[rl_iter][r_iter]].LocationX);
				f_coordinates[f_coordinates.size() - 1].push_back(NodeList[FacilityList[f_iter].RouteList[rl_iter][r_iter]].LocationY);
			}
			f_coordinates.push_back(vector<double>());
			f_coordinates[f_coordinates.size() - 1].push_back(FacilityList[f_iter].LocationX);
			f_coordinates[f_coordinates.size() - 1].push_back(FacilityList[f_iter].LocationY);
		}

		ofstream myfile;
		string Title = name + to_string(f_iter) + string(".txt");
		myfile.open(Title);
		for (int coord_iter = 0; coord_iter < f_coordinates.size(); coord_iter++) {
			myfile << f_coordinates[coord_iter][0] << " " << f_coordinates[coord_iter][1] << "\n";
		};
		myfile.close();
	};

	return true;
}




//vector<FacilityClass> ReadSolomon(string Title) {
//    ifstream myfile(Title);
//    
//}
