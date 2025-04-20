from abc import ABC, abstractmethod
import math

from multiprocessing import Pool
import os
from itertools import product
from functools import partial

from camera import Camera, DEFAULT_NEAR, DEFAULT_FAR
from canvas import Canvas
from object import Object, EPSILON
from scene import Scene
from ray import Ray
from color import SKY_BLUE, WHITE, Color
from vector import Vector
from light import PointLight, DirectionalLight


"""
return Color.normalize(255 * normal.x**2, 255 * normal.y**2, 255 * normal.z**2)

return Color.normalize(
    255 * abs(normal.x), 255 * abs(normal.y), 255 * abs(normal.z)
)

r = my_map(normal.x, -1, 1, 0, 255)
g = my_map(normal.y, -1, 1, 0, 255)
b = my_map(normal.z, -1, 1, 0, 255)
return Color.normalize(r, g, b)
"""


class Renderer(ABC):
    @staticmethod
    @abstractmethod
    def render(canvas: Canvas, scene: Scene, camera: Camera): ...


def my_map(v, cl, cr, tl, tr):
    return tl + (v - cl) / (cr - cl) * (tr - tl)


def find_closest_obj(
    scene: Scene, ray: Ray, near: float = DEFAULT_NEAR, far: float = DEFAULT_FAR
) -> Object | None:
    closest_obj = None
    closest_dist = float("inf")
    for obj in scene.iter_objects():
        dist = obj.dist(ray)
        if dist and near <= dist <= far and dist < closest_dist:
            closest_dist = dist
            closest_obj = obj
    return closest_obj


def calc_pixel_color(
    pixel: tuple[int, int],
    canvas: Canvas,
    scene: Scene,
    camera: Camera,
    view_width: float,
    view_height: float,
    left_bottom_corner_view: Vector,
) -> tuple[int, int, Color]:
    x, y = pixel
    dx = my_map(x + 0.5, 0.5, canvas.width - 0.5, 0, view_width)
    dy = my_map(y + 0.5, 0.5, canvas.height - 0.5, 0, view_height)
    point = left_bottom_corner_view + camera.right * dx + camera.up * dy
    direction = point - camera.pos
    direction.normalize()
    ray = Ray(camera.pos.copy(), direction)
    color = RayTracingRenderer.trace_ray(scene, ray, near=camera.near, far=camera.far)
    return x, y, color


class RayTracingRenderer(Renderer):
    @staticmethod
    def render(canvas: Canvas, scene: Scene, camera: Camera):
        view_center = camera.pos + camera.dir * camera.near

        view_height = 2 * math.tan(camera.fov / 2) * camera.near
        view_width = view_height * camera.aspect

        left_bottom_corner_view = (
            view_center
            - camera.right * (view_width / 2)
            - camera.up * (view_height / 2)
        )
        print(view_center)
        print(left_bottom_corner_view)
        ray = Ray(camera.pos.copy(), Vector(0, 0, -1))

        for x in range(canvas.width):
            dx = my_map(x + 0.5, 0.5, canvas.width - 0.5, 0, view_width)
            for y in range(canvas.height):
                dy = my_map(y + 0.5, 0.5, canvas.height - 0.5, 0, view_height)
                point = left_bottom_corner_view + camera.right * dx + camera.up * dy
                direction = point - camera.pos
                direction.normalize()
                ray.direction = direction
                color = RayTracingRenderer.trace_ray(
                    scene, ray, near=camera.near, far=camera.far
                )
                canvas.set_pixel(x, y, color)

    @staticmethod
    def parallel_render(canvas: Canvas, scene: Scene, camera: Camera):
        view_center = camera.pos + camera.dir * camera.near

        view_height = 2 * math.tan(camera.fov / 2) * camera.near
        view_width = view_height * camera.aspect

        left_bottom_corner_view = (
            view_center
            - camera.right * (view_width / 2)
            - camera.up * (view_height / 2)
        )

        func = partial(
            calc_pixel_color,
            canvas=canvas,
            scene=scene,
            camera=camera,
            view_width=view_width,
            view_height=view_height,
            left_bottom_corner_view=left_bottom_corner_view,
        )

        with Pool(os.cpu_count()) as p:
            for x, y, c in p.map(
                func, product(range(canvas.width), range(canvas.height))
            ):
                canvas.set_pixel(x, y, c)

    @staticmethod
    def is_in_shadow(scene: Scene, point: Vector, light: DirectionalLight) -> bool:
        ray = Ray(point, -light.dir)
        for obj in scene.iter_objects():
            if EPSILON < obj.dist(ray) < float("inf"):
                return True
        return False

    @staticmethod
    def trace_ray(
        scene: Scene,
        ray: Ray,
        depth: int = 2,
        near: float = DEFAULT_NEAR,
        far: float = DEFAULT_FAR,
    ) -> Color:
        closest_obj = find_closest_obj(scene, ray, near, far)
        if closest_obj is None:
            k = my_map(ray.direction.x, -1, 1, 0, 1)
            return (1 - k) * SKY_BLUE + k * WHITE

        dist = closest_obj.dist(ray)
        intersection = ray.at(dist)
        normal = closest_obj.normal(intersection)

        final_color = Color()
        for light in scene.iter_lights():
            if isinstance(light, DirectionalLight):
                if not (
                    normal.dot(light.dir) >= 0
                    or RayTracingRenderer.is_in_shadow(scene, intersection, light)
                ):
                    N = normal
                    L = -light.dir
                    H = 2 * N.dot(L) * N - L
                    diffuse = (
                        closest_obj.material.diffuse
                        * light.color
                        * max(N.dot(L), 0)
                        * light.intensity
                    )
                    specular = (
                        closest_obj.material.specular
                        * light.color
                        * max(N.dot(H), 0) ** closest_obj.material.shininess
                        * light.intensity
                    )
                    final_color += diffuse + specular

        final_color = final_color.mul(closest_obj.material.color)
        final_color *= 1 - closest_obj.material.reflective

        if depth > 1 and closest_obj.material.reflective > 0:
            reflected_dir = ray.direction - 2 * normal.dot(ray.direction) * normal
            reflected_ray = Ray(intersection, reflected_dir)
            reflective_color = RayTracingRenderer.trace_ray(
                scene, reflected_ray, depth - 1, near, far
            )
            final_color += reflective_color * closest_obj.material.reflective

        return final_color
