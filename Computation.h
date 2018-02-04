#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <limits.h>
using namespace std;



#ifndef PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_COMPUTATION_H
#define PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_COMPUTATION_H


class Computation {

public:

    double entropy(vector <double> counts);

    double getInfoGainOfData(vector <int>& data, vector<vector<int>>& fileContent,int numOfAttrib);

    double infoGain(int attr,vector <int>& data, vector<vector<int>>& fileContent, int numOfAttrib);

    int popularVote(vector<int>& data, vector<vector<int>>& fileContent, int numOfAttrib);

};


#endif //PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_COMPUTATION_H
