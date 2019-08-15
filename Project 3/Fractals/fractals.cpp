// This is the CPP file you will edit and turn in. (TODO: Remove this comment!)

#include "fractals.h"
using namespace std;

int checkPixelColor(GWindow& window, int x, int y, int originalColor, int newColor);

void drawSierpinskiTriangle(GWindow& window, double x, double y, double size, int order) {
    if (order == 1) {
        window.drawLine(x, y, x + size, y);
        window.drawLine(x, y, x + 0.5*size, y + 0.5*sqrt(3)*size);
        window.drawLine(x + size, y, x + 0.5*size, y + 0.5*sqrt(3)*size);
    } else if (order > 1) {
        drawSierpinskiTriangle(window, x, y, 0.5*size, order-1);
        drawSierpinskiTriangle(window, x + 0.5*size, y, 0.5*size, order-1);
        drawSierpinskiTriangle(window, x + 0.25*size, y + 0.25*sqrt(3)*size, 0.5*size, order-1);
    }
}

int floodFill(GWindow& window, int x, int y, int color) {
    int originalColor = window.getPixel(x, y);
    if (originalColor == color) {
        // Base case: if the pixel's original color is the same
        // as the color to be changed, do nothing and return 0
        return 0;
    } else {
        // Recursive case:
        // To fill an area of pixels with the color specified,
        // check the pixels in 4 directions (top, left, bottom, right)
        // and floodFill them if their color is the same as the original color
        window.setPixel(x, y, color);
        double width = window.getCanvasWidth();
        double height = window.getCanvasHeight();

        int left = 0; int right = 0; int top = 0; int bot = 0;
        if (x > 0) {
            left = checkPixelColor(window, x-1, y, originalColor, color);
        }
        if (x < width-1) {
            right = checkPixelColor(window, x+1, y, originalColor, color);
        }
        if (y > 0) {
            top = checkPixelColor(window, x, y-1, originalColor, color);
        }
        if (y < height-1) {
            bot = checkPixelColor(window, x, y+1, originalColor, color);
        }
        return left + right + top + bot + 1;   // return the total number of pixels changed
    }
}

int checkPixelColor(GWindow& window, int x, int y, int originalColor, int newColor) {
    if (window.getPixel(x, y) == originalColor) {
        // **************************************************************************
        // By intuition, this should be "1+floodFill(window, x, y, newColor)".
        // But after checking the x and y coordinates of pixels,
        // it seems that "0.5+floodFill(window, x, y, newColor)" will give me
        // the answer closer to the correct number of pixels.
        // If you find out the logic here, please feel free to modify it
        // **************************************************************************
        return 0.5+floodFill(window, x, y, newColor);
    }
    return 0;
}
