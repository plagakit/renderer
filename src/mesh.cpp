#include "mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <strstream>

bool Mesh::LoadFromOBJ(std::string path, Mesh& outMesh, bool isRHS, const Mat4& model)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

	if (!warn.empty())	std::cout << warn << std::endl;
	if (!err.empty())	std::cerr << err << std::endl;
	if (!ret) return false;

	for (const auto& face : shapes[0].mesh.indices)
	{
		Vertex v;
		int vid = face.vertex_index;
		int nid = face.normal_index;
		int tid = face.texcoord_index;

		// Vertex positions
		v.p.x = attrib.vertices[vid * 3];
		v.p.y = attrib.vertices[vid * 3 + 1];
		v.p.z = attrib.vertices[vid * 3 + 2] * (isRHS ? -1.0f : 1.0f);
		v.p.w = 1.0f;
		v.p = model * v.p;

		// Vertex normals
		if (vid >= 0) 
		{
			v.n.x = attrib.normals[nid * 3];
			v.n.y = attrib.normals[nid * 3 + 1];
			v.n.z = attrib.normals[nid * 3 + 2];
			v.n = Vec3(model * Vec4(v.n, 0.0f));
		}

		// Texture coordinates
		if (tid >= 0)
		{
			v.t.x = attrib.texcoords[tid * 2];
			v.t.y = attrib.texcoords[tid * 2 + 1];
		}

		// Colors
		v.c.r = attrib.colors[3 * face.vertex_index + 0];
		v.c.g = attrib.colors[3 * face.vertex_index + 1];
		v.c.b = attrib.colors[3 * face.vertex_index + 2];

		outMesh.vertCount++;
		outMesh.v.push_back(v);
	}
}

Texture::Texture(const char* path)
{
	int success = stbi_info(path, &width, &height, &channels);
	if (!success)
	{
		std::cout << "Error finding texture '" << path << "': " << stbi_failure_reason() << std::endl;
		exit(1);
	}

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path, &width, &height, &channels, 4);
	if (data == NULL)
	{
		std::cout << "Error loading texture '" << path << "': " << stbi_failure_reason() << std::endl;
		exit(1);
	}
}

Texture::~Texture()
{
	stbi_image_free(data);
}
