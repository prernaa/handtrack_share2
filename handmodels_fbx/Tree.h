#pragma once
#include <QDebug>
#include <stdio.h>
class Tree{
    class Node{
        std::string nodeName;
        std::vector<Node*> children;
        double *translation;
        double *rotation;
        double *scaling;
    public:
        Node(std::string n, double *t, double *r, double *s){nodeName = n; translation=t; rotation=r; scaling=s;}
        std::string getNodeName(){return nodeName;}
        std::vector<Node*> getAllChildren(){ return children; }
        Node* getChildAt(int i){return children[i];}
        double* getTranslation(){return translation;}
        double* getRotation(){return rotation;}
        double* getScaling(){return scaling;}
        void insertChild(Node *node){children.push_back(node);}
    };

    Node *Root;
    int totalNumNodes;
public:
    Tree(){Root=NULL; totalNumNodes = 0;}
    Tree(Node *R){Root=R; totalNumNodes = 0;}
    Node* getRoot(){return Root;}
    int getTotalNumNodes(){ return totalNumNodes;}

    Node* findNodeGivenName(std::string name, Node *node){
        int numOfChildren = 0;
        if(node == NULL){return NULL;}
        else if(node->getNodeName()==name){return node;}
        else{
            numOfChildren = node->getAllChildren().size();
            if(numOfChildren == 0){return NULL;}
        }
        for(int i=0; i<numOfChildren; i++){
            Node *foundNode = findNodeGivenName(name, node->getChildAt(i));
            if(foundNode!=NULL){return foundNode;}
        }
        return NULL;
    }

    bool insertNode(std::string name, double translation[3], double rotation[3], double scaling[3], std::string parentName=""){
        if(name.empty() || name==""){return false;}
        if(findNodeGivenName(name, Root)!=NULL){return false;} // a node of the same name must NOT already exist
        qDebug()<<"proceed to insert node";
        Node *newNode = new Node(name, translation, rotation, scaling); // create the new node
        if(parentName.empty() && Root!=NULL){return false;}
        if(Root==NULL && (parentName.empty() || parentName=="")){ Root = newNode; totalNumNodes++; return true;} // if tree is empty, insert the new node
        else {
            Node* parentNode = findNodeGivenName(parentName, Root); // find the parent node
            if(parentNode==NULL){return false;} // parent node MUST EXIST
            else{
                parentNode->insertChild(newNode); // insert the new node as a child into the parent
                qDebug()<<"Node inserted";
                return true;
            }
        }
        return false;
    }
};
