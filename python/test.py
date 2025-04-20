import numpy as np
from PIL import Image
import math


class Vector3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __add__(self, other):
        return Vector3(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other):
        return Vector3(self.x - other.x, self.y - other.y, self.z - other.z)

    def __mul__(self, scalar):
        if isinstance(scalar, (int, float)):
            return Vector3(self.x * scalar, self.y * scalar, self.z * scalar)
        raise TypeError("Can only multiply Vector3 by scalar")

    def __neg__(self):
        return Vector3(-self.x, -self.y, -self.z)

    def dot(self, other):
        return self.x * other.x + self.y * other.y + self.z * other.z

    def norm(self):
        length = math.sqrt(self.dot(self))
        if length == 0:
            return Vector3(0, 0, 0)
        return Vector3(self.x / length, self.y / length, self.z / length)

    def __repr__(self):
        return f"Vector3({self.x}, {self.y}, {self.z})"


class Sphere:
    def __init__(self, center, radius, color, specular=0.0, reflective=0.0):
        self.center = center
        self.radius = radius
        self.color = color
        self.specular = specular
        self.reflective = reflective

    def intersect(self, ray_origin, ray_direction):
        oc = ray_origin - self.center
        a = ray_direction.dot(ray_direction)
        b = 2.0 * oc.dot(ray_direction)
        c = oc.dot(oc) - self.radius * self.radius
        discriminant = b * b - 4 * a * c

        if discriminant < 0:
            return float("inf")

        t1 = (-b + math.sqrt(discriminant)) / (2 * a)
        t2 = (-b - math.sqrt(discriminant)) / (2 * a)

        return min(t1, t2) if min(t1, t2) > 0.001 else max(t1, t2)


def trace_ray(ray_origin, ray_direction, objects, lights, depth=3):
    if depth <= 0:
        return Vector3(0, 0, 0)  # Черный цвет

    # Находим ближайший объект пересечения
    closest_t = float("inf")
    closest_obj = None

    for obj in objects:
        t = obj.intersect(ray_origin, ray_direction)
        if t < closest_t:
            closest_t = t
            closest_obj = obj

    if closest_obj is None:
        return Vector3(0.2, 0.7, 0.8)  # Цвет фона

    # Вычисляем точку пересечения и нормаль
    intersection = ray_origin + ray_direction * closest_t
    normal = (intersection - closest_obj.center).norm()

    # Локальное освещение
    color = closest_obj.color * compute_lighting(
        intersection, normal, -ray_direction, closest_obj.specular, objects, lights
    )

    # Отражение
    reflective_color = Vector3(0, 0, 0)
    if closest_obj.reflective > 0:
        reflected_dir = reflect_ray(-ray_direction, normal)
        reflective_color = trace_ray(
            intersection, reflected_dir, objects, lights, depth - 1
        )

    return (
        color * (1 - closest_obj.reflective) + reflective_color * closest_obj.reflective
    )


def compute_lighting(point, normal, view_dir, specular, objects, lights):
    intensity = 0.0
    for light in lights:
        if light["type"] == "ambient":
            intensity += light["intensity"]
        else:
            if light["type"] == "point":
                light_dir = (light["position"] - point).norm()
                light_distance = (light["position"] - point).dot(
                    light["position"] - point
                )
            else:  # directional
                light_dir = light["direction"].norm()
                light_distance = float("inf")

            # Проверка тени
            shadow_intersection = False
            shadow_t = 0
            for obj in objects:
                t = obj.intersect(point, light_dir)
                if 0.001 < t < light_distance:
                    shadow_intersection = True
                    break

            if shadow_intersection:
                continue

            # Диффузное освещение
            n_dot_l = normal.dot(light_dir)
            if n_dot_l > 0:
                intensity += (
                    light["intensity"] * n_dot_l / (normal.norm().dot(normal.norm()))
                )

            # Зеркальное освещение (по Фонгу)
            if specular > 0:
                reflected = reflect_ray(light_dir, normal)
                r_dot_v = reflected.dot(view_dir)
                if r_dot_v > 0:
                    intensity += light["intensity"] * math.pow(r_dot_v, specular)

    return intensity


def reflect_ray(ray, normal):
    return ray - normal * 2 * ray.dot(normal)


def render(width, height, fov, objects, lights):
    image = np.zeros((height, width, 3), dtype=np.uint8)
    aspect_ratio = width / height
    camera = Vector3(0, 0, 0)

    for y in range(height):
        for x in range(width):
            # Преобразование координат пикселя в координаты сцены
            px = (
                (2 * ((x + 0.5) / width) - 1)
                * math.tan(fov / 2 * math.pi / 180)
                * aspect_ratio
            )
            py = (1 - 2 * ((y + 0.5) / height)) * math.tan(fov / 2 * math.pi / 180)

            ray_dir = Vector3(px, py, 1).norm()
            color = trace_ray(camera, ray_dir, objects, lights)

            # Ограничиваем значения цвета и преобразуем в 8-битный формат
            r = min(255, max(0, int(color.x * 255)))
            g = min(255, max(0, int(color.y * 255)))
            b = min(255, max(0, int(color.z * 255)))

            image[y, x] = [r, g, b]

    return Image.fromarray(image, "RGB")


# Пример использования
if __name__ == "__main__":
    # Создаем сцену
    objects = [
        Sphere(Vector3(0, -1, 3), 1, Vector3(1, 0, 0), specular=500, reflective=0.2),
        Sphere(Vector3(2, 0, 4), 1, Vector3(0, 0, 1), specular=500, reflective=0.3),
        Sphere(Vector3(-2, 0, 4), 1, Vector3(0, 1, 0), specular=10, reflective=0.4),
        # Sphere(
        #     Vector3(0, -5001, 0), 5000, Vector3(1, 1, 0), specular=1000, reflective=0.5
        # ),
    ]

    lights = [
        {"type": "ambient", "intensity": 0.2},
        {"type": "point", "position": Vector3(2, 1, 0), "intensity": 0.6},
        {"type": "directional", "direction": Vector3(1, 4, 4), "intensity": 0.2},
    ]

    # Рендерим сцену
    img = render(800, 600, 60, objects, lights)
    img.save("ray_traced_image.png")
    img.show()
