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

    double getInfoGainOfData(vector <int>& dataRows, vector<vector<int>>& dataSet,int numOfAttrib);

    double infoGain(int attr,vector <int>& dataRows, vector<vector<int>>& dataSet, int numOfAttrib);

    int maxClass(vector<int> &dataRows, vector<vector<int>> &dataSet, int numOfAttrib);

};


#endif //PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_COMPUTATION_H
