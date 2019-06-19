#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"
#include "ShaderProgram.h"


class Model
{
private:
	/*  Model Data  */
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	std::string m_directory;

	std::vector<TexturePointer> m_loadedTextures;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	/*  Functions   */
	void processNode(aiNode* node, const aiScene* scene);
	std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<TexturePointer> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
public:
	Model()
		: m_directory(""), m_meshes(std::vector<std::shared_ptr<Mesh>>()), m_loadedTextures(std::vector<TexturePointer>()) {};
	~Model() { std::cout << "[Model] destroyed" << std::endl; }

	void loadModel(const std::string& path);

	void Draw(std::shared_ptr<ShaderProgram> &shader);
};

