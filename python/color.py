from typing import Self


class Color:
    def __init__(self, R: int = 0, G: int = 0, B: int = 0) -> None:
        if not Color.check(R) or not Color.check(G) or not Color.check(B):
            print(R, G, B)
            raise ValueError("All color components must be between 0 and 255")
        self.R = R
        self.G = G
        self.B = B

    @staticmethod
    def check(c: int) -> bool:
        return 0 <= c <= 255

    @staticmethod
    def normalize(R: float = 0, G: float = 0, B: float = 0):
        if max(R, G, B) > 255:
            k = 255 / max(R, G, B)
            R *= k
            G *= k
            B *= k
        return Color(int(R), int(G), int(B))

    def get(self) -> tuple[int, int, int]:
        return self.R, self.G, self.B

    def avg(self, other: Self):
        return Color(
            (self.R + other.R) // 2, (self.G + other.G) // 2, (self.B + other.B) // 2
        )

    def mul(self, other: Self):
        R = int(self.R * other.R)
        G = int(self.G * other.G)
        B = int(self.B * other.B)
        return Color.normalize(R, G, B)

    def __add__(self, other: Self):
        R = self.R + other.R
        G = self.G + other.G
        B = self.B + other.B
        return Color.normalize(R, G, B)

    def __mul__(self, factor: float):
        R = self.R * factor
        G = self.G * factor
        B = self.B * factor
        return Color.normalize(R, G, B)

    def __rmul__(self, k: float):
        return self * k

    def __truediv__(self, k: float):
        return self * (1 / k)

    def __str__(self) -> str:
        return f"Color({self.R}, {self.G}, {self.B})"


BLACK = Color(0, 0, 0)
WHITE = Color(255, 255, 255)

RED = Color(R=255)
GREEN = Color(G=255)
BLUE = Color(B=255)
SKY_BLUE = Color(127, 178, 255)

YELLOW = Color(255, 255, 0)
PURPLE = Color(139, 0, 255)

GRAY = Color(128, 128, 128)
