#pragma once

#include <DirectXMath.h>
#include "model.h"
#include "game_object.h"
#include "stage.h"

using namespace DirectX;

struct HitResult
{
	XMFLOAT3 Pos{ 0,0,0 };
	XMFLOAT3 Normal{ 0,0,0 };
	float Distance{ 0 };
	int materialIndex{ -1 };
};

struct OctreeNode
{
	DirectX::XMFLOAT3 center; // Center of the node
	float size;               // Length of the sides of the node
	OctreeNode* children[8];  // Child nodes
	std::vector<GAME_OBJECT::vertex> vertices; // Vertices in this octant

	// Initialize with center and size
	OctreeNode(DirectX::XMFLOAT3 center, float size)
		: center(center), size(size)
	{
		// Initialize children to null
		for (int i = 0; i < 8; i++)
			children[i] = nullptr;
	}
};

class Collision
{

public:
	// 球と球の当たり判定
	static bool SphereVsSphere(const XMFLOAT3& pos1, float radius1, const XMFLOAT3& pos2, float radius2);

	// 円柱と円柱の当たり判定
	static bool CylinderVsCylinder(const XMFLOAT3& pos1, float radius1, float height1, const XMFLOAT3& pos2, float radius2, float height2);

	// レイとモデルの当たり判定
	static bool RayVsStaticModel(	const XMFLOAT3& start, 
									const XMFLOAT3& end,
									const static_mesh* mesh,
									HitResult& result);

	// レイとモデルの当たり判定
	static bool RayVsSkinnedModel(	const XMFLOAT3& start,
									const XMFLOAT3& end,
									const skinned_mesh* mesh,
									HitResult& result);

	// レイとステージの当たり判定
	static bool VsStage(const XMFLOAT3& start,
						const XMFLOAT3& end,
						const std::vector<triangle>& triangles,
						HitResult& result);


	// レイと三角形の当たり判定
	static bool RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 start, XMFLOAT3 end, XMFLOAT3* hit, XMFLOAT3* normal);
};