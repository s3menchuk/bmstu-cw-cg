from dataclasses import dataclass

from vector import Vector


@dataclass
class Ray:
    origin: Vector
    direction: Vector

    def at(self, dist: float) -> Vector:
        return self.origin + self.direction * dist
