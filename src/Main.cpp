#include "Camera.hpp"
#include "Canvas.hpp"
#include "Math.hpp"
#include "ModelLoader.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"

#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>

#include <array>
#include <iostream>
#include <memory>
#include <numbers>

/*
TODO:
	- GUI | Ok
	- Добавить реализацию оставшихся объектов | Ok
	- Reorganization object class hierarchy (Object, GeometricPrimitive, ...) | Ok???
	- Object rotation
	- Emission materials
	- Ambient light source
	- Performce

	- Reorganization method hit

	- Antialiasing (several rays per pixel)
	- Real diffuse (cancel - too much expensive)

	- BVH (AABB) | KD-Tree

	- Make Vec3 class template class
*/

namespace Settings {
constexpr float ASPECT = 1; // 16.0f / 9.0f
constexpr unsigned int WIDTH = 600;
constexpr unsigned int HEIGHT = WIDTH / ASPECT;

constexpr float FOV = std::numbers::pi / 2.0f; // std::numbers::pi * 2.0f / 3.0f;
constexpr float NEAR = 1.0f;
constexpr float FAR = 1000.0f;

constexpr float MAX_ZENITH_DEGREES = 75.0f;
constexpr float MAX_ZENITH_RADIANS = std::numbers::pi / 2 * MAX_ZENITH_DEGREES / 90;

constexpr Vec3 INIT_CAMERA_POS(-0.5, -0.25, 1);
constexpr Vec3 INIT_CAMERA_DIR(0, 0, -1);
constexpr float CAMERA_MOVEMENT_SPEED = 0.1;
constexpr float CAMERA_ROTATION_SPEED = 0.15;

size_t ray_tracing_depth = 2;
} // namespace Settings

struct AppContext {
	SFML_Canvas &canvas;
	Scene &scene;
	Camera &camera;
	Renderer &renderer;
};

void draw_camera_ui(Camera &camera) {
	if (ImGui::CollapsingHeader("Camera")) {
		float azimuth = radians2degrees(camera.get_azimuth());
		float zenith = radians2degrees(camera.get_zenith());

		ImGui::SliderFloat("x", &camera.pos.x, -25, 25);
		ImGui::SliderFloat("y", &camera.pos.y, -25, 25);
		ImGui::SliderFloat("z", &camera.pos.z, -25, 25);
		if (ImGui::SliderFloat("azimuth", &azimuth, -180, 180))
			camera.set_azimuth(degrees2radians(azimuth));
		if (ImGui::SliderFloat("zenith", &zenith, -Settings::MAX_ZENITH_DEGREES, Settings::MAX_ZENITH_DEGREES))
			camera.set_zenith(degrees2radians(zenith));
	}
}

