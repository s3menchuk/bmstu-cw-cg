from camera import Camera
from scene import Scene
from canvas import Canvas
from renderer import Renderer


def main():
    """
    1. Scene
    2. Camera
    3. Canvas
    4. Renderer
    5. Light sources
    """
    scene: Scene
    camera: Camera
    canvas: Canvas
    renderer: Renderer
    Renderer.render(canvas, scene, camera)
    ...


if __name__ == "__main__":
    main()
