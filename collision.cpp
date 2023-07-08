#include "collision.h"

bool Collision::SphereVsSphere(const XMFLOAT3& pos1, float radius1, const XMFLOAT3& pos2, float radius2)
{
	XMVECTOR v1 = XMLoadFloat3(&pos1);
	XMVECTOR v2 = XMLoadFloat3(&pos2);
	XMVECTOR v = v1 - v2;
	float distance = XMVectorGetX(XMVector3Length(v));
	float radius = radius1 + radius2;
	return distance <= radius;
}

bool Collision::CylinderVsCylinder(const XMFLOAT3& pos1, float radius1, float height1, const XMFLOAT3& pos2, float radius2, float height2)
{
	XMVECTOR v1 = XMLoadFloat3(&pos1);
	XMVECTOR v2 = XMLoadFloat3(&pos2);
	XMVECTOR v = v1 - v2;
	float distance = XMVectorGetX(XMVector3Length(v));
	float radius = radius1 + radius2;
	float height = height1 + height2;
	return distance <= radius && abs(pos1.y - pos2.y) <= height;
}

bool Collision::RayVsStaticModel(const XMFLOAT3& start, const XMFLOAT3& end, const static_mesh* mesh, HitResult& result)
{
	XMVECTOR WorldStart = XMLoadFloat3(&start);
	XMVECTOR WorldEnd = XMLoadFloat3(&end);
	XMVECTOR WorldRayVec = XMVectorSubtract(WorldEnd, WorldStart);
	XMVECTOR WorldRayLength = XMVector3Length(WorldRayVec);

	result.Distance = XMVectorGetX(WorldRayLength);

	bool hit = false;

	for (uint32_t i = 0; i < mesh->indices.size(); i+=3)
	{
		// ポリゴンの頂点を取得
		XMVECTOR p0 = XMLoadFloat3(&mesh->vertices[mesh->indices[i]].position);
		XMVECTOR p1 = XMLoadFloat3(&mesh->vertices[mesh->indices[i + 1]].position);
		XMVECTOR p2 = XMLoadFloat3(&mesh->vertices[mesh->indices[i + 2]].position);

		// ワールド座標に変換
		XMFLOAT3 p0w, p1w, p2w;
		XMStoreFloat3(&p0w ,XMVector3TransformCoord(p0, mesh->world_matrix));
		XMStoreFloat3(&p1w, XMVector3TransformCoord(p1, mesh->world_matrix));
		XMStoreFloat3(&p2w, XMVector3TransformCoord(p2, mesh->world_matrix));

		// レイとポリゴンの衝突判定
		if (RayCast(p0w, p1w, p2w, start, end, &result.Pos, &result.Normal))
		{
			hit = true;
		}
	}

	return hit;
}

bool Collision::RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 start, XMFLOAT3 end, XMFLOAT3* hit, XMFLOAT3* normal)
{
	XMVECTOR	p0 = XMLoadFloat3(&xp0);
	XMVECTOR	p1 = XMLoadFloat3(&xp1);
	XMVECTOR	p2 = XMLoadFloat3(&xp2);
	XMVECTOR	pos0 = XMLoadFloat3(&start);
	XMVECTOR	pos1 = XMLoadFloat3(&end);

	XMVECTOR	nor;	// ポリゴンの法線
	XMVECTOR	vec1;
	XMVECTOR	vec2;
	float		d1, d2;

	// ポリゴンの外積をとって法線を求める
	vec1 = p1 - p0;
	vec2 = p2 - p0;
	nor = XMVector3Cross(vec1, vec2);
	nor = XMVector3Normalize(nor);
	XMStoreFloat3(normal, nor);

	// ポリゴン平面と線分の内積とって衝突している可能性を調べる（鋭角なら＋、鈍角ならー、直角なら０）
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	// 求めたポリゴンの法線と２つのベクトル（線分の両端とポリゴン上の任意の点）の内積とって衝突している可能性を調べる
	d1 = XMVectorGetX(XMVector3Dot(nor, vec1));
	d2 = XMVectorGetX(XMVector3Dot(nor, vec2));
	if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
	{
		// 両方の内積の符号が同じなら衝突していない
		return false;
	}

	// ポリゴンと線分の交点を求める
	d1 = (float)fabs(d1);	// 絶対値を求めている
	d2 = (float)fabs(d2);	// 絶対値を求めている
	float a = d1 / (d1 + d2);	// 内分比

	// 交点を求める
	XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0から交点へのベクトル
	XMVECTOR	p3 = p0 + vec3;							// 交点
	XMStoreFloat3(hit, p3);				// 求めた交点を入れておく

	// 求めた交点がポリゴンの中にあるか調べる
	// ポリゴンの各辺のベクトル
	XMVECTOR	v1 = p1 - p0;
	XMVECTOR	v2 = p2 - p1;
	XMVECTOR	v3 = p0 - p2;

	// 各頂点と交点とのベクトル
	XMVECTOR	v4 = p3 - p1;
	XMVECTOR	v5 = p3 - p2;
	XMVECTOR	v6 = p3 - p0;

	// 外積で各辺の法線を求めて、ポリゴンの法線との内積をとって符号をチェックする
	XMVECTOR	n1, n2, n3;

	n1 = XMVector3Cross(v1,v4);
	if (XMVectorGetX(XMVector3Dot(n1, nor)) < 0.0f) return false;	// 当たっていない

	n2 = XMVector3Cross(v5, v2);
	if (XMVectorGetX(XMVector3Dot(n2, nor)) < 0.0f) return false;	// 当たっていない

	n3 = XMVector3Cross(v6, v3);
	if (XMVectorGetX(XMVector3Dot(n3, nor)) < 0.0f) return false;	// 当たっていない

	return true;	// 当たっている！(hitには当たっている交点が入っている。normalには法線が入っている)
}
