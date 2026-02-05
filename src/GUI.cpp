#include "GUI.hpp"
#include "AppContext.hpp"
#include "CanvasExporter.hpp"
#include "LightTypes.hpp"
#include "Math.hpp"
#include "PrimitiveTypes.hpp"
#include "Types.hpp"

#include "imgui.h"

void draw_camera_ui(Camera &camera, CameraSettings &settings) {
    if (ImGui::CollapsingHeader("Camera")) {
        float pos[3] = {camera.pos.x, camera.pos.y, camera.pos.z};
        ImGui::DragFloat3("Position", pos);
        camera.pos = pos;

        Real azimuth = radians2degrees(camera.get_azimuth());
        if (ImGui::SliderFloat("Azimuth", &azimuth, -180, 180))
            camera.set_azimuth(degrees2radians(azimuth));
        Real zenith = radians2degrees(camera.get_zenith());
        if (ImGui::SliderFloat("Zenith", &zenith, -radians2degrees(settings.max_zenith_radians), radians2degrees(settings.max_zenith_radians)))
            camera.set_zenith(degrees2radians(zenith));
    }
}

void draw_objects_ui(Scene &scene) {
    if (ImGui::CollapsingHeader("Objects")) {
        for (size_t i = 0; i < scene.objects.size();) {
            auto &obj = scene.objects[i];

            ImGui::PushID(&obj);

            Hittable *raw_obj = obj->get().get();
            const ePrimitive obj_type = PRIMITIVE_TYPES.at(typeid(*raw_obj));
            const std::string obj_name = PRIMITIVE_NAMES.at(obj_type);

            if (ImGui::TreeNode((obj_name + "##" + std::to_string(i)).c_str())) {
                if (obj_type == ePrimitive::Sphere) {
                    auto sphere = std::dynamic_pointer_cast<Sphere>(obj->get());

                    ImGui::Text("Center");
                    ImGui::SameLine();
                    std::array<float, 3> center(sphere->center);
                    if (ImGui::DragFloat3(("##Center" + std::to_string(i)).c_str(), center.data()))
                        sphere->center = center;

                    ImGui::Text("Radius");
                    ImGui::SameLine();
                    float radius = sphere->get_radius();
                    if (ImGui::DragFloat(("##Radius" + std::to_string(i)).c_str(), &radius, 1.0f, 0.01, 100))
                        sphere->set_radius(radius);
                } else if (obj_type == ePrimitive::Plane) {
                    auto plane = std::dynamic_pointer_cast<Plane>(obj->get());

                    ImGui::Text("Normal");
                    ImGui::SameLine();
                    std::array<float, 3> normal(plane->get_normal());
                    if (ImGui::DragFloat3(("##Normal" + std::to_string(i)).c_str(), normal.data()))
                        plane->set_normal(normal);  // TODO

                    ImGui::Text("Offset");
                    ImGui::SameLine();
                    ImGui::DragFloat(("##Offset" + std::to_string(i)).c_str(), &plane->offset, 1.0f, 0.01, 100);
                } else if (obj_type == ePrimitive::Box) {
                    std::shared_ptr<Box> box = std::dynamic_pointer_cast<Box>(obj->get());

                    ImGui::Text("Center");
                    ImGui::SameLine();
                    std::array<float, 3> center(box->get_center());
                    if (ImGui::DragFloat3(("##Center" + std::to_string(i)).c_str(), center.data()))
                        box->set_center(center);
                } else if (obj_type == ePrimitive::RightPrism) {
                    std::shared_ptr<RightPrism> prism = std::dynamic_pointer_cast<RightPrism>(obj->get());

                    ImGui::Text("Base center");
                    ImGui::SameLine();
                    std::array<float, 3> base_center(prism->get_base_center());
                    if (ImGui::DragFloat3(("##Center" + std::to_string(i)).c_str(), base_center.data()))
                        prism->set_base_center(base_center);

                    ImGui::Text("Radius");
                    ImGui::SameLine();
                    float radius = prism->get_radius();
                    if (ImGui::DragFloat(("##Radius" + std::to_string(i)).c_str(), &radius, 1.0f, 0.01, 100))
                        prism->set_radius(radius);

                    ImGui::Text("Height");
                    ImGui::SameLine();
                    float height = prism->get_height();
                    if (ImGui::DragFloat(("##Height" + std::to_string(i)).c_str(), &height, 1.0f, 0.01, 100))
                        prism->set_height(height);

                    ImGui::Text("Order");
                    ImGui::SameLine();
                    int order = prism->get_order();
                    if (ImGui::DragInt(("##Order" + std::to_string(i)).c_str(), &order, 1.0f, 3, 16))
                        prism->set_order(order);
                } else if (obj_type == ePrimitive::RightPyramid) {
                    std::shared_ptr<RightPyramid> pyramid = std::dynamic_pointer_cast<RightPyramid>(obj->get());

                    ImGui::Text("Base center");
                    ImGui::SameLine();
                    std::array<float, 3> base_center(pyramid->get_base_center());
                    if (ImGui::DragFloat3(("##Center" + std::to_string(i)).c_str(), base_center.data()))
                        pyramid->set_base_center(base_center);

                    ImGui::Text("Radius");
                    ImGui::SameLine();
                    float radius = pyramid->get_radius();
                    if (ImGui::DragFloat(("##Radius" + std::to_string(i)).c_str(), &radius, 1.0f, 0.01, 100))
                        pyramid->set_radius(radius);

                    ImGui::Text("Height");
                    ImGui::SameLine();
                    float height = pyramid->get_height();
                    if (ImGui::DragFloat(("##Height" + std::to_string(i)).c_str(), &height, 1.0f, 0.01, 100))
                        pyramid->set_height(height);

                    ImGui::Text("Order");
                    ImGui::SameLine();
                    int order = pyramid->get_order();
                    if (ImGui::DragInt(("##Order" + std::to_string(i)).c_str(), &order, 1.0f, 3, 16))
                        pyramid->set_order(order);
                } else if (obj_type == ePrimitive::Model) {
                } else
                    throw std::runtime_error("Unknown primitive type");

                // Rotation
                Vec3 rotation_vec = obj->get_rotation();
                float rotation_arr[3] = {rotation_vec.x, rotation_vec.y, rotation_vec.z};
                ImGui::Text("Rotation");
                ImGui::SameLine();
                if (ImGui::DragFloat3(("##Rotation" + std::to_string(i)).c_str(), rotation_arr, 1.0f, 0.0f, 360.0f))
                    obj->set_rotation(rotation_arr);

                // Color
                ImGui::Text("Color");
                ImGui::SameLine();
                std::array<float, 3> float_rgb = obj->material.color.as_linear_array();
                ImGui::ColorEdit3("", &float_rgb[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                Color new_color(float_rgb[0], float_rgb[1], float_rgb[2]);
                obj->material.color = new_color;

                // Visible
                ImGui::Text("Visible");
                ImGui::SameLine();
                ImGui::Checkbox("##", &obj->visible);

                // Delete button
                if (ImGui::Button("Delete"))
                    scene.objects.erase(scene.objects.begin() + i);
                else
                    ++i;

                ImGui::TreePop();
            } else
                ++i;
            ImGui::PopID();
        }

        static ePrimitive selected_obj_type = ePrimitive::Sphere;

        if (ImGui::BeginCombo("##combo_primitives", PRIMITIVE_NAMES.at(selected_obj_type).c_str())) {
            for (auto primitive : SELECTED_AVAILABLE_PRIMITIVES) {
                bool is_selected = (selected_obj_type == primitive);
                if (ImGui::Selectable(PRIMITIVE_NAMES.at(primitive).c_str(), is_selected)) {
                    selected_obj_type = primitive;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        static float sphere_center[3];
        static float sphere_radius = 1;

        static float plane_normal[3] = {-1, 0, 0};
        static float plane_offset = 0;

        static float box_center[3] = {};
        static float box_sizes[3] = {1, 1, 1};

        static float prism_base_center[3] = {};
        static float prism_radius = 1;
        static float prism_height = 1;
        static int prism_order = 3;

        static float pyramid_base_center[3] = {};
        static float pyramid_radius = 1;
        static float pyramid_height = 1;
        static int pyramid_order = 3;

        if (selected_obj_type == ePrimitive::Sphere) {
            ImGui::Text("Center");
            ImGui::SameLine();
            ImGui::DragFloat3("##sphere-center", sphere_center);

            ImGui::Text("Radius");
            ImGui::SameLine();
            ImGui::DragFloat("##sphere-radius", &sphere_radius, 1.0f, 0.01, 100);
        } else if (selected_obj_type == ePrimitive::Plane) {
            ImGui::Text("Normal");
            ImGui::SameLine();
            ImGui::DragFloat3("##plane-normal", plane_normal);

            ImGui::Text("Offset");
            ImGui::SameLine();
            ImGui::DragFloat("##plane-offset", &plane_offset, 1.0f, -100, 100);
        } else if (selected_obj_type == ePrimitive::Box) {
            ImGui::Text("Center");
            ImGui::SameLine();
            ImGui::DragFloat3("##box-center", box_center);

            ImGui::Text("Sizes");
            ImGui::SameLine();
            ImGui::DragFloat("##box-sizes", box_sizes);
        } else if (selected_obj_type == ePrimitive::RightPrism) {
            ImGui::Text("Base center");
            ImGui::SameLine();
            ImGui::DragFloat3("##prism-base_center", prism_base_center);

            ImGui::Text("Radius");
            ImGui::SameLine();
            ImGui::DragFloat("##prism-radius", &prism_radius, 1.0f, 0.01, 100);

            ImGui::Text("Height");
            ImGui::SameLine();
            ImGui::DragFloat("##prism-height", &prism_height, 1.0f, 0.01, 100);

            ImGui::Text("Order");
            ImGui::SameLine();
            ImGui::DragInt("##prism-order", &prism_order, 1.0f, 3, 16);
        } else if (selected_obj_type == ePrimitive::RightPyramid) {
            ImGui::Text("Base center");
            ImGui::SameLine();
            ImGui::DragFloat3("##pyramid-base_center", pyramid_base_center);

            ImGui::Text("Radius");
            ImGui::SameLine();
            ImGui::DragFloat("##pyramid-radius", &pyramid_radius, 1.0f, 0.01, 100);

            ImGui::Text("Height");
            ImGui::SameLine();
            ImGui::DragFloat("##pyramid-height", &pyramid_height, 1.0f, 0.01, 100);

            ImGui::Text("Order");
            ImGui::SameLine();
            ImGui::DragInt("##pyramid-order", &pyramid_order, 1.0f, 3, 16);
        }

        ImGui::Text("Color");
        ImGui::SameLine();
        static float rgb[3];
        ImGui::ColorEdit3("", rgb, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        if (ImGui::Button("Add object")) {
            std::shared_ptr<Hittable> primitive = nullptr;
            if (selected_obj_type == ePrimitive::Sphere)
                primitive = std::make_shared<Sphere>(sphere_center, sphere_radius);
            if (selected_obj_type == ePrimitive::Plane)
                primitive = std::make_shared<Plane>(plane_normal, plane_offset);
            if (selected_obj_type == ePrimitive::Box)
                primitive = std::make_shared<Box>(Vec3(box_center) - Vec3(box_sizes) / 2.f, Vec3(box_center) + Vec3(box_sizes) / 2.f);
            if (selected_obj_type == ePrimitive::RightPrism)
                primitive = std::make_shared<RightPrism>(prism_base_center, prism_radius, prism_height, prism_order);
            if (selected_obj_type == ePrimitive::RightPyramid)
                primitive = std::make_shared<RightPyramid>(pyramid_base_center, pyramid_radius, pyramid_height, pyramid_order);

            if (primitive) {
                Color color(rgb);
                auto object = std::make_shared<Object>(primitive, Material(color, 0));
                scene.add_object(object);
            }
        }
    }
}

void draw_lights_ui(Scene &scene) {
    if (ImGui::CollapsingHeader("Lights")) {
        for (int i = 0; i < scene.lights.size(); ++i) {
            auto &light = scene.lights[i];

            ImGui::PushID(&light);

            Light *raw_light = light.get();
            const eLight light_type = LIGHT_TYPES.at(typeid(*raw_light));
            const std::string light_name = LIGHT_NAMES.at(light_type);

            if (ImGui::TreeNode((light_name + "##" + std::to_string(i)).c_str())) {
                if (light_type == eLight::Directional) {
                    auto directional_light = std::dynamic_pointer_cast<DirectionLight>(light);

                    ImGui::Text("Dir");
                    ImGui::SameLine();
                    std::array<float, 3> dir(directional_light->dir);
                    if (ImGui::DragFloat3(("##Dir" + std::to_string(i)).c_str(), dir.data()))
                        directional_light->dir = dir;

                    ImGui::Text("Color");
                    ImGui::SameLine();
                    std::array<float, 3> float_rgb = directional_light->color.as_linear_array();
                    ImGui::ColorEdit3("", &float_rgb[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                    Color new_color(float_rgb[0], float_rgb[1], float_rgb[2]);
                    directional_light->color = new_color;

                    ImGui::Text("Intensity");
                    ImGui::SameLine();
                    float intensity = directional_light->intensity;
                    if (ImGui::DragFloat(("##Intensity" + std::to_string(i)).c_str(), &intensity, 1.0f, 0.01, 100))
                        directional_light->intensity = intensity;
                } else if (light_type == eLight::Point) {
                    auto point_light = std::dynamic_pointer_cast<PointLight>(light);

                    ImGui::Text("Pos");
                    ImGui::SameLine();
                    std::array<float, 3> pos(point_light->pos);
                    if (ImGui::DragFloat3(("##Pos" + std::to_string(i)).c_str(), pos.data()))
                        point_light->pos = pos;

                    ImGui::Text("Color");
                    ImGui::SameLine();
                    std::array<float, 3> float_rgb = point_light->color.as_linear_array();
                    ImGui::ColorEdit3("", &float_rgb[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                    Color new_color(float_rgb[0], float_rgb[1], float_rgb[2]);
                    point_light->color = new_color;

                    ImGui::Text("Intensity");
                    ImGui::SameLine();
                    float intensity = point_light->intensity;
                    if (ImGui::DragFloat(("##Intensity" + std::to_string(i)).c_str(), &intensity, 1.0f, 0.01, 100))
                        point_light->intensity = intensity;
                }

                ImGui::TreePop();
            }
            ImGui::PopID();
        }

        static eLight selected_light_type = eLight::Directional;

        if (ImGui::BeginCombo("##combo_lights", LIGHT_NAMES.at(selected_light_type).c_str())) {
            for (const auto &[light_type, light] : LIGHT_TYPES) {
                bool is_selected = (selected_light_type == light);
                if (ImGui::Selectable(LIGHT_NAMES.at(light).c_str(), is_selected)) {
                    selected_light_type = light;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        static float light_dir[3];
        static float light_pos[3];

        if (selected_light_type == eLight::Directional) {
            ImGui::Text("Dir");
            ImGui::SameLine();
            ImGui::DragFloat3("##light-dir", light_dir);
        } else if (selected_light_type == eLight::Point) {
            ImGui::Text("Pos");
            ImGui::SameLine();
            ImGui::DragFloat3("##light-pos", light_pos);
        }

        ImGui::Text("Color");
        ImGui::SameLine();
        static float light_color[3] = {1, 1, 1};
        ImGui::ColorEdit3("", light_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        static float light_intensity = 1;
        ImGui::Text("Intensity");
        ImGui::SameLine();
        ImGui::DragFloat("##light-intensity", &light_intensity, 1.0f, 0, 100);

        if (ImGui::Button("Add light")) {
            std::shared_ptr<Light> light;
            if (selected_light_type == eLight::Directional)
                light = std::make_shared<DirectionLight>(light_dir, light_color, light_intensity);
            else if (selected_light_type == eLight::Point)
                light = std::make_shared<PointLight>(light_pos, light_color, light_intensity);

            scene.add_light(light);
        }
    }
}

void draw_render_ui(AppContext &app) {
    if (ImGui::CollapsingHeader("Render settings")) {
        ImGui::Text("Max ray bounces");
        ImGui::SameLine();
        static int max_ray_bounces = app.render_settings.max_ray_bounces;
        const int MIN_VALUE_RAY_BOUNCES = 0;
        const int MAX_VALUE_RAY_BOUNCES = 20;
        ImGui::InputInt("##MaxRayBounces", &max_ray_bounces, MIN_VALUE_RAY_BOUNCES, MIN_VALUE_RAY_BOUNCES);
        max_ray_bounces = clamp(max_ray_bounces, MIN_VALUE_RAY_BOUNCES, MAX_VALUE_RAY_BOUNCES);
        app.render_settings.max_ray_bounces = max_ray_bounces;
    }

    if (ImGui::Button("Render")) {
        app.state.need_render = true;
    }

    if (ImGui::Button("Save image")) {
        std::shared_ptr<CanvasExporter> canvas_exporter = std::make_shared<CanvasPPMBinaryExporter>();
        canvas_exporter->save(app.canvas, "image.ppm");
    }
}

bool handle_keystrokes(Camera &camera, const Scene &scene, const CameraSettings &camera_settings) {
    auto camera_movement_speed = camera_settings.camera_movement_speed;
    auto camera_rotation_speed = camera_settings.camera_rotation_speed;
    auto max_zenith_radians = camera_settings.max_zenith_radians;

    bool is_key_pressed = false;

    // Movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        camera.pos += camera.dir * camera_movement_speed;
        is_key_pressed = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        camera.pos -= camera.right * camera_movement_speed;
        is_key_pressed = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        camera.pos -= camera.dir * camera_movement_speed;
        is_key_pressed = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        camera.pos += camera.right * camera_movement_speed;
        is_key_pressed = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::PageUp)) {
        camera.pos += scene.world_up * camera_movement_speed;
        is_key_pressed = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::PageDown)) {
        camera.pos -= scene.world_up * camera_movement_speed;
        is_key_pressed = true;
    }

    // Rotation
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && camera.get_zenith() <= max_zenith_radians) {
        camera.rotate_vertically(camera_rotation_speed);
        is_key_pressed = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && camera.get_zenith() >= -max_zenith_radians) {
        camera.rotate_vertically(-camera_rotation_speed);
        is_key_pressed = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        camera.rotate_by(scene.world_up, camera_rotation_speed);
        is_key_pressed = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        camera.rotate_by(scene.world_up, -camera_rotation_speed);
        is_key_pressed = true;
    }

    return is_key_pressed;
}

void draw_settings_iu(AppContext &app) {
    ImGui::Begin("Settings");
    draw_camera_ui(app.camera, app.camera_settings);
    draw_objects_ui(app.scene);
    draw_lights_ui(app.scene);
    draw_render_ui(app);
    ImGui::End();
}