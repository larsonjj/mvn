#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"
#include <SDL3/SDL.h>
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

typedef struct {
    float vx;
    float vy;
} Velocity;

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);

void MoveSystem(ecs_iter_t *iter)
{
    Position *pos = ecs_field(iter, Position, 0);
    Velocity *vel = ecs_field(iter, Velocity, 1);
    for (int i = 0; i < iter->count; i++) {
        // Update position using velocity and delta time.
        pos[i].x += vel[i].vx * iter->delta_time;
        pos[i].y += vel[i].vy * iter->delta_time;
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
    ECS_COMPONENT(world, Velocity);  // Register Velocity component

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

    // Create the bunny entity with its position set to the center and an initial velocity.
    ecs_entity_t bunny_entity = ecs_new(world);
    ecs_set(world, bunny_entity, Position, {10.0f, 10.0f});
    // For example, set a velocity of 10 pixels/second in both x and y.
    ecs_set(world, bunny_entity, Velocity, {10.0f, 10.0f});

    // Register a MoveSystem that moves all Position components.
    ECS_SYSTEM(world, MoveSystem, EcsOnUpdate, Position, Velocity);

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

    // Capture high-resolution time stamp.
    Uint64 now = SDL_GetPerformanceCounter();
    static Uint64 last = 0;
    if (last == 0) {
        last = now;
    }
    // Compute delta time in seconds.
    float deltaTime = (float)(now - last) / (float)SDL_GetPerformanceFrequency();
    last = now;

    // Progress the ECS world with delta time.
    ecs_progress(app->ecs_world, deltaTime);

    // Poll mouse state to spawn new bunnies while left button is held.
    float mouseX;
    float mouseY;
    Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
    // Use a static timer to limit spawn rate.
    static Uint32 lastSpawnTime = 0;
    Uint32 currentTime = SDL_GetTicks(); // milliseconds
    if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) && (currentTime - lastSpawnTime > 200)) {
        // Create a new bunny entity at mouse position.
        ecs_entity_t new_bunny = ecs_new(app->ecs_world);
        ecs_set(app->ecs_world, new_bunny, Position, {(float)mouseX, (float)mouseY});
        // Give it an initial velocity (adjust as needed).
        ecs_set(app->ecs_world, new_bunny, Velocity, {100.0f, 100.0f});
        lastSpawnTime = currentTime;
    }

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
    Velocity *vel = ecs_get_mut(app->ecs_world, app->bunny_entity, Velocity);
    if (!pos) {
        SDL_Log("Could not get bunny Position");
        return SDL_APP_FAILURE;
    }
    if (!vel) {
        SDL_Log("Could not get bunny Velocity");
        return SDL_APP_FAILURE;
    }

    // Clear the screen to black.
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    // Create a query that selects all entities with a Position component.
    ecs_query_t *query = ecs_query(app->ecs_world, {
                                                       .terms = {{ecs_id(Position)}},
                                                   });
    ecs_iter_t iter = ecs_query_iter(app->ecs_world, query);
    while (ecs_query_next(&iter)) {
        Position *pos = ecs_field(&iter, Position, 0);
        for (int i = 0; i < iter.count; i++) {
            SDL_FRect destRect;
            destRect.x = pos[i].x * app->pixel_density;
            destRect.y = pos[i].y * app->pixel_density;
            destRect.w = bunnyW;
            destRect.h = bunnyH;
            SDL_RenderTexture(app->renderer, app->bunny_texture, NULL, &destRect);
        }
    }

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

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    void *appstate = NULL;
    // Initialize the application.
    if (SDL_AppInit(&appstate, argc, argv) != SDL_APP_CONTINUE) {
        SDL_Log("Failed to initialize the application.");
        return EXIT_FAILURE;
    }

    AppContext *app = (AppContext *)appstate;
    SDL_Event event;
    bool running = true;
    ECS_COMPONENT_DEFINE(app->ecs_world, Position);
    ECS_COMPONENT_DEFINE(app->ecs_world, Velocity);

    // Main loop.
    while (running && app->app_quit == SDL_APP_CONTINUE) {
        // Process all pending events.
        while (SDL_PollEvent(&event)) {
            if (SDL_AppEvent(appstate, &event) != SDL_APP_CONTINUE) {
                running = false;
                break;
            }
        }

        // Update ECS and render frame.
        if (SDL_AppIterate(appstate) != SDL_APP_CONTINUE) {
            running = false;
        }
    }

    // Clean up
    SDL_AppQuit(appstate, app->app_quit);
    return EXIT_SUCCESS;
}
