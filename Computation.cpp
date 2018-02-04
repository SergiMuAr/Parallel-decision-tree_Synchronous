#include "Computation.h"

// Function used to calculate the entropy values of the different elements in
// the vector counts.
double Computation::entropy(vector <double> counts)
{
    double total = 0;
    double entropy;
    int i;

    for(i=0;i<counts.size();i++){
        total+=counts[i];
    }

    entropy=0;
    for(i=0;i<counts.size();i++) {
        entropy += (counts[i]/total)*(log2(counts[i]/total));
    }
    return -1 * entropy;
}

// function to get information gain of dataset
double Computation::getInfoGainOfData(vector <int>& dataRows, vector<vector<int>>& dataSet,int numOfAttrib)
{
    int i,classVal;
    map<int, int> classCount;
    map<int, int>::iterator it;
    vector<double> counts;
    for(i=0;i<dataRows.size();i++){
        classVal = dataSet[dataRows[i]][numOfAttrib-1];
        if(classCount.find(classVal) == classCount.end()){
            classCount.insert(make_pair(classVal,1));
        }
        else{
            classCount[classVal]++;
        }
    }
    for(it=classCount.begin();it!=classCount.end();it++){
        counts.push_back((double)it->second);
    }


    return entropy(counts);
}

double Computation::infoGain(int attr,vector <int>& dataRows, vector<vector<int>>& dataSet, int numOfAttrib)
{
    int i,branchVal,dataSize,subDataValue;
    double attrInfoGain;
    map<int, int> branchCount;
    map<int, int>::iterator branchCountIT;
    map<int, vector<int> > dataElements;
    for(i=0;i<dataRows.size();i++){
        branchVal = dataSet[dataRows[i]][attr];
        if(branchCount.find(branchVal) == branchCount.end()){
            branchCount.insert(make_pair(branchVal,1));
            vector <int> x;
            x.push_back(dataRows[i]);
            dataElements.insert(make_pair(branchVal,x));
        }
        else{
            branchCount[branchVal]++;
            dataElements[branchVal].push_back(dataRows[i]);
        }
    }
    attrInfoGain=0;
    dataSize=dataRows.size();
    for(branchCountIT = branchCount.begin();branchCountIT!=branchCount.end();branchCountIT++){
        vector <int> subData = dataElements[branchCountIT->first];
        map <int, int> subDataCounts;
        map <int, int>::iterator subDataCountsIT;
        for(i=0;i<subData.size();i++){
            subDataValue = dataSet[subData[i]][numOfAttrib-1];
            if(subDataCounts.find(subDataValue) == subDataCounts.end()){
                subDataCounts.insert(make_pair(subDataValue,1));
            }
            else{
                subDataCounts[subDataValue]++;
            }
        }
        vector <double> subDataCountsArr;
        for(subDataCountsIT=subDataCounts.begin();subDataCountsIT!=subDataCounts.end();subDataCountsIT++){
            subDataCountsArr.push_back((double)subDataCountsIT->second);
        }
        attrInfoGain+= ((double)branchCountIT->second/(double)dataSize)*entropy(subDataCountsArr);
    }
    return getInfoGainOfData(dataRows, dataSet, numOfAttrib) - attrInfoGain;
}

int Computation::maxClass(vector<int> &dataRows, vector<vector<int>> &dataSet, int numOfAttrib)
{
    int i,outputClass,ans,maxVal;
    maxVal = INT_MIN;
    ans = dataSet[dataRows[0]][numOfAttrib-1];
    map <int, int> dataCount;
    map <int, int>::iterator it;
    for(i=0;i<dataRows.size();i++){
        outputClass = dataSet[dataRows[i]][numOfAttrib-1];
        if(dataCount.find(outputClass) == dataCount.end()){
            dataCount.insert(make_pair(outputClass,1));
        }
        else{
            dataCount[outputClass]++;
            if (dataCount[outputClass] > maxVal) ans = dataCount[outputClass];
        }
    }
    return ans;
}

