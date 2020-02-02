/*
 * Simple program to draw Bezier curves.
 *
 * Bezier curves are based on interpolations between other fixed points.
 *
 * If you're unfamiliar with interpolation, for now just think of it
 * as 'moving smoothly from one point to another in a straight line'.
 *
 * A 'quadratic' curve is based on 3 fixed points; p0, p1 and p2
 *
 * A 'cubic' curve is based on 4 fixed points; p0, p1, p2 and p3
 *
 * Quadratic and cubic curves are drawn in nearly the same way.
 * The cubic is slightly more complicated because it uses one extra 'layer' of interpolation.
 * It's recommended that you look at the cubic logic only once you're happy with the quadratic.
 *
 * This program draws two curves - quadratic line at the top (green), and a cubic line underneath (red).
 *
 * This video gives an excellent and short visual description:
 * https://www.youtube.com/watch?v=pnYccz1Ha34
 *
 */


#include <iostream>
#include <cmath>

#include "SDL2/SDL.h"


// Quadratic fixed-point parameters.
// 0,0 is the upper-left of the window and 1,1 is the lower-right.
// Change these to modify the quadratic curve.

#define QUAD_P0_X 0.2
#define QUAD_P0_Y 0.2

#define QUAD_P1_X 0.5
#define QUAD_P1_Y 0.9

#define QUAD_P2_X 0.9
#define QUAD_P2_Y 0.1


// Cubic fixed-point parameters.
// 0,0 is the upper-left of the window and 1,1 is the lower-right.
// Change these to modify the cubic curve.

#define CUBIC_P0_X 0.1
#define CUBIC_P0_Y 0.9

#define CUBIC_P1_X 0.3
#define CUBIC_P1_Y 0.2

#define CUBIC_P2_X 0.5
#define CUBIC_P2_Y 1.6

#define CUBIC_P3_X 0.8
#define CUBIC_P3_Y 0.4


using std::cout;
using std::cerr;
using std::endl;


// Window size
const int W = 400;
const int H = 400;


/*
 * We draw the curve on the computer screen by pretending that it's made of lots of short, straight lines.
 *
 * STEPS is the number of lines which we pretend it's made from.
 *
 * Setting STEPS to be low will produce a more jagged line, but will take less time to compute.
 * Setting STEPS to be high will produce a smoother line, but will take more time to compute.
 */
const int STEPS = 20;


/*
 * Data type to represent a position on the screen.
 * To make this easier to think about, in this program, we take x and y to be between 0 and 1.
 * 0,0 is the upper-left of the window and 1,1 is the lower-right.
 */
typedef struct {
    float x;
    float y;
} Point;


/*
 * Clear the window
 */
void clear(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}


/*
 * Linear interpolation between two points.
 * When interp is 0, the result is the first point, p0.
 * When interp is 1, the result is the first point, p1.
 * As interp moves from 0 to 1, the result moves smoothly from p0 to p1.
 * If interp is 0.8, the result is 80% of the way from p0 to p1.
 */
Point lerp(float interp, const Point &p0, const Point &p1) {
    return Point{(1 - interp) * p0.x + interp * p1.x,
                 (1 - interp) * p0.y + interp * p1.y};
}


/*
 * Draw a quadratic Bezier curve based on 3 control points.
 * The curve will be drawn in green.
 */
void draw_bezier_quadratic(SDL_Renderer *renderer, const Point& p0, const Point& p1, const Point& p2) {
    Point p0_p1_interp;
    Point p1_p2_interp;

    // We already know that the curve will start at p0.
    // Later, this will keep track of the end of the line which we drew most recently,
    // and where we should therefore start drawng the next line from.
    Point curve_prev = p0;

    // This will keep track of the next point which we need to draw to.
    Point curve_current;

    // Set the colour for the quadratic curve
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

    // Loop through the number of line segments which we need to draw
    for (int i = 1; i <= STEPS; ++i) {
        // Work out our interpolation value for this line segment.
        // It will be between 0 and 1.
        float interp = (float) i / STEPS;

        // Interpolate between p0 and p1.
        p0_p1_interp = lerp(interp, p0, p1);

        // Interpolate between p1 and p2 by the same amount again.
        p1_p2_interp = lerp(interp, p1, p2);

        // Now interpolate between the two points we just calculated,
        // again by the same amount.
        // This will give us the next point in our curve.
        curve_current = lerp(interp, p0_p1_interp, p1_p2_interp);

        // Draw this line segment, from the end of the previous line
        // to the point we just calculated.
        SDL_RenderDrawLine(renderer,
                           W * curve_prev.x,    H * curve_prev.y,
                           W * curve_current.x, H * curve_current.y);

        // Make a note of where the line we just drew ends, because
        // we need to start drawing from here next time.
        curve_prev = curve_current;
    }
}


