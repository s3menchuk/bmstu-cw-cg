from object import Object, Material
from light import Light
from color import BLACK


class Scene:
    def __init__(self) -> None:
        self.__objects: list[Object] = []
        self.__lights: list[Light] = []

    def add_obj(self, obj: Object):
        self.__objects.append(obj)

    def remove_obj(self, obj: Object):
        self.__objects.remove(obj)

    def add_light(self, light: Light):
        self.__lights.append(light)

    def remove_light_source(self, light: Light):
        self.__lights.remove(light)

    def iter_objects(self):
        return iter(self.__objects)

    def iter_lights(self):
        return iter(self.__lights)
