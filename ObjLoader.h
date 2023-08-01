#pragma once
#include <iostream>
#include <fstream>
#include "Mesh.h"
#include "Material.h"
#include "DXIncludes.h"

// 
// This code is taken from/based off of the tutorial at http://www.rastertek.com/dx10tut08.html
// That tutorial writes the data to a file that is then read in by another class to make the meshes
// Two options:
//	1) use the same file format, modifying none of this code and instead writing code to read in the data from that format
//		- easier, but seems like a very lazy way to do things from our standpoint. would rather not take the easy way out unless we have to 
//	2) modify this code to, instead of writing the code to a file, return the data in the structures we already have defined
//		- makes the code "ours", doesn't require us to change much of the code we already have, and it's basically as efficient. .obj files are already
//			pretty small from what I understand, so it doesn't change much in terms of storage space, and we'll have to read the data in one way or another regardless of which approach we take
//
// Basic idea:
// - getVertices() function, takes in a file name for the obj file
// - goes through most of the provided code, but instead stores it in our own data structures (as Vertex-es)
// - returns an array of Vertex-es and an int for number of indexes that can go straight into a mesh
//


typedef struct
{
	float x, y, z;
} VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
} FaceType;

class ObjLoader
{
public:
	//ObjLoader(void);
	//~ObjLoader(void);

	//void GetModelFilename(char*);
	static bool LoadModel(char*, Mesh**, ID3D11Device*, ID3D11DeviceContext*, ID3D11SamplerState*);
	static bool ReadFileCounts(char*, int&, int&, int&, int&);
	static bool LoadDataStructures(char*, int, int, int, int, Mesh**, ID3D11Device*, ID3D11DeviceContext*, ID3D11SamplerState*);

	//int init();

	//Vertex* getVertices();

	//Vertex* vertices;
};