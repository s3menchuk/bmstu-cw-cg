from abc import ABC, abstractmethod

from camera import Camera
from canvas import Canvas
from scene import Scene


class Renderer(ABC):
    @staticmethod
    @abstractmethod
    def render(canvas: Canvas, scene: Scene, camera: Camera): ...


class RayTracingRenderer(Renderer):
    @staticmethod
    def render(canvas: Canvas, scene: Scene, camera: Camera): ...
