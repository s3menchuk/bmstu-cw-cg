from abc import ABC, abstractmethod
from dataclasses import dataclass

from ray import Ray
from vector import Vector
from color import Color

import math

EPSILON = 1e-7


@dataclass
class Material:
    color: Color
    reflective: float = 0.5
    diffuse: float = 0.5
    specular: float = 4
    shininess: float = 32


class Object(ABC):
    def __init__(self, material: Material) -> None:
        self.material = material

    def is_intersect(self, ray: Ray) -> bool:
        return self.dist(ray) < float("inf")

    @abstractmethod
    def dist(self, ray: Ray) -> float: ...

    @abstractmethod
    def normal(self, point: Vector) -> Vector: ...


class Sphere(Object):
    def __init__(self, center: Vector, radius: float, material: Material) -> None:
        super().__init__(material)
        self.center = center
        self.radius = radius

    def dist(self, ray: Ray) -> float:
        co = ray.origin - self.center

        a = ray.direction.sqr()
        b = 2 * ray.direction.dot(co)
        c = co.sqr() - self.radius**2

        D = b**2 - 4 * a * c
        if D < 0:
            return float("inf")
        t1 = (-b - math.sqrt(D)) / (2 * a)
        t2 = (-b + math.sqrt(D)) / (2 * a)

        t_min = float("inf")
        if t1 >= 0:
            t_min = t1
        if t2 >= 0 and (t_min is None or t2 < t_min):
            t_min = t2
        return t_min

    def normal(self, point: Vector) -> Vector:
        n = point - self.center
        n.normalize()
        return n


class Plane(Object):
    def __init__(self, N: Vector, D: float, material: Material) -> None:
        super().__init__(material)
        self.N = N
        self.D = D

    def dist(self, ray: Ray) -> float:
        N = self.N.dot(ray.origin) + self.D
        D = self.N.dot(ray.direction)
        if D == 0:
            return float("inf")
        t = -N / D
        if t < 0:
            return float("inf")
        return t

    def normal(self, point: Vector) -> Vector:
        return self.N
