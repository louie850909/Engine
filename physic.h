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

	// �������͍X�V����
	void updateVerticalVelocity(float elapsed_time);
	// �������͍X�V����
	void updateHorizontalVelocity(float elapsed_time);

	// �����ʒu�X�V����
	XMFLOAT3 updateVerticalPosition(XMFLOAT3 position, float elapsed_time);
	// �����ʒu�X�V����
	XMFLOAT3 updateHorizontalPosition(XMFLOAT3 position, float elapsed_time);
	
	//�@�Ռ���^���� 
	void addImpulse(XMFLOAT3 impulse);

private:
	float gravity = -9.8f * 10.0f;
	float friction = 0.1f;
	XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);
};
