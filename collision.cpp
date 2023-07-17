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
	/*float length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&start) - XMLoadFloat3(&result.Pos)));
	float rayLengthsqr = length * length;*/

	for (uint32_t i = 0; i < mesh->subsets.size(); i++)
	{
		for (uint32_t j = 0; j < mesh->subsets.at(i).index_count; j+=3)
		{
			// ���_�C���f�b�N�X���擾
			uint32_t index0 = mesh->indices.at(mesh->subsets.at(i).index_start + j + 0);
			uint32_t index1 = mesh->indices.at(mesh->subsets.at(i).index_start + j + 1);
			uint32_t index2 = mesh->indices.at(mesh->subsets.at(i).index_start + j + 2);

			// ���_���W���擾
			XMFLOAT3 p0w = mesh->vertices_world.at(index0).position;
			XMFLOAT3 p1w = mesh->vertices_world.at(index1).position;
			XMFLOAT3 p2w = mesh->vertices_world.at(index2).position;

			float lengthToV0 = powf(p0w.x - start.x, 2) + powf(p0w.y - start.y, 2) + powf(p0w.z - start.z, 2);
			float lengthToV1 = powf(p1w.x - start.x, 2) + powf(p1w.y - start.y, 2) + powf(p1w.z - start.z, 2);
			float lengthToV2 = powf(p2w.x - start.x, 2) + powf(p2w.y - start.y, 2) + powf(p2w.z - start.z, 2);

			//// ���_���烌�C�܂ł̋��������C�̒�����蒷���ꍇ�͓�����Ȃ�
			//if (lengthToV0 > rayLengthsqr || lengthToV1 > rayLengthsqr || lengthToV2 > rayLengthsqr)
			//{
			//	continue;
			//}

			// ���C�ƎO�p�`�̓����蔻��
			if (RayCast(p0w, p1w, p2w, start, end, &result.Pos, &result.Normal))
			{
				result.Distance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&start) - XMLoadFloat3(&result.Pos)));
				return true;
			}
		}
	}

	return false;
}

bool Collision::RayVsSkinnedModel(const XMFLOAT3& start, const XMFLOAT3& end, const skinned_mesh* mesh, HitResult& result)
{
	XMMATRIX world, scale, rotation, translation, invWorld;
	world = XMMatrixIdentity();
	scale = XMMatrixScaling(mesh->scale.x, mesh->scale.y, mesh->scale.z);
	world = XMMatrixMultiply(world, scale);
	rotation = XMMatrixRotationRollPitchYaw(mesh->rotation.x, mesh->rotation.y + XM_PI, mesh->rotation.z);
	world = XMMatrixMultiply(world, rotation);
	translation = XMMatrixTranslation(mesh->position.x, mesh->position.y, mesh->position.z);
	world = XMMatrixMultiply(world, translation);

	//float lengthsqr = powf(start.x - end.x, 2) + powf(start.y - end.y, 2) + powf(start.z - end.z, 2);

	for (const skinned_mesh::mesh m : mesh->meshes)
	{
		for (const skinned_mesh::mesh::subset s : m.subsets)
		{
			for (uint32_t i = 0; i < s.index_count; i += 3)
			{
				// ���_�C���f�b�N�X���擾
				uint32_t index0 = m.indices.at(s.start_index_location + i + 0);
				uint32_t index1 = m.indices.at(s.start_index_location + i + 1);
				uint32_t index2 = m.indices.at(s.start_index_location + i + 2);

				// ���_���W���擾
				XMFLOAT3 p0 = m.vertices.at(index0).position;
				XMFLOAT3 p1 = m.vertices.at(index1).position;
				XMFLOAT3 p2 = m.vertices.at(index2).position;

				// ���[���h���W�ɕϊ�
				XMFLOAT3 p0w, p1w, p2w;
				XMStoreFloat3(&p0w, XMVector3Transform(XMLoadFloat3(&p0), XMLoadFloat4x4(&m.default_global_transform) * world));
				XMStoreFloat3(&p1w, XMVector3Transform(XMLoadFloat3(&p1), XMLoadFloat4x4(&m.default_global_transform) * world));
				XMStoreFloat3(&p2w, XMVector3Transform(XMLoadFloat3(&p2), XMLoadFloat4x4(&m.default_global_transform) * world));

				/*float lengthToV0 = powf(p0w.x - start.x, 2) + powf(p0w.y - start.y, 2) + powf(p0w.z - start.z, 2);
				float lengthToV1 = powf(p1w.x - start.x, 2) + powf(p1w.y - start.y, 2) + powf(p1w.z - start.z, 2);
				float lengthToV2 = powf(p2w.x - start.x, 2) + powf(p2w.y - start.y, 2) + powf(p2w.z - start.z, 2);

				if (lengthToV0 > lengthsqr || lengthToV1 > lengthsqr || lengthToV2 > lengthsqr)
				{
					continue;
				}*/

				// ���C�ƎO�p�`�̓����蔻��
				if (RayCast(p0w, p1w, p2w, start, end, &result.Pos, &result.Normal))
				{
					result.Distance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&start) - XMLoadFloat3(&result.Pos)));
					return true;
				}
			}
		}
	}

	return false;
}

