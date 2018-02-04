#include "readDS.h"

// Function to read the dataSet needed. The function stores in dataSetEntries the values
// in filepath
void readDS::read(string filepath, vector<vector<int> > &dataSetEntries)
{
    ifstream ifs(filepath);
    string line;

    while(getline(ifs,line)){
        stringstream lineStream(line);
        string cell;
        vector <int> values;
        while(getline(lineStream,cell,',')){
            const char *c = cell.c_str();
            values.push_back(atoi(c));
        }
        dataSetEntries.push_back(values);
    }
    ifs.close();
}

readDS::readDS() {}
