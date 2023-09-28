#pragma once
#include "../Framework/model.h"

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
	XMFLOAT3 centers[8];

	std::shared_ptr<static_mesh> mesh;
	std::shared_ptr<skinned_mesh> skinnedMesh;

	STAGE(render* Render);
	~STAGE();

	void initialize();
	void update(float elapsed_time);
	void draw(float elapsed_time);
	void uninitialize();

	std::vector<triangle> subDivisions[64];
private:
};
