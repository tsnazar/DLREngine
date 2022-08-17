#include "Model.h"

#include "windows/winapi.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "MathUtils.h"
#include "Ray.h"

namespace engine
{
	Model& Model::LoadFromFile(const std::string& filepath)
	{
		std::vector<VertexPosTexNorTanBitan> vertices;
		std::vector<uint32_t> indices;

		uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

		Assimp::Importer importer;
		const aiScene* assimpScene = importer.ReadFile(filepath, flags);
		ALWAYS_ASSERT(assimpScene != nullptr);

		uint32_t numMeshes = assimpScene->mNumMeshes;
		
		this->m_Submeshes.resize(numMeshes);
		this->m_Meshes.resize(numMeshes);

		static_assert(sizeof(DirectX::XMFLOAT3) == sizeof(aiVector3D), "");

		uint32_t verticesOffset = 0;
		uint32_t indicesOffset = 0;

		for (uint32_t i = 0; i < numMeshes; ++i)
		{
			auto& srcMesh = assimpScene->mMeshes[i];
			auto& dstMeshRange = m_Submeshes[i];
			auto& dstMesh = m_Meshes[i];
			
			auto& mat = assimpScene->mRootNode->mChildren[i]->mTransformation;
			dstMesh.box.min = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mAABB.mMin);
			dstMesh.box.max = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mAABB.mMax);
			dstMesh.meshToModel = reinterpret_cast<DirectX::XMFLOAT4X4&>(mat);
			DirectX::XMStoreFloat4x4(&dstMesh.modelToMesh, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&dstMesh.meshToModel)))));
			//DirectX::XMStoreFloat4x4(&dstMesh.modelToMesh, DirectX::XMMatrixIdentity());
			//DirectX::XMStoreFloat4x4(&dstMesh.meshToModel, DirectX::XMMatrixIdentity());


			uint32_t numVertices = srcMesh->mNumVertices;
			uint32_t numFaces = srcMesh->mNumFaces;

			dstMesh.vertices.resize(numVertices);
			dstMesh.triangles.resize(numFaces);

			dstMeshRange.vertexNum = numVertices;
			dstMeshRange.vertexOffset = verticesOffset;
			dstMeshRange.indexNum = numFaces * 3;
			dstMeshRange.indexOffset = indicesOffset;

			for (uint32_t v = 0; v < numVertices; ++v)
			{
				VertexPosTexNorTanBitan vertex;
				vertex.pos = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mVertices[v]);
				vertex.texCoord = reinterpret_cast<DirectX::XMFLOAT2&>(srcMesh->mTextureCoords[0][v]);
				vertex.nor = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mNormals[v]);
				vertex.tan = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mTangents[v]);
				vertex.bitan = reinterpret_cast<DirectX::XMFLOAT3&>(srcMesh->mBitangents[v]) * -1.0f;
				vertices.push_back(vertex);
				dstMesh.vertices[v] = vertex;
			}

			if (!m_VertexBufferOnly)
			{
				for (uint32_t f = 0; f < numFaces; ++f)
				{
					const auto& face = srcMesh->mFaces[f];
					ALWAYS_ASSERT(face.mNumIndices == 3);
					dstMesh.triangles[f] = *reinterpret_cast<Mesh::Triangle*>(face.mIndices);
					for (uint32_t i = 0; i < face.mNumIndices; ++i)
						indices.push_back(face.mIndices[i]);
				}
				indicesOffset += numFaces * 3;
			}

			dstMesh.UpdateOctree();
			
			verticesOffset += numVertices;
		}

		m_Vertices.Create<VertexPosTexNorTanBitan>(D3D11_USAGE_DYNAMIC, vertices.data(), vertices.size());
		
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
		const uint32_t numVertices = 36;

		m_VertexBufferOnly = true;

		m_Submeshes.clear();

		m_Submeshes.push_back(Model::SubMesh{  0, 0, numVertices, 0});

		m_Meshes.clear();
		m_Meshes.emplace_back();
		m_Meshes[0].meshToModel = { 1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1 };
		m_Meshes[0].modelToMesh = { 1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1 };

		const VertexPosTexNorTanBitan vertexData[] = {
			//front
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			//back
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.0f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			////right
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			////left
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			////bottom
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			////top
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},

			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}, DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}, DirectX::XMFLOAT3{0, 0, 0},  DirectX::XMFLOAT3{0, 0, 0}},
		};

		m_Meshes[0].vertices.assign(vertexData, vertexData + numVertices);
		m_Meshes[0].box = { {-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f} };
		m_Meshes[0].UpdateOctree();

		m_Vertices.Create<VertexPosTexNorTanBitan>(D3D11_USAGE_DYNAMIC, vertexData, numVertices);
	}

	void Model::InitUnitSphere()
	{
		const uint32_t SIDES = 6;
		const uint32_t GRID_SIZE = 12;
		const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
		const uint32_t VERT_PER_SIDE = 3 * TRIS_PER_SIDE;
		const uint32_t VERTEX_COUNT = VERT_PER_SIDE * SIDES;

		std::vector<VertexPosTexNorTanBitan> vertices;
		vertices.resize(VERTEX_COUNT);

		auto* vertex = vertices.data();

		m_VertexBufferOnly = true;

		m_Submeshes.clear();
		m_Submeshes.push_back(Model::SubMesh{ 0, 0, VERTEX_COUNT, 0 });

		m_Meshes.clear();
		m_Meshes.emplace_back();
		m_Meshes[0].meshToModel = { 1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1 };
		m_Meshes[0].modelToMesh = { 1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1 };

		const int sideMasks[6][3] =
		{
			{ 2, 1, 0 },
			{ 0, 1, 2 },
			{ 2, 1, 0 },
			{ 0, 1, 2 },
			{ 0, 2, 1 },
			{ 0, 2, 1 }
		};

		const float sideSigns[6][3] =
		{
			{ +1, +1, +1 },
			{ -1, +1, +1 },
			{ -1, +1, -1 },
			{ +1, +1, -1 },
			{ +1, -1, -1 },
			{ +1, +1, +1 }
		};

		for (int side = 0; side < SIDES; ++side)
		{
			for (int row = 0; row < GRID_SIZE; ++row)
			{
				for (int col = 0; col < GRID_SIZE; ++col)
				{
					float left = (col + 0) / float(GRID_SIZE) * 2.f - 1.f;
					float right = (col + 1) / float(GRID_SIZE) * 2.f - 1.f;
					float bottom = (row + 0) / float(GRID_SIZE) * 2.f - 1.f;
					float top = (row + 1) / float(GRID_SIZE) * 2.f - 1.f;

					DirectX::XMFLOAT3 quad[4] =
					{
						{ left, bottom, 1.f },
						{ right, bottom, 1.f },
						{ left, top, 1.f },
						{ right, top, 1.f }
					};

					vertex[0] = vertex[1] = vertex[2] = vertex[3] = VertexPosTexNorTanBitan{};

					auto setPos = [sideMasks, sideSigns](int side, VertexPosTexNorTanBitan& dst, const DirectX::XMFLOAT3& pos)
					{
						float* ptr = reinterpret_cast<float*>(&(dst.pos));
						ptr[sideMasks[side][0]] = pos.x * sideSigns[side][0];
						ptr[sideMasks[side][1]] = pos.y * sideSigns[side][1];
						ptr[sideMasks[side][2]] = pos.z * sideSigns[side][2];
						DirectX::XMStoreFloat3(&dst.pos, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dst.pos)));
					};

					setPos(side, vertex[0], quad[0]);
					setPos(side, vertex[1], quad[2]);
					setPos(side, vertex[2], quad[1]);

					{
						DirectX::XMVECTOR AB = DirectX::XMVectorSet(vertex[1].pos.x - vertex[0].pos.x, vertex[1].pos.y - vertex[0].pos.y, vertex[1].pos.z - vertex[0].pos.z, 0.0f);
						DirectX::XMVECTOR AC = DirectX::XMVectorSet(vertex[2].pos.x - vertex[0].pos.x, vertex[2].pos.y - vertex[0].pos.y, vertex[2].pos.z - vertex[0].pos.z, 0.0f);;
						DirectX::XMVECTOR norm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AC, AB));
						DirectX::XMStoreFloat3(&vertex[0].nor, norm);
						DirectX::XMStoreFloat3(&vertex[1].nor, norm);
						DirectX::XMStoreFloat3(&vertex[2].nor, norm);
					}

					vertex += 3;

					setPos(side, vertex[0], quad[1]);
					setPos(side, vertex[1], quad[2]);
					setPos(side, vertex[2], quad[3]);

					{
						DirectX::XMVECTOR AB = DirectX::XMVectorSet(vertex[1].pos.x - vertex[0].pos.x, vertex[1].pos.y - vertex[0].pos.y, vertex[1].pos.z - vertex[0].pos.z, 0.0f);
						DirectX::XMVECTOR AC = DirectX::XMVectorSet(vertex[2].pos.x - vertex[0].pos.x, vertex[2].pos.y - vertex[0].pos.y, vertex[2].pos.z - vertex[0].pos.z, 0.0f);;
						DirectX::XMVECTOR norm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AC, AB));
						DirectX::XMStoreFloat3(&vertex[0].nor, norm);
						DirectX::XMStoreFloat3(&vertex[1].nor, norm);
						DirectX::XMStoreFloat3(&vertex[2].nor, norm);
					}
					
					vertex += 3;
				}
			}
		}

		m_Meshes[0].vertices.assign(vertices.begin(), vertices.end());
		m_Meshes[0].box = { {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f} };
		m_Meshes[0].UpdateOctree();

		m_Vertices.Create<VertexPosTexNorTanBitan>(D3D11_USAGE_DYNAMIC, vertices.data(), VERTEX_COUNT);
	}
	
	bool Model::Intersect(const Ray& ray, MeshIntersection& intersection, const DirectX::XMFLOAT4X4& transform)
	{
		bool intersect = false;
		
		Ray meshRay;

		DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixInverse(nullptr,DirectX::XMLoadFloat4x4(&transform));
		DirectX::XMVECTOR origin = DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&ray.origin), 1.0f);
		DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&ray.direction);
		origin = DirectX::XMVector4Transform(origin, modelMatrix);
		direction = DirectX::XMVector4Transform(direction, modelMatrix);

		for (auto& mesh : m_Meshes)
		{
			DirectX::XMMATRIX meshMatrix = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&mesh.modelToMesh));
			DirectX::XMVECTOR meshOrigin = DirectX::XMVector4Transform(origin, meshMatrix);
			DirectX::XMVECTOR meshDirection = DirectX::XMVector4Transform(direction, meshMatrix);

			DirectX::XMStoreFloat3(&meshRay.origin, meshOrigin);
			DirectX::XMStoreFloat3(&meshRay.direction, meshDirection);

			intersect |= mesh.tree.intersect(meshRay, intersection);
		}

		return intersect;
	}
}