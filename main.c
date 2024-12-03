#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define STEPS 200

typedef SDL_FPoint point;

void bezier(point *interpolated_points, const size_t n, const size_t steps) {
    for (size_t i = 0; i < steps; i++) {
        interpolated_points[n * steps + i].x +=
            (interpolated_points[(n + 1) * steps + i].x - interpolated_points[n * steps + i].x) *
            ((float32_t) i / (float32_t) steps);
        interpolated_points[n * steps + i].y +=
            (interpolated_points[(n + 1) * steps + i].y - interpolated_points[n * steps + i].y) *
            ((float32_t) i / (float32_t) steps);
    }
}

void fillPoints(point *points, const point p, const size_t n, const size_t steps) {
    for (size_t i = 0; i < steps; i++) {
        points[n * steps + i] = p;
    }
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <control points>=3>\n", argv[0]);
        return 1;
    }
    // atoi() is good enough in this use-case, disable warnings:
    const int32_t control_points_count = atoi(argv[1]); // NOLINT(*-err34-c)
    if (control_points_count < 3) {
        printf("Usage: %s <control points>=3>\n", argv[0]);
    }
    srandom(time(nullptr));
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
    printf("Usage: Drag the points using your mouse");

    SDL_Event event;
    point *control_points = malloc(sizeof(point) * control_points_count);
    for (int32_t i = 0; i < control_points_count; i++) {
        control_points[i].x = fabsf((float32_t) (random() % WINDOW_WIDTH));
        control_points[i].y = fabsf((float32_t) (random() % WINDOW_HEIGHT));
    }
    point *points = malloc(sizeof(point) * control_points_count * STEPS);
    if (points == NULL || control_points == NULL) {
        printf("Failed to allocate memory for points.\n");
        free(points);
        free(control_points);
        return 1;
    }
    int32_t mouseX = 0, mouseY = 0;
    int32_t grabbed = -1;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Get the mouse position and state
        const Uint32 mouse_state = SDL_GetMouseState(&mouseX, &mouseY);

        // If close to one of the control points and clicked, make it movable with left click of the mouse
        if (grabbed != -1) {
            control_points[grabbed].x = (float32_t) mouseX;
            control_points[grabbed].y = (float32_t) mouseY;
            if (!(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT))) {
                grabbed = -1;
            }
        } else {
            if (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                for (int32_t i = 0; i < control_points_count; i++) {
                    if (fabsf(control_points[i].x - (float32_t) mouseX) < 30 && fabsf(
                            control_points[i].y - (float32_t) mouseY) < 30) {
                        grabbed = i;
                        break;
                    }
                }
            }
        }

        // Reset the canvas
        SDL_SetRenderDrawColor(renderer, 60, 56, 54, 0);
        SDL_RenderClear(renderer);

        // Fill the points array
        for (int32_t i = 0; i < control_points_count; i++) {
            fillPoints(points, control_points[i], i, STEPS);
        }

        // DRAW the curve
        SDL_SetRenderDrawColor(renderer, 204, 36, 29, 255);
        for (int32_t i = control_points_count - 1; i > 0; i--) {
            for (int j = 0; j < i; j++) {
                bezier(points, j, STEPS);
            }
        }
        SDL_RenderDrawLinesF(renderer, points, STEPS);

        // Draw Control Lines
        SDL_SetRenderDrawColor(renderer, 142, 192, 124, 0);
        for (int32_t i = 0; i < control_points_count - 1; i++) {
            SDL_RenderDrawLineF(renderer, control_points[i].x, control_points[i].y, control_points[i + 1].x,
                                control_points[i + 1].y);
        }

        // TODO: Draw points as circles
        //  Color for Points SDL_SetRenderDrawColor(renderer, 69, 133, 136, 0);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    free(points);
    free(control_points);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
