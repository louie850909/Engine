#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Physic
{
private:
	Physic() {}
	~Physic() {}

public:
	static Physic& getInstance()
	{
		static Physic instance;
		return instance;
	}

	// 垂直速力更新処理
	void updateVerticalVelocity(float elapsed_time);
	// 水平速力更新処理
	void updateHorizontalVelocity(float elapsed_time);

	// 垂直位置更新処理
	XMFLOAT3 updateVerticalPosition(XMFLOAT3 position, float elapsed_time);
	// 水平位置更新処理
	XMFLOAT3 updateHorizontalPosition(XMFLOAT3 position, float elapsed_time);
	
	//　衝撃を与える 
	void addImpulse(XMFLOAT3 impulse);

private:
	float gravity = -9.8f * 10.0f;
	float friction = 0.1f;
	XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);
};
