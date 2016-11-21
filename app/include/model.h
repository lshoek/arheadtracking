#include "stdafx.h"
#include <fstream>
#include <algorithm>

class OGLModel
{
private:
	class Vertex {
	public:
		int position;
		int normal;
		int texcoord;
	};

	class Face {
	public:
		std::vector<Vertex> vertices;
	};

	class MaterialInfo {
	public:
		MaterialInfo();
		std::string name;
		bool hasTexture;
	};

	class ObjGroup {
	public:
		std::string name;
		int materialIndex;
		std::vector<Face> faces;
	};

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;

	std::vector<ObjGroup*> groups;
	std::vector<MaterialInfo*> materials;

	void loadMaterialFile(std::string fileName, std::string dirName);

public:
	OGLModel(std::string filename);
	~OGLModel(void);

	void draw();
};
