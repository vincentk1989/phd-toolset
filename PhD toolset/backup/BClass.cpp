//# include "stdafx.h"
#include "bClass.h"

float GVOriginalCosts;
float GVOptimalCosts;
int GVstep;
int GVCount = 0;
vector<vector<vector<float>>> CostVector;
vector<vector<float>> DistanceMatrix;



FacilityClass::FacilityClass(float x, float y) {
	LocationX = x;
	LocationY = y;
	Number = 0;
	red = 0;
	green = 0;
	blue = 0;
	Color = "Black";
};

NodeClass::NodeClass(float x, float y, float d, float p, float m, float s, bool n) {
	LocationX = x;
	LocationY = y;
	Demand = d;
    Probability = p;
    Mean = m;
    Stdev = s;
    Service = n;
};

NodeClass::NodeClass() {

};

FacilityClass::FacilityClass() {

};

void NodeClass::set_location(float x, float y) {
	LocationX = x;
	LocationY = y;
};

void NodeClass::set_demand(float d) {
	Demand = d;
};


void FacilityClass::set_location(float x, float y) {
	LocationX = x;
	LocationY = y;
};

ScenarioClass::ScenarioClass() {
    
};

customer::customer(int i, double x, double y, int d, int s, int e, int u) :
id(i), x_pos(x), y_pos(y), demand(d), start(s), end(e), unload(u) {
}

bool problem::loadSolomon(const char* filename){
    FILE *fp = fopen(filename, "r");
    
    if(fp == NULL) return false;
    
    if( fscanf(fp, "%d", &capacity) != 1 ){
        fclose(fp);
        return false;
    }
    
    int inputNum;
    if( fscanf(fp, "%d", &inputNum) != 1 ){
        fclose(fp);
        return false;
    }
    allCustomer.reserve(inputNum + 1);    // with depot
    
    int totalDemand = 0;
    for(int i = 0; i <= inputNum; ++i){
        double x, y;
        int d, s, e, u;
        
        if( fscanf(fp, "%*d%lf%lf%d%d%d%d", &x, &y, &d, &s, &e, &u) != 6 ){
            fclose(fp);
            return false;
        }
        
        totalDemand += d;
        customer newCustomer(i, x, y, d, s, e, u);
        allCustomer.push_back(newCustomer);
    }
    fclose(fp);
    
    allFacilities.push_back(allCustomer[0]);
    allCustomer.erase(allCustomer.begin());
    
    
    calDistances();
    
    double avgCusPerVehicle = capacity / (totalDemand / inputNum);
    shortHorizon = (calMinRoute() > avgCusPerVehicle);
    
    return true;
}

bool problem::loadCordeau(const char* filename){
    FILE *fp = fopen(filename, "r");
    
    int type;
    int numveh;
    int inputNum;
    int inputDep;
    
    if(fp == NULL) return false;
    
    if( fscanf(fp, "%d%d%d%d", &type, &numveh, &inputNum, &inputDep) != 4 ){
        fclose(fp);
        return false;
    }
    
    allCustomer.reserve(inputNum);
    allFacilities.reserve(inputDep);
    
    Capacities.clear();
    
    
    for(int i = 0; i < inputDep; ++i){
        int D, Q;
        
        if( fscanf(fp, "%d%d", &D, &Q) != 2 ){
            fclose(fp);
            return false;
        }
        
        Capacities.push_back(Q);
        
    }
    
    capacity = Capacities[0];
    
    int totalDemand = 0;
    for(int i = 0; i < inputNum; ++i){
        float x, y;
        int ii, d, q, f, a, l, e, list, w, w2;
        
        if( fscanf(fp, "%d%f%f%d%d%d%d%d%d%d%d", &ii, &x, &y, &d, &q, &f, &a, &l, &e, &list, &w) != 11 ){ //&l, &w, &w2
            fclose(fp);
            return false;
        }
        
        totalDemand += q;
        customer newCustomer(ii, x, y, q, e, l, d);
        allCustomer.push_back(newCustomer);
    }
    
    for(int i = 0; i < inputDep; ++i){
        float x, y;
        int ii, d, q, f, a;
        
        if( fscanf(fp, "%d%f%f%d%d%d%d", &ii, &x, &y, &d, &q, &f, &a) != 7 ){
            fclose(fp);
            return false;
        }
        
        customer newCustomer(ii, x, y, q, 0, 9999, d);
        allFacilities.push_back(newCustomer);
    }
    fclose(fp);
    
    calDistances();
    
    double avgCusPerVehicle = capacity / (totalDemand / inputNum);
    shortHorizon = (calMinRoute() > avgCusPerVehicle);
    
    return true;
}

int problem::getCapacity() const {
    return capacity;
}

int problem::getNumCusto() const {
    return allCustomer.size() - 1;    // ignore depot
}

double problem::getDistance(int id1, int id2) const {
    return distance[id1][id2];
}

const customer& problem::operator [] (int id) const {
    return allCustomer[id];
}

// minimum # of route = ceil(total demand / vehicle's capacity);
int problem::calMinRoute() const {
    int totalDemand = getCapacity() - 1;    // ceiling
    for(int i = 1; i <= getNumCusto(); ++i) totalDemand += allCustomer[i].demand;
    return totalDemand / getCapacity();
}

void problem::calDistances(){
    distance.resize( allCustomer.size() );
    
    for(unsigned int i = 0; i < allCustomer.size(); ++i){
        distance[i].resize( allCustomer.size() );
    }
    
    for(unsigned int x = 0;   x < allCustomer.size(); ++x){
        for(unsigned int y = x+1; y < allCustomer.size(); ++y){
            double ans = hypot(allCustomer[x].x_pos - allCustomer[y].x_pos,
                               allCustomer[x].y_pos - allCustomer[y].y_pos);
            distance[x][y] = distance[y][x] = ans;
        }}
}
