/*
 * File: fractals.cpp
 * --------------------------
 * Name: Silvia Fernandez (SUNet ID: silviaf)
 * Section leader: Nick Bowman
 * This file contains the code for CS 106B's fractal problems in assignment 3.
 * Specifically this file contains code for 3 functions that will draw in the graphical window
 * passed as reference to them, respectively:
 * - Sierpinski triangles: fractal set of equilateral triangles subdivided into smaller equilateral
 * triangles using recursion. The triangle at order K is composed of 3 triangles of order K-1 type
 * which have BRANCH_RATIO (in our case half) the edge length of the original triangle and are
 * positioned at the corners of the larger triangle.
 * - A recursive fractal tree image: when drawn, the tree will have at least one trunk, then K-1
 * levels of subtrees formed by NBRANCHES branches (K is the order of the tree passed to the function).
 * The branches in each subtree are half the length or their parent branch length and extend from the
 * tip of the previous tree trunk at relative angles relative to their parent branch's angle of +-45,
 * +-30, +-15 and 0.
 * - A Mandelbrot set visualization: colors points in the graphical window if the complex number formed
 * by their coordinates are in the Mandelbrot set. In this program a complex number is in the Mandelbrot set
 * if we can successfully apply a maxium number of iterations (passed to this function)
 * the recursive formula Zn+1 = Zn^2 + c starting at Z0 =1; with the absolute value of the result never
 * exceeding ABS_MAX.
 * This file only includes homework core functionality. Please refer to any files with "extension" as suffix
 * for extension functionality.
 */
 
 
#include "fractals.h"
#include "gmath.h"
#include "gtypes.h"
#include "gobjects.h"
#include "gwindow.h"
 
using namespace std;
 
const int LEAF_COLOR = 0x2e8b57;   /* Color of all leaves of recursive tree (level 1) */
const int BRANCH_COLOR = 0x8b7765; /* Color of all branches of recursive tree (level >=2) */
/* Error message displayed when parameter passed to drawSierpinksiTriangle function < 0*/
const string ERROR_SIERPINSKI = "All parameters passed to drawSierpinksiTriangle function should be > 0";
/* Error message displayed when any parameter passed to the drawTree function is < 0 */
const string ERROR_DRAWTREE = "All parameters passed to the drawTree function should be > 0";
 
const int NBRANCHES = 7; /* Number of branches on trees for orders > 1*/
/* Limit for the absolute value of the complex number coordinates */
const int ABS_MAX = 4;
const double BRANCH_RATIO = 0.5; /* Ratio between child and parent branch length */
const double TRIANGLE_RATIO = 0.5; /* Ratio between child and parent triangle length*/
const double INITIAL_OFFSET_ANGLE = 90; /* Offset Angle for tree's order 1 branch */
const double BRANCHES_ANGLE_SEP = 15; /* Angle between 2 of the 7 branches at each level */
/*Absolute value of angle between outside branch and x axis"*/
const double EXTERNAL_BRANCH_XANGLE = 45;
 
/* Function prototypes */
void drawSierpinskiTriangle(GWindow &gw, double x, double y, double size, int order);
void triangleDrawerHelper(GWindow gw, double x1, double y1, double x2, double y2, double x3, double y3, int order);
void drawingHelper (GWindow gw, double  xBase, double yBase, double size, int order, double offsetAngle);
int mandelbrotSetIterations(Complex z, Complex c, int remainingIterations);
void mandelbrotSet(GWindow& gw, double minX, double incX,
                   double minY, double incY, int maxIterations, int color);
int mandelbrotSetIterations(Complex c, int maxIterations);
 
 
 
/** Function: drawSierpinksiTriangle()
 * Usage: draw SierpinksiTriange(gw, x, y, size, order)
 * -----------------------------------------------------
 * Draws a Sierpinski triangle of the specified size and order, placing its top-left corner at
 * position (x, y). This recursive function will only draw a figure when the values of all the
 * parameters passed are > 0. This will be called by fractalgui.cpp. Takes as parameters:
 * @param gw - The window in which to draw the Sierpinski triangle.
 * @param x - The x coordinate of the top-left corner of the triangle.
 * @param y - The y coordinate of the top-left corner of the triangle.
 * @param size - The length of one side of the triangle.
 * @param order - The order of the fractal.
 */
