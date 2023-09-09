//# include "stdafx.h"
#include "bClass.h"
#include "DistanceMatrix.h"
#include "ClarkeWright.h"
#include "Write.h"
//#include <GLUT/glut.h>

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

bool c_Write::WriteGraphToFile_Real(std::vector< FacilityClass > FacilityList, std::vector< NodeClass > NodeList, ofstream& myfile) {
    //float CSC = GVMaxCoordX / 15;
    
    float CSC = 1.0;
    
    cout << "miny: " << MinimumCoordX << endl;
    cout <<  MinimumCoordY << endl;

    myfile << "\\centerline{ \\textbf{ \\huge{test} } }" << "\n";;
    myfile << "\\centerline{}" << "\n";;

    //DrawNodes
    myfile << "\\begin{tikzpicture}" << "\n";
    
    myfile << "\\filldraw[green](" << ( avgY - MinimumCoordY ) / CSC << "," << (avgX - MinimumCoordX) / CSC << ") circle(0.8pt) node[anchor = west]{  };" << "\n";

    for (int i = 1; i < NodeList.size(); i++) {
        myfile << "\\filldraw[black](" << ( NodeList[i].LocationY - MinimumCoordY ) / CSC << "," << ( NodeList[i].LocationX - MinimumCoordX) / CSC << ") circle(0.8pt) node[anchor = west]{  };" << "\n";
    };

    for (int i = 0; i < FacilityList.size(); i++) {
        myfile << "\\filldraw[red](" << ( FacilityList[i].LocationY - MinimumCoordY ) / CSC << "," << ( FacilityList[i].LocationX - MinimumCoordX ) / CSC << ") circle(1.2pt) node[anchor = west]{  };" << "\n";
        
        for (int j = 0; j < FacilityList[i].RouteList.size(); j++) {
            if (FacilityList[i].RouteList[j].size() > 0) {
                
                int node = FacilityList[i].RouteList[j][0];
 
                for (int w=0; w < DrawMatrix[DrawMatrix.size()-1][node].listOfCoordinates.size() - 1; w++ ) {
                    try {
                        myfile << "\\draw[black, thick](" << ( DrawMatrix[DrawMatrix.size()-1][node].listOfCoordinates[w].coordinateY - MinimumCoordY )  / CSC << "," << ( DrawMatrix[DrawMatrix.size()-1][node].listOfCoordinates[w].coordinateX - MinimumCoordX ) / CSC << ") -- (" << ( DrawMatrix[DrawMatrix.size()-1][node].listOfCoordinates[w + 1].coordinateY - MinimumCoordY ) / CSC << "," << ( DrawMatrix[DrawMatrix.size()-1][node].listOfCoordinates[w + 1].coordinateX - MinimumCoordX ) / CSC << ");" << "\n";
                    } catch (const std::exception& e) {
                    }
                }

                
                int nextnode = 0;

                for (int z = 0; z < FacilityList[i].RouteList[j].size() - 1; z++) {
                    node = FacilityList[i].RouteList[j][z];
                    nextnode = FacilityList[i].RouteList[j][z + 1];
                    for (int w=0; w < DrawMatrix[node][nextnode].listOfCoordinates.size() - 1; w++ ) {
                        try {
                            myfile << "\\draw[black, thick](" << ( DrawMatrix[node][nextnode].listOfCoordinates[w].coordinateY - MinimumCoordY )  / CSC << "," << ( DrawMatrix[node][nextnode].listOfCoordinates[w].coordinateX - MinimumCoordX )/ CSC << ") -- (" << ( DrawMatrix[node][nextnode].listOfCoordinates[w + 1].coordinateY - MinimumCoordY ) / CSC << "," << ( DrawMatrix[node][nextnode].listOfCoordinates[w + 1].coordinateX - MinimumCoordX ) / CSC << ");" << "\n";
                        } catch (const std::exception& e) {
                        }
                    }
                }
                
                node = FacilityList[i].RouteList[j][FacilityList[i].RouteList[j].size() - 1];
                for (int w=0; w < DrawMatrix[node][DrawMatrix.size()-1].listOfCoordinates.size() - 1; w++ ) {
                    try {
                        myfile << "\\draw[black, thick](" << ( DrawMatrix[node][DrawMatrix.size()-1].listOfCoordinates[w].coordinateY - MinimumCoordY )/ CSC << "," << ( DrawMatrix[node][DrawMatrix.size()-1].listOfCoordinates[w].coordinateX - MinimumCoordX )/ CSC << ") -- (" << ( DrawMatrix[node][DrawMatrix.size()-1].listOfCoordinates[w + 1].coordinateY - MinimumCoordY ) / CSC << "," << ( DrawMatrix[node][DrawMatrix.size()-1].listOfCoordinates[w + 1].coordinateX - MinimumCoordX ) / CSC << ");" << "\n";
                    } catch (const std::exception& e) {
                    }
                }
            }
        }
    };

    myfile << "\\end{tikzpicture}" << "\n";
    myfile << "\\\\" << "\n";

    return true;
};

