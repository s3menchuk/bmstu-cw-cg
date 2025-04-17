import PIL
import PIL.Image
import PIL.ImageDraw

from canvas import Canvas


def save2file(cnvs: Canvas, filename: str):
    # img = PIL.Image.new("RGB", cnvs.sizes())
    # draw = PIL.ImageDraw.Draw(img)

    # for x in range(cnvs.width):
    #     for y in range(cnvs.height):
    #         draw.point((x, y), cnvs.get_pixel(x, y).get())

    img = PIL.Image.fromarray(cnvs.get())
    print(cnvs.get())
    img.save(filename)
    img.show()
