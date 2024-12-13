#pragma once

#include "common.h"

class Camera {

public:
	Camera();

	enum Mode {
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	Transform transform;

	Mat4 GetViewMatrix();
	const Mat4& GetProjectionMatrix();

	Mode GetMode() const;
	void SetMode(Mode mode);

	float GetFov() const;
	void SetFov(float fov);

	float GetOrthographicScale() const;
	void SetOrthographicScale(float newScale);

private:
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

	Mode cameraMode;

	const float NEAR = 1.0f;
	const float FAR = 1000.0f;
	const float ASPECT_RATIO = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	// Represents vertical FOV (fovy)
	float fov = 60.0f;
	float orthographicScale = 3.0f;

	Mat4 projectionMatrix;

	void RecalculateMatrix();
};