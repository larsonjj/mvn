#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <flecs.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static int SDL_AppFail(void)
{
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *bunny_texture;
    SDL_Texture *text_texture;
    Mix_Music *music;
    float pixel_density;
    int app_quit;
    TTF_Font *font;
    ecs_world_t *ecs_world;    // New: flecs world pointer
    ecs_entity_t bunny_entity; // New: bunny entity handle
} AppContext;

typedef struct {
    float x;
    float y;
} Position;

ECS_COMPONENT_DECLARE(Position);

void MoveSystem(ecs_iter_t *iter)
{
    Position *pos = ecs_field(iter, Position, 0);
    for (int i = 0; i < iter->count; i++) {
        // Example: move the bunny 0.1 unit to the right and 1 unit down per frame.
        pos[i].x += 0.1F;
        pos[i].y += 0.1F;
    }
}

// Called at startup.
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    // Initialize TTF for text rendering.
    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize TTF: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    // Initialize audio.
    SDL_AudioSpec spec;
    spec.freq = MIX_DEFAULT_FREQUENCY;
    spec.format = MIX_DEFAULT_FORMAT;
    spec.channels = MIX_DEFAULT_CHANNELS;
    if (!Mix_OpenAudio(0, &spec)) {
        SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    // Create a window.
    const int window_width = 320;
    const int window_height = 180;
    SDL_Window *window = SDL_CreateWindow("Maven Game Engine", window_width, window_height,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    // Create a renderer.
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    // Get the pixel density of the window.
    float pixel_density = SDL_GetWindowPixelDensity(window);

    const char *basePath = SDL_GetBasePath();
    if (!basePath) {
        return SDL_AppFail();
    }
    char assets_path[512];
    SDL_snprintf(assets_path, sizeof(assets_path), "%sassets/", basePath);
    basePath = assets_path;
    char combined_path[512];

    // Load the bunny image.
    SDL_snprintf(combined_path, sizeof(combined_path), "%s%s", basePath, "bunny.png");
    SDL_Surface *bunnySurface = IMG_Load(combined_path);
    if (!bunnySurface) {
        fprintf(stderr, "IMG_Load Error: %s\n", SDL_GetError());
        return SDL_AppFail();
    }
    SDL_Texture *bunny_texture = SDL_CreateTextureFromSurface(renderer, bunnySurface);
    SDL_DestroySurface(bunnySurface);
    if (!bunny_texture) {
        fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    // Load the font and create the text texture.
    SDL_snprintf(combined_path, sizeof(combined_path), "%s%s", basePath, "monogram.ttf");
    float ptsize = 32 * pixel_density;
    TTF_Font *font = TTF_OpenFont(combined_path, ptsize);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont Error: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    SDL_Color textColor = {255, 255, 255, 255};
    const char *message = "FPS: 60";
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, message, SDL_strlen(message), textColor);
    if (!textSurface) {
        SDL_Log("Couldn't render text: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(textSurface);
    if (!text_texture) {
        fprintf(stderr, "SDL_CreateTextureFromSurface (text) Error: %s\n", SDL_GetError());
        return SDL_AppFail();
    }

    SDL_snprintf(combined_path, sizeof(combined_path), "%s%s", basePath, "background.mp3");
    // Load and play background music in an infinite loop.
    Mix_Music *music = Mix_LoadMUS(combined_path);
    if (!music) {
        fprintf(stderr, "Mix_LoadMUS Error: %s\n", SDL_GetError());
        return SDL_AppFail();
    }
    Mix_PlayMusic(music, -1); // Loop indefinitely

    ecs_world_t *world = ecs_init(); // Create world
    ECS_COMPONENT(world, Position);  // Register Position component

    // Get window dimensions for initial bunny position.
    int winW;
    int winH;
    SDL_GetWindowSize(window, &winW, &winH);

    // Query bunny texture dimensions for centering.
    float bunnyW;
    float bunnyH;
    if (!SDL_GetTextureSize(bunny_texture, &bunnyW, &bunnyH)) {
        SDL_Log("SDL_GetTextureSize Error: %s", SDL_GetError());
        return SDL_AppFail();
    }

    // Create the bunny entity with its position set to the center.
    ecs_entity_t bunny_entity = ecs_new(world);
    ecs_set(world, bunny_entity, Position, {2, 2});

    // Register a MoveSystem that moves all Position components
    ECS_SYSTEM(world, MoveSystem, EcsOnUpdate, Position);

    AppContext *app = malloc(sizeof(AppContext));
    if (!app) {
        return SDL_AppFail();
    }
    app->window = window;
    app->renderer = renderer;
    app->bunny_texture = bunny_texture;
    app->text_texture = text_texture;
    app->music = music;
    app->pixel_density = pixel_density;
    app->app_quit = SDL_APP_CONTINUE;
    app->font = font;
    app->ecs_world = world;           // Store flecs world.
    app->bunny_entity = bunny_entity; // Store bunny entity.

    *appstate = app;

    return app->app_quit;
}

// Called for each event.
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppContext *app = (AppContext *)appstate;
    if (event->type == SDL_EVENT_QUIT) {
        app->app_quit = SDL_APP_SUCCESS;
    }

    return app->app_quit;
}

// Called every frame.
SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppContext *app = (AppContext *)appstate;

    ECS_COMPONENT_DEFINE(app->ecs_world, Position);

    // Capture high-resolution time stamp.
    Uint64 now = SDL_GetPerformanceCounter();
    static Uint64 last = 0;
    if (last == 0) {
        last = now;
    }
    // Compute delta time in seconds.
    float deltaTime = (float)(now - last) / (float)SDL_GetPerformanceFrequency();
    last = now;

    // Run flecs systems (progress the ECS world).
    ecs_progress(app->ecs_world, deltaTime);

    // Get window dimensions.
    int winW;
    int winH;
    SDL_GetWindowSize(app->window, &winW, &winH);

    // Query the bunny texture's width and height.
    float bunnyW;
    float bunnyH;
    if (!SDL_GetTextureSize(app->bunny_texture, &bunnyW, &bunnyH)) {
        SDL_Log("SDL_GetTextureSize Error: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Retrieve the updated bunny position from the ECS.
    // Get a read-only pointer to the Position component
    const Position *pos = ecs_get(app->ecs_world, app->bunny_entity, Position);
    if (!pos) {
        SDL_Log("Could not get bunny Position");
        return SDL_APP_FAILURE;
    }

    // Compute destination rectangle to center the texture.
    SDL_FRect destRect;
    destRect.x = pos->x * app->pixel_density;
    destRect.y = pos->y * app->pixel_density;
    destRect.w = bunnyW;
    destRect.h = bunnyH;

    // Clear the screen to black.
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    // Render the bunny texture at the computed rectangle.
    SDL_RenderTexture(app->renderer, app->bunny_texture, NULL, &destRect);

    // Present the updated frame.
    SDL_RenderPresent(app->renderer);

    return app->app_quit;
}

// Called when the application is exiting.
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void)result;

    AppContext *app = (AppContext *)appstate;
    if (app) {
        ecs_fini(app->ecs_world);
        Mix_FreeMusic(app->music);
        SDL_DestroyTexture(app->bunny_texture);
        SDL_DestroyTexture(app->text_texture);
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        TTF_CloseFont(app->font);
        free(app);
    }
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    SDL_Log("Application quit successfully!");
}
