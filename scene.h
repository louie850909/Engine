#pragma once

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Init() = 0;
	virtual void Update(float elapsed_time) = 0;
	virtual void Draw(float elapsed_time) = 0;
	virtual void Uninit() = 0;

	bool isInit = false;
};