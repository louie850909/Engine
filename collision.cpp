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
		// �|���S���̒��_���擾
		XMVECTOR p0 = XMLoadFloat3(&mesh->vertices[mesh->indices[i]].position);
		XMVECTOR p1 = XMLoadFloat3(&mesh->vertices[mesh->indices[i + 1]].position);
		XMVECTOR p2 = XMLoadFloat3(&mesh->vertices[mesh->indices[i + 2]].position);

		// ���[���h���W�ɕϊ�
		XMFLOAT3 p0w, p1w, p2w;
		XMStoreFloat3(&p0w ,XMVector3TransformCoord(p0, mesh->world_matrix));
		XMStoreFloat3(&p1w, XMVector3TransformCoord(p1, mesh->world_matrix));
		XMStoreFloat3(&p2w, XMVector3TransformCoord(p2, mesh->world_matrix));

		// ���C�ƃ|���S���̏Փ˔���
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

	XMVECTOR	nor;	// �|���S���̖@��
	XMVECTOR	vec1;
	XMVECTOR	vec2;
	float		d1, d2;

	// �|���S���̊O�ς��Ƃ��Ė@�������߂�
	vec1 = p1 - p0;
	vec2 = p2 - p0;
	nor = XMVector3Cross(vec1, vec2);
	nor = XMVector3Normalize(nor);
	XMStoreFloat3(normal, nor);

	// �|���S�����ʂƐ����̓��ςƂ��ďՓ˂��Ă���\���𒲂ׂ�i�s�p�Ȃ�{�A�݊p�Ȃ�[�A���p�Ȃ�O�j
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	// ���߂��|���S���̖@���ƂQ�̃x�N�g���i�����̗��[�ƃ|���S����̔C�ӂ̓_�j�̓��ςƂ��ďՓ˂��Ă���\���𒲂ׂ�
	d1 = XMVectorGetX(XMVector3Dot(nor, vec1));
	d2 = XMVectorGetX(XMVector3Dot(nor, vec2));
	if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
	{
		// �����̓��ς̕����������Ȃ�Փ˂��Ă��Ȃ�
		return false;
	}

	// �|���S���Ɛ����̌�_�����߂�
	d1 = (float)fabs(d1);	// ��Βl�����߂Ă���
	d2 = (float)fabs(d2);	// ��Βl�����߂Ă���
	float a = d1 / (d1 + d2);	// ������

	// ��_�����߂�
	XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0�����_�ւ̃x�N�g��
	XMVECTOR	p3 = p0 + vec3;							// ��_
	XMStoreFloat3(hit, p3);				// ���߂���_�����Ă���

	// ���߂���_���|���S���̒��ɂ��邩���ׂ�
	// �|���S���̊e�ӂ̃x�N�g��
	XMVECTOR	v1 = p1 - p0;
	XMVECTOR	v2 = p2 - p1;
	XMVECTOR	v3 = p0 - p2;

	// �e���_�ƌ�_�Ƃ̃x�N�g��
	XMVECTOR	v4 = p3 - p1;
	XMVECTOR	v5 = p3 - p2;
	XMVECTOR	v6 = p3 - p0;

	// �O�ςŊe�ӂ̖@�������߂āA�|���S���̖@���Ƃ̓��ς��Ƃ��ĕ������`�F�b�N����
	XMVECTOR	n1, n2, n3;

	n1 = XMVector3Cross(v1,v4);
	if (XMVectorGetX(XMVector3Dot(n1, nor)) < 0.0f) return false;	// �������Ă��Ȃ�

	n2 = XMVector3Cross(v5, v2);
	if (XMVectorGetX(XMVector3Dot(n2, nor)) < 0.0f) return false;	// �������Ă��Ȃ�

	n3 = XMVector3Cross(v6, v3);
	if (XMVectorGetX(XMVector3Dot(n3, nor)) < 0.0f) return false;	// �������Ă��Ȃ�

	return true;	// �������Ă���I(hit�ɂ͓������Ă����_�������Ă���Bnormal�ɂ͖@���������Ă���)
}