void drawSierpinskiTriangle(GWindow &gw, double x, double y, double size, int order) {
    // Order = 0 does not draw anything
    if (order == 0) {
        return;
    } else if (order < 0 || x < 0 || y < 0 || size < 0) {
        cout << ERROR_SIERPINSKI << endl;
    } else if (order == 1) {
        // base case: order = 1
        gw.drawLine(x, y, x + size, y);
        gw.drawLine(x, y, x + size * TRIANGLE_RATIO, y + sqrt(3) * TRIANGLE_RATIO * size);
        gw.drawLine(x + size, y, x + size * TRIANGLE_RATIO, y + sqrt(3) * TRIANGLE_RATIO * size);
    } else {
        // recursive case
        // calculate size of child triangle
        size = size * TRIANGLE_RATIO;
        drawSierpinskiTriangle(gw, x, y, size, order - 1);
        drawSierpinskiTriangle(gw, x + size, y, size, order - 1);
        drawSierpinskiTriangle(gw, x + size * TRIANGLE_RATIO, y + sqrt(3) * size * TRIANGLE_RATIO, size, order - 1);
    }
}
 
/** Function: drawTree()
 * Usage: drawTree(gw, x, y, size, order)
 * --------------------------------------
 * Draws a recursive tree fractal image of the specified size and order, placing the bounding box's
 * top-left corner at position (x,y). Each level of the image with order > 1 has NBRANCHES.
 * Relies on a recursive helper method, drawingHelper, to draw NBRANCHES for orders > 1.
 * This will be called by fractalgui.cpp.
 * This function takes as parameters:
 * @param gw - The window in which to draw the recursive tree.
 * @param x - The x coordinate of the top-left corner of the bounding box.
 * @param y - The y coordinate of the top-left corner of the bounding box.
 * @param size - The length of one side of the bounding box.
 * @param order - The order of the fractal.
 */
void drawTree(GWindow& gw, double x, double y, double size, int order) {
    if (order == 0) {
        return;
    } else if (order < 0 || x < 0 || y < 0 || size < 0) {
        cout << ERROR_DRAWTREE << endl;
      } else {
        drawingHelper(gw, x + size * BRANCH_RATIO, y + size, size * BRANCH_RATIO, order, INITIAL_OFFSET_ANGLE);
    }
}
 
/** Function: drawingHelper()
 * Usage: drawingHelper(gw, x, y, size, order, offsetAngle)
 * --------------------------------------------------------
 * Draws the NBRANCHES branches of a recursive tree fractal image for the specified size and order,
 * placing the beginning of the branches at (xBase, yBase), which is the end point of the
 * parent branch.
 * The branches at each level are half the size of the branches of the parent branch and are
 * at +-45, +-30, +-15 and 0 degrees relative to their parent branch's angle (offsetAngle).
 * The branches from the last level of the tree are of color LEAF_COLOR, all other branches are drawn
 * in color BRANCH_COLOR. This function is recursive. This function does not draw the branch for
 * order 1, which is drawn by drawTree instead.
 * Assumptions: all the parameters passed to this method except from offsetAngle are > 0.
 * This function takes as parameters:
 * @param gw - The window in which to draw the recursive tree.
 * @param xBase - The x coordinate where the NBRANCHES of the order being drawn start.
 * @param yBase - The y coordinate where the NBRANCHES of the order being drawn start.
 * @param size - The length of the parent branch.
 * @param order - The order of the fractal.
 * @param offsetAngle - The angle of the parent branch.
 */
 
void drawingHelper (GWindow gw, double  xBase, double yBase, double size, int order, double offsetAngle) {
    if (order == 0) {
        return;
    } else {
        // We set the correct color for a given order
        if (order == 1) {
            gw.setColor(LEAF_COLOR);
        }
        else {
            gw.setColor(BRANCH_COLOR);
        }
        //We draw the parent branch
        GPoint point = gw.drawPolarLine(xBase, yBase, size, offsetAngle);
        double xEndPoint = point.getX();
        double yEndPoint = point.getY();
        //We draw the children
        for (int i = 0; i < NBRANCHES; i++) {
            double angleForDrawing = i * BRANCHES_ANGLE_SEP - EXTERNAL_BRANCH_XANGLE + offsetAngle;
            double newOffsetAngle  = angleForDrawing;
            if (order > 1) {
                drawingHelper(gw, xEndPoint, yEndPoint, size * BRANCH_RATIO, order - 1, newOffsetAngle);
            }
        }
    }
}
 
