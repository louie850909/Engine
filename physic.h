#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

class Physic
{
public:
	// 力の構造体
	struct Force
	{
		XMFLOAT3 forward;	// 方向
		float power;		// 力
	};

	Physic() 
	{ 
		velocity = XMFLOAT3(0, 0, 0);
	}
	~Physic() {}

	// 速力更新処理
	void updateVelocity(float elapsed_time, bool isGround);

	// 位置更新処理
	XMFLOAT3 updateMove(XMFLOAT3 position, float elapsed_time);
	
	//　衝撃を与える 
	void addImpulse(Force f);

	XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);
private:
	float gravity = -9.8f * 30.0f;
	float friction = 0.7f;
};
