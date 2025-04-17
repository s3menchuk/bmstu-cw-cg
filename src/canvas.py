import numpy as np

from color import Color


class Canvas:
    def __init__(
        self, width: int, height: int, background_color: Color | None = None
    ) -> None:
        self.__pixels = np.zeros((width, height, 3), dtype=np.uint8)
        self.__width = width
        self.__height = height
        self.__background_color = background_color

    @property
    def width(self) -> int:
        return self.__width

    @property
    def height(self) -> int:
        return self.__height

    @property
    def sizes(self) -> tuple[int, int]:
        return self.__width, self.__height

    def set_pixel(self, x: int, y: int, color: Color):
        self.__pixels[x, y] = color.R, color.G, color.B

    def get_pixel(self, x: int, y: int) -> Color:
        # return self.__data.get((x, y), self.__background_color)
        return Color(*self.__pixels[x, y])

    def get(self):
        return self.__pixels
