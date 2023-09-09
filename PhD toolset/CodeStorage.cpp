//
//  CodeStorage.cpp
//  PhD toolset
//
//  Created by Vincent Karels on 10/12/19.
//  Copyright © 2019 Vincent Karels. All rights reserved.
//

#include <stdio.h>

/*
 
 for (int i = 1; i < 20; i++) {

     time_t start = time(NULL);

     c_SolutionSingle Solution = c_SolutionSingle(i, 13, 8, 7, false);

     //Completely separate

     FacilityClass FirstStageFacility = Solution.FacilityList[0];

     vector<int> t_SN = FirstStageFacility.ServiceNodes;
     FirstStageFacility.ServiceNodes = FirstStageFacility.FirstStageNodes;
     FirstStageFacility = Solution.GurobiSol(Solution.NodeList, FirstStageFacility);

     float SeparateCost = 0;
     SeparateCost = SeparateCost + Solution.CalculateFacilityCosts(FirstStageFacility, Solution.NodeList);
     SeparateCost = SeparateCost + Solution.CalculateSecondCost(Solution.NodeList, Solution.FacilityList[0]);

     //Using First

     FirstStageFacility.ServiceNodes = t_SN;
     float NoInfoCost = Solution.CalculateAll(Solution.NodeList, FirstStageFacility);

     //optimal

     Solution.CalculateExact(Solution.NodeList, Solution.FacilityList[0]);

     ofstream myfile;
     stringstream ss;
     ss << "instance_" << i << ".txt";
     string Title = ss.str();
     myfile.open(Title);

     myfile << SeparateCost << "\n";
     myfile << NoInfoCost << "\n";
     myfile << Solution.Zstar << "\n";

     myfile.close();

     time_t end = time(NULL);
     float time = difftime(end,start);

     cout << "Time taken: " <<  time << " seconds." << endl;

 }

    char buff[FILENAME_MAX];
    GetCurrentDir( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    
    std::cout << "Current path is " << current_working_dir << endl;
    
    for (int iterator = 1; iterator < 10; iterator++) {
    c_Solution Solution = c_Solution(0, 10, 6, false);
    c_RealSolution Solution = c_RealSolution(0, 1000, 30);

    ofstream myfile;
    stringstream ss;
    ss << "test";
    string Title = ss.str();
    myfile.open(Title);

    cout << "Solution Method: " <<  Solution.SolutionMethod << endl;

    for (int i = 0; i < Solution.FacilityList.size(); i++) {
        Solution.FacilityList[i] = Solution.ClarkeWright(Solution.NodeList, Solution.FacilityList[i]);
    };

    cout << Solution.CalculateTotalCosts(Solution.FacilityList, Solution.NodeList) << endl;

    vector<FacilityClass> OriginalFacilityList = Solution.FacilityList;

    Solution.WriteCoords(OriginalFacilityList, Solution.NodeList, "org");

    vector<FacilityClass> OptimalFacilityList = Solution.OptimalSolution(Solution.FacilityList, Solution.NodeList);
    
    cout << Solution.CalculateTotalCosts(OptimalFacilityList, Solution.NodeList) << endl;
    
    Solution.WriteCoords(OptimalFacilityList, Solution.NodeList, "auc");

    vector<FacilityClass> AuctionFacilityList = Solution.RouteAuction(Solution.FacilityList, Solution.NodeList, 3, myfile);

    AuctionFacilityList = Solution.ClusterAuction(AuctionFacilityList, Solution.NodeList);

    AuctionFacilityList = Solution.BlackListSingleAuction(AuctionFacilityList, Solution.NodeList, 3, myfile);

    Solution.WriteCoords(AuctionFacilityList, Solution.NodeList, "auc");
    
    
        Solution.DrawMAP(OriginalFacilityList, argc, argv, coordinates);
        
        Solution.WriteFacilityToXML(OriginalFacilityList, OptimalFacilityList, AuctionFacilityList, Solution.NodeList, "SRRA", iterator);
        
    myfile.close();
    

        
    }
        
    
    c_CordeauSolution Solution = c_CordeauSolution(0, "p09");

    Solution.FacilityList = Solution.OptimalSolution(Solution.FacilityList, Solution.NodeList);

    cout << Solution.CalculateTotalCosts(Solution.FacilityList, Solution.NodeList);

*/