bool Collision::VsStage(const XMFLOAT3& start, const XMFLOAT3& end, const std::vector<triangle>& triangles, HitResult& result)
{
	for (const triangle& t : triangles)
	{
		if (RayCast(t.v0, t.v1, t.v2, start, end, &result.Pos, &result.Normal))
		{
			result.Distance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&start) - XMLoadFloat3(&result.Pos)));
			return true;
		}
	}

	return false;
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

	// �|���S���̊O�ς��Ƃ��Ė@�������߂�(���̏����͌Œ蕨�Ȃ�\��Init()�ōs���Ă����Ɨǂ�)
	vec1 = p1 - p0;
	vec2 = p2 - p0;
	nor = XMVector3Cross(vec2, vec1);
	nor = XMVector3Normalize(nor);			// �v�Z���₷���悤�ɖ@�����m�[�}���C�Y���Ă���(���̃x�N�g���̒������P�ɂ��Ă���)
	XMStoreFloat3(normal, nor);	// ���߂��@�������Ă���

	// �|���S�����ʂƐ����̓��ςƂ��ďՓ˂��Ă���\���𒲂ׂ�i�s�p�Ȃ�{�A�݊p�Ȃ�[�A���p�Ȃ�O�j
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	// ���߂��|���S���̖@���ƂQ�̃x�N�g���i�����̗��[�ƃ|���S����̔C�ӂ̓_�j�̓��ςƂ��ďՓ˂��Ă���\���𒲂ׂ�
	d1 = XMVectorGetX(XMVector3Dot(vec1, nor));
	d2 = XMVectorGetX(XMVector3Dot(vec2, nor));
	if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
	{
		// �������Ă���\���͖���
		return(false);
	}

	// �|���S���Ɛ����̌�_�����߂�
	d1 = (float)fabs(d1);	// ��Βl�����߂Ă���
	d2 = (float)fabs(d2);	// ��Βl�����߂Ă���
	float a = d1 / (d1 + d2);							// ������

	XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0�����_�ւ̃x�N�g��
	XMVECTOR	p3 = p0 + vec3;							// ��_
	XMStoreFloat3(hit, p3);								// ���߂���_�����Ă���

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

	n1 = XMVector3Cross( v4, v1);
	if (XMVectorGetX(XMVector3Dot(n1, nor)) < 0.0f) return(false);	// �������Ă��Ȃ�

	n2 = XMVector3Cross(v5, v2);
	if (XMVectorGetX(XMVector3Dot(n2, nor)) < 0.0f) return(false);	// �������Ă��Ȃ�

	n3 = XMVector3Cross(v6, v3);
	if (XMVectorGetX(XMVector3Dot(n3, nor)) < 0.0f) return(false);	// �������Ă��Ȃ�

	return(true);	// �������Ă���I(hit�ɂ͓������Ă����_�������Ă���Bnormal�ɂ͖@���������Ă���)
}