/** Function mandelbrotSet()
 * Usage: mandelbrotSet(gw, minX, incX, minY, incY, maxIterations, color)
 * -----------------------------------------------------------------------
 * Draws a Mandelbrot Set in the graphical window given, with maxIterations (size in GUI) and in a
 * given color (zero for palette). Specifically, this function colors points in the graphical window
 * given if the complex number c formed by its coordinates (c = x+iy) are in the MandelBrot Set.
 * This method uses a helper function, mandelbrotSetIterations, which calculates the number of
 * iterations we ran the Mandelbrot Set recursive formula on c. If the number of iterations returned
 * by mandelbrotSetIterations is equal to maxIterations, c is in the Mandelbrotset and is colored
 * either by the color passed as parameter to this function or by a color in the palette if the latter
 * is 0. This will be called by fractalgui.cpp. This function takes as parameters:
 * @param gw - The window in which to draw the Mandelbrot set.
 * @param minX - left-most column of grid
 * @param incX - increment value of columns of grid
 * @param minY - top-most row of grid
 * @param incY - increment value of rows of grid
 * @param maxIterations - The maximum number of iterations to run recursive step
 * @param color - The color of the fractal; zero if palette is to be used
 */
void mandelbrotSet(GWindow& gw, double minX, double incX,
                   double minY, double incY, int maxIterations, int color) {
    // Creates palette of colors
    Vector<int> palette = setPalette();
    int width = gw.getCanvasWidth();
    int height = gw.getCanvasHeight();
    GBufferedImage image(width,height,0xffffff);
    gw.add(&image);
    Grid<int> pixels = image.toGrid(); // Convert image to grid
    // Check if the elements in our grid are in the mandelbrotSet
    for (int r = 0; r < pixels.numRows(); r++) {
        for (int c = 0; c < pixels.numCols(); c++) {
            Complex complex = Complex(minX + c * incX, minY + r * incY);
            // Count the number of iterations the Mandelbrot recursive formula can be applied on complex
            int nbIterations = mandelbrotSetIterations(complex, maxIterations);
            /* If the number of iterations is equal to maxIterations z's absolute value never exceeded
             * ABS_MAX: complex is in the Mandelbrot Set */
            if (nbIterations == maxIterations) {
                if (color != 0 ) {
                    pixels[r][c] = color;
                } else {
                    pixels[r][c] = palette[nbIterations%palette.size()];
                }
            } else {
                if (color == 0) {
                    pixels[r][c] = palette[nbIterations%palette.size()];
                }
            }
        }
    }
    image.fromGrid(pixels);
}
 
/** Function mandelbrotSetIterations()
 * Usage: mandelbrotSetIterations(Complex c, int maxIterations)
 * ------------------------------------------------------------
 * Runs the Mandelbrot Set recursive formula on complex number c a maximum of maxIterations times.
 * This function returns the number of iterations the recursive formula was run. Relies on the
 * subsequent definition of mandelbrotSetIterations to calculate the number of iterations the
 * recursive formula was run on c. This function takes as parameters:
 * @param c - Complex number to use for recursive formula.
 * @param maxIterations - The maximum number of iterations to run recursive step
 * @return number of iterations needed to determine if c is unbounded
 */
int mandelbrotSetIterations(Complex c, int maxIterations) {
    int nbIterations = mandelbrotSetIterations(Complex(0,0), c, maxIterations);
    return nbIterations;
}
 
/** Function mandelbrotSetIterations()
 * Usage: mandelbrotSetIterations(Complex z, Complex c, int remainingIterations)
 * -----------------------------------------------------------------------------
 * This function will apply the Mandelbrot set formula to c starting from z as long as the updated z's
 * absolute value is < ABS_MAX, at most remainingIterations times. It will return the number of such
 * iterations that the formula was successfully applied.This function takes as parameters:
 * @param z - Complex number for a given number of iterations
 * @param c - Complex number to use for recursive formula.
 * @param remainingIterations - The remaining number of iterations to run recursive step
 * @return number of iterations needed to determine if c is unbounded
 */
int mandelbrotSetIterations(Complex z, Complex c, int remainingIterations) {
    if (z.abs() > ABS_MAX || remainingIterations == 0) {
        return 0;
    } else {
        Complex znPlusOne = z*z + c;
        remainingIterations--;
        return 1+ mandelbrotSetIterations(znPlusOne, c, remainingIterations);
    }
}
 
// Helper function to set the palette
Vector<int> setPalette() {
    Vector<int> colors;
    string colorSt =  "#04182B,#5A8C8C,#F2D99D,#738585,#AB1111,#04182B,#5A8C8C,#F2D99D";
    Vector<string>colorsStrVec = stringSplit(colorSt,",");
    for (string color : colorsStrVec) {
        colors.add(convertColorToRGB(trim(color)));
    }
    return colors;
}
 