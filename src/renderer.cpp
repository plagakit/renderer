#include "renderer.h"

#include <iostream>

void Renderer::Init()
{
	framebufferImage = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
	framebufferTexture = LoadTextureFromImage(framebufferImage);

	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	{
		framebuffer.push_back(BLACK);
		depthBuffer.push_back(std::numeric_limits<float>::max());
	}

	ConstructProjectionMatrix();
}

void Renderer::Shutdown()
{
	//delete frameBuffer;
}

void Renderer::SetClearColor(Color color)
{
	clearColor = color;
}

void Renderer::DrawMesh(TriMesh* mesh, RMatrix transform)
{
	commandQueue.push_back({ mesh, transform });
}

void Renderer::FlushCommands()
{
	ResetDepthBuffer();
	ClearScreen();

	DrawLine(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RED);
	
	for (const auto& command : commandQueue)
	{
		if (!command.mesh)
			continue;

		for (int i = 0; i < command.mesh->tris.size(); i++)
		{
			Tri tri = command.mesh->tris[i];
			Tri vn = command.mesh->vertexNormals[i];

			//// Model Space -> Object Space
			//for (auto& p : tri.p)
			//{
			//	// Scale
			//	p *= 0.25f;
			//
			//	// Translate
			//	p += RVector3(0, 0, 10.0f);
			//}

			// Object Space -> World Space
			tri.p[0] = tri.p[0].Transform(command.transform);
			tri.p[1] = tri.p[1].Transform(command.transform);
			tri.p[2] = tri.p[2].Transform(command.transform);
			for (auto& p : vn.p) 
				p = p.Transform(command.transform).Normalize();


			// Cull if not facing camera (backface)
			RVector3 normal = TriangleFaceNormal(tri);
			if (config.doBackfaceCulling)
			{
				RVector3 cameraFacing = RVector3(0, 0, 1);
				if (normal.DotProduct(cameraFacing) < 0)
					continue;
			}

			// World Space -> Screen Space
			for (auto& p : tri.p)
			{
				// Project onto viewport
				p = p.Transform(projectionMatrix);

				// Transform it into the middle of the screen
				// (equivalent to moving an imaginary camera)
				p += RVector3(1.0f, 1.0f, 0);
				p.x *= 0.5f * SCREEN_WIDTH;
				p.y *= 0.5 * SCREEN_HEIGHT;
			}

			switch (config.lightingType)
			{
			case LightingType::FLAT:
			case LightingType::GOURAUD:
			case LightingType::DEPTH_MAP:
				RasterizeTriangle(tri, vn, normal, GREEN);
				break;

			case LightingType::WIREFRAME:
				DrawTriangle(tri, RAYWHITE);
				break;
			}
		}
	}

	if (config.lightingType == LightingType::DEPTH_MAP)
	{
		for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
			framebuffer[i] = DarkenColor(WHITE, minDepth - depthBuffer[i] + 1); //depthBuffer[i] / minDepth);
	}

	commandQueue.clear();
}

void Renderer::BlitToScreen()
{
	UpdateTexture(framebufferTexture, framebuffer.data());
	DrawTexture(framebufferTexture, 0, 0, WHITE);
}

// HELPER FUNCTIONS

void Renderer::ConstructProjectionMatrix()
{
	float right = near * tanf(fov * 0.5f * DEG2RAD);
	float top = right * aspectRatio;

	projectionMatrix.m0 = near / right;
	projectionMatrix.m5 = near / top;
	projectionMatrix.m10 = -(far + near) / (far - near);
	projectionMatrix.m11 = -1.0f;
	projectionMatrix.m14 = -(2.0f * far * near) / (far - near);
	projectionMatrix.m15 = 0.0f;
}

void Renderer::ClearScreen()
{
	std::fill(framebuffer.begin(), framebuffer.end(), clearColor);
}

void Renderer::ResetDepthBuffer()
{
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		depthBuffer[i] = std::numeric_limits<float>::max();
}

Color Renderer::DarkenColor(Color col, float factor) const
{
	return {
		static_cast<unsigned char>(col.r * factor),
		static_cast<unsigned char>(col.g * factor),
		static_cast<unsigned char>(col.b * factor),
		col.a
	};
}

RVector3 Renderer::TriangleFaceNormal(const Tri& tri) const
{
	// The two lines running along the face
	RVector3 line1 = tri.p[1] - tri.p[0];
	RVector3 line2 = tri.p[2] - tri.p[0];

	return line1.CrossProduct(line2).Normalize();
}

float Renderer::Triangle2DArea(const RVector3& a, const RVector3& b, const RVector3& c) const
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool Renderer::IsPointInsideTriangle2D(const Tri& tri, const RVector3& point) const
{
	float edge01 = Triangle2DArea(tri.p[0], tri.p[1], point);
	float edge12 = Triangle2DArea(tri.p[1], tri.p[2], point);
	float edge20 = Triangle2DArea(tri.p[2], tri.p[0], point);

	return edge01 >= 0 && edge12 >= 0 && edge20 >= 0;
}

