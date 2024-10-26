#include "camera.h"

const RMatrix& MyCamera::GetProjectionMatrix()
{
	if (dirtyProjMat)
	{
		dirtyProjMat = false;

		if (cameraMode == Mode::PERSPECTIVE)
		{
			projectionMatrix = RMatrix(
				ASPECT_RATIO * fovy, 0, 0, 0,
				0, fovy, 0, 0,
				0, 0, FAR / (FAR - NEAR), 1.0f,
				0, 0, (-FAR * NEAR) / (FAR - NEAR), 0
			);
		}

	}

	return projectionMatrix;
}

MyCamera::Mode MyCamera::GetMode() const
{
	return cameraMode;
}

void MyCamera::SetMode(Mode newMode)
{
	dirtyProjMat = newMode != cameraMode;
	cameraMode = newMode;
}

float MyCamera::GetFov() const
{
	return fov;
}

void MyCamera::SetFov(float newFov)
{
	fov = newFov;
	fovy = 1.0f / tanf(newFov * 0.5f / 180.0f * PI);
}
