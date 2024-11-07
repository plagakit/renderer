#pragma once

#include "typedefs.h"

class Camera {

public:
	enum Mode {
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	Transform transform;

	const Mat4& GetProjectionMatrix();

	Mode GetMode() const;
	void SetMode(Mode mode);

	float GetFov() const;
	void SetFov(float fov);

private:
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

	Mode cameraMode;

	const float NEAR = 0.0f;
	const float FAR = 1000.0f;
	const float ASPECT_RATIO = (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH;

	float fov = 60.0f;
	float fovy = 1.0f / tanf(fov * 0.5f / 180.0f * PI);

	Mat4 projectionMatrix;
	bool dirtyProjMat = true;
};