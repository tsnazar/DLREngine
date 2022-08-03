#include "Model.h"

#include "windows/winapi.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace engine
{
	Model& Model::LoadFromFile(const std::string& filepath)
	{
		std::vector<VertexPosTexNorTan> vertices;
		std::vector<uint32_t> indices;

		uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

		Assimp::Importer importer;
		const aiScene* assimpScene = importer.ReadFile(filepath, flags);
		ALWAYS_ASSERT(assimpScene != nullptr);

		uint32_t numMeshes = assimpScene->mNumMeshes;
		
		this->m_Meshes.resize(numMeshes);

		static_assert(sizeof(DirectX::XMFLOAT3) == sizeof(aiVector3D), "");

		uint32_t verticesOffset = 0;
		uint32_t indicesOffset = 0;

		for (uint32_t i = 0; i < numMeshes; ++i)
		{
			auto& srcMesh = assimpScene->mMeshes[i];
			auto& dstMeshRange = m_Meshes[i];

			uint32_t numVertices = srcMesh->mNumVertices;
			uint32_t numFaces = srcMesh->mNumFaces;

			auto& mat = assimpScene->mRootNode->mChildren[i]->mTransformation;
			dstMeshRange.meshToModel = reinterpret_cast<DirectX::XMFLOAT4X4&>(mat);
			dstMeshRange.vertexNum = numVertices;
			dstMeshRange.vertexOffset = verticesOffset;
			dstMeshRange.indexNum = numFaces * 3;
			dstMeshRange.indexOffset = indicesOffset;

			for (uint32_t v = 0; v < numVertices; ++v)
			{
				VertexPosTexNorTan vertex;
				vertex.pos = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mVertices[v]);
				vertex.texCoord = reinterpret_cast<DirectX::XMFLOAT2&>(srcMesh->mTextureCoords[0][v]);
				vertex.nor = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mNormals[v]);
				vertex.tan = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mTangents[v]);
				vertices.push_back(vertex);
			}

			if (!m_VertexBufferOnly)
			{
				for (uint32_t f = 0; f < numFaces; ++f)
				{
					const auto& face = srcMesh->mFaces[f];
					ALWAYS_ASSERT(face.mNumIndices == 3);
					for (uint32_t i = 0; i < face.mNumIndices; ++i)
						indices.push_back(face.mIndices[i]);
				}
				indicesOffset += numFaces * 3;
			}
			
			verticesOffset += numVertices;
		}

		m_Vertices.Create<VertexPosTexNorTan>(D3D11_USAGE_DYNAMIC, vertices.data(), vertices.size());
		
		if(!m_VertexBufferOnly)
			m_Indices.Create<uint32_t>(D3D11_USAGE_DYNAMIC, indices.data(), indices.size());

		return *this;
	}

	void Model::Bind(uint32_t slot)
	{
		m_Vertices.SetBuffer(slot);
		
		if(!m_VertexBufferOnly)
			m_Indices.Bind();
	}

	void Model::InitUnitCube()
	{
		m_VertexBufferOnly = true;

		m_Meshes.clear();

		m_Meshes.push_back(Model::SubMesh{ { 1, 0, 0, 0,
											0, 1, 0, 0,
											0, 0, 1, 0,
											0, 0, 0, 1 }, 0, 0, 36, 0});

		const VertexPosTexNorTan vertexData[] = {
			//back
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			//front
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.0f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			//right
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			//left
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			//bottom
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			//top
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0}},
		};

		m_Vertices.Create<VertexPosTexNorTan>(D3D11_USAGE_DYNAMIC, vertexData, 36);
	}
}