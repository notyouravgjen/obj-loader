#include "ObjLoader.h"
#include "DebugOutput.h"
using namespace std;

bool ObjLoader::LoadModel(char* filename, Mesh** mesh, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11SamplerState* ss)
{
	int vertexCount, textureCount, normalCount, faceCount;
	bool result;

	// Read in the number of vertices, tex coords, normals, and faces so that the data structures can be initialized with the exact sizes needed.
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if(!result)
	{
		return false;
	}

	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount, mesh, device, deviceContext, ss);
	if(!result)
	{
		return false;
	}

	return true;
}

bool ObjLoader::ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;

	// Initialize the counts.
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// Open the file.
	fin.open(filename);
	
	// Check if it was successful in opening the file.
	if(!fin.good())
	{
		// If the file does not exist or there was an issue opening it then notify the debug.
		fin.clear();

		DebugOutput::debugLog("Error opening model: " + *filename);

		return false;
	}
	if(fin.fail() == true)
	{
		// If the file does not exist or there was an issue opening it then notify the debug.
		fin.clear();

		DebugOutput::debugLog("Error opening model: " + *filename);

		return false;
	}

	// Read from the file and get that first char.
	fin.get(input);

	// and continue to read until the end of the file is reached.
	while(!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ') { vertexCount++; }
			if(input == 't') { textureCount++; }
			if(input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if(input == 'f')
		{
			fin.get(input);
			if(input == ' ') { faceCount++; }
		}
		
		// Otherwise read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return true;
}

bool ObjLoader::LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount, Mesh** mesh, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11SamplerState* ss)
{
	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	//ofstream fout;


	// Initialize the four data structures.
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	texcoords = new VertexType[textureCount];
	if(!texcoords)
	{
		return false;
	}

	normals = new VertexType[normalCount];
	if(!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if(!faces)
	{
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if(!fin.good())
	{
		// If the file does not exist or there was an issue opening it then notify the debug.
		fin.clear();

		DebugOutput::debugLog("Error opening model: " + *filename);

		return false;
	}
	if(fin.fail() == true)
	{
		// If the file does not exist or there was an issue opening it then notify the debug.
		fin.clear();

		DebugOutput::debugLog("Error opening model: " + *filename);

		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if(input == ' ') 
			{ 
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++; 
			}

			// Read in the texture uv coordinates.
			if(input == 't') 
			{ 
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++; 
			}

			// Read in the normals.
			if(input == 'n') 
			{ 
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++; 
			}
		}

		// Read in the faces.
		if(input == 'f') 
		{
			fin.get(input);
			if(input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				/*fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
				    >> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
				    >> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;*/

				// The big fix we've been looking for: don't read it in backwards
				fin >> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3;

				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	Vertex* outVertices = new Vertex[faceCount * 3];
	UINT* outIndices = new UINT[faceCount * 3];

	int indexIndex = 0;

	DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// Now loop through all the faces and output the three vertices for each face.
	for(int i=0; i<faceIndex; i++)
	{
		faces[i].vIndex1 -= 1;
		faces[i].tIndex1 -= 1;
		faces[i].nIndex1 -= 1;
		faces[i].vIndex2 -= 1;
		faces[i].tIndex2 -= 1;
		faces[i].nIndex2 -= 1;
		faces[i].vIndex3 -= 1;
		faces[i].tIndex3 -= 1;
		faces[i].nIndex3 -= 1;

		outIndices[indexIndex] = indexIndex;
		outVertices[indexIndex].Position = DirectX::XMFLOAT3(vertices[faces[i].vIndex3].x, vertices[faces[i].vIndex3].y, vertices[faces[i].vIndex3].z);
		outVertices[indexIndex].Color = red;
		outVertices[indexIndex].UVPosition = DirectX::XMFLOAT2(texcoords[faces[i].tIndex3].x, texcoords[faces[i].tIndex3].y);
		outVertices[indexIndex].Normal = DirectX::XMFLOAT3(normals[faces[i].nIndex3].x, normals[faces[i].nIndex3].y, normals[faces[i].nIndex3].z);
		//std::string sint = std::to_string(outIndices[indexIndex]);
		//DebugOutput::debugLog("Index1: " + sint);
		indexIndex++;
		outIndices[indexIndex] = indexIndex;
		outVertices[indexIndex].Position = DirectX::XMFLOAT3(vertices[faces[i].vIndex2].x, vertices[faces[i].vIndex2].y, vertices[faces[i].vIndex2].z);
		outVertices[indexIndex].Color = red;
		outVertices[indexIndex].UVPosition = DirectX::XMFLOAT2(texcoords[faces[i].tIndex2].x, texcoords[faces[i].tIndex2].y);
		outVertices[indexIndex].Normal = DirectX::XMFLOAT3(normals[faces[i].nIndex2].x, normals[faces[i].nIndex2].y, normals[faces[i].nIndex2].z);
		//sint = std::to_string(outIndices[indexIndex]);
		//DebugOutput::debugLog("Index2: " + sint);
		indexIndex++;
		outIndices[indexIndex] = indexIndex;
		outVertices[indexIndex].Position = DirectX::XMFLOAT3(vertices[faces[i].vIndex1].x, vertices[faces[i].vIndex1].y, vertices[faces[i].vIndex1].z);
		outVertices[indexIndex].Color = red;
		outVertices[indexIndex].UVPosition = DirectX::XMFLOAT2(texcoords[faces[i].tIndex1].x, texcoords[faces[i].tIndex1].y);
		outVertices[indexIndex].Normal = DirectX::XMFLOAT3(normals[faces[i].nIndex1].x, normals[faces[i].nIndex1].y, normals[faces[i].nIndex1].z);
		//sint = std::to_string(outIndices[indexIndex]);
		//DebugOutput::debugLog("Index3: " + sint);
		indexIndex++;
	}

	*mesh = new Mesh(outVertices, outIndices, faceCount * 3, faceCount * 3);
	(*mesh)->createBuffers(device, deviceContext);

	// These should be deleted because the Mesh's constructor allocates memory and makes its own copy of them
	delete outVertices;
	delete outIndices;

	// Release the four data structures.
	if(vertices)
	{
		delete [] vertices;
		vertices = 0;
	}
	if(texcoords)
	{
		delete [] texcoords;
		texcoords = 0;
	}
	if(normals)
	{
		delete [] normals;
		normals = 0;
	}
	if(faces)
	{
		delete [] faces;
		faces = 0;
	}

	return true;
}