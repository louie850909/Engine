#pragma once

#include <string>
#include <map>
#include <memory>
#include "model.h"
#include "render.h"
#include <DirectXMath.h>

using namespace DirectX;

class ModelManager
{
private:
	ModelManager(){}
	~ModelManager(){}

public:
	static ModelManager& getInstance()
	{
			static ModelManager instance;
			return instance;
	}

	std::shared_ptr<skinned_mesh>	LoadSkinnedMesh(const char* filename, render* Render, bool triangulate,
													XMFLOAT3 pos = XMFLOAT3(0, 0, 0),
													XMFLOAT3 scl = XMFLOAT3(1, 1, 1),
													XMFLOAT3 rot = XMFLOAT3(0, 0, 0),
													float sampleRate = 0.0f);

	std::shared_ptr<static_mesh>	LoadStaticMesh(	const char* filename, render* Render, 
													XMFLOAT3 pos = XMFLOAT3(0, 0, 0),
													XMFLOAT3 scl = XMFLOAT3(1, 1, 1),
													XMFLOAT3 rot = XMFLOAT3(0, 0, 0), 
													bool textureInvert = false);

private:
	using skinned_mesh_map = std::map<std::string, std::shared_ptr<skinned_mesh>>;
	using static_mesh_map = std::map<std::string, std::shared_ptr<static_mesh>>;
	
	skinned_mesh_map	skinnedMeshes;
	static_mesh_map		staticMeshes;
};
