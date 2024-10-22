#include "mesh.h"

#include <fstream>
#include <strstream>

bool TriMesh::LoadFromOBJ(const std::string& path, TriMesh& outMesh, const RMatrix& model)
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
		else if (line[0] == 'v' && line[1] == 'n')
		{
			RVector3 v;
			s >> junk >> junk >> v.x >> v.y >> v.z;
			vNormals.push_back(v);
		}

		// Face
		else if (line[0] == 'f' && line[1] == ' ')
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

			for (auto& p : tri.p)		p = p.Transform(model);
			for (auto& p : normals.p)	p = p.Transform(model);

			mesh.tris.push_back(tri);
			mesh.vertexNormals.push_back(normals);
		}

		else
			s >> junk;
	}

	outMesh = mesh;
	return true;
}