#include "modelManager.h"

std::shared_ptr<skinned_mesh> ModelManager::LoadSkinnedMesh(const char* filename, render* Render, bool triangulate, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot, float sampleRate)
{
	// �w�肳�ꂽ�t�@�C�����̃��f�������ɓǂݍ��܂�Ă��邩�ǂ������`�F�b�N
	auto it = skinnedMeshes.find(filename);
	if (it != skinnedMeshes.end())
	{
		// ���ɓǂݍ��܂�Ă���̂ŁA���̂܂ܕԂ�
		return it->second;
	}
	// �ǂݍ��܂�Ă��Ȃ��̂ŁA�V�����ǂݍ���
	auto newMesh = std::make_shared<skinned_mesh>(Render->get_device(), filename, triangulate, pos, scl, rot, sampleRate);
	
	// �ǂݍ��񂾃��f����o�^
	skinnedMeshes[filename] = newMesh;
	return newMesh;
}

std::shared_ptr<static_mesh> ModelManager::LoadStaticMesh(const char* filename, render* Render, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot, bool textureInvert)
{
	// �w�肳�ꂽ�t�@�C�����̃��f�������ɓǂݍ��܂�Ă��邩�ǂ������`�F�b�N
	auto it = staticMeshes.find(filename);
	if (it != staticMeshes.end())
	{
		// ���ɓǂݍ��܂�Ă���̂ŁA���̂܂ܕԂ�
		return it->second;
	}
	// �ǂݍ��܂�Ă��Ȃ��̂ŁA�V�����ǂݍ���
	auto newMesh = std::make_shared<static_mesh>(Render->get_device(), filename, pos, scl, rot, textureInvert);

	// �ǂݍ��񂾃��f����o�^
	staticMeshes[filename] = newMesh;
	return newMesh;
}
