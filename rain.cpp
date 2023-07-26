#include "rain.h"
#include "camera.h"

Rain::Rain(render r, float width, float height, int maxInstanceCount) : Particle(r, width, height, maxInstanceCount, false, true)
{
	this->maxInstanceCount = maxInstanceCount;
}

void Rain::update(float elapsed_time)
{
	for (int nCntParticle = 0; nCntParticle < maxInstanceCount; nCntParticle++)
	{
		if (instances[nCntParticle].use)
		{
			instances[nCntParticle].pos.x += instances[nCntParticle].move.x * elapsed_time;
			instances[nCntParticle].pos.z += instances[nCntParticle].move.z * elapsed_time;
			instances[nCntParticle].pos.y += instances[nCntParticle].move.y * elapsed_time;

			// 着地した
			if (instances[nCntParticle].pos.y <= height / 2)
			{
				instances[nCntParticle].use = false;
				instances[nCntParticle].life = 0;
				isFull = false;
				startIndex = nCntParticle;
			}
		}
	}

	//time += elapsed_time;

	// パーティクル発生
	XMFLOAT3 pos;
	XMFLOAT3 move;
	XMFLOAT3 scl;
	int nLife;
	float angle, length;

	angle = CAMERA::Instance().get_rotation().y + (float)(rand() % 315 - 157) / 100.0f;
	length = (float)(rand() % 2000) / 10.0f;

	// 発生位置はいつでもカメラの前方範囲内
	pos.x = CAMERA::Instance().get_position().x + sinf(angle) * length;
	pos.y = 100.0f;
	pos.z = CAMERA::Instance().get_position().z + cosf(angle) * length;

	// 運動方向は下向き
	move.x = 3.0f;
	move.y = -50.0f;
	move.z = 0.0f;

	scl.x = scl.y = scl.z = 1.0f;

	nLife = 5;

	
	addParticle(pos, move, scl, XMFLOAT4(1, 1, 1, 1), nLife, startIndex);
	//time = 0;
	
}