void Renderer::BresenhamMajorAxis(Color* curPixel, int dx, int dy, int stepMajor, int stepMinor, Color color)
{
	const int dx2 = dx * 2;
	const int dy2 = dy * 2;
	const int dydx2 = dy2 - dx2;
	int d = dy2 - dx;

	*curPixel = color;
	while (dx--)
	{
		if (d > 0)
		{
			curPixel += stepMajor + stepMinor;
			d += dydx2;
		}
		else
		{
			curPixel += stepMajor;
			d += dy2;
		}
		*curPixel = color;
	}
}

void Renderer::BresenhamStraight(Color* curPixel, int dx, int step, Color color)
{
	*curPixel = color;
	while (dx--)
	{
		curPixel += step;
		*curPixel = color;
	}
}

void Renderer::DrawLineBresenham(int x0, int y0, int x1, int y1, Color color)
{
	// not actually bresenham
	// TODO: do bresenham
	
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	float m = (y1 - y0) / (float)(x1 - x0);
	float c = (x1 * y0 - x0 * y1) / (float)(x1 - x0);

	for (int x = x0; x <= x1; x++)
	{
		int y = m * x + c;
		framebuffer[y * SCREEN_WIDTH + x] = color;
	}
}

void Renderer::DrawTriangle(const Tri& tri, Color color)
{
	DrawLineBresenham(tri.p[0].x, tri.p[0].y, tri.p[1].x, tri.p[1].y, color);
	DrawLineBresenham(tri.p[1].x, tri.p[1].y, tri.p[2].x, tri.p[2].y, color);
	DrawLineBresenham(tri.p[2].x, tri.p[2].y, tri.p[0].x, tri.p[0].y, color);
}

void Renderer::RasterizeTriangle(const Tri& tri, const Tri& vertexNormals, const RVector3& normal, Color color)
{
	// We treat this triangle as a 2D triangle, ignoring Z
	float area = Triangle2DArea(tri.p[0], tri.p[1], tri.p[2]);
	if (area <= 0)
		return;

	if (config.lightingType == LightingType::FLAT)
	{
		// Map (-1, 1) to (1, 0), from dot prod. to shading factor
		float fac = (-normal.DotProduct(lightDirection) + 1) * 0.5f;
		color = DarkenColor(color, fac);
	}

	// Only check the pixels in the triangle's bounding box
	int minX = std::max(0, static_cast<int>(std::floorf(std::min({ tri.p[0].x, tri.p[1].x, tri.p[2].x }))));
	int maxX = std::min(SCREEN_WIDTH, static_cast<int>(std::ceilf(std::max({ tri.p[0].x, tri.p[1].x, tri.p[2].x }))));
	int minY = std::max(0, static_cast<int>(std::floorf(std::min({ tri.p[0].y, tri.p[1].y, tri.p[2].y }))));
	int maxY = std::min(SCREEN_HEIGHT, static_cast<int>(std::ceilf(std::max({ tri.p[0].y, tri.p[1].y, tri.p[2].y }))));

	// Test every pixel to see if it is inside the triangle
	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)
		{
			int idx = y * SCREEN_WIDTH + x;//x * SCREEN_HEIGHT + y;
			RVector3 point = RVector3(x, y, 0);

			if (config.useDepthBuffer)
			{
				float edge01 = Triangle2DArea(tri.p[0], tri.p[1], point);
				float edge12 = Triangle2DArea(tri.p[1], tri.p[2], point);
				float edge20 = Triangle2DArea(tri.p[2], tri.p[0], point);

				// Triangle area is positive when the third point is
				// "to the right" of the line formed by first and second
				// point. A point is inside the triangle if it is to the
				// right of every edge.
				if (edge01 >= 0 && edge12 >= 0 && edge20 >= 0)
				{
					edge01 /= area;
					edge12 /= area;
					edge20 /= area;

					// Interpolate the depth along the tri w/ barycentric coords
					float z = tri.p[0].z * edge12
						+ tri.p[1].z * edge20
						+ tri.p[2].z * edge01;
					if (z < depthBuffer[idx])
					{
						depthBuffer[idx] = z;

						if (config.lightingType == LightingType::GOURAUD)
						{
							float alpha = edge12;
							float beta = edge20;
							float gamma = 1 - alpha - beta;

							RVector3 interpNormal = RVector3(
								vertexNormals.p[0] * alpha
								+ vertexNormals.p[1] * beta
								+ vertexNormals.p[2] * gamma
							).Normalize();

							//interpNormal = interpNormal.RotateByQuaternion();
							//interpNormal = interpNormal.Transform(objRotZ).Transform(objRotX);
							float dot = (-interpNormal.DotProduct(lightDirection) + 1) * 0.5f;
							framebuffer[idx] = DarkenColor(color, dot);
						}
						else if (config.lightingType == LightingType::FLAT)
						{
							//DrawPixel(x, y, color);
							framebuffer[idx] = color;
						}

						if (z < minDepth)
							minDepth = z;
					}
				}
			}
			else
			{
				if (IsPointInsideTriangle2D(tri, point))
					framebuffer[idx] = color;
					//DrawPixel(x, y, color);
			}
		}
	}
}