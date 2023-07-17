#pragma once
#include "model.h"

struct triangle
{
	XMFLOAT3 v0;
	XMFLOAT3 v1;
	XMFLOAT3 v2;
};

class STAGE
{
public :
	render* Render;

	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	XMFLOAT3 center;

	std::unique_ptr<static_mesh> mesh;
	std::unique_ptr<skinned_mesh> skinnedMesh;

	STAGE(render* Render);
	~STAGE();

	static STAGE& Instance();

	void initialize();
	void update(float elapsed_time);
	void draw(float elapsed_time);
	void uninitialize();

	std::vector<triangle> subDivisions[8];
private:
};
