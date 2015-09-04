#pragma once
#include <stdio.h>
#include "Fbx_IO.h"
#include "Tree.h"
class SkeletonIOFbx : public FbxIOHeader{
private:
    Tree *skeletonTree;
public:
    SkeletonIOFbx(Tree *t){
        skeletonTree = t;
    }

    void read(const String& filename){
        bool initialize = initializeFBX(filename);
        bool retval = read_legacy();
        bool destroy = destroyFBX();
        if(!retval || !initialize || !destroy)
            qDebug()<<"FBX file open failed";

    }

    void write(const String& /*path*/){
        bool OBJ_WRITE_NOT_IMPLEMENTED = false;
        assert(OBJ_WRITE_NOT_IMPLEMENTED);
    }

    bool read_legacy(){
        /// Now, we have to store each joint as a node in a tree
        /// This can be done recursively too, but since the nodes are known, I am doing it without recursion
        double translation[3], rotation[3], scaling[3];

        /// Getting the root note and the mesh
        FbxNode* rootNode = scene->GetRootNode();
#if 0

        FbxNode* handNode = rootNode->FindChild("Hand");
        FbxMesh* meshFbx = handNode->GetMesh();
        FbxGeometry* geo = handNode->GetGeometry();

        qDebug()<<"Deformers in mesh:"<<meshFbx->GetDeformerCount();
        qDebug()<<geo->GetDeformer(0);
#endif

#if 0
        /// lCylinder
        FbxMesh* lCylinder = (FbxMesh*) scene->GetSrcObject(FBX_TYPE(FbxMesh), 0);
        FbxSkin* lSkin = (FbxSkin *) lCylinder->GetDeformer(0, FbxDeformer::eSkin);
        //FbxArray<FbxCluster*> lClusterArray;
        for (int lClusterIndex = 0; lClusterIndex < lSkin->GetClusterCount(); ++lClusterIndex)
        {
            FbxCluster* lCluster=lSkin->GetCluster(lClusterIndex);
            qDebug()<<lCluster->GetLinkMode();
            FbxNode *associatedNode = lCluster->GetAssociateModel();
            //qDebug()<<associatedNode->GetName();
            //lClusterArray.Add(lCluster);
        }
        //double *lControl01= lSkin->GetControlPointBlendWeights();
        //qDebug()<<lSkin->GetControlPointIndicesCount(); //0
        //qDebug()<<lClusterArray.Size(); // 17
        /// We have 17 clusters. Lets try and get the control points from each cluster
        /// Now, lets traverse through every cluster and fetch the name of the associated node
#endif




        FbxNode *rigNode = rootNode->FindChild("HandRig");
        qDebug()<<rigNode->GetName();
        //no parent name required in INSERT function, since this is the root node.
        convertFbxdouble3ToArray(translation, rigNode->GeometricTranslation.Get());
        convertFbxdouble3ToArray(rotation, rigNode->GeometricRotation.Get());
        // For some reason, the same thing doesn't work for Geometric scaling. However, scaling is (1.000000, 1.000000, 1.000000) for each & every node!
        //convertFbxdouble3ToArray(scaling, rigNode->GeometricScaling.Get());
        scaling[0]=1.000000; scaling[1]=1.000000; scaling[2]=1.000000;

        skeletonTree->insertNode(std::string(rigNode->GetName()), &translation[0], &rotation[0], &scaling[0]);


        FbxNode *lowerArm = rigNode->GetChild(0);
        qDebug()<<lowerArm->GetName();
        convertFbxdouble3ToArray(translation, lowerArm->GeometricTranslation.Get());
        convertFbxdouble3ToArray(rotation, lowerArm->GeometricRotation.Get());
        skeletonTree->insertNode(std::string(lowerArm->GetName()), &translation[0], &rotation[0], &scaling[0], std::string(rigNode->GetName()));

        FbxNode *hand_0 = lowerArm->GetChild(0);
        qDebug()<<hand_0->GetName();
        convertFbxdouble3ToArray(translation, hand_0->GeometricTranslation.Get());
        convertFbxdouble3ToArray(rotation, hand_0->GeometricRotation.Get());
        skeletonTree->insertNode(std::string(hand_0->GetName()), &translation[0], &rotation[0], &scaling[0], std::string(lowerArm->GetName()));

        for(int i=0; i<hand_0->GetChildCount(); i++){
            FbxNode *child = hand_0->GetChild(i);
            qDebug()<<child->GetName();
            convertFbxdouble3ToArray(translation, child->GeometricTranslation.Get());
            convertFbxdouble3ToArray(rotation, child->GeometricRotation.Get());
            skeletonTree->insertNode(std::string(child->GetName()), &translation[0], &rotation[0], &scaling[0], std::string(hand_0->GetName()));

            /// Every child has 2 children/bones
            FbxNode *joint1 = child->GetChild(0);
            qDebug()<<joint1->GetName();
            convertFbxdouble3ToArray(translation, joint1->GeometricTranslation.Get());
            convertFbxdouble3ToArray(rotation, joint1->GeometricRotation.Get());
            skeletonTree->insertNode(std::string(joint1->GetName()), &translation[0], &rotation[0], &scaling[0], std::string(child->GetName()));

            FbxNode *joint2 = child->GetChild(0);
            qDebug()<<joint2->GetName();
            convertFbxdouble3ToArray(translation, joint2->GeometricTranslation.Get());
            convertFbxdouble3ToArray(rotation, joint2->GeometricRotation.Get());
            skeletonTree->insertNode(std::string(joint2->GetName()), &translation[0], &rotation[0], &scaling[0], std::string(child->GetName()));

        }
        qDebug()<<"Tree Loaded!";
#if 0
        FbxSkeleton *skeleton = rigNode->GetSkeleton();
        if(skeleton->GetSkeletonType() == FbxSkeleton::eLimb){
            qDebug()<<"yes";
        }
#endif

        return true;
    }
};
