#include "camera.h"

#include <iostream>

const Mat4& Camera::GetProjectionMatrix()
{
	return Mat4(
		ASPECT_RATIO * fovy, 0, 0, 0,
		0, fovy, 0, 0,
		0, 0, -FAR / (FAR - NEAR), 1.0f,
		0, 0, (-FAR * NEAR) / (FAR - NEAR), 0
	);
	//if (dirtyProjMat)
	//{
	//	dirtyProjMat = false;

	//	if (cameraMode == Mode::PERSPECTIVE)
	//	{
	//		std::cout << "a\n";
	//		projectionMatrix = RMatrix(
	//			ASPECT_RATIO * fovy, 0, 0, 0,
	//			0, fovy, 0, 0,
	//			0, 0, -FAR / (FAR - NEAR), 1.0f,
	//			0, 0, (-FAR * NEAR) / (FAR - NEAR), 0
	//		);
	//	}
	//	else
	//	{

	//	}

	//}

	//return projectionMatrix;
}

Camera::Mode Camera::GetMode() const
{
	return cameraMode;
}

void Camera::SetMode(Mode newMode)
{
	dirtyProjMat = true;
	cameraMode = newMode;
}

float Camera::GetFov() const
{
	return fov;
}

void Camera::SetFov(float newFov)
{
	dirtyProjMat = true;

	fov = newFov;
	fovy = 1.0f / tanf(newFov * 0.5f / 180.0f * PI);
}