/*
 * Draw a cubic Bezier curve based on 4 control points.
 * The curve will be drawn in red.
 */
void draw_bezier_cubic(SDL_Renderer *renderer, const Point& p0, const Point& p1, const Point& p2, const Point& p3) {
    Point p0_p1_interp;
    Point p1_p2_interp;
    Point p2_p3_interp;

    Point p0p1_p1p2_interp;
    Point p1p2_p2p3_interp;

    // We already know that the curve will start at p0.
    // Later, this will keep track of the end of the line which we drew most recently,
    // and where we should therefore start drawng the next line from.
    Point curve_prev = p0;

    // This will keep track of the next point which we need to draw to.
    Point curve_current;

    // Set the colour for the cubic curve
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

    // Loop through the number of line segments which we need to draw
    for (int i = 1; i <= STEPS; ++i) {
        // Work out our interpolation value for this line segment.
        // It will be between 0 and 1.
        float interp = (float) i / STEPS;

        // In the quadratic curve, we interpolated between two pairs of points;
        // p0 and p1, and then p1 and p2.
        // For a cubic curve, we interpolate between three pairs;
        // p0 and p1, p1 and p2, and finally p2 and p3.
        p0_p1_interp = lerp(interp, p0, p1);
        p1_p2_interp = lerp(interp, p1, p2);
        p2_p3_interp = lerp(interp, p2, p3);

        // Next, we interpolate each pair of values which we just computed,
        // again using the same interpolation vaule.
        p0p1_p1p2_interp = lerp(interp, p0_p1_interp, p1_p2_interp);
        p1p2_p2p3_interp = lerp(interp, p1_p2_interp, p2_p3_interp);

        // We perform one final interpolation on the previous two points to get
        // our next position in the curve, with the same interpolation again.
        //
        // Note that all we have done differently from the quadratic curve
        // is to use 3 'layers' of interpolation instead of 2.
        //
        // We needed 4 control points instead of 3 in order to have enough
        // data to perform this many interpolations.
        curve_current = lerp(interp, p0p1_p1p2_interp, p1p2_p2p3_interp);

        // Draw this line segment, from the end of the previous line
        // to the point we just calculated.
        SDL_RenderDrawLine(renderer,
                           W * curve_prev.x,    H * curve_prev.y,
                           W * curve_current.x, H * curve_current.y);

        // Make a note of where the line we just drew ends, because
        // we need to start drawing from here next time.
        curve_prev = curve_current;
    }
}


int main(int argc, char** argv) {
    SDL_Surface* w;
    Uint32* pixels;

    // Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "Error: Could not initialise SDL" << endl;
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Create the window
    if (SDL_CreateWindowAndRenderer(W, H, 0, &window, &renderer) != 0) {
        cerr << "Error: Could not create window" << endl;
        return 2;
    }

    // Clear the screen
    clear(renderer);

    // Draw a quadratic bezier curve based on 3 fixed points
    draw_bezier_quadratic(renderer, Point{QUAD_P0_X, QUAD_P0_Y},
                                    Point{QUAD_P1_X, QUAD_P1_Y},
                                    Point{QUAD_P2_X, QUAD_P2_Y});

    // Draw a cubic bezier curve based on 4 fixed points
    draw_bezier_cubic(renderer, Point{CUBIC_P0_X, CUBIC_P0_Y},
                                Point{CUBIC_P1_X, CUBIC_P1_Y},
                                Point{CUBIC_P2_X, CUBIC_P2_Y},
                                Point{CUBIC_P3_X, CUBIC_P3_Y});

    // Display everything that we have drawn on the screen
    SDL_RenderPresent(renderer);

    /*
     * Nothing interesting from this point on;
     * We just wait until we're told to quit, and then tidy up.
     */

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        quit = SDL_TRUE;
                    }
                    break;
                case SDL_QUIT:
                    quit = SDL_TRUE;
                    break;
            }
        }

        SDL_Delay(5);
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
    return 0;
}
