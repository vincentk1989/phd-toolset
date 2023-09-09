#pragma once

//#include <gl/freeglut.h>
//# include "stdafx.h"
#include <iostream>
#include <string>
#include <list>
#include <stdlib.h> 
#include <time.h> 
#include <vector>
#include <math.h>
#include <cmath>
#include <array>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
//#include <omp.h>
#include <sstream>
#include "include/cdflib.hpp"
#include <random>
//#include "include/stats.hpp"
//#define STATS_NO_ARMA
#include "gurobi_c++.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include <iomanip>
#include "omp.h"

using namespace std;

class NodeClass {
public:
	float LocationX, LocationY;
    float Mean;
    float Stdev;
	float Demand;
    float StartTW, EndTW, ServiceTime;
    float Probability;
    bool Service;
    vector<vector<float>> DiscreteDemand;
	string pcc6;
	NodeClass(float, float, float,float, float, float, bool);
	NodeClass();
	void set_location(float, float);
	void set_demand(float);
};

class FacilityClass {
public:
	float LocationX, LocationY;
	std::vector< int > ServiceNodes;
    std::vector< int > FirstStageNodes;
    std::vector< int > SecondStageNodes;
	std::vector< std::vector<int>> RouteList;
    
	int Number;
	int red;
	int green;
	int blue;
	int Capacity;
    float Cost;
	string pcc6;
	string Color;
    
    int RemovedNode;
    vector<int> Tabu;
    
	FacilityClass(float, float);
	FacilityClass();
	void set_location(float, float);
};

class ScenarioClass {
public:
    vector<int> PresentNodes;
    vector<float> Demands;
    vector<vector<int>> CurOpt;
    float CurCost;
    float Probability;
    ScenarioClass();
};


#ifndef CUSTOMER_H
#define CUSTOMER_H

class customer{
public:
    customer(int id, double x, double y, int demand, int start, int end, int unload);
    
    int id;
    double x_pos, y_pos;
    int demand, start, end, unload;
};

#endif


#ifndef PROBLEM_H
#define PROBLEM_H

class problem{
public:
    bool loadSolomon(const char* filename);
    bool loadCordeau(const char* filename);
    int getCapacity() const;
    int getNumCusto() const;
    double getDistance(int id1, int id2) const;
    const customer& operator [] (int id) const;
    int calMinRoute() const;
    
    void calDistances();
    
    std::vector<customer> allCustomer;
    std::vector<customer> allFacilities;
    vector<int> Capacities;
    std::vector< std::vector<double> > distance;
    int capacity;
    bool shortHorizon;
};

#endif

