#pragma once
#include <stdio.h>
#include "Fbx_IO.h"
#include "SurfaceMesh/IO/Base.h"

class SurfaceMeshIOFbx : public SurfaceMeshIOBase, FbxIOHeader{
public:
    SurfaceMeshIOFbx(SurfaceMesh& mesh) : SurfaceMeshIOBase(mesh){}
    String extension(){ return "fbx"; }

    void read(const String& filename){
        bool initialize = initializeFBX(filename);
        bool retval = read_legacy();
        bool destroy = destroyFBX();
        if(!retval || !initialize || !destroy)
            throw SurfaceMeshIOException("FBX file open failed");

    }
    void write(const String& /*path*/){
        bool OBJ_WRITE_NOT_IMPLEMENTED = false;
        assert(OBJ_WRITE_NOT_IMPLEMENTED);
    }

protected:
    int read_legacy(){
        /// Getting the root note and the mesh
        FbxNode* rootNode = scene->GetRootNode();
        FbxNode* handNode = rootNode->FindChild("Hand");
        FbxMesh* meshFbx = handNode->GetMesh();

        /// Getting the vertices from the mesh
        int numVerts = 0;
        numVerts = meshFbx->GetControlPointsCount();
        qDebug()<<numVerts;
        for ( int j=0; j<numVerts; j++)
        {
            FbxVector4 vert = meshFbx->GetControlPointAt(j);
            mesh().add_vertex(Surface_mesh::Point((float)vert.mData[0],(float)vert.mData[1],(float)vert.mData[2]));
        }

        /// Now, we need to get polygon vertex indices
        std::vector<Surface_mesh::Vertex>  polyVerts;
        int numPoly = meshFbx->GetPolygonCount();
        int *allPolyVerts = meshFbx->GetPolygonVertices();
        int numVertsInPoly = 0;
        int polyStartIdx = 0;


        for(int i=0; i<numPoly; i++){ // for each polygon
            numVertsInPoly = meshFbx->GetPolygonSize(i); // number of vertices in each polygon
            polyStartIdx = meshFbx->GetPolygonVertexIndex(i); // the start index in allPolyVerts of the vertices of that particular polygon
            polyVerts.clear();
            for(int j=0; j<numVertsInPoly; j++){
                polyVerts.push_back(SurfaceMesh::Vertex(allPolyVerts[polyStartIdx+j]));
            }
            mesh().add_face(polyVerts);
            mesh().triangulate();
        }
    }
};
