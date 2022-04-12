#include "ThreeDModel.h"

ThreeDModel::ThreeDModel()
{
}

ThreeDModel::~ThreeDModel()
{
}

void ThreeDModel::LoadModel(const std::string& fileName, bool smoothNormals)
{
	Assimp::Importer importer;
	auto flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
	flags |= smoothNormals ? aiProcess_GenSmoothNormals : aiProcess_GenNormals;

	const aiScene* scene = importer.ReadFile(fileName, flags);

	if (!scene)
	{
		std::cout << "failed to load model: " << fileName << " " << importer.GetErrorString() << std::endl;
		return;
	}

	LoadNode(scene->mRootNode, scene);
	LoadMaterials(scene);
}

void ThreeDModel::ClearModel()
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		if (meshes[i])
		{
			delete meshes[i];
			meshes[i] = nullptr;
		}
	}

	for (size_t i = 0; i < textures.size(); ++i)
	{
		if (textures[i])
		{
			delete textures[i];
			textures[i] = nullptr;
		}
	}
}

void ThreeDModel::Render()
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto matIdx = meshToTexLinks[i];
		if (matIdx < textures.size() && textures[matIdx])
			textures[matIdx]->UseTexture();

		meshes[i]->RenderMesh();
	}
}

void ThreeDModel::LoadNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		auto nextMeshIdx = node->mMeshes[i];
		LoadMesh(scene->mMeshes[nextMeshIdx], scene);
	}

	for (size_t i = 0; i < node->mNumChildren; ++i)
		LoadNode(node->mChildren[i], scene);
}

void ThreeDModel::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<GLfloat> verts;
	std::vector<unsigned> inds;

	for (size_t i = 0; i < mesh->mNumVertices; ++i)
	{
		verts.insert(verts.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y , mesh->mVertices[i].z });

		if (mesh->mTextureCoords[0])
			verts.insert(verts.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		else
			verts.insert(verts.end(), { 0.f, 0.f });

		verts.insert(verts.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y , mesh->mNormals[i].z });
	}

	for (size_t i = 0; i < mesh->mNumFaces; ++i)
		for (size_t j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
			inds.push_back(mesh->mFaces[i].mIndices[j]);

	Mesh* newMesh = new Mesh();
	newMesh->CreateMesh(verts.data(), verts.size(), inds.data(), inds.size());
	meshes.push_back(newMesh);
	meshToTexLinks.push_back(mesh->mMaterialIndex);
}

void ThreeDModel::LoadMaterials(const aiScene* scene)
{
	textures.resize(scene->mNumMaterials);
	for (size_t i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* mat = scene->mMaterials[i];
		textures[i] = nullptr;
		if (mat->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (AI_SUCCESS == mat->GetTexture(aiTextureType_DIFFUSE, 0, &path))
			{
				auto cutoffIdx = std::string(path.data).rfind(PATH_SEPARATOR);
				std::string fileName = std::string(path.data).substr(cutoffIdx + 1);
				std::string texPath = std::string("Assets/") + fileName;
				textures[i] = new Texture(texPath.c_str());

				if (!textures[i]->LoadTexture(false))
				{
					delete textures[i];
					textures[i] = nullptr;
				}
			}
		}

		if (!textures[i])
		{
			textures[i] = new Texture("Assets/blank.png");
		}
	}
}
