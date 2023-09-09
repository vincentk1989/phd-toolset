// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include "Solution.h"
#include <chrono>
//#include "include/cdflib.hpp"



int main(int argc, char *argv[]);
//
//
//
//
int main(int argc, char *argv[])
{
//    //Paper1Runsolution();
//
////    ofstream xfile;
////    stringstream ss;
////    ss << "results.txt";
////    xfile.open(ss.str());
//
    int i = 0;
//
//    //for (int i = 1; i < 6; i++) {
        time_t start = time(NULL);
//
//       // for (int j = 0; j < 10; j++) {
//
//
//        //c_SolutionSingle Solution = c_SolutionSingle(i, 2 + j, 10 - j, 4, false);

        c_SolutionSingle Solution = c_SolutionSingle(i, 6, 6, 4, false);

        Solution.CalculateExact(Solution.NodeList, Solution.FacilityList[0]);
//
//        //xfile << i << ";" << 2 + j << ";" << 10 - j << ";" << Solution.Zstar << "\n";

        time_t end = time(NULL);
        float time = difftime(end,start);

        cout << "Time taken: " <<  time << " seconds." << endl;
//
//    //    }
//    //}
//
//    //xfile.close();
//
//
    return 0;
}

