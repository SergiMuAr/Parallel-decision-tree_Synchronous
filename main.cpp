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

/*
#define trainingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Balloons/balloons-data.int.txt"
#define testingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Balloons/balloons-test.int.txt"
*/


#define trainingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Car/car-data.int.txt"
#define testingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Car/car-test.int.txt"


/*
#define trainingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/hayes-roth.data/hayes-roth.data.txt"
#define testingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/hayes-roth.data/hayes-roth.test.txt"
*/

/*
#define trainingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Nursery/nursery-data.int.txt"
#define testingData "/home/dabi/experiments/Parallel-Decision-Tree-Classifier-master/Nursery/nursery-test.int.txt"
*/
using namespace std;

vector <vector <int> > fileContent;

int numOfAttrib, numOfDataEle;


typedef struct Node node;

// function to calculate entropy
double entropy(vector <double> counts)
{
	double total,entropy;
	int i;

	total=0;
	for(i=0;i<counts.size();i++){
		total+=counts[i];
	}

	entropy=0;
	// Entropy E = (a/(a+b+...))*(log2(a/(a+b+...))) + (b/(a+b+...))*(log2(b/(a+b+...))) + ...
	for(i=0;i<counts.size();i++) {
		entropy += (counts[i]/total)*(log2(counts[i]/total));
	}
	return -1 * entropy;
}

// function to get information gain of training data
double getInfoGainOfData(vector <int> data)
{
	int i,classVal;
	// classCount: keeps a count of the number of data points belonging to a particular class
	map<int, int> classCount;
	map<int, int>::iterator it;
	// counts: store all the counts of all the classes, used for calculating entropy
	vector<double> counts;
	for(i=0;i<data.size();i++){
		classVal = fileContent[data[i]][numOfAttrib-1];
		// result->second = false if insert failed
		// insert operation fails if "classVal" key already present in map "classCount"
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

// function to calculate information gain
// attr: attribute for which gin must be calculated
// data: data row nos(in the file and index in "fileContent" vector) used for calculating information gains
double infoGain(int attr,vector <int> data)
{
	int i,branchVal,dataSize,subDataValue;
	double attrInfoGain;
	// branchCount: count of each attribute value
	map<int, int> branchCount;
	map<int, int>::iterator branchCountIT;
	// dataElements[i]: vector containing all data elements having attribute value "i"
	map<int, vector<int> > dataElements;
	for(i=0;i<data.size();i++){
		branchVal = fileContent[data[i]][attr];
		if(branchCount.find(branchVal) == branchCount.end()){
			// if branchCount does not contain the key branchVal, then insert the pair(branchVal,1)
			branchCount.insert(make_pair(branchVal,1));
			vector <int> x;
			x.push_back(data[i]);
			// add "i" to the vector containing all data elements whose attribute value is branchVal
			dataElements.insert(make_pair(branchVal,x));
		}
		else{
			branchCount[branchVal]++;
			dataElements[branchVal].push_back(data[i]);
		}
	}
	attrInfoGain=0;
	dataSize=data.size();
	for(branchCountIT = branchCount.begin();branchCountIT!=branchCount.end();branchCountIT++){
		vector <int> subData = dataElements[branchCountIT->first];
		// subDataCounts: contains count of data elements belonging to the different output classes
		map <int, int> subDataCounts;
		map <int, int>::iterator subDataCountsIT;
		for(i=0;i<subData.size();i++){
			subDataValue = fileContent[subData[i]][numOfAttrib-1];
			if(subDataCounts.find(subDataValue) == subDataCounts.end()){
				// if subDataCounts does not contain subDataValue as key, then insert the pair(subDataValue,1)
				subDataCounts.insert(make_pair(subDataValue,1));
			}
			else{
				// if it contains the key, then increment count
				subDataCounts[subDataValue]++;
			}
		}
		// subDataCountsArr: contains all counts of each output class value
		vector <double> subDataCountsArr;
		for(subDataCountsIT=subDataCounts.begin();subDataCountsIT!=subDataCounts.end();subDataCountsIT++){
			subDataCountsArr.push_back((double)subDataCountsIT->second);
		}
		attrInfoGain+= ((double)branchCountIT->second/(double)dataSize)*entropy(subDataCountsArr);
	}
	return getInfoGainOfData(data) - attrInfoGain;
}

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
                iGain = infoGain(i, data);
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

// function for returning most probable output class
int popularVote(vector<int> data)
{
	int i,outputClass,ans,maxVal;
	// dataCount: keeps count of each output class in data vector
	map <int, int> dataCount;
	map <int, int>::iterator it;
	for(i=0;i<data.size();i++){
		outputClass = fileContent[data[i]][numOfAttrib-1];
		if(dataCount.find(outputClass) == dataCount.end()){
			// if outputClass not present as key, insert pair(outputClass, 1)
			dataCount.insert(make_pair(outputClass,1));
		}
		else{
			dataCount[outputClass]++;
		}
	}
	maxVal = INT_MIN;
	// ans contains popularVote
	for(it=dataCount.begin();it!=dataCount.end();it++){
		if(it->second > maxVal){
			ans=it->first;
		}
	}
	return ans;
}

int popularVote2(vector<int> data)
{
	int i,outputClass,ans,maxVal;
	maxVal = INT_MIN;
	ans = fileContent[data[0]][numOfAttrib-1];
	// dataCount: keeps count of each output class in data vector
	map <int, int> dataCount;
	map <int, int>::iterator it;
	for(i=0;i<data.size();i++){
		outputClass = fileContent[data[i]][numOfAttrib-1];
		if(dataCount.find(outputClass) == dataCount.end()){
			// if outputClass not present as key, insert pair(outputClass, 1)
			dataCount.insert(make_pair(outputClass,1));
		}
		else{
			dataCount[outputClass]++;
			if (dataCount[outputClass] > maxVal) ans = dataCount[outputClass];
		}
	}
	return ans;
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
		root->setVal(popularVote2(data)) ;
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
	printDecisionTree(root);

	// test decision tree
	//test(root);

	printf("Time taken:%f\n", end-start);

	return 0;
}