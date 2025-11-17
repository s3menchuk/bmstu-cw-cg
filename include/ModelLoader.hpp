#pragma once

#include "Object.hpp"
#include "Types.hpp"
#include "Vec3.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class ModelLoader {
  public:
	virtual std::shared_ptr<GeometricPrimitive> load(const std::string &path) const = 0;
	virtual ~ModelLoader() = default;
};

class ObjLoader : public ModelLoader {
  public:
	std::shared_ptr<GeometricPrimitive> load(const std::string &path) const override {
		std::ifstream file(path);

		if (!file)
			return nullptr;

		std::vector<Vec3> vertices;
		std::vector<std::vector<size_t>> faces;

		std::string line;
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;

			if (prefix == "v") {
				Vec3 vertex;
				iss >> vertex.z >> vertex.x >> vertex.y;

				vertices.push_back(vertex);
			} else if (prefix == "f") {
				std::vector<size_t> face;
				std::string vertexToken;
				while (iss >> vertexToken) {
					size_t slashPos = vertexToken.find('/');
					std::string vertexIndexStr =
						(slashPos == std::string::npos) ? vertexToken : vertexToken.substr(0, slashPos);
					int vertexIndex = std::stoi(vertexIndexStr);
					face.push_back(static_cast<size_t>(vertexIndex - 1));
				}
				faces.push_back(face);
			}
		}

		T min_x = vertices[0].x;
		T max_x = vertices[0].x;
		T min_y = vertices[0].y;
		T max_y = vertices[0].y;
		T min_z = vertices[0].z;
		T max_z = vertices[0].z;
		for (const auto &v : vertices) {
			min_x = std::min(v.x, min_x);
			max_x = std::max(v.x, max_x);
			min_y = std::min(v.y, min_y);
			max_y = std::max(v.y, max_y);
			min_z = std::min(v.z, min_z);
			max_z = std::max(v.z, max_z);
		}
		T k = std::max(std::max(max_x - min_x, max_y - min_y), max_z - min_z);
		for (auto &v : vertices) {
			v.x = (v.x - min_x) / k;
			v.y = (v.y - min_y) / k;
			v.z = (v.z - min_z) / k;
			v = -v;
		}
		return std::make_shared<Model>(vertices, faces);
	}
};