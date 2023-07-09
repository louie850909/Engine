#pragma once
#include "model.h"

class STAGE
{
public :
	render* Render;

	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	std::unique_ptr<static_mesh> mesh;
	std::unique_ptr<skinned_mesh> skinnedMesh;

	STAGE(render* Render);
	~STAGE();

	static STAGE& Instance();

	void initialize();
	void update(float elapsed_time);
	void draw(float elapsed_time);
	void uninitialize();

private:
};
