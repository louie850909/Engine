#include "character.h"

Character::Character(render* Rneder)
{
	Render = Rneder;
}

Character::~Character()
{
}

void Character::initialize()
{
}

void Character::update(float elapsed_time)
{
}

void Character::draw(float elapsed_time)
{
}

void Character::uninitialize()
{
}

void Character::playAnimation(int clip_index, bool loop, float blendSec)
{
	this->clip_index = clip_index;
	animation_tick = 0.0f;
	animation_loop = loop;
	blendSecond = blendSec;
}

bool Character::isPlayAnimation()
{
	if (animation_tick < 0)
		return false;
	if (animation_tick >= mesh->animation_clips.at(clip_index).sequence.size() - 1)
		return false;
	return true;
}

void Character::updateAnimation(float elapsed_time)
{
	animation& animation = mesh->animation_clips.at(clip_index);
	if (blendSecond != 0)
	{
		blendTick += elapsed_time;
		float blendRate = blendTick / blendSecond;
		if (blendRate < 1.0f)
		{
			mesh->blend_animations(blendframes, blendRate, keyframe);
			mesh->update_animation(keyframe);
		}
		else
		{
			blendSecond = 0.0f;
			blendTick = 0.0f;
		}
	}
	else
	{
		frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
		if (frame_index >= animation.sequence.size() - 1)
		{
			if (animation_loop)
			{
				frame_index = 0;
				animation_tick = 0.0f;
			}
			else
			{
				frame_index = animation.sequence.size() - 1;
				animation_tick = static_cast<float>(frame_index) / animation.sampling_rate;
			}
		}
		else
		{
			animation_tick += elapsed_time;
		}
		keyframe = animation.sequence.at(frame_index);
	}
}

void Character::updateplaceIndex(const STAGE* stage)
{
	// �ʒu����
	placeIndex = 0;
	if (position.x < stage->center.x && position.y < stage->center.y && position.z < stage->center.z)
	{
		placeIndex |= (position.x < stage->centers[0].x) ? 1 : 0;
		placeIndex |= (position.y < stage->centers[0].y) ? 2 : 0;
		placeIndex |= (position.z < stage->centers[0].z) ? 4 : 0;
		placeIndex = 0 * 8 + placeIndex;
	}
	else if (position.x > stage->center.x && position.y < stage->center.y && position.z < stage->center.z)
	{
		placeIndex |= (position.x < stage->centers[1].x) ? 1 : 0;
		placeIndex |= (position.y < stage->centers[1].y) ? 2 : 0;
		placeIndex |= (position.z < stage->centers[1].z) ? 4 : 0;
		placeIndex = 1 * 8 + placeIndex;
	}
	else if (position.x < stage->center.x && position.y > stage->center.y && position.z < stage->center.z)
	{
		placeIndex |= (position.x < stage->centers[2].x) ? 1 : 0;
		placeIndex |= (position.y < stage->centers[2].y) ? 2 : 0;
		placeIndex |= (position.z < stage->centers[2].z) ? 4 : 0;
		placeIndex = 2 * 8 + placeIndex;
	}
	else if (position.x > stage->center.x && position.y > stage->center.y && position.z < stage->center.z)
	{
		placeIndex |= (position.x < stage->centers[3].x) ? 1 : 0;
		placeIndex |= (position.y < stage->centers[3].y) ? 2 : 0;
		placeIndex |= (position.z < stage->centers[3].z) ? 4 : 0;
		placeIndex = 3 * 8 + placeIndex;
	}
	else if (position.x < stage->center.x && position.y < stage->center.y && position.z > stage->center.z)
	{
		placeIndex |= (position.x < stage->centers[4].x) ? 1 : 0;
		placeIndex |= (position.y < stage->centers[4].y) ? 2 : 0;
		placeIndex |= (position.z < stage->centers[4].z) ? 4 : 0;
		placeIndex = 4 * 8 + placeIndex;
	}
	else if (position.x > stage->center.x && position.y < stage->center.y && position.z > stage->center.z)
	{
		placeIndex |= (position.x < stage->centers[5].x) ? 1 : 0;
		placeIndex |= (position.y < stage->centers[5].y) ? 2 : 0;
		placeIndex |= (position.z < stage->centers[5].z) ? 4 : 0;
		placeIndex = 5 * 8 + placeIndex;
	}
	else if (position.x < stage->center.x && position.y > stage->center.y && position.z > stage->center.z)
	{
		placeIndex |= (position.x < stage->centers[6].x) ? 1 : 0;
		placeIndex |= (position.y < stage->centers[6].y) ? 2 : 0;
		placeIndex |= (position.z < stage->centers[6].z) ? 4 : 0;
		placeIndex = 6 * 8 + placeIndex;
	}
	else if (position.x > stage->center.x && position.y > stage->center.y && position.z > stage->center.z)
	{
		placeIndex |= (position.x < stage->centers[7].x) ? 1 : 0;
		placeIndex |= (position.y < stage->centers[7].y) ? 2 : 0;
		placeIndex |= (position.z < stage->centers[7].z) ? 4 : 0;
		placeIndex = 7 * 8 + placeIndex;
	}
}

void Character::drawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Render->get_debug_renderer();

	// �f�o�b�O�p��`�悷��
	debugRenderer->DrawCylinder(position, collision_radius, collision_height, XMFLOAT4(0, 0, 0, 1));
}
