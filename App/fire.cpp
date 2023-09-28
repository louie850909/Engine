#include "fire.h"

Fire::Fire(render r, float width, float height, XMFLOAT3 pos, int maxInstanceCount) : Particle(r, width, height, maxInstanceCount, true, false)
{
	this->pos = pos;
	this->maxInstanceCount = maxInstanceCount;
}

void Fire::update(float elapsed_time)
{

	for (int nCntParticle = 0; nCntParticle < maxInstanceCount; nCntParticle++)
	{
		if (instances[nCntParticle].use)
		{
			// �ʒu�X�V
			instances[nCntParticle].pos.x += instances[nCntParticle].move.x * elapsed_time;
			instances[nCntParticle].pos.z += instances[nCntParticle].move.z * elapsed_time;
			instances[nCntParticle].pos.y += instances[nCntParticle].move.y * elapsed_time;

			// �����X�V
			instances[nCntParticle].life -= elapsed_time;

			// �F�͎����ɂ���ď����Ă���
			instances[nCntParticle].color.w = instances[nCntParticle].life;

			// �������s������
			if (instances[nCntParticle].life <= 0)
			{
				instances[nCntParticle].use = false;
				instances[nCntParticle].life = 0;
				isFull = false;
				startIndex = nCntParticle;
			}
		}
	}

	// �p�[�e�B�N������
	XMFLOAT3 move;
	XMFLOAT3 scl;
	float fAngle, fLength;
	float nLife;

	fAngle = (float)(rand() % 628 - 314) / 100.0f;
	fLength = rand() % (int)(width * 200) / 100.0f - width;
	move.x = sinf(fAngle) * fLength * 10;
	move.y = (rand() % 300 / 100.0f + height) * 5;
	move.z = cosf(fAngle) * fLength * 10;
	scl.x = scl.y = scl.z = (float)(rand() % 501) / 100.0f + 0.5f;

	nLife = 1.0f;

	addParticle(pos, move, scl, XMFLOAT4(1.0f, 0.4f, 0.2f, 0.85f), nLife, startIndex);
}
