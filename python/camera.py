from dataclasses import dataclass, field
import math

from vector import Vector

DEFAULT_NEAR = 0.001
DEFAULT_FAR = 1_000_000


@dataclass
class Camera:
    pos: Vector
    dir: Vector
    up: Vector
    right: Vector = field(init=False)
    fov: float = (2 / 3) * math.pi
    aspect: float = 16 / 9
    near: float = DEFAULT_NEAR
    far: float = DEFAULT_FAR

    def __post_init__(self):
        self.dir.normalize()
        self.up.normalize()
        self.right = self.dir.cross(self.up)
