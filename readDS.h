#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

#ifndef PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_READDS_H
#define PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_READDS_H


class readDS {

public:

    readDS();

    void read(string filepath, vector <vector <int> >& dataSetEntries);

};


#endif //PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_READDS_H