bool c_Write::WriteFacilityToFile(FacilityClass Facility, std::vector< NodeClass > NodeList, ofstream& myfile) {
    float CSC = GVMaxCoordX / 15;

    //DrawNodes
    myfile << "\\begin{tikzpicture}[scale=0.5]" << "\n";

    for (int i = 0; i < Facility.ServiceNodes.size(); i++) {
        myfile << "\\filldraw[black](" << NodeList[Facility.ServiceNodes[i]].LocationX / CSC << "," << NodeList[Facility.ServiceNodes[i]].LocationY / CSC << ") circle(2pt) node[anchor = west]{" << i << "};" << "\n";
    };

    // Draw Facility
    myfile << "\\filldraw[" << Facility.Color << "](" << Facility.LocationX / CSC << "," << Facility.LocationY / CSC << ") circle(3pt) node[anchor = west]{ Facility };" << "\n";
    
    // Draw Edges
    for (int j = 0; j < Facility.RouteList.size(); j++) {
        if (Facility.RouteList[j].size() > 0) {
            myfile << "\\draw[" << Facility.Color << ",thick,->](" << Facility.LocationX / CSC << "," << Facility.LocationY / CSC << ") -- (" << NodeList[Facility.RouteList[j][0]].LocationX / CSC << "," << NodeList[Facility.RouteList[j][0]].LocationY / CSC << ");" << "\n";
            for (int w = 0; w < Facility.RouteList[j].size() - 1; w++) {
                myfile << "\\draw[" << Facility.Color << ", thick,->](" << NodeList[Facility.RouteList[j][w]].LocationX / CSC << "," << NodeList[Facility.RouteList[j][w]].LocationY / CSC << ") -- (" << NodeList[Facility.RouteList[j][w + 1]].LocationX / CSC << "," << NodeList[Facility.RouteList[j][w + 1]].LocationY / CSC << ");" << "\n";
            };
            myfile << "\\draw[" << Facility.Color << ",thick,->](" << NodeList[Facility.RouteList[j][Facility.RouteList[j].size() - 1]].LocationX / CSC << "," << NodeList[Facility.RouteList[j][Facility.RouteList[j].size() - 1]].LocationY / CSC << ") -- (" << Facility.LocationX / CSC << "," << Facility.LocationY / CSC << ");" << "\n";
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
//    vector<vector<double>> f_coordinates;
//    for (int f_iter = 0; f_iter < FacilityList.size(); f_iter++) {
//        f_coordinates.clear();
//        f_coordinates.push_back(vector<double>());
//        f_coordinates[f_coordinates.size() - 1].push_back(FacilityList[f_iter].LocationX);
//        f_coordinates[f_coordinates.size() - 1].push_back(FacilityList[f_iter].LocationY);
//
//        for (int rl_iter = 0; rl_iter < FacilityList[f_iter].RouteList.size(); rl_iter++) {
//            for (int r_iter = 0; r_iter < FacilityList[f_iter].RouteList[rl_iter].size(); r_iter++) {
//                f_coordinates.push_back(vector<double>());
//                f_coordinates[f_coordinates.size() - 1].push_back(NodeList[FacilityList[f_iter].RouteList[rl_iter][r_iter]].LocationX);
//                f_coordinates[f_coordinates.size() - 1].push_back(NodeList[FacilityList[f_iter].RouteList[rl_iter][r_iter]].LocationY);
//            }
//            f_coordinates.push_back(vector<double>());
//            f_coordinates[f_coordinates.size() - 1].push_back(FacilityList[f_iter].LocationX);
//            f_coordinates[f_coordinates.size() - 1].push_back(FacilityList[f_iter].LocationY);
//        }
//
//        ofstream myfile;
//        string Title = name + to_string(f_iter) + string(".txt");
//        myfile.open(Title);
//        for (int coord_iter = 0; coord_iter < f_coordinates.size(); coord_iter++) {
//            myfile << f_coordinates[coord_iter][0] << " " << f_coordinates[coord_iter][1] << "\n";
//        };
//        myfile.close();
//    };
    
    
    vector<vector<vector<double>>> coordinates = GenerateCoordinates(FacilityList, NodeList);
    
    ofstream myfile1;
    stringstream ss1;
    ss1 << name << "1";
    string Title1 = ss1.str();
    myfile1.open(Title1);
    
    for (int i = 0; i < coordinates[0].size(); i++) {
        myfile1 << coordinates[0][i][0] << " " << coordinates[0][i][1] << "\n";
    }
    
    myfile1.close();
    
    ofstream myfile2;
    stringstream ss2;
    ss2 << name << "2";
    string Title2 = ss2.str();
    myfile2.open(Title2);
    
    for (int i = 0; i < coordinates[1].size(); i++) {
        myfile2 << coordinates[1][i][0] << " " << coordinates[1][i][1] << "\n";
    }
    
    myfile2.close();
    
    ofstream myfile3;
    stringstream ss3;
    ss3 << name << "3";
    string Title3 = ss3.str();
    myfile3.open(Title3);
    
    for (int i = 0; i < coordinates[2].size(); i++) {
        myfile3 << coordinates[2][i][0] << " " << coordinates[2][i][1] << "\n";
    }
    
    myfile3.close();

	return true;
}

string c_Write::WriteFacilityToXML(std::vector< FacilityClass > OriginalFacilityList, std::vector< FacilityClass > OptimalFacilityList, std::vector< FacilityClass > AuctionFacilityList, std::vector< NodeClass > NodeList, string type, int iterator) {
    
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




void c_Write::DrawMAP(vector<FacilityClass> FacilityList, int argc, char **argv, vector<vector<vector<double>>> coordinates) {
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
//    //glutDisplayFunc(display_mat_points);
//    glutInitWindowSize(1200, 1200);
//
//
//    glutGetWindow();
//
//
//    glutCreateWindow("GLUT");
//
//    glClearColor(255, 255, 255, 0);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//
//
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(3, 8, 50, 54, -1, 1);
//
//    glMatrixMode(GL_MODELVIEW);
//
//
//    for (int f = 0; f < coordinates.size(); f++) {
//        glColor3ub(FacilityList[f].red,FacilityList[f].green,FacilityList[f].blue);
//        for (int i = 0; i < coordinates[f].size() - 1; i++) {
//            glBegin(GL_LINE_STRIP);
//
//            glVertex2f(coordinates[f][i][1], coordinates[f][i][0]);
//            glVertex2f(coordinates[f][i+ 1][1],coordinates[f][i+1 ][0]);
//            glEnd();
//        }
//    };
//
//   // glutDisplayFunc(myDisplayFunc);
//
//    glutSwapBuffers();
//
//    glutMainLoop();
}




//vector<FacilityClass> ReadSolomon(string Title) {
//    ifstream myfile(Title);
//    
//}
