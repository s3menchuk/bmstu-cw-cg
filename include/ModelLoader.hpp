#pragma once

#include "Object.hpp"
#include "Types.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class ModelLoader {
  public:
    virtual std::shared_ptr<Hittable> load(const std::string &path) const = 0;
    virtual ~ModelLoader() = default;
};

class ObjLoader : public ModelLoader {
  public:
    std::shared_ptr<Hittable> load(const std::string &path) const override {
        std::ifstream file(path);

        if (!file)
            return nullptr;

        std::vector<Vec3> coords;
        std::vector<std::vector<VertexIndex>> faces;
        std::vector<Vec3> normals;
        std::vector<Vec2> textures;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                Vec3 pos;
                iss >> pos.x >> pos.y >> pos.z;
                coords.push_back(pos);
            } else if (prefix == "vn") {
                Vec3 norm;
                iss >> norm.x >> norm.y >> norm.z;
                normals.push_back(norm);
            } else if (prefix == "vt") {
                Vec2 texture;
                iss >> texture.x >> texture.y;
                textures.push_back(texture);
            } else if (prefix == "f") {
                std::vector<VertexIndex> face;
                std::string vertex_token;
                while (iss >> vertex_token) {
                    std::replace(vertex_token.begin(), vertex_token.end(), '/', ' ');
                    std::istringstream viss(vertex_token);
                    VertexIndex vertex_index;
                    viss >> vertex_index.v >> vertex_index.vt >> vertex_index.vn;
                    vertex_index.v--;
                    vertex_index.vt--;
                    vertex_index.vn--;
                    face.push_back(vertex_index);
                }
                faces.push_back(face);
            }
        }

        return std::make_shared<Model>(coords, faces, normals);
    }
};
