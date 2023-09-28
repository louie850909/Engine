#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

class Physic
{
public:
	// �͂̍\����
	struct Force
	{
		XMFLOAT3 forward;	// ����
		float power;		// ��
	};

	Physic() 
	{ 
		velocity = XMFLOAT3(0, 0, 0);
	}
	~Physic() {}

	// ���͍X�V����
	void updateVelocity(float elapsed_time, bool isGround);

	// �ʒu�X�V����
	XMFLOAT3 updateMove(XMFLOAT3 position, float elapsed_time);
	
	//�@�Ռ���^���� 
	void addImpulse(Force f);

	XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);
private:
	float gravity = -9.8f * 30.0f;
	float friction = 0.7f;
};
