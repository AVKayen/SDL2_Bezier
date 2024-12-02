#include <stdio.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define STEPS 100

int min(const int a, const int b) {
    return a < b ? a : b;
}

int max(const int a, const int b) {
    return a > b ? a : b;
}

typedef SDL_Point point;

typedef struct line {
    int x1, y1, x2, y2;
} line;

line lineFromPoints(const point a, const point b) {
    const line l = {a.x, a.y, b.x, b.y};
    return l;
}

point getPointOnLine(const line l, const double percent, const double divisor) {
    point p;
    p.x = l.x1 + (int) ((double) (l.x2 - l.x1) * (percent / divisor));
    p.y = l.y1 + (int) ((double) (l.y2 - l.y1) * (percent / divisor));
    return p;
}

void drawLine(SDL_Renderer *renderer, const line l) {
    SDL_RenderDrawLine(renderer, l.x1, l.y1, l.x2, l.y2);
}

void drawPoint(SDL_Renderer *renderer, const point p) {
    SDL_RenderDrawPoint(renderer, p.x, p.y);
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer;
    SDL_Window *window;

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) != 0) {
        printf("SDL_CreateWindowAndRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_RenderSetVSync(renderer, SDL_TRUE);
    printf("Usage: Move the mouse to change the position of the Control Point of the curve."
           "Right and Left clicks change the positions of the End Points\n");
    point p1 = {0, 0};
    point pw = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    point p2 = {WINDOW_WIDTH, WINDOW_HEIGHT};

    SDL_Event event;

    int mouseX = 0, mouseY = 0;

    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Get the mouse position. Right and Left click set the positions of end points.
        // Moving the mouse sets the position of the control point
        const Uint32 mouse_state = SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= 0 && mouseY >= 0 && mouseX < WINDOW_WIDTH && mouseY < WINDOW_HEIGHT) {
            pw.x = mouseX;
            pw.y = mouseY;
        }
        if (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            p1.x = mouseX;
            p1.y = mouseY;
        }
        if (mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            p2.x = mouseX;
            p2.y = mouseY;
        }

        // DRAW reset the canvas
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Calculate the lines between end points and the control point
        const line l1w = lineFromPoints(p1, pw);
        const line lw2 = lineFromPoints(p2, pw);

        // DRAW the curve
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        point points[STEPS];
        for (int i = 0; i < STEPS; i++) {
            const point m1w = getPointOnLine(l1w, i, STEPS);
            const point m2w = getPointOnLine(lw2, STEPS - i, STEPS);
            const line middle = lineFromPoints(m1w, m2w);
            points[i] = getPointOnLine(middle, i, STEPS);
        }
        SDL_RenderDrawLines(renderer, points, STEPS);


        // DRAW control lines
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawLine(renderer, l1w);
        drawLine(renderer, lw2);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