void draw_objects_ui(Scene &scene) {
	if (ImGui::CollapsingHeader("Objects")) {
		std::unordered_map<std::string, size_t> counter;
		for (size_t i = 0; i < scene.objects.size();) {
			auto &obj = scene.objects[i];
			ImGui::PushID(&obj);

			const std::string obj_type = (*obj)->get_type();
			if (counter.find(obj_type) == counter.end())
				counter[obj_type] = 0;
			counter[obj_type]++;
			const std::string name = obj_type + "##" + std::to_string(counter[obj_type]);
			if (ImGui::TreeNode(name.c_str())) {
				if (obj_type == Sphere::type) {
					std::shared_ptr<Sphere> sphere(std::dynamic_pointer_cast<Sphere>(obj->operator->()));

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
				} else if (obj_type == Plane::type) {
					std::shared_ptr<Plane> plane(std::dynamic_pointer_cast<Plane>(obj->operator->()));

					ImGui::Text("Normal");
					ImGui::SameLine();
					std::array<float, 3> normal(plane->get_normal());
					if (ImGui::DragFloat3(("##Normal" + std::to_string(i)).c_str(), normal.data()))
						plane->set_normal(normal); // TODO

					ImGui::Text("Offset");
					ImGui::SameLine();
					ImGui::DragFloat(("##Offset" + std::to_string(i)).c_str(), &plane->offset, 1.0f, 0.01, 100);
				} else if (obj_type == Box::type) {
					std::shared_ptr<Box> box(std::dynamic_pointer_cast<Box>(obj->operator->()));

					ImGui::Text("Center");
					ImGui::SameLine();
					std::array<float, 3> center(box->get_center());
					if (ImGui::DragFloat3(("##Center" + std::to_string(i)).c_str(), center.data()))
						box->set_center(center);
				} else if (obj_type == RightPrism::type) {
					std::shared_ptr<RightPrism> prism(std::dynamic_pointer_cast<RightPrism>(obj->operator->()));

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
				} else if (obj_type == RightPyramid::type) {
					std::shared_ptr<RightPyramid> pyramid(std::dynamic_pointer_cast<RightPyramid>(obj->operator->()));

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
				} else if (obj_type == Model::type) {
				} else
					throw std::runtime_error("Unknown primitive type");

				// Rotation

				// Diffuse color
				ImGui::Text("Color");
				ImGui::SameLine();
				std::array<float, 3> float_rgb = obj->material.diffuse.as_float_rgb();
				ImGui::ColorEdit3("", &float_rgb[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				Color new_color(float_rgb);
				obj->material.diffuse = new_color;

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

		const char *primitives[] = {Sphere::type.c_str(), Plane::type.c_str(), Box::type.c_str(),
									RightPrism::type.c_str(), RightPyramid::type.c_str()};
		static int selected_primitive = 0;

		if (ImGui::BeginCombo("##combo_primitives", primitives[selected_primitive])) {
			for (int i = 0; i < IM_ARRAYSIZE(primitives); i++) {
				bool is_selected = (selected_primitive == i);
				if (ImGui::Selectable(primitives[i], is_selected)) {
					selected_primitive = i;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		static std::array<float, 3> sphere_center{};
		static float sphere_radius = 1;

		static std::array<float, 3> plane_normal = {-1, 0, 0};
		static float plane_offset = 0;

		static std::array<float, 3> box_center{};
		static std::array<float, 3> box_sizes = {1, 1, 1};

		static std::array<float, 3> prism_base_center{};
		static float prism_radius = 1;
		static float prism_height = 1;
		static int prism_order = 3;

		static std::array<float, 3> pyramid_base_center{};
		static float pyramid_radius = 1;
		static float pyramid_height = 1;
		static int pyramid_order = 3;

		if (primitives[selected_primitive] == Sphere::type.c_str()) {
			ImGui::Text("Center");
			ImGui::SameLine();
			ImGui::DragFloat3("##sphere-center", sphere_center.data());

			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::DragFloat("##sphere-radius", &sphere_radius, 1.0f, 0.01, 100);
		} else if (primitives[selected_primitive] == Plane::type.c_str()) {
			ImGui::Text("Normal");
			ImGui::SameLine();
			ImGui::DragFloat3("##plane-normal", plane_normal.data());

			ImGui::Text("Offset");
			ImGui::SameLine();
			ImGui::DragFloat("##plane-offset", &plane_offset, 1.0f, -100, 100);
		} else if (primitives[selected_primitive] == Box::type.c_str()) {
			ImGui::Text("Center");
			ImGui::SameLine();
			ImGui::DragFloat3("##box-center", box_center.data());

			ImGui::Text("Sizes");
			ImGui::SameLine();
			ImGui::DragFloat("##box-sizes", box_sizes.data());
		} else if (primitives[selected_primitive] == RightPrism::type.c_str()) {
			ImGui::Text("Base center");
			ImGui::SameLine();
			ImGui::DragFloat3("##prism-base_center", prism_base_center.data());

			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::DragFloat("##prism-radius", &prism_radius, 1.0f, 0.01, 100);

			ImGui::Text("Height");
			ImGui::SameLine();
			ImGui::DragFloat("##prism-height", &prism_height, 1.0f, 0.01, 100);

			ImGui::Text("Order");
			ImGui::SameLine();
			ImGui::DragInt("##prism-order", &prism_order, 1.0f, 3, 16);
		} else if (primitives[selected_primitive] == RightPyramid::type.c_str()) {
			ImGui::Text("Base center");
			ImGui::SameLine();
			ImGui::DragFloat3("##pyramid-base_center", pyramid_base_center.data());

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
		static std::array<float, 3> float_rgb{};
		ImGui::ColorEdit3("", &float_rgb[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		const char *MATTE_PLASTIC = "Matte Plastic";
		const char *GLOSSY_PLASTIC = "Glossy Plastic";
		const char *METAL = "Metal";
		const char *GLASS = "Glass";

		const char *materials[] = {MATTE_PLASTIC, GLOSSY_PLASTIC, METAL, GLASS};
		static int selected_material = 0;

		if (ImGui::BeginCombo("##combo_materials", materials[selected_material])) {
			for (int i = 0; i < IM_ARRAYSIZE(materials); i++) {
				bool is_selected = (selected_material == i);
				if (ImGui::Selectable(materials[i], is_selected)) {
					selected_material = i;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Add object")) {
			std::shared_ptr<GeometricPrimitive> primitive;
			if (primitives[selected_primitive] == Sphere::type.c_str())
				primitive = std::make_shared<Sphere>(sphere_center, sphere_radius);
			if (primitives[selected_primitive] == Plane::type.c_str())
				primitive = std::make_shared<Plane>(plane_normal, plane_offset);
			if (primitives[selected_primitive] == Box::type.c_str())
				primitive = std::make_shared<Box>(Vec3(box_center) - Vec3(box_sizes) / 2.f,
												  Vec3(box_center) + Vec3(box_sizes) / 2.f);
			if (primitives[selected_primitive] == RightPrism::type.c_str())
				primitive = std::make_shared<RightPrism>(prism_base_center, prism_radius, prism_height, prism_order);
			if (primitives[selected_primitive] == RightPyramid::type.c_str())
				primitive =
					std::make_shared<RightPyramid>(pyramid_base_center, pyramid_radius, pyramid_height, pyramid_order);

			std::shared_ptr<Object> object;
			Color color(float_rgb);
			std::shared_ptr<Material> material;
			if (materials[selected_material] == MATTE_PLASTIC)
				object = std::make_shared<Object>(primitive, MattePlastic(color));
			if (materials[selected_material] == GLOSSY_PLASTIC)
				object = std::make_shared<Object>(primitive, GlossyPlastic(color));
			if (materials[selected_material] == METAL)
				object = std::make_shared<Object>(primitive, Metal(color));
			if (materials[selected_material] == GLASS)
				object = std::make_shared<Object>(primitive, Glass(color));

			scene.add_object(object);
		}
	}
}

void draw_lights_ui() {
	if (ImGui::CollapsingHeader("Lights")) {
	}
}

void draw_render_ui(const AppContext &app, bool is_key_pressed) {
	ImGui::Text("RT Depth");
	ImGui::SameLine();
	static int rt_depth = Settings::ray_tracing_depth;
	ImGui::SliderInt("##RT-Depth", &rt_depth, 1, 5);
	Settings::ray_tracing_depth = rt_depth;

	if (ImGui::Button("Render") || is_key_pressed) {
		auto start = std::chrono::high_resolution_clock::now();

		app.renderer.render(app.canvas, app.scene, app.camera, Settings::ray_tracing_depth);

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "Time: " << elapsed.count() << " ms\n";
	}
}

bool process_key_input(Camera &camera) {
	bool is_key_pressed = false;

	// Movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		camera.pos += camera.dir * Settings::CAMERA_MOVEMENT_SPEED;
		is_key_pressed = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		camera.pos -= camera.right * Settings::CAMERA_MOVEMENT_SPEED;
		is_key_pressed = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		camera.pos -= camera.dir * Settings::CAMERA_MOVEMENT_SPEED;
		is_key_pressed = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		camera.pos += camera.right * Settings::CAMERA_MOVEMENT_SPEED;
		is_key_pressed = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::PageUp)) {
		camera.pos -= camera.up * Settings::CAMERA_MOVEMENT_SPEED;
		is_key_pressed = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::PageDown)) {
		camera.pos += camera.up * Settings::CAMERA_MOVEMENT_SPEED;
		is_key_pressed = true;
	}

	// Rotation
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && camera.get_zenith() <= Settings::MAX_ZENITH_RADIANS) {
		camera.rotate_vertically(-Settings::CAMERA_ROTATION_SPEED);
		is_key_pressed = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && camera.get_zenith() >= -Settings::MAX_ZENITH_RADIANS) {
		camera.rotate_vertically(Settings::CAMERA_ROTATION_SPEED);
		is_key_pressed = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
		camera.rotate_horizontally(Settings::CAMERA_ROTATION_SPEED);
		is_key_pressed = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
		camera.rotate_horizontally(-Settings::CAMERA_ROTATION_SPEED);
		is_key_pressed = true;
	}

	return is_key_pressed;
}

int main() {
	sf::RenderWindow window(sf::VideoMode({Settings::WIDTH, Settings::HEIGHT}), "Graphics Engine");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	auto canvas = std::make_unique<SFML_Canvas>(Settings::WIDTH, Settings::HEIGHT); // std::unique_ptr<Canvas>
	Camera camera(Settings::INIT_CAMERA_POS, Settings::INIT_CAMERA_DIR, Settings::FOV, Settings::ASPECT, Settings::NEAR,
				  Settings::FAR);

	Scene scene;
	scene.ambient_color = WHITE;
	scene.ambient_intensity = 0.2;

	scene.add_object(
		std::make_shared<Object>(std::make_shared<Box>(Vec3(0, -0.5, -0.5), Vec3(-1, -1.5, -1.5)), GlossyPlastic(RED)));
	scene.add_object(std::make_shared<Object>(std::make_shared<Sphere>(Vec3(-5, 10, 0), 5), Metal(BLUE), false));
	scene.add_object(std::make_shared<Object>(std::make_shared<Plane>(Vec3(-1, 0, 0), 0), GlossyPlastic(GRAY)));
	/*scene.add_object(std::make_shared<Object>(std::make_shared<Sphere>(Vec3(-0.5, 0.5, -0.75), 0.5), Metal(GREEN)));
	scene.add_object(std::make_shared<Object>(std::make_shared<RightPrism>(Vec3(-8, 0, 0), 5, 6, 16), Glass(GREEN),
	false)); scene.add_object(std::make_shared<Object>(std::make_shared<RightPyramid>(Vec3(0, 10, 8), 5, 8, 3),
	MattePlastic(ORANGE), false));*/

	/*std::unique_ptr<ModelLoader> loader = std::make_unique<ObjLoader>();
	scene.add_object(std::make_shared<Object>(loader->load("pinguin.obj"), Glass(ORANGE)));*/

	// scene.add_light(std::make_shared<DirectionLight>(Vec3(1, -1, -1), WHITE, 0.5));

	auto renderer = std::make_unique<RayTracingRenderer>();

	AppContext app = {*canvas, scene, camera, *renderer};

	sf::Clock deltaClock;
	while (window.isOpen()) {
		bool is_key_pressed = false;
		while (const std::optional event = window.pollEvent()) {
			ImGui::SFML::ProcessEvent(window, *event);
			if (event->is<sf::Event::Closed>())
				window.close();
			is_key_pressed = process_key_input(camera);
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Settings");
		draw_camera_ui(camera);
		draw_objects_ui(scene);
		draw_lights_ui();
		draw_render_ui(app, is_key_pressed);
		ImGui::End();

		window.clear();
		window.draw(canvas->pixels);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}