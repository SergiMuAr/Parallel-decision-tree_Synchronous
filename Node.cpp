//
// Created by dabi on 02/02/18.
//

#include "Node.h"

int Node::getNumOfChildren() const {
    return node.numOfChildren;
}

void Node::setNumOfChildren(int numOfChildren) {
    node.numOfChildren = numOfChildren;
}

int Node::getVal() const {
    return node.val;
}

void Node::setVal(int val) {
    node.val = val;
}

int Node::getBranchVal() const {
    return node.branchVal;
}

void Node::setBranchVal(int branchVal) {
    node.branchVal = branchVal;
}

int Node::getAttribute() const {
    return node.attribute;
}

void Node::setAttribute(int attribute) {
    node.attribute = attribute;
}

Node *const *Node::getChild() const {
    return node.child;
}

void Node::setChild(Node *child, int i) {
    node.child[i] = child;
}
Node::Node(const Node::innerNode &node) : node(node) {}

Node::Node() {
    node.numOfChildren = 0;
    node.attribute = -1;
    node.val = -1;
    node.branchVal = -1;
}
