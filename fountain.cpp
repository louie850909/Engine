#include "fountain.h"

Fountain::Fountain(render r, float width, float height, int maxInstanceCount) : Particle(r, width, height, maxInstanceCount)
{
	this->maxInstanceCount = maxInstanceCount;
}

void Fountain::update(float elapsed_time)
{
	for (int nCntParticle = 0; nCntParticle < maxInstanceCount; nCntParticle++)
	{
		if (instances[nCntParticle].use)
		{
			instances[nCntParticle].pos.x += instances[nCntParticle].move.x * elapsed_time;
			instances[nCntParticle].pos.z += instances[nCntParticle].move.z * elapsed_time;

			instances[nCntParticle].pos.y += instances[nCntParticle].move.y * elapsed_time;
			if (instances[nCntParticle].pos.y <= height / 2)
			{// 着地した
				instances[nCntParticle].pos.y = height / 2;
				instances[nCntParticle].move.y = -instances[nCntParticle].move.y * 0.75f * elapsed_time;
			}

			instances[nCntParticle].move.x += (0.0f - instances[nCntParticle].move.x) * 0.15f * elapsed_time;
			instances[nCntParticle].move.y -= 2.5f * elapsed_time;
			instances[nCntParticle].move.z += (0.0f - instances[nCntParticle].move.z) * 0.15f * elapsed_time;

			instances[nCntParticle].life -= elapsed_time;
			if (instances[nCntParticle].life <= 0)
			{
				instances[nCntParticle].use = false;
				instances[nCntParticle].life = 0;
			}
			else
			{
				if (instances[nCntParticle].life <= 4.0f)
				{
					instances[nCntParticle].color.x = 0.8f - (float)((80 - instances[nCntParticle].life) / 80 * 0.8f) * elapsed_time;
					instances[nCntParticle].color.y = 0.7f - (float)((80 - instances[nCntParticle].life) / 80 * 0.7f) * elapsed_time;
					instances[nCntParticle].color.z = 0.2f - (float)((80 - instances[nCntParticle].life) / 80 * 0.2f) * elapsed_time;
				}

				if (instances[nCntParticle].life <= 1.0f)
				{
					// α値設定
					instances[nCntParticle].color.w = instances[nCntParticle].life;
				}
			}
		}
	}

	// パーティクル発生
	XMFLOAT3 pos;
	XMFLOAT3 move;
	float fAngle, fLength;
	int nLife;

	pos = { 0.0f, 0.0f, 0.0f };
	fAngle = (float)(rand() % 628 - 314) / 100.0f;
	fLength = rand() % (int)(width * 200) / 100.0f - width;
	move.x = sinf(fAngle) * fLength * 10;
	move.y = (rand() % 300 / 100.0f + height) * 10;
	move.z = cosf(fAngle) * fLength * 10;

	nLife = rand() % 10;
	pos.y = height / 2;

	addParticle(pos, move, XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f), nLife);
}
