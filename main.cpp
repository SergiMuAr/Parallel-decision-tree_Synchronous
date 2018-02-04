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

vector <vector <int> > fileContent;

int numOfAttrib, numOfDataEle;


typedef struct Node node;

Computation computation;

// function to get information gain of training data

// function to determine the splitting attribute
// attr: candidate attributes for splitting attribute, attr[i]=1 if already used
// data: data row nos(in the file and index in "fileContent" vector) used for calculating information gains
int select(vector <int> &attr,vector <int> data)
{
	int i,splitAttr;
	double iGain,maxIGain;
	maxIGain = INT_MIN;
	// //to be deleted
	// printf("infoGain of data: %f\n",getInfoGainOfData(data));
	// printf("attribute gains:\n");
	// //to be deleted

    #pragma omp parallel for shared(maxIGain, splitAttr) private(i)
        for(i=1;i<attr.size()-1;i++) {
            if (attr[i] == 0) {
                iGain = computation.infoGain(i, data, fileContent, numOfAttrib);
                // //to be deleted
                // printf("%d %f\n",i,iGain);
                // //to be deleted
                if (iGain > maxIGain) {
                    // store maximum information gain value along with attribute
                    maxIGain = iGain;
                    splitAttr = i;
                }
            }
        }

	// //to be deleted
	// printf("\n");
	// //to be deleted
	if(maxIGain==INT_MIN){
		return -1;
	}
	// mark splitAttr as used
	attr[splitAttr]=1;
	return splitAttr;
}

// builder function for generating decision tree
// attr: candidate attributes for splitting attribute, attr[i]=1 if already used
// data: data row nos(in the file and index in "fileContent" vector) used for calculating information gains
void decision(vector<int> attr,vector<int> data,node *root)
{
	// //to be deleted
	// printf("Data Points:\n");
	// for(int i=0;i<data.size();i++){
	// 	printf("%d ",data[i]);
	// }
	// printf("\n");
	// //to be deleted
	int flag,selectedAttribute,i;
	if(data.size()==0){
		return;
	}
	flag=1;
	for(i=1;i<data.size();i++){
		if(fileContent[data[i]][numOfAttrib-1]!=fileContent[data[i-1]][numOfAttrib-1]){
			// flag = 0 if data contains more than one class value
			flag=0;
			break;
		}
	}
	// flag = 1 if all the data belong to the same class
	if(flag==1){
		// assign class value to node and return
		//root->val=fileContent[data[0]][numOfAttrib-1];
		root->setVal(fileContent[data[0]][numOfAttrib-1]);
		return;
	}
	// selectedAttribute : splitting attribute
	selectedAttribute=select(attr,data);
	//root->attribute = selectedAttribute;
	root->setAttribute(selectedAttribute);

	if(selectedAttribute == -1){
		// running out of attributes
        //root->val = popularVote(data);
		root->setVal(computation.popularVote(data, fileContent, numOfAttrib)) ;
		return;
	}

	// dividedData: divide data and store based on attribute values : map from attrVal to data elements
	map<int, vector <int> > dividedData;
	map<int, vector <int> >::iterator it;
	int attrVal;

	for(i=0;i<data.size();i++){
		attrVal = fileContent[data[i]][selectedAttribute];
		if(dividedData.find(attrVal) == dividedData.end()){
			// if attrVal not present as key in dividedData, then insert pair (attrVal,x), where x is a vector
			vector <int> x;
			x.push_back(data[i]);
			dividedData.insert(make_pair(attrVal,x));
		}
		else{
			// if attrVal is present, add "i" to the corresponding vector
			dividedData[attrVal].push_back(data[i]);
		}
	}
	// create and recurse on child nodes
	for(i=0,it=dividedData.begin();it!=dividedData.end();it++,i++){
		// create childNode and recurse on it
		//root->numOfChildren++;
		root->setNumOfChildren(root->getNumOfChildren()+1);
		node* childNode;
		childNode = new node();
		childNode->setBranchVal(it->first);
		root->setChild(childNode,i);

		decision(attr, it->second, childNode);
	}

}

// function for printing and debugging decision tree : bfs traversal
void printDecisionTree(node *root)
{
	printf("Printing decision tree:\n");
	queue <node> bfsQ;
	int x,j;
	node* nextNode;
	bfsQ.push(*root);
	cout << root->getAttribute() << endl;
	// implementing bfs traversal of tree
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

// function for testing decision tree
void test(node* root)
{
	int i,pos,neg,noResult,attr,attrVal,j,flag;
	node* temp;
	pos=0;
	neg=0;
	noResult=0;
	vector<vector<int>> testFileContent;

	readDS read = readDS();
	read.read(testingData, testFileContent);

	for(i=0;i<testFileContent.size();i++){
		temp=root;
		flag=0;
		//traverse decision tree
		while(temp->getVal() ==-1 && temp->getAttribute()!=-1){
			attr = temp->getAttribute();
			attrVal=testFileContent[i][attr];
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
		if(temp->getVal() == testFileContent[i][numOfAttrib-1]){
			// predicted value = actual value
			pos++;
		}
		else{
			// predicted value != actual value
			neg++;
		}
		if(temp->getVal() == -1 || flag==1){
			// no predicted value
			noResult++;
		}
	}
	cout << "Positive: " << pos << endl;
	cout << "Negative: " << neg << endl;
	cout << "No Result: " << noResult << endl;

	return;
}

int main()
{
	int i;
	//node* root;
    node* root = new node();

	computation = Computation();
    // vector to store row number for data in file
	vector <int> data;
	// vector to check if attribute has already been used or not
	vector <int> attr;

    readDS read = readDS();
	read.read(trainingData, fileContent);

	numOfAttrib = fileContent[0].size();
	numOfDataEle = fileContent.size();

	for(i=0;i<numOfDataEle;i++){
		data.push_back(i);
	}
	for(i=0;i<numOfAttrib;i++){
		attr.push_back(0);
	}

	// create decision tree
	//root = new node();

	double start = omp_get_wtime();

    #pragma omp parallel num_threads(4)
    {
        #pragma omp single
        {
            #pragma omp task
    	        decision(attr,data,root);
        };
    }

	double end = omp_get_wtime();

	//print decision tree
	//printDecisionTree(root);

	// test decision tree
	test(root);

	printf("Time taken:%f\n", end-start);

	return 0;
}