#include <OpenGL/gl.h>
#include <SDL.h>
#include <stdio.h>

int main(int argc, char* args[])
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("SDL initialize: failure. SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    printf("SDL initialize: success.\n");

    SDL_Window* window = SDL_CreateWindow(
        "CSC 461",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        512,
        512,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        printf("SDL create window: failure. SDL_Error: %s\n", SDL_GetError());
    }
    printf("SDL create window: success.\n");

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        printf("SDL create OpenGL context: failure. SDL_Error: %s\n", SDL_GetError());
    }
    printf("SDL create OpenGL context: success.\n");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // adaptive vsync; else regular vsync
    if (SDL_GL_SetSwapInterval(-1) == -1) {
        SDL_GL_SetSwapInterval(1);
    }

    bool quit = false;
    while (!quit) {

        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {

            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
