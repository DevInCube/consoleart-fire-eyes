#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>

#include "console.h"
#include "canvas.h"

const float pi = 3.14159;
const int delay = 20;

int randColor() { return abs(32768 + rand()) % 256; }

// wrapper types & functions
typedef uint32_t Uint32;

unsigned long getTime() { return clock(); }

int std_min(int a, int b) { return fmin(a, b); }

void waitFrame(int millis, float sec) { sleepMillis(delay); }

int done() { return 0; }

struct Vec2D
{
    float x;
    float y;
};

struct Eye
{
    struct Vec2D loc;
    int radius;
    //
    float rotRadius;
    float rotAngle;
};

struct FireBall
{
    struct Vec2D loc;
    float radius;
    //
    float rotRadius;
    float rotAngle;
    float rotSpeed;
    //
    float rotT0;
};

void setCell(Uint32 *arr, int w, int h, int bx, int by, Uint32 val)
{
    if (by > 0 && bx > 0 && by < h - 1 && bx < w - 1)
    {
        *(arr + by * w + bx) = val;
    }
}

// Based on: https://lodev.org/cgtutor/fire.html
int main(int argc, char *argv[])
{
    //define the width and height of the screen and the buffers
    struct ConsoleSize cs = Console_size();
    int screenWidth = cs.columns;
    int screenHeight = cs.rows * 2;
    // Y-coordinate first because we use horizontal scanlines
    Uint32 fire[screenHeight][screenWidth];   //this buffer will contain the fire
    Uint32 buffer[screenHeight][screenWidth]; //this is the buffer to be drawn to the screen
    Uint32 palette[256];                      //this will contain the color palette
    //
    srand(time(0));
    int h = screenHeight;
    int w = screenWidth;
    //set up the screen
    Canvas_setSize(screenWidth, screenHeight);

    //declarations
    float time = getTime(), oldTime; //the time of this and the previous frame, for timing

    //make sure the fire buffer is zero in the beginning
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            fire[y][x] = 0;

    //generate the palette

    for (int x = 0; x < 256; x++)
    {
        int r = x < 100 ? 0 : (x) % 255;
        int g = x < 100 ? 0 : (x / 3) % 255;
        int b = 0;
        palette[x] = 16 + 36 * (r / 51) + 6 * (g / 51) + (b / 51);
    }

    const int nEyes = 3;
    struct Eye eyes[nEyes];
    for (int i = 0; i < nEyes; i++)
    {
        eyes[i].rotAngle = 2 * pi / nEyes * i;
        eyes[i].rotRadius = 20;
        eyes[i].radius = 10;
    }

    float r = 0;
    const int nFireBalls = 20;
    struct FireBall balls[nFireBalls];

    struct Vec2D center = {w / 2, h / 2};
    for (int i = 0; i < nFireBalls; i++)
    {
        balls[i].radius = 3;
        balls[i].rotRadius = sqrt(pow(center.x - balls[i].loc.x, 2) + pow(center.y - balls[i].loc.y, 2));
        balls[i].rotAngle = atan2(balls[i].loc.y - center.y, balls[i].loc.x - center.x);
        balls[i].rotSpeed = (rand() / (float)RAND_MAX) * pi + (-pi / 2.);
        balls[i].rotT0 = (rand() / (float)RAND_MAX) * pi + (-pi / 2.);
    }

    float t = 0;
    //start the loop (one frame per loop)
    while (!done())
    {
        const float dt = delay / 1000.0;
        center.x = w / 2 + r;
        t += dt;
        const float moveSpeed = 5;
        r = sin(moveSpeed * t) * 10;

        for (int i = 0; i < nEyes; i++)
        {
            eyes[i].rotAngle += pi / 2 * dt;
            const float speed = 2;
            eyes[i].loc.x = center.x + eyes[i].rotRadius * cos(speed * eyes[i].rotAngle);
            eyes[i].loc.y = center.y + eyes[i].rotRadius * sin(speed * eyes[i].rotAngle);
        }

        for (int i = 0; i < nFireBalls; i++)
        {
            balls[i].loc.x = center.x + balls[i].rotRadius * cos(balls[i].rotAngle);
            balls[i].loc.y = center.y + balls[i].rotRadius * sin(balls[i].rotAngle);
            balls[i].rotAngle += balls[i].rotSpeed * dt;
            balls[i].rotRadius = sin(balls[i].rotT0 + 2 * t) * 50;
        }
        //
        //timing: set to maximum 50 milliseconds per frame = 20 frames per second
        oldTime = time;
        waitFrame(oldTime, 0.05);
        time = getTime();

        //randomize the bottom row of the fire buffer
        for (int x = 0; x < w; x++)
            fire[h - 1][x] = randColor();
        // eyes fire
        for (int i = 0; i < nEyes; i++)
        {
            struct Eye eye1 = eyes[i];
            for (int y = -eye1.radius; y <= eye1.radius; y++)
            {
                for (int x = -eye1.radius; x <= eye1.radius; x++)
                {
                    if ((x * x) + (y * y) <= (eye1.radius * eye1.radius))
                    {
                        setCell(&fire[0][0], w, h, eye1.loc.x + x, eye1.loc.y + y, randColor());
                    }
                }
            }
        }
        // fireballs fire
        for (int i = 0; i < nFireBalls; i++)
        {
            struct FireBall *b = &balls[i];
            for (int y = -(*b).radius; y <= (*b).radius; y++)
            {
                for (int x = -(*b).radius; x <= (*b).radius; x++)
                {
                    if ((x * x) + (y * y) <= ((*b).radius * (*b).radius) && y > (*b).radius / 2)
                    {
                        setCell(&fire[0][0], w, h, (*b).loc.x + x, (*b).loc.y + y, randColor());
                    }
                }
            }
        }
        //
        //do the fire calculations for every pixel, from top to bottom
        for (int y = 0; y < h - 1; y++)
            for (int x = 0; x < w; x++)
            {
                fire[y][x] =
                    ((fire[(y + 1) % h][(x - 1 + w) % w] + fire[(y + 1) % h][(x) % w] + fire[(y + 1) % h][(x + 1) % w] + fire[(y + 2) % h][(x) % w]) * 32) / 129;
            }

        //set the drawing buffer to the fire buffer, using the palette colors
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                buffer[y][x] = palette[fire[y][x]];
            }
        }

        // draw
        Canvas_beginDraw();
        Canvas_setColorRGB(100, 10, 0);
        Canvas_fillCircle(w / 2 + r, h / 2 - 12, 40);
        // draw fire
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                int col = buffer[i][j];
                Canvas_setColorInt(col);
                Canvas_putPixel(j, i);
            }
        }
        // draw objects
        for (int i = 0; i < nEyes; i++)
        {
            struct Eye eye1 = eyes[i];
            Canvas_setColorRGB(255, 255, 255);
            Canvas_fillCircle(eye1.loc.x, eye1.loc.y, eye1.radius);
            Canvas_setColorRGB(0, 0, 0);
            Canvas_fillCircle(eye1.loc.x + r / 3, eye1.loc.y, 4);
        }
        Canvas_endDraw();
    }
}
