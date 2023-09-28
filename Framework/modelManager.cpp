#include "modelManager.h"

std::shared_ptr<skinned_mesh> ModelManager::LoadSkinnedMesh(const char* filename, render* Render, bool triangulate, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot, float sampleRate)
{
	// 指定されたファイル名のモデルが既に読み込まれているかどうかをチェック
	auto it = skinnedMeshes.find(filename);
	if (it != skinnedMeshes.end())
	{
		// 既に読み込まれているので、そのまま返す
		return it->second;
	}
	// 読み込まれていないので、新しく読み込む
	auto newMesh = std::make_shared<skinned_mesh>(Render->get_device(), filename, triangulate, pos, scl, rot, sampleRate);
	
	// 読み込んだモデルを登録
	skinnedMeshes[filename] = newMesh;
	return newMesh;
}

std::shared_ptr<static_mesh> ModelManager::LoadStaticMesh(const char* filename, render* Render, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot, bool textureInvert)
{
	// 指定されたファイル名のモデルが既に読み込まれているかどうかをチェック
	auto it = staticMeshes.find(filename);
	if (it != staticMeshes.end())
	{
		// 既に読み込まれているので、そのまま返す
		return it->second;
	}
	// 読み込まれていないので、新しく読み込む
	auto newMesh = std::make_shared<static_mesh>(Render->get_device(), filename, pos, scl, rot, textureInvert);

	// 読み込んだモデルを登録
	staticMeshes[filename] = newMesh;
	return newMesh;
}
