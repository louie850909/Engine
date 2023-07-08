#pragma once

#include <DirectXMath.h>
#include "model.h"

using namespace DirectX;

struct HitResult
{
	XMFLOAT3 Pos{ 0,0,0 };
	XMFLOAT3 Normal{ 0,0,0 };
	float Distance{ 0 };
	int materialIndex{ -1 };
};

class Collision
{
public:
	// ‹…‚Æ‹…‚Ì“–‚½‚è”»’è
	static bool SphereVsSphere(const XMFLOAT3& pos1, float radius1, const XMFLOAT3& pos2, float radius2);

	// ‰~’Œ‚Æ‰~’Œ‚Ì“–‚½‚è”»’è
	static bool CylinderVsCylinder(const XMFLOAT3& pos1, float radius1, float height1, const XMFLOAT3& pos2, float radius2, float height2);

	// ƒŒƒC‚Æƒ‚ƒfƒ‹‚Ì“–‚½‚è”»’è
	static bool RayVsStaticModel(	const XMFLOAT3& start, 
									const XMFLOAT3& end,
									const static_mesh* mesh,
									HitResult& result);

	// ƒŒƒC‚ÆŽOŠpŒ`‚Ì“–‚½‚è”»’è
	static bool RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 start, XMFLOAT3 end, XMFLOAT3* hit, XMFLOAT3* normal);
};