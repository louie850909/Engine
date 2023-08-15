#include "physic.h"

void Physic::updateVelocity(float elapsed_time, bool isGround)
{
	// 重力更新
	if (!isGround)
		velocity.y += gravity * elapsed_time;
	else
		velocity.y = 0.0f;

	// 摩擦力更新
	velocity.x *= 1.0f - friction * elapsed_time;
	velocity.z *= 1.0f - friction * elapsed_time;
	if(powf(velocity.x, 2) + powf(velocity.z, 2) < 0.01f)
		velocity.x = velocity.z = 0.0f;
}

XMFLOAT3 Physic::updateMove(XMFLOAT3 position, float elapsed_time)
{
	position.x += velocity.x * elapsed_time;
	position.y += velocity.y * elapsed_time;
	position.z += velocity.z * elapsed_time;

	return position;
}

void Physic::addImpulse(Force f)
{
	// 方向を正規化する
	XMVECTOR v = XMVector3Normalize(XMLoadFloat3(&f.forward));
	XMStoreFloat3(&f.forward, v);

	// 力を加える
	velocity.x += f.forward.x * f.power;
	velocity.y += f.forward.y * f.power;
	velocity.z += f.forward.z * f.power;
}
