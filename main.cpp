#include <vector>
#include <map>
#include <queue>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <set>
#include <climits>
#include <omp.h>
#include "Node.h"
#include "readDS.h"
#include "Computation.h"

/*
#define trainingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Balloons/balloons-data.int.txt"
#define testingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Balloons/balloons-test.int.txt"
*/

/*
#define trainingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Car/car-data.int.txt"
#define testingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Car/car-test.int.txt"
*/

/*
#define trainingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/hayes-roth.data/hayes-roth.data.txt"
#define testingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/hayes-roth.data/hayes-roth.test.txt"
*/


#define trainingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Nursery/nursery-data.int.txt"
#define testingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Nursery/nursery-test.int.txt"


using namespace std;

vector <vector <int> > dataSet;

int numOfAttrib, numOfDataEle;

Computation computation;


// function to determine the splitting attribute
int selectSplitAttribute(vector<int> &attr, vector<int> dataRows)
{
	int i,splitAttr;
	double iGain,maxIGain;
	maxIGain = INT_MIN;

    #pragma omp parallel for shared(maxIGain, splitAttr) private(i)
        for(i=1;i<attr.size()-1;i++) {
            if (attr[i] == 0) {
                iGain = computation.infoGain(i, dataRows, dataSet, numOfAttrib);
                if (iGain > maxIGain) {
                    maxIGain = iGain;
                    splitAttr = i;
                }
            }
        }

	if(maxIGain==INT_MIN){
		return -1;
	}
	attr[splitAttr]=1;
	return splitAttr;
}

// function to build the decision tree
void decisionTreeBuilder(vector<int> attr, vector<int> dataRows, Node *root)
{
	int flag,selectedAttribute,i;
	if(dataRows.size()==0){
		return;
	}
	flag=1;
	for(i=1;i<dataRows.size();i++){
		if(dataSet[dataRows[i]][numOfAttrib-1]!=dataSet[dataRows[i-1]][numOfAttrib-1]){
			flag=0;
			break;
		}
	}
	if(flag==1){
		root->setVal(dataSet[dataRows[0]][numOfAttrib-1]);
		return;
	}
	selectedAttribute= selectSplitAttribute(attr, dataRows);
	root->setAttribute(selectedAttribute);

	if(selectedAttribute == -1){
		root->setVal(computation.maxClass(dataRows, dataSet, numOfAttrib)) ;
		return;
	}

	map<int, vector <int> > dividedData;
	map<int, vector <int> >::iterator it;
	int attrVal;

	for(i=0;i<dataRows.size();i++){
		attrVal = dataSet[dataRows[i]][selectedAttribute];
		if(dividedData.find(attrVal) == dividedData.end()){
			vector <int> x;
			x.push_back(dataRows[i]);
			dividedData.insert(make_pair(attrVal,x));
		}
		else{
			dividedData[attrVal].push_back(dataRows[i]);
		}
	}
	for(i=0,it=dividedData.begin();it!=dividedData.end();it++,i++){
		root->setNumOfChildren(root->getNumOfChildren()+1);
		Node* childNode = new Node();
		childNode->setBranchVal(it->first);
		root->setChild(childNode,i);

        decisionTreeBuilder(attr, it->second, childNode);
	}

}

// function to print the decision tree
void printDT(Node *root)
{
	printf("Printing decision tree:\n");
	queue <Node> bfsQ;
	int x,j;
	Node* nextNode;
	bfsQ.push(*root);
	cout << "root attribute: " << root->getAttribute() << endl;
	while(bfsQ.size()!=0){
		nextNode = &(bfsQ.front());
		bfsQ.pop();
		x = nextNode->getNumOfChildren();
		j=0;
		while(j<x){
			bfsQ.push(*(nextNode->getChild()[j]));
            cout << nextNode->getChild()[j]->getAttribute() << " ";
			j++;
		}
		cout << endl;
	}
	return;
}

// function to test the decision tree build against a dataset
void test(Node* root)
{
	int i,pos,neg,noResult,attr,attrVal,j,flag;
	Node* temp;
	pos=0;
	neg=0;
	noResult=0;

	for(i=0;i<dataSet.size();i++){
		temp=root;
		flag=0;
		//traverse decisionTreeBuilder tree
		while(temp->getVal() ==-1 && temp->getAttribute()!=-1){
			attr = temp->getAttribute();
			attrVal=dataSet[i][attr];
			for(j=0;j<temp->getNumOfChildren();j++){
				if(temp->getChild()[j]->getBranchVal() == attrVal){
					break;
				}
			}
			if(j==temp->getNumOfChildren()){
				flag=1;
				break;
			}
			else{
				temp=temp->getChild()[j];
			}
		}
		if(temp->getVal() == dataSet[i][numOfAttrib-1]){
			pos++;
		}
		else{
			neg++;
		}
		if(temp->getVal() == -1 || flag==1){
			noResult++;
		}
	}
	cout << "Rows with positive result: " << pos << endl;
	cout << "Rows with negative result: " << neg << endl;
	cout << "No Result: " << noResult << endl;

	return;
}

int main()
{
	int i;
    Node* root = new Node();

	computation = Computation();
	vector <int> dataRows;
	vector <int> attr;

    readDS read = readDS();
	read.read(trainingData, dataSet);

	numOfAttrib = (int) dataSet[0].size();
	numOfDataEle = (int) dataSet.size();

	for(i=0;i<numOfDataEle;i++){
		dataRows.push_back(i);
	}
	for(i=0;i<numOfAttrib;i++){
		attr.push_back(0);
	}

	double start = omp_get_wtime();

    #pragma omp parallel num_threads(4)
    {
        #pragma omp single
        {
            #pragma omp task
            decisionTreeBuilder(attr, dataRows, root);
        };
    }

	double end = omp_get_wtime();

	//printDT(root);

    dataSet = vector<vector<int>>();
	read.read(testingData, dataSet);
	test(root);

	printf("Time taken:%f\n", end-start);

	return 0;
}