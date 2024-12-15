#include "renderer.h"

#include <iostream>

void Renderer::Init()
{
	InitFramebuffer();

	// Init OpenGL stuff
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, &fb.colorBuffer.front());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error: OpenGL framebuffer is incomplete." << std::endl;
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::Shutdown()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &FBO);
}

void Renderer::SetClearColor(Color color)
{
	clearColor = color;
}

//void Renderer::DrawMesh(Mesh* mesh, Mat4 transform, Mat4 view, Mat4 proj)
//{
//	VertexUniforms u = { transform, view, proj, proj * view * transform };
//	commandQueue.push_back({ mesh, u });
//}

void Renderer::SendCommand(RenderCommand command)
{
	commandQueue.push_back(command);
}

void Renderer::FlushCommands()
{
	ClearFramebuffer();
	metrics.rasterizedTriangles = 0;
	metrics.clippedTriangles = 0;

	for (const auto& command : commandQueue)
	{
		if (!command.mesh)
			continue;

		curTexture = command.texture;

		Vec3 cameraPos = Vec3(
			command.uniforms.view[0][3], 
			command.uniforms.view[1][3], 
			command.uniforms.view[2][3]
		);

		for (int i = 0; i < command.mesh->v.size() / 3; i++)
		{
			//metrics.modelSpace = command.mesh->tris[i];

			Tri tri = {
				command.mesh->v[i * 3],
				command.mesh->v[i * 3 + 1],
				command.mesh->v[i * 3 + 2]
			};

			// Model Space -> Clip Space
			vertexShader(tri.v0, command.uniforms, &metrics);
			vertexShader(tri.v1, command.uniforms, nullptr);
			vertexShader(tri.v2, command.uniforms, nullptr);

			metrics.clipSpace = tri;
			tri.faceNormal = TriangleFaceNormal(tri.v0.p, tri.v1.p, tri.v2.p);

			// Near-culling 
			Tri subTris[2] = { tri };
			int subTrisCount = 1;
			if (config.doNearCulling)
			{
				subTrisCount = ClipTriAgainstPlane({ config.nearPlanePos, config.nearPlaneNormal },
					tri,
					subTris[0], subTris[1]);
			}
				

			for (int j = 0; j < subTrisCount; j++)
			{
				Tri& tri = subTris[j];

				// Perspective division
				for (Vertex& v : tri.v)
				{
					float w = v.p.w;
					v.p /= w;
					v.p.w = w;
				}

				metrics.ndcSpace = tri;

				// Backface culling
				if (config.doBackfaceCulling)
				{
					if (Triangle2DArea(tri.v0.p, tri.v1.p, tri.v2.p) <= 0)
						continue;
				}

				// Clip Space -> Screen Space
				for (int i = 0; i < 3; i++)
				{
					tri.v[i].p.x = (tri.v[i].p.x + 1.0f) * 0.5f * SCREEN_WIDTH;
					tri.v[i].p.y = (tri.v[i].p.y + 1.0f) * 0.5f * SCREEN_HEIGHT;
				}

				metrics.screenSpace = tri;
				metrics.rasterizedTriangles++;

				if (config.polygonMode == PolygonMode::POINT)
				{
					for (const auto& v : tri.v)
					{
						int x = (int)v.p.x;
						int y = (int)v.p.y;
						if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
							fb.colorBuffer[y * SCREEN_WIDTH + x] = WHITE;
					}
				}
				else if (config.polygonMode == PolygonMode::WIREFRAME)
				{
					for (int i = 0; i < 3; i++)
					{
						int x0 = (int)tri.v[i].p.x;
						int y0 = (int)tri.v[i].p.y;
						int x1 = (int)tri.v[(i + 1) % 3].p.x;
						int y1 = (int)tri.v[(i + 1) % 3].p.y;
						DrawLineBresenham(x0, y0, x1, y1, WHITE);
					}
				}
				else if (config.polygonMode == PolygonMode::FILLED || config.polygonMode == PolygonMode::DEPTH_MAP)
					RasterizeTriangle(tri);
			}
		}
	}

	if (config.polygonMode == PolygonMode::DEPTH_MAP)
	{
		for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		{
			//float factor = (fb.depthBuffer[i] - minDepth) / (maxDepth - minDepth);
			fb.colorBuffer[i] = DarkenColor(WHITE, 1 - fb.depthBuffer[i]);
		}
	}

	commandQueue.clear();
}

void Renderer::BlitToScreen()
{
	// update texture with color buffer data
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, &fb.colorBuffer.front());

	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);	// bind offscreen fb as read fb
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);		// bind default fb as draw fb
	glBlitFramebuffer(
		0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,			// source dimensions
		0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,			// destination dimensions
		GL_COLOR_BUFFER_BIT, GL_NEAREST				// blit only color with nearest-neighbor sampling
	);
}

GLuint Renderer::GetGLScreenTexture() const
{
	return screenTexture;
}

void Renderer::InitFramebuffer()
{
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	{
		fb.colorBuffer.push_back(BLACK);
		fb.depthBuffer.push_back(1.0f);//std::numeric_limits<float>::max());
	}
}

void Renderer::ClearFramebuffer()
{
	std::fill(fb.colorBuffer.begin(), fb.colorBuffer.end(), BLACK);
	std::fill(fb.depthBuffer.begin(), fb.depthBuffer.end(), 1.0f);//std::numeric_limits<float>::max());

	minDepth = std::numeric_limits<float>::max();
	maxDepth = std::numeric_limits<float>::min();
}

// HELPER FUNCTIONS

Color Renderer::DarkenColor(Color col, float factor) const
{
	return {
		static_cast<unsigned char>(col.r * factor),
		static_cast<unsigned char>(col.g * factor),
		static_cast<unsigned char>(col.b * factor),
		col.a
	};
}

Color Renderer::LerpColor(Color begin, Color end, float t) const
{
	return {
		(unsigned char)(begin.r + t * (end.r - begin.r)),
		(unsigned char)(begin.g + t * (end.g - begin.g)),
		(unsigned char)(begin.b + t * (end.b - begin.b)),
		(unsigned char)(begin.a + t * (end.a - begin.a)),
	};
}

bool Renderer::IsEdgeTopOrLeft(Vec3 a, Vec3 b) const
{
	return a.x > b.x;
}

bool Renderer::IsPointInsideTriangle2D(Vec3 a, Vec3 b, Vec3 c, Vec3 point) const
{
	return Triangle2DArea(a, b, point) >= 0
		&& Triangle2DArea(b, c, point) >= 0
		&& Triangle2DArea(c, a, point) >= 0;
}

float Renderer::Triangle2DArea(Vec3 a, Vec3 b, Vec3 c) const
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

float Renderer::TriangleHomogenousArea(Vec4 a, Vec4 b, Vec4 c) const
{
	return (b.x / b.w - a.x / a.w) * (c.y / c.w - a.y / a.w) 
		- (b.y / b.w - a.y / a.w) * (c.x / c.w - a.x / a.w);
}

Vec3 Renderer::TriangleFaceNormal(Vec3 a, Vec3 b, Vec3 c) const
{
	// The two lines running along the face
	Vec3 line1 = b - a;
	Vec3 line2 = c - a;
	return glm::normalize(glm::cross(line1, line2));
}

float Renderer::LinePlaneIntersection(Plane plane, Line line, Vec4& intersection) const
{
	//planeNormal = glm::normalize(planeNormal);
	float planeD = -glm::dot(plane.normal, plane.pos);
	float ad = glm::dot(Vec3(line.start), plane.normal);
	float bd = glm::dot(Vec3(line.end), plane.normal);
	float t = (-planeD - ad) / (bd - ad);

	intersection = line.start + t * (line.end - line.start);
	return t;
}

int Renderer::ClipTriAgainstPlane(Plane plane, Tri tri, Tri& outTri1, Tri& outTri2)
{
	//planeNormal = glm::normalize(planeNormal);

	const Vertex* inside[3];
	const Vertex* outside[3];
	int numInside = 0;
	int numOutside = 0;
	
	for (int i = 0; i < 3; i++)
	{
		float dist = (
			plane.normal.x * tri.v[i].p.x
			+ plane.normal.y * tri.v[i].p.y 
			+ plane.normal.z * tri.v[i].p.z
			- glm::dot(plane.normal, plane.pos)
		);

		if (dist >= 0)	
			inside[numInside++] = &tri.v[i];
		else			
			outside[numOutside++] = &tri.v[i];
	}

	// All points outside of the plane - discard tri
	if (numInside == 0)
		return 0;

	// All points are inside the plane - tri is all good
	if (numInside == 3)
	{
		outTri1 = tri;
		return 1;
	}

	metrics.clippedTriangles++;

	// One point inside -> clip w/ plane to form a triangle
	if (numInside == 1 && numOutside == 2)
	{
		outTri1.v0 = *inside[0];

		float t1 = LinePlaneIntersection(plane, { inside[0]->p, outside[0]->p }, outTri1.v1.p);
		outTri1.v1.t = inside[0]->t + t1 * (outside[0]->t - inside[0]->t);
		outTri1.v1.c = inside[0]->c + t1 * (outside[0]->c - inside[0]->c);

		float t2 = LinePlaneIntersection(plane, { inside[0]->p, outside[1]->p }, outTri1.v2.p);
		outTri1.v2.t = inside[0]->t + t2 * (outside[1]->t - inside[0]->t);
		outTri1.v2.c = inside[0]->c + t2 * (outside[1]->c - inside[0]->c);

		if (!IsCounterClockwise(outTri1))
			std::swap(outTri1.v0, outTri1.v2);

		return 1;
	}

	// Two points inside -> clip w/ plane to form a quad
	if (numInside == 2 && numOutside == 1)
	{
		outTri1.v0 = *inside[0];
		outTri1.v1 = *inside[1];
		float t1 = LinePlaneIntersection(plane, { inside[0]->p, outside[0]->p }, outTri1.v2.p);
		outTri1.v2.t = inside[0]->t + t1 * (outside[0]->t - inside[0]->t);
		//outTri1.v2.c = LerpColor(inside[0]->c, outside[0]->c, t1);

		outTri2.v0 = outTri1.v2;
		outTri2.v1 = *inside[1];
		float t2 = LinePlaneIntersection(plane, { inside[1]->p, outside[0]->p }, outTri2.v2.p);
		outTri2.v2.t = inside[1]->t + t2 * (outside[0]->t - inside[1]->t);
		//outTri2.v2.c = LerpColor(inside[1]->c, outside[0]->c, t2);

		if (!IsCounterClockwise(outTri1)) 
			std::swap(outTri1.v0, outTri1.v2);
		if (!IsCounterClockwise(outTri2)) 
			std::swap(outTri2.v0, outTri2.v2);

		return 2;
	}

	return -1;
}

bool Renderer::IsCounterClockwise(Tri& t) const
{
	float area = (t.v1.p.x - t.v0.p.x) * (t.v2.p.y - t.v0.p.y) - (t.v1.p.y - t.v0.p.y) * (t.v2.p.x - t.v0.p.x);
	return area > 0;
}

void Renderer::DrawLineBresenham(int x0, int y0, int x1, int y1, Color color)
{	
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;

	while (true) 
	{
		if (x0 >= 0 && x0 < SCREEN_WIDTH && y0 >= 0 && y0 < SCREEN_HEIGHT)
			fb.colorBuffer[y0 * SCREEN_WIDTH + x0] = color;

		if (x0 == x1 && y0 == y1) 
			return;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

void Renderer::RasterizeTriangle(Tri tri)
{
	float area = Triangle2DArea(tri.v0.p, tri.v1.p, tri.v2.p);

	// Perpsective-correct attribute interpolation
	if (config.doPerspCorrectInterp)
	{
		for (Vertex& v : tri.v)
		{
			v.c /= v.p.w;
			v.t /= v.p.w;
			v.p.w = 1.0f / v.p.w;
		}
	}

	float fac = (-glm::dot(tri.faceNormal, lightDirection) + 1) * 0.5f;
	Color darkedCol = DarkenColor(WHITE, fac);

	// Biases for abiding by fill rules
	//int bias0 = IsEdgeTopOrLeft(tri.p[1], tri.p[2]) ? 0 : -1;
	//int bias1 = IsEdgeTopOrLeft(tri.p[2], tri.p[0]) ? 0 : -1;
	//int bias2 = IsEdgeTopOrLeft(tri.p[0], tri.p[1]) ? 0 : -1;

	// Only check the pixels in the triangle's bounding box
	int minX = std::max(0, static_cast<int>(std::floorf(std::min({ tri.v0.p.x, tri.v1.p.x, tri.v2.p.x }))));
	int maxX = std::min(SCREEN_WIDTH, static_cast<int>(std::ceilf(std::max({ tri.v0.p.x, tri.v1.p.x, tri.v2.p.x }))));
	int minY = std::max(0, static_cast<int>(std::floorf(std::min({ tri.v0.p.y, tri.v1.p.y, tri.v2.p.y }))));
	int maxY = std::min(SCREEN_HEIGHT, static_cast<int>(std::ceilf(std::max({ tri.v0.p.y, tri.v1.p.y, tri.v2.p.y }))));
	
	// Incremental Pineda rasterization
	Vec3 point = Vec3(minX, minY, 0.0f);
	float row12 = Triangle2DArea(tri.v1.p, tri.v2.p, point);
	float row20 = Triangle2DArea(tri.v2.p, tri.v0.p, point);
	float row01 = Triangle2DArea(tri.v0.p, tri.v1.p, point);

	// https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
	float A01 = tri.v0.p.y - tri.v1.p.y;
	float A12 = tri.v1.p.y - tri.v2.p.y;
	float A20 = tri.v2.p.y - tri.v0.p.y;
	float B01 = tri.v1.p.x - tri.v0.p.x;
	float B12 = tri.v2.p.x - tri.v1.p.x;
	float B20 = tri.v0.p.x - tri.v2.p.x;

	for (int y = minY; y < maxY; y++)
	{
		float edge01 = row01;
		float edge12 = row12;
		float edge20 = row20;

		for (int x = minX; x < maxX; x++)
		{
			if (config.useDepthBuffer)
			{
				int idx = y * SCREEN_WIDTH + x;

				// Triangle area is positive when the third point is
				// "to the right" of the line formed by first and second
				// point. A point is inside the triangle if it is to the
				// right of every edge.
				if (edge01 >= 0 && edge12 >= 0 && edge20 >= 0)
				{
					float alpha = edge12 / area;
					float beta = edge20 / area;
					float gamma = 1.0f - alpha - beta;

					// Interpolate the depth along the tri w/ barycentric coords
					float z = tri.v0.p.z * alpha
							+ tri.v1.p.z * beta
							+ tri.v2.p.z * gamma;

					if (z < fb.depthBuffer[idx])
					{
						fb.depthBuffer[idx] = z;
						if (z < minDepth) minDepth = z;
						if (z > maxDepth) maxDepth = z;

						float w = (
							config.doPerspCorrectInterp 
							? tri.v0.p.w * alpha + tri.v1.p.w * beta + tri.v2.p.w * gamma
							: 1.0f
						);

						if (config.objectColor == ObjectColor::COLORED_VERTS)
						{
							Vec3 color = (tri.v0.c * alpha + tri.v1.c * beta + tri.v2.c * gamma) / w;
							fb.colorBuffer[idx] = {
								static_cast<unsigned char>(color.r * 255),
								static_cast<unsigned char>(color.g * 255),
								static_cast<unsigned char>(color.b * 255),
								255
							};
						}
						else if (config.objectColor == ObjectColor::CHECKERBOARD)
						{
							Vec2 uv = (tri.v0.t * alpha + tri.v1.t * beta + tri.v2.t * gamma) / w;

							const float M = 15.0f;
							float pattern = (fmodf(uv.x * M, 1.0f) > 0.5f) ^ (fmodf(uv.y * M, 1.0f) < 0.5f);
							unsigned char col = static_cast<unsigned char>(pattern * 255);
							fb.colorBuffer[idx] = { col, col, col, 255 };
						}
						else if (config.objectColor == ObjectColor::TEXTURED)
						{
							Vec2 uv = (tri.v0.t * alpha + tri.v1.t * beta + tri.v2.t * gamma) / w;

							int u = static_cast<int>(uv.x * curTexture->width);
							int v = static_cast<int>(uv.y * curTexture->height);
							int tx = (v * curTexture->width + u) * 4;
							if (tx >= 0 && tx < curTexture->width * curTexture->height * 4)
								fb.colorBuffer[idx] = { curTexture->data[tx], curTexture->data[tx + 1], curTexture->data[tx + 2], 255 };
						}
						else
						{
							fb.colorBuffer[idx] = darkedCol;
						}
					}
				}
			}
			else
			{
				if (edge01 >= 0 && edge12 >= 0 && edge20 >= 0)
					fb.colorBuffer[y * SCREEN_WIDTH + x] = darkedCol;
			}
			
			edge01 += A01;
			edge12 += A12;
			edge20 += A20;
		}

		row01 += B01;
		row12 += B12;
		row20 += B20;
	}
}