import numpy as np
import math
from typing import Self

"""
# Rotate X
([1, 0, 0],)
([0, math.cos(angle), -math.sin(angle)],)
([0, math.sin(angle), math.cos(angle)],)

# Rotate Y
([math.cos(angle), 0, math.sin(angle)],)
([0, 1, 0],)
([-math.sin(angle), 0, math.cos(angle)],)

# Rotate Z
([math.cos(angle), -math.sin(angle), 0],)
([math.sin(angle), math.cos(angle), 0],)
([0, 0, 1],)
"""


class Vector:
    def __init__(self, x: float = 0, y: float = 0, z: float = 0):
        self.x = x
        self.y = y
        self.z = z

    def get(self):
        return [self.x, self.y, self.z]

    def copy(self):
        return Vector(self.x, self.y, self.z)

    def length(self) -> float:
        return math.sqrt(self.x**2 + self.y**2 + self.z**2)

    def normalize(self):
        length = self.length()
        self.x /= length
        self.y /= length
        self.z /= length

    def dot(self, other: Self) -> float:
        return self.x * other.x + self.y * other.y + self.z * other.z

    def cross(self, other: Self):
        return Vector(*map(float, np.cross(self.get(), other.get())))

    def sqr(self) -> float:
        return self.dot(self)

    def __neg__(self):
        return Vector(-self.x, -self.y, -self.z)

    def __add__(self, other: Self):
        return Vector(self.x + other.x, self.y + other.y, self.z + other.z)

    def __mul__(self, scalar: float):
        return Vector(self.x * scalar, self.y * scalar, self.z * scalar)

    def __sub__(self, other: Self):
        return self + (-other)

    def __rmul__(self, k: float):
        return self * k

    def __truediv__(self, k: float):
        return self * (1 / k)

    def __str__(self) -> str:
        return f"({self.x}, {self.y}, {self.z})"

    def __repr__(self) -> str:
        return f"Vector({self.x}, {self.y}, {self.z})"
