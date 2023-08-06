#include "stage.h"
#include <comdef.h>
#include "modelManager.h"

STAGE::STAGE(render* Render)
{
	this->Render = Render;
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(10.0f, 10.0f, 10.0f);
}

STAGE::~STAGE()
{
}

void STAGE::initialize()
{
	//mesh = std::make_unique<static_mesh>(Render->get_device(), L".\\resources\\landscape.obj", position, scale, rotation, false);
	//skinnedMesh = std::make_unique<skinned_mesh>(Render->get_device(), ".\\resources\\testStage\\ExampleStage.fbx", false, position, scale, rotation, 0.0f);
	skinnedMesh = ModelManager::getInstance().LoadSkinnedMesh(".\\resources\\testStage\\ExampleStage.fbx", Render, false, position, scale, rotation, 0.0f);

	// 頂点をワールド座標に変換し、分割する
	XMMATRIX w, s, r, t;
	w = XMMatrixIdentity();
	s = XMMatrixScaling(scale.x, scale.y, scale.z);
	w = XMMatrixMultiply(w, s);
	r = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y + XM_PI, rotation.z);
	w = XMMatrixMultiply(w, r);
	t = XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
	w = XMMatrixMultiply(w, t);

	XMFLOAT3 boxMin = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 boxMax = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (const skinned_mesh::mesh m : skinnedMesh->meshes)
	{
		for (const skinned_mesh::mesh::subset s : m.subsets)
		{
			for (uint32_t i = 0; i < s.index_count; i += 3)
			{
				// 頂点インデックスを取得
				uint32_t index0 = m.indices.at(s.start_index_location + i + 0);
				uint32_t index1 = m.indices.at(s.start_index_location + i + 1);
				uint32_t index2 = m.indices.at(s.start_index_location + i + 2);

				// 頂点座標を取得
				XMFLOAT3 p0 = m.vertices.at(index0).position;
				XMFLOAT3 p1 = m.vertices.at(index1).position;
				XMFLOAT3 p2 = m.vertices.at(index2).position;

				// ワールド座標に変換
				XMFLOAT3 p0w, p1w, p2w;
				XMStoreFloat3(&p0w, XMVector3Transform(XMLoadFloat3(&p0), XMLoadFloat4x4(&m.default_global_transform) * w));
				XMStoreFloat3(&p1w, XMVector3Transform(XMLoadFloat3(&p1), XMLoadFloat4x4(&m.default_global_transform) * w));
				XMStoreFloat3(&p2w, XMVector3Transform(XMLoadFloat3(&p2), XMLoadFloat4x4(&m.default_global_transform) * w));

				boxMax.x = max(boxMax.x, p0w.x);
				boxMax.y = max(boxMax.y, p0w.y);
				boxMax.z = max(boxMax.z, p0w.z);
				boxMax.x = max(boxMax.x, p1w.x);
				boxMax.y = max(boxMax.y, p1w.y);
				boxMax.z = max(boxMax.z, p1w.z);
				boxMax.x = max(boxMax.x, p2w.x);
				boxMax.y = max(boxMax.y, p2w.y);
				boxMax.z = max(boxMax.z, p2w.z);

				boxMin.x = min(boxMin.x, p0w.x);
				boxMin.y = min(boxMin.y, p0w.y);
				boxMin.z = min(boxMin.z, p0w.z);
				boxMin.x = min(boxMin.x, p1w.x);
				boxMin.y = min(boxMin.y, p1w.y);
				boxMin.z = min(boxMin.z, p1w.z);
				boxMin.x = min(boxMin.x, p2w.x);
				boxMin.y = min(boxMin.y, p2w.y);
				boxMin.z = min(boxMin.z, p2w.z);
			}
		}
	}

	center = XMFLOAT3((boxMin.x + boxMax.x) / 2, (boxMin.y + boxMax.y) / 2, (boxMin.z + boxMax.z) / 2);

	for (const skinned_mesh::mesh m : skinnedMesh->meshes)
	{
		for (const skinned_mesh::mesh::subset s : m.subsets)
		{
			for (uint32_t i = 0; i < s.index_count; i += 3)
			{
				// 頂点インデックスを取得
				uint32_t index0 = m.indices.at(s.start_index_location + i + 0);
				uint32_t index1 = m.indices.at(s.start_index_location + i + 1);
				uint32_t index2 = m.indices.at(s.start_index_location + i + 2);

				// 頂点座標を取得
				XMFLOAT3 p0 = m.vertices.at(index0).position;
				XMFLOAT3 p1 = m.vertices.at(index1).position;
				XMFLOAT3 p2 = m.vertices.at(index2).position;

				// ワールド座標に変換
				XMFLOAT3 p0w, p1w, p2w;
				XMStoreFloat3(&p0w, XMVector3Transform(XMLoadFloat3(&p0), XMLoadFloat4x4(&m.default_global_transform) * w));
				XMStoreFloat3(&p1w, XMVector3Transform(XMLoadFloat3(&p1), XMLoadFloat4x4(&m.default_global_transform) * w));
				XMStoreFloat3(&p2w, XMVector3Transform(XMLoadFloat3(&p2), XMLoadFloat4x4(&m.default_global_transform) * w));

				// 頂点を分類
				triangle t;
				t.v0 = p0w;
				t.v1 = p1w;
				t.v2 = p2w;

				if ((t.v0.x < center.x && t.v0.y < center.y && t.v0.z < center.z) ||
					(t.v1.x < center.x && t.v1.y < center.y && t.v1.z < center.z) ||
					(t.v2.x < center.x && t.v2.y < center.y && t.v2.z < center.z))
					subDivisions[0].push_back(t);
				if ((t.v0.x > center.x && t.v0.y < center.y && t.v0.z < center.z) ||
					(t.v1.x > center.x && t.v1.y < center.y && t.v1.z < center.z) ||
					(t.v2.x > center.x && t.v2.y < center.y && t.v2.z < center.z))
					subDivisions[1].push_back(t);
				if ((t.v0.x < center.x && t.v0.y > center.y && t.v0.z < center.z) ||
					(t.v1.x < center.x && t.v1.y > center.y && t.v1.z < center.z) ||
					(t.v2.x < center.x && t.v2.y > center.y && t.v2.z < center.z))
					subDivisions[2].push_back(t);
				if ((t.v0.x > center.x && t.v0.y > center.y && t.v0.z < center.z) ||
					(t.v1.x > center.x && t.v1.y > center.y && t.v1.z < center.z) ||
					(t.v2.x > center.x && t.v2.y > center.y && t.v2.z < center.z))
					subDivisions[3].push_back(t);
				if ((t.v0.x < center.x && t.v0.y < center.y && t.v0.z > center.z) ||
					(t.v1.x < center.x && t.v1.y < center.y && t.v1.z > center.z) ||
					(t.v2.x < center.x && t.v2.y < center.y && t.v2.z > center.z))
					subDivisions[4].push_back(t);
				if ((t.v0.x > center.x && t.v0.y < center.y && t.v0.z > center.z) ||
					(t.v1.x > center.x && t.v1.y < center.y && t.v1.z > center.z) ||
					(t.v2.x > center.x && t.v2.y < center.y && t.v2.z > center.z))
					subDivisions[5].push_back(t);
				if ((t.v0.x < center.x && t.v0.y > center.y && t.v0.z > center.z) ||
					(t.v1.x < center.x && t.v1.y > center.y && t.v1.z > center.z) ||
					(t.v2.x < center.x && t.v2.y > center.y && t.v2.z > center.z))
					subDivisions[6].push_back(t);
				if ((t.v0.x > center.x && t.v0.y > center.y && t.v0.z > center.z) ||
					(t.v1.x > center.x && t.v1.y > center.y && t.v1.z > center.z) ||
					(t.v2.x > center.x && t.v2.y > center.y && t.v2.z > center.z))
					subDivisions[7].push_back(t);
			}
		}
	}
}

void STAGE::update(float elapsed_time)
{
}

void STAGE::draw(float elapsed_time)
{
	//mesh->draw(*Render);
	animation::keyframe keyframe;
	skinnedMesh->draw(*Render, XMFLOAT4(1, 1, 1, 1), skinned_mesh::LHS_YUP, &keyframe);
#ifdef _DEBUG
	ImGui::Begin("STAGE");
	ImGui::DragFloat3("center", &center.x, 0.01f);
	ImGui::End();
#endif // _DEBUG

}

void STAGE::uninitialize()
{
}
