from abc import ABC, abstractmethod

from color import Color
from vector import Vector
from ray import Ray


class Light(ABC):
    def __init__(self, color: Color, intensity: float) -> None:
        self.color = color
        self.intensity = intensity


class PointLight(Light):
    def __init__(self, pos: Vector, color: Color, intensity: float) -> None:
        super().__init__(color, intensity)
        self.pos = pos


class DirectionalLight(Light):
    def __init__(self, dir: Vector, color: Color, intensity: float) -> None:
        super().__init__(color, intensity)
        self.dir = dir
        self.dir.normalize()
