//
// Created by dabi on 02/02/18.
//

#ifndef PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_NODE_H
#define PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_NODE_H




class Node {

private:

// node structure of the decision tree
// attribute: splitting attribute (= -1 if leaf node)
// val: class value at leaf node (= -1 if decision node)
// branchVal: make branch decision based on this value


    struct innerNode{
        int numOfChildren;
        int val;
        int branchVal;
        int attribute;
        struct Node *child[10];
    };

    struct innerNode node;

public:

    Node(const innerNode &node);

    Node();

    int getNumOfChildren() const;

    void setNumOfChildren(int numOfChildren);

    int getVal() const;

    void setVal(int val);

    int getBranchVal() const;

    void setBranchVal(int branchVal);

    int getAttribute() const;

    void setAttribute(int attribute);

    Node *const *getChild() const;

    void setChild(Node *child, int i);


};


#endif //PARALLEL_DECISION_TREE_CLASSIFIER_MASTER_NODE_H
