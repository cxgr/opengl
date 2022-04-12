#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"
#include "Constants.h"

class ThreeDModel
{
public:
	ThreeDModel();
	~ThreeDModel();

	void LoadModel(const std::string& fileName, bool smoothNormals = false);
	void ClearModel();

	void Render();

private:
	std::vector<Mesh*> meshes;
	std::vector<Texture*> textures;
	std::vector<unsigned> meshToTexLinks;

	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);
};

