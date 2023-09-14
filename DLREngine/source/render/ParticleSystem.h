#pragma once
#include <DirectXMath.h>
#include <vector>
#include <algorithm>
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "Camera.h"
#include "MathUtils.h"
#include "TransformSystem.h"
#include "Sky.h"
#include "DepthTarget.h"

namespace engine
{
	class ParticleSystem
	{
	public:
		static const uint32_t MAX_PARTICLES_COUNT = 500;

		struct ShaderDescription
		{
			enum Bindings : uint32_t {
				SMOKE_TEXTURE = 0, LIGHTMAP1_TEXTURE = 1, LIGHTMAP2_TEXTURE = 2, DEPTH_TEXTURE = 3,
				SHADOWMAP_TEXTURE = 4, IRRADIANCE_TEXTURE = 5,INSTANCE_BUFFER = 0,
				TARGET_DIMENSIONS_CONSTANTS = 4,
				SHADOWMAP_MATRICES = 1, SHADOWMAP_DIMENSIONS = 3
			};
		};
	public:
		struct ParticleTextures
		{
			Texture2D* EMVA = nullptr;
			Texture2D* lightMapRLT = nullptr;
			Texture2D* lightMapBotBF = nullptr;

			ParticleTextures(){}
			ParticleTextures(Texture2D* EMVA, Texture2D* lightMapRLT, Texture2D* lightMapBotBF) 
				: EMVA(EMVA), lightMapRLT(lightMapRLT), lightMapBotBF(lightMapBotBF)
			{}

			bool IsValid() { return !(EMVA == nullptr || lightMapRLT == nullptr || lightMapBotBF == nullptr); }

		};

		struct Particle
		{
			DirectX::XMFLOAT3 pos;
			float lifeTime;
			DirectX::XMFLOAT4 tint;
			DirectX::XMFLOAT2 rot[2];
			DirectX::XMFLOAT2 size;
			float thickness;
		};

		struct SmokeEmitter
		{
			uint32_t transformId;
			float spawnRadius;
			float spawnRate;
			DirectX::XMFLOAT3 tint;
			float maxLifeTime;
			DirectX::XMFLOAT2 initialSize;
			DirectX::XMFLOAT2 maxSize;
			float prevDelta;

			std::vector<Particle> particles;

			SmokeEmitter(DirectX::XMFLOAT3 pos, float spawnRadius, float spawnRate, DirectX::XMFLOAT3 tint, float maxLifeTime, DirectX::XMFLOAT2 initialSize, DirectX::XMFLOAT2 maxSize)
				: spawnRadius(spawnRadius), spawnRate(spawnRate), tint(tint), maxLifeTime(maxLifeTime), initialSize(initialSize), maxSize(maxSize)
			{
				auto& transforms = TransformSystem::Get().GetTransforms();
				this->transformId = transforms.insert(TransformSystem::Transform{ pos, {0.1f, 0.1f, 0.1f}, {0,0,0} });
			};

			void Update(float dt)
			{
				for (auto& particle : particles)
				{
					particle.pos.y += dt;
					particle.lifeTime -= dt / maxLifeTime;
					particle.tint.w = 1 - std::abs(2 * particle.lifeTime - 1);
					particle.size.x = maxSize.x - (maxSize.x - initialSize.x) * particle.lifeTime;
					particle.size.y = maxSize.y - (maxSize.y - initialSize.y) * particle.lifeTime;
					particle.thickness = std::max(particle.size.x, particle.size.y) / 2.f;
				}

				std::sort(particles.begin(), particles.end(), [](Particle a, Particle b) {
					return a.lifeTime > b.lifeTime;
					});

				const auto& ptr = std::find_if(particles.begin(), particles.end(), [&](Particle p) { return p.lifeTime < 0.0f; });

				particles.erase(ptr, particles.end());

				auto& transforms = TransformSystem::Get().GetTransforms();
				DirectX::XMFLOAT3 pos = transforms[transformId].position;

				prevDelta += dt;
				uint32_t newParticlesCount = prevDelta / spawnRate;

				if (newParticlesCount > 0)
					prevDelta = 0;

				for (uint32_t i = 0; i < newParticlesCount && particles.size() < MAX_PARTICLES_COUNT; ++i)
				{
					Particle particle;
					particle.lifeTime = 1.0f;
					float angle = ((rand() % 360) * DirectX::XM_PI) / 180.f;
					float cos = cosf(angle);
					float sin = sinf(angle);
					particle.rot[0] = { cos, sin };
					particle.rot[1] = { -sin, cos };
					particle.pos = pos;
					particle.pos.x += spawnRadius * std::sqrtf((rand() % 100) / 100.f) * cos;
					particle.pos.z += spawnRadius * std::sqrtf((rand() % 100) / 100.f) * sin;
					particle.size = initialSize;
					particle.thickness = std::max(initialSize.x, initialSize.y) / 2.f;
					particle.tint = { tint.x,tint.y, tint.z, 0.0f };

					particles.push_back(particle);
				}
			}
		};

	public:
		static void Init();

		static void Fini();

		void SetShaders(Shader* forwardShader) { m_ForwardShader = forwardShader; }

		void SetTextures(ParticleTextures textures) { m_Textures = textures; }

		void Render(Sky::IblResources iblResources, Texture2D& depth, ConstantBuffer& dimensions);

		void Update(float dt, Camera& camera);

		void AddSmoke(const SmokeEmitter& smoke);

		static ParticleSystem& Get() { return *s_Instance; }

	private:
		VertexBuffer m_InstanceBuffer;

		std::vector<SmokeEmitter>  m_Emmiters;

		Shader* m_ForwardShader = nullptr;
		ParticleTextures m_Textures;
	private:
		static ParticleSystem* s_Instance;
	};
}