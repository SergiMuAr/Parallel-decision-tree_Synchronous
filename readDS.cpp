#include "readDS.h"

void readDS::read(string filepath, vector<vector<int> > &dataSetEntries)
{
    // input file stream (ifs) for reading data from file
    ifstream ifs(filepath);
    string line;

    // read from ifs into string 'line'
    while(getline(ifs,line)){
        stringstream lineStream(line);
        string cell;
        vector <int> values;
        // collecting row data from file delimited by ','
        while(getline(lineStream,cell,',')){
            const char *c = cell.c_str();
            values.push_back(atoi(c));
        }
        dataSetEntries.push_back(values);
    }
    ifs.close();
}

readDS::readDS() {}
