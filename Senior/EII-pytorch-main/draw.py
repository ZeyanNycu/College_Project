import numpy as np
import cv2
import argparse

drawing = False  # true if mouse is pressed
mode = True  # if True, draw rectangle. Press 'm' to toggle to curve
color_guided = False  # if True, draw guided color

ix, iy = -1, -1
radius = 10

COLOR = (0, 0, 255)
WHITE = (255, 255, 255)

image = None
color = None
mask = None
guide = None


def nothing(x):
    pass


def draw(x, y):
    global mode, color_guided, COLOR, WHITE, image, guide, color, mask, radius, ix, iy
    if mode == True:
        if color_guided:
            cv2.rectangle(image, (ix, iy), (x, y), COLOR, -1)
            cv2.rectangle(guide, (ix, iy), (x, y), COLOR, -1)
        else:
            cv2.rectangle(image, (ix, iy), (x, y), WHITE, -1)
            cv2.rectangle(color, (ix, iy), (x, y), WHITE, -1)
            cv2.rectangle(mask, (ix, iy), (x, y), WHITE, -1)
    else:
        if color_guided:
            cv2.circle(image, (x, y), radius, COLOR, -1)
            cv2.circle(guide, (x, y), radius, COLOR, -1)
        else:
            cv2.circle(image, (x, y), radius, WHITE, -1)
            cv2.circle(color, (x, y), radius, WHITE, -1)
            cv2.circle(mask, (x, y), radius, WHITE, -1)


def mouse_callback(event, x, y, flags, param):
    global ix, iy, drawing
    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        ix, iy = x, y
    elif event == cv2.EVENT_MOUSEMOVE:
        if drawing == True:
            draw(x, y)
    elif event == cv2.EVENT_LBUTTONUP:
        drawing = False
        draw(x, y)


def main(img_path, output_path):
    global image, color, mask, guide, mode, color_guided, COLOR, radius

    color = cv2.imread(img_path)
    gray = color[:, :, 0] * 0.11 + color[:, :, 1] * 0.59 + color[:, :, 2] * 0.3
    mask = np.zeros(color.shape, np.uint8)
    guide = np.zeros(color.shape, np.uint8)

    h, w = color.shape[:2]
    image = np.concatenate((color, np.zeros((h // 3, w, 3), np.uint8)), axis=0)

    cv2.namedWindow('image')
    cv2.setMouseCallback('image', mouse_callback)
    cv2.createTrackbar('R', 'image', 0, 255, nothing)
    cv2.createTrackbar('G', 'image', 0, 255, nothing)
    cv2.createTrackbar('B', 'image', 0, 255, nothing)
    cv2.createTrackbar('radius', 'image', 0, 20, nothing)
    cv2.setTrackbarPos('radius', 'image', 10)

    while (1):
        cv2.imshow('image', image)

        k = cv2.waitKey(1) & 0xFF
        if k == ord('m'):
            mode = not mode
        elif k == ord('c'):
            color_guided = not color_guided
        elif k == ord('q') or k == 27:
            cv2.imwrite(f"color/{output_path}", color)
            cv2.imwrite(f"gray/{output_path}", gray)
            cv2.imwrite(f"mask/{output_path}", mask)
            cv2.imwrite(f"guide/{output_path}", guide)
            break

        r = cv2.getTrackbarPos('R', 'image')
        g = cv2.getTrackbarPos('G', 'image')
        b = cv2.getTrackbarPos('B', 'image')
        radius = cv2.getTrackbarPos('radius', 'image')
        image[h:] = [b, g, r]
        COLOR = (b, g, r)

    cv2.destroyAllWindows()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--image', type=str, default="images/img.png")
    parser.add_argument('--output', type=str, default="img.png")
    args = parser.parse_args()
    main(args.image, args.output)
