#include "renderer.h"

#include <iostream>
#include <algorithm>
#include <vector>	
#include <limits>
#include <fstream>
#include <strstream>

void Renderer::Init()
{
	threadPool.Start();

	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		depthBuffer.push_back(std::numeric_limits<float>::max());

	ConstructProjectionMatrix();
	
	if (!LoadFromObjFile("res/teapot.obj", object))
	{
		printf("Suzanne couldn't load!\n");
		exit(1);
	}

	printf("Initialized!");
}

void Renderer::Update(float dt)
{
	rotation += dt;
	objRotX = objRotX.RotateX(rotation * 0.28f);
	objRotZ = objRotZ.RotateZ(rotation);
}

void Renderer::Render()
{
	DrawLine(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RED);
	ResetDepthBuffer();

	//std::cout << object.tris.size() << std::endl;
	//
	//std::vector<Tri> tris = object.tris;
	//std::vector<Tri> vn;
	//std::vector<RVector3> fn;
	//std::fill_n(vn.begin(), object.tris.size(), tris[0]);
	//std::fill_n(fn.begin(), object.tris.size(), RVector3::Zero);

	//for (int i = 0; i < object.tris.size(); i++)
	//{
	//	threadPool.QueueJob([this, i, &tris, &vn, &fn] {
	//		std::cout << i << std::endl;
	//		Tri tri = object.tris[i];

	//		ModelSpaceToWorldSpace(tri);

	//		// Cull if not facing camera (backface)
	//		RVector3 cameraFacing = RVector3(0, 0, 1);
	//		RVector3 normal = TriangleFaceNormal(tri);
	//		vn[i] = object.vertexNormals[i];
	//		fn[i] = normal;
	//		if (normal.DotProduct(cameraFacing) < 0)
	//			return;

	//		WorldSpaceToScreenSpace(tri);
	//		tris[i] = tri;

	//		//RasterizeTriangle(tri, object.vertexNormals[i], normal, GREEN);
	//	});
	//}

	for (int i = 0; i < object.tris.size(); i++)
	{
		Tri tri = object.tris[i];

		ModelSpaceToWorldSpace(tri);

		// Cull if not facing camera (backface)
		RVector3 cameraFacing = RVector3(0, 0, 1);
		RVector3 normal = TriangleFaceNormal(tri);
		if (normal.DotProduct(cameraFacing) < 0)
			continue;
		
		WorldSpaceToScreenSpace(tri);

		RasterizeTriangle(tri, object.vertexNormals[i], normal, GREEN);
		//DrawTriangle(tri, RAYWHITE);
	}

	if (lightingType == LightingType::DEPTH_MAP)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			for (int y = 0; y < SCREEN_HEIGHT; y++)
			{
				float pixel = depthBuffer[x * SCREEN_HEIGHT + y];
				float diff = minDepth + 1 - pixel;
				DrawPixel(x, y, DarkenColor(WHITE, diff));
			}
		}
		DrawFPS(0, 0);
	}
}

void Renderer::Shutdown()
{
	threadPool.Stop();
}

// HELPER METHODS

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

void Renderer::ResetDepthBuffer()
{
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		depthBuffer[i] = std::numeric_limits<float>::max();
	minDepth = std::numeric_limits<float>::max();
}

bool Renderer::LoadFromObjFile(std::string path, TriMesh& outMesh) const
{
	std::ifstream f(path);
	if (!f.is_open())
		return false;

	TriMesh mesh;
	std::vector<RVector3> vertices;
	std::vector<RVector3> vNormals;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;
		
		char junk;

		// Vertex
		if (line[0] == 'v' && line[1] == ' ')
		{
			RVector3 v;
			s >> junk >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}

		// Vertex normal
		if (line[0] == 'v' && line[1] == 'n')
		{
			RVector3 v;
			s >> junk >> junk >> v.x >> v.y >> v.z;
			vNormals.push_back(v);
		}

		// Face
		if (line[0] == 'f' && line[1] == ' ')
		{
			int f[3], n[3];
			int i = 0;

			s >> junk;
			std::string token;
			while (s >> token)
			{
				size_t pos1 = token.find('/');
				f[i] = std::stoi(token.substr(0, pos1));

				std::string remainder = token.substr(pos1 + 1, std::string::npos);
				size_t pos2 = remainder.find('/');
				n[i] = std::stoi(remainder.substr(pos2 + 1, std::string::npos));

				i++;
			}

			Tri tri = { vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1] };
			Tri normals = { vNormals[n[0] - 1], vNormals[n[1] - 1], vNormals[n[2] - 1] };

			mesh.tris.push_back(tri);
			mesh.vertexNormals.push_back(normals);
		}
	}

	outMesh = mesh;
	return true;
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

void Renderer::ModelSpaceToWorldSpace(Tri& tri) const
{
	for (auto& p : tri.p)
	{
		// Scale
		p *= 0.25f;

		// Rotate
		p = p.Transform(objRotZ).Transform(objRotX);

		// Translate
		p += RVector3(0, 0, 10.0f);
	}
}

void Renderer::WorldSpaceToScreenSpace(Tri& tri) const
{
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
}

void Renderer::DrawTriangle(const Tri& tri, Color color)
{
	DrawLine(tri.p[0].x, tri.p[0].y, tri.p[1].x, tri.p[1].y, color);
	DrawLine(tri.p[1].x, tri.p[1].y, tri.p[2].x, tri.p[2].y, color);
	DrawLine(tri.p[2].x, tri.p[2].y, tri.p[0].x, tri.p[0].y, color);
}

void Renderer::RasterizeTriangle(const Tri& tri, const Tri& vertexNormals, const RVector3& normal, Color color)
{
	// We treat this triangle as a 2D triangle, ignoring Z
	float area = Triangle2DArea(tri.p[0], tri.p[1], tri.p[2]);
	if (area <= 0)
		return;

	if (lightingType == LightingType::FLAT)
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
			RVector3 point = RVector3(x, y, 0);

			if (useDepthBuffer)
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
					if (z < depthBuffer[x * SCREEN_HEIGHT + y])
					{
						depthBuffer[x * SCREEN_HEIGHT + y] = z;

						if (lightingType == LightingType::GOURAUD)
						{
							float alpha = edge12;
							float beta = edge20;
							float gamma = 1 - alpha - beta;

							RVector3 interpNormal = (
								vertexNormals.p[0] * alpha
								+ vertexNormals.p[1] * beta
								+ vertexNormals.p[2] * gamma
							).Normalize();

							interpNormal = interpNormal.Transform(objRotZ).Transform(objRotX);
							float dot = (-interpNormal.DotProduct(lightDirection) + 1) * 0.5f;
							DrawPixel(x, y, DarkenColor(color, dot));
						}
						else
						{
							DrawPixel(x, y, color);
						}

						if (z < minDepth)
							minDepth = z;
					}
				}
			}
			else
			{
				if (IsPointInsideTriangle2D(tri, point))
					DrawPixel(x, y, color);
			}
		}
	}
}