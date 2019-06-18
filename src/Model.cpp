#include "Model.h"


void Model::loadModel(const std::string &path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	m_directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	if (node->mNumMeshes > 0) std::cout << "[aiNode] Loading " << node->mNumMeshes << " meshes from: " << node->mName.C_Str() << std::endl;

	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TexturePointer> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		
		vertex.position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;

		vertex.normal = vector;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates? Assimp allows 8 different texture coordinates.
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	// Assimp's interface defined each mesh having an array of faces where each face represents a single primitive,
	// which in our case (due to the aiProcess_Triangulate option) are always triangles
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<TexturePointer> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<TexturePointer> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, Texture::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		GLenum texSlot = GL_TEXTURE0 + i;

		textures[i]->setTexSlot(texSlot);
	}

	return std::make_shared<Mesh>( vertices, indices, textures );
}

std::vector<TexturePointer> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<TexturePointer> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;

		for (unsigned int j = 0; j < m_loadedTextures.size(); j++)
		{
			std::string path = (std::string)str.C_Str();
			std::string cachedFilename = m_directory + "/" + path;

			std::string filename = m_loadedTextures[j]->getPath();

			if (std::strcmp(filename.c_str(), cachedFilename.c_str()) == 0)
			{
				textures.emplace_back(m_loadedTextures[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			std::string path = (std::string)str.C_Str();

			std::string filename = m_directory + "/" + path;

			TexturePointer texture = std::make_shared<Texture>(filename, typeName);

			std::cout << "[Texture] Loaded " << typeName << " from " << filename << std::endl;

			texture->load();

			textures.emplace_back(texture);

			m_loadedTextures.emplace_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return textures;
}

void Model::Draw(std::shared_ptr<ShaderProgram> &shader)
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i]->Draw(shader);
	}
}

