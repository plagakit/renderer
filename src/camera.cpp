#include "camera.h"

#include <glm/gtx/string_cast.hpp>
//#include <iostream>

Camera::Camera()
{
	cameraMode = Mode::PERSPECTIVE;
	RecalculateMatrix();
}

Mat4 Camera::GetViewMatrix()
{
	return glm::inverse(transform.GetTransform());
}

const Mat4& Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

Camera::Mode Camera::GetMode() const
{
	return cameraMode;
}

void Camera::SetMode(Mode newMode)
{
	cameraMode = newMode;
	RecalculateMatrix();
}

float Camera::GetFov() const
{
	return fov;
}

void Camera::SetFov(float newFov)
{
	fov = newFov;
	RecalculateMatrix();
}

float Camera::GetOrthographicScale() const
{
	return orthographicScale;
}

void Camera::SetOrthographicScale(float newScale)
{
	orthographicScale = newScale;
	RecalculateMatrix();
}

void Camera::RecalculateMatrix()
{
	if (cameraMode == Mode::PERSPECTIVE)
	{
		// RHS [-1, 1]
		//projectionMatrix = glm::perspective(fovy, ASPECT_RATIO, NEAR, FAR);

		// LHS [-1, 1]
		//const float tanHalfFovy = tanf(fovy / 2.0f);
		//projectionMatrix = Mat4(0.0f);
		//projectionMatrix[0][0] = 1.0f / (ASPECT_RATIO * tanHalfFovy);
		//projectionMatrix[1][1] = 1.0f / (tanHalfFovy);
		//projectionMatrix[2][2] = (FAR + NEAR) / (FAR - NEAR);
		//projectionMatrix[2][3] = 1.0f;
		//projectionMatrix[3][2] = -(2.0f * FAR * NEAR) / (FAR - NEAR);

		// LHS [0, 1]
		const float tanHalfFovy = tanf(glm::radians(fov) / 2.0f);
		projectionMatrix = Mat4(0.0f);
		projectionMatrix[0][0] = 1.0f / (ASPECT_RATIO * tanHalfFovy);
		projectionMatrix[1][1] = 1.0f / (tanHalfFovy);
		projectionMatrix[2][2] = FAR / (FAR - NEAR);
		projectionMatrix[2][3] = 1.0f;
		projectionMatrix[3][2] = -(FAR * NEAR) / (FAR - NEAR);
		//std::cout << glm::to_string(projectionMatrix) << std::endl;
	}
	else
	{
		float orthWidth = ASPECT_RATIO * orthographicScale;
		float orthHeight = orthographicScale;

		float r = -orthWidth * 0.5f;
		float l = r + orthWidth;
		float b = -orthHeight * 0.5f;
		float t = b + orthHeight;
		
		projectionMatrix = Mat4(1.0f);
		projectionMatrix[0][0] = 2.0f / orthWidth;
		projectionMatrix[1][1] = 2.0f / orthHeight;
		projectionMatrix[2][2] = -1.0f / (FAR - NEAR);
		projectionMatrix[3][0] = -(r + l) / orthWidth;
		projectionMatrix[3][1] = -(t + b) / orthHeight;
		projectionMatrix[3][2] = -NEAR / (FAR - NEAR);
	}
}
