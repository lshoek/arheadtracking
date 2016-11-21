#include "model.h"

std::string replace(std::string str, std::string toReplace, std::string replacement)
{
	size_t index = 0;

	while (true)
	{
		index = str.find(toReplace, index);
		if (index == std::string::npos) break;
		str.replace(index, toReplace.length(), replacement);
		++index;
	}
	return str;
}

std::vector<std::string> split(std::string str, std::string sep)
{
	std::vector<std::string> ret;
	size_t index;
	while (true)
	{
		index = str.find(sep);
		if (index == std::string::npos) break;
		ret.push_back(str.substr(0, index));
		str = str.substr(index + 1);
	}
	ret.push_back(str);
	return ret;
}

std::string toLower(std::string data)
{
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

OGLModel::OGLModel(std::string fname)
{
	std::string dname = fname;
	if (dname.rfind("/") != std::string::npos) dname = dname.substr(0, dname.rfind("/"));
	if (dname.rfind("\\") != std::string::npos) dname = dname.substr(0, dname.rfind("\\"));
	if (fname == dname) dname = "";

	std::ifstream pFile(fname.c_str());

	ObjGroup* currentGroup = new ObjGroup();
	currentGroup->materialIndex = -1;

	while (!pFile.eof())
	{
		std::string line;
		std::getline(pFile, line);

		line = replace(line, "\t", " ");
		while (line.find("  ") != std::string::npos) line = replace(line, "  ", " ");
		if (line == "")	continue;
		if (line[0] == ' ')	line = line.substr(1);
		if (line == "")	continue;
		if (line[line.length()-1] == ' ') line = line.substr(0, line.length() - 1);
		if (line == "")	continue;
		if (line[0] == '#') continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if (params[0] == "v")
			verts.push_back(glm::vec3((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), (float)atof(params[3].c_str())));
		else if (params[0] == "vn")
			normals.push_back(glm::vec3((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), (float)atof(params[3].c_str())));
		else if (params[0] == "vt")
			texcoords.push_back(glm::vec2((float)atof(params[1].c_str()), (float)atof(params[2].c_str())));
		else if (params[0] == "f")
		{
			for (size_t ii = 4; ii <= params.size(); ii++)
			{
				Face face;
				for (size_t i = ii - 3; i < ii; i++)
				{
					Vertex vertex;
					std::vector<std::string> indices = split(params[i == (ii - 3) ? 1 : i], "/");
					if (indices.size() >= 1) vertex.position = atoi(indices[0].c_str()) - 1;
					if (indices.size() == 2) vertex.texcoord = atoi(indices[1].c_str()) - 1;
					if (indices.size() == 3) 
					{
						if (indices[1] != "") vertex.texcoord = atoi(indices[1].c_str()) - 1;
						vertex.normal = atoi(indices[2].c_str()) - 1;
					}
					face.vertices.push_back(vertex);
				}
				currentGroup->faces.push_back(face);
			}
		}
		else if (params[0] == "s");
		else if (params[0] == "mtllib") loadMaterialFile(dname + "/" + params[1], dname);
		else if (params[0] == "usemtl")
		{
			if (currentGroup->faces.size() != 0) groups.push_back(currentGroup);
			currentGroup = new ObjGroup();
			currentGroup->materialIndex = -1;

			for (size_t i = 0; i < materials.size(); i++)
			{
				MaterialInfo* info = materials[i];
				if (info->name == params[1])
				{
					currentGroup->materialIndex = i;
					break;
				}
			}
			if (currentGroup->materialIndex == -1)
				std::cout << "Could not find material name " << params[1] << std::endl;
		}
	}
	groups.push_back(currentGroup);
}


OGLModel::~OGLModel(void)
{

}

void OGLModel::draw()
{
	for (auto group : groups)
	{
		glBindTexture(0, materials[group->materialIndex]->hasTexture);

		glBegin(GL_TRIANGLES);
		for (auto face : group->faces)
		{
			if (face.vertices.size() == 3)
			{
				//alles oke
			}
			if (face.vertices.size() != 3)
			{
				//probeem
				//glBegin
				//4 ken ik fixen
			}
			for (auto vertex : face.vertices)
			{
				glNormal3f(normals[vertex.normal].x, normals[vertex.normal].y, normals[vertex.normal].z);
				glTexCoord2f(texcoords[vertex.texcoord].x, texcoords[vertex.texcoord].y);
				glVertex3f(verts[vertex.position].x, verts[vertex.position].y, verts[vertex.position].z);
			}
		}
		glEnd();
	}
	glEnd();
}

void OGLModel::loadMaterialFile(std::string fname, std::string dname)
{
	std::ifstream pFile(fname.c_str());
	MaterialInfo* currentMaterial = NULL;

	while (!pFile.eof())
	{
		std::string line;
		std::getline(pFile, line);

		line = replace(line, "\t", " ");
		while (line.find("  ") != std::string::npos) line = replace(line, "  ", " ");
		if (line == "") continue;
		if (line[0] == ' ') line = line.substr(1);
		if (line == "") continue;
		if (line[line.length() - 1] == ' ') line = line.substr(0, line.length() - 1);
		if (line == "") continue;
		if (line[0] == '#') continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if (params[0] == "newmtl")
		{
			if (currentMaterial != NULL) materials.push_back(currentMaterial);
			currentMaterial = new MaterialInfo();
			currentMaterial->name = params[1];
		}
		else if (params[0] == "map_kd")
		{
			currentMaterial->hasTexture = true;
			// currentMaterial->texture = new Texture(dirName + "/" + params[1]);
		}
		else std::cout << "Didn't parse " << params[0] << " in material file" << std::endl;
	}
	if (currentMaterial != NULL) materials.push_back(currentMaterial);
}

OGLModel::MaterialInfo::MaterialInfo()
{
	hasTexture = false;
}
