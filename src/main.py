import math
import pygame

from camera import Camera
from scene import Scene
from canvas import Canvas
from renderer import RayTracingRenderer, Renderer
from vector import Vector
from color import BLACK, RED, GREEN, BLUE, WHITE, YELLOW, GRAY, PURPLE
from image_saver import save2file
from object import Sphere, Plane, Material
from light import DirectionalLight

WIDTH = 400
ASPECT = 1
HEIGHT = int(WIDTH / ASPECT)

FOV = 2 / 3 * math.pi
NEAR = 1

BACKGROUND_COLOR = BLACK


def main():
    scene: Scene = Scene()

    scene.add_obj(Plane(Vector(-1, 0, 0), 0, Material(GRAY, 0.2)))
    scene.add_obj(Sphere(Vector(-10, 5, 0), 10, Material(RED, 0.65)))
    scene.add_obj(Sphere(Vector(-7.5, 20, 10), 7.5, Material(GREEN, 0.25)))
    scene.add_obj(Sphere(Vector(-5, 30, 20), 5, Material(BLUE, 0.4)))
    # scene.add_obj(Sphere(Vector(10, 10, -50), 50, Material(YELLOW, 0.5)))
    scene.add_light(DirectionalLight(Vector(3, 10, -3), YELLOW, 0.7))

    camera: Camera = Camera(
        pos=Vector(-10, 15, 40),
        dir=Vector(0, 0, -1),
        up=Vector(1, 0, 0),
        fov=FOV,
        aspect=ASPECT,
        near=NEAR,
    )

    canvas: Canvas = Canvas(WIDTH, HEIGHT, BACKGROUND_COLOR)
    renderer: Renderer = RayTracingRenderer()
    # renderer.render(canvas, scene, camera)

    # save2file(canvas, "image.png")

    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    done = False

    changed = True

    speed = 1

    while not done:
        pressed = pygame.key.get_pressed()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True
            if pressed[pygame.K_w]:
                camera.pos += camera.dir * speed
                changed = True
            if pressed[pygame.K_s]:
                camera.pos -= camera.dir * speed
                changed = True
            if pressed[pygame.K_d]:
                camera.pos += camera.right * speed
                changed = True
            if pressed[pygame.K_a]:
                camera.pos -= camera.right * speed
                changed = True

        if changed:
            renderer.render(canvas, scene, camera)
            pygame.surfarray.blit_array(screen, canvas.get())
            pygame.display.flip()
            changed = False


if __name__ == "__main__":
    main()
