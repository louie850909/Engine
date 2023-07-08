#include "camera.h"
#include "GamePad.h"
#include "Input.h"

#define	VALUE_ROTATE_CAMERA	(XM_PI)	// �J�����̉�]��

CAMERA* CAMERA::instance = nullptr;

CAMERA::CAMERA()
{
	instance = this;
}

CAMERA::~CAMERA()
{
}

void CAMERA::initialize()
{
	position = { 100.0f, 10.0f, 100.0f };
	target = { 0.0f, 0.0f, 0.0f };
	up = { 0.0f, 1.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f };
	length = 40.0f;
}

void CAMERA::update(float elapsed_time)
{
	// �L�[���͂��擾����
	GamePad& gamePad = Input::Instance().GetGamePad();
	
	// ���_����u��v
	if (gamePad.GetButton() == gamePad.BTN_UP)
	{
		rotation.x += VALUE_ROTATE_CAMERA * elapsed_time;
		if(rotation.x > XM_PI / 2.0f)
			rotation.x = XM_PI / 2.0f;

		position.y = target.y - sinf(rotation.x) * length;
	}

	// ���_����u���v
	if (gamePad.GetButton() == gamePad.BTN_DOWN)
	{
		rotation.x -= VALUE_ROTATE_CAMERA * elapsed_time;
		if (rotation.x < -XM_PI / 2.0f)
			rotation.x = -XM_PI / 2.0f;

		position.y = target.y - sinf(rotation.x) * length;
	}

	// ���_����u���v
	if (gamePad.GetButton() == gamePad.BTN_RIGHT)
	{
		rotation.y += VALUE_ROTATE_CAMERA * elapsed_time;
		if (rotation.y > XM_PI)
		{
			rotation.y -= XM_PI * 2.0f;
		}

		position.x = target.x - sinf(rotation.y) * length;
		position.z = target.z - cosf(rotation.y) * length;
	}

	// ���_����u�E�v
	if (gamePad.GetButton() == gamePad.BTN_LEFT)
	{
		rotation.y -= VALUE_ROTATE_CAMERA * elapsed_time;
		if (rotation.y < -XM_PI)
		{
			rotation.y += XM_PI * 2.0f;
		}

		position.x = target.x - sinf(rotation.y) * length;
		position.z = target.z - cosf(rotation.y) * length;
	}

	// ���_�����u�߂Â��v
	if (gamePad.GetButton() == gamePad.BTN_X)
	{
		length -= 10.0f * elapsed_time;
		if (length < 1.0f)
			length = 1.0f;

		position.x = target.x - sinf(rotation.y) * length;
		position.z = target.z - cosf(rotation.y) * length;
		position.y = target.y - sinf(rotation.x) * length;
	}

	// ���_�����u��������v
	if (gamePad.GetButton() == gamePad.BTN_Y)
	{
		length += 10.0f * elapsed_time;
		if (length > 1000.0f)
			length = 1000.0f;

		position.x = target.x - sinf(rotation.y) * length;
		position.z = target.z - cosf(rotation.y) * length;
		position.y = target.y - sinf(rotation.x) * length;
	}

	// Imgui�ŃJ�����̈ʒu��ύX�ł���悤�ɂ���
#ifdef USE_IMGUI
	ImGui::Begin("Camera");
	ImGui::SliderFloat3("Position", &position.x, -1000.0f, 1000.0f);
	ImGui::SliderFloat3("Target", &target.x, -1000.0f, 1000.0f);
	ImGui::SliderFloat3("Rotation", &rotation.x, -100.0f, 10.0f);
	ImGui::SliderFloat("Length", &length, 1.0f, 1000.0f);
	ImGui::End();
#endif // USE_IMGUI
}

void CAMERA::SetCameraAT(XMFLOAT3 pos)
{
	// �J�����̒����_���v���C���[�̍��W�ɂ��Ă݂�
	target = pos;

	// �J�����̎��_���J������Y����]�ɑΉ������Ă���
	position.x = target.x - sinf(rotation.y) * length;
	position.z = target.z - cosf(rotation.y) * length;
	position.y = target.y - sinf(rotation.x) * length;
}
