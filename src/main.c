// #define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include "SDL3_mixer/SDL_mixer.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INITIAL_BUNNY_CAPACITY 64

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *bunny_texture;
    SDL_Texture *font_texture;
    Mix_Music *music;
    float pixel_density;
    int app_quit;
    // Dynamic arrays for bunnies and speed values.
    SDL_FRect *bunnies;
    size_t bunny_count;
    size_t bunny_capacity;
    float *bunny_x_speeds;
    float *bunny_y_speeds;
    TTF_Font *font;
} AppContext;

static int SDL_AppFail(void)
{
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

static void BunnyArrayPush(AppContext *app, SDL_FRect rect, float x_speed, float y_speed)
{
    if (app->bunny_count >= app->bunny_capacity) {
        app->bunny_capacity *= 2;

        SDL_FRect *tmpBunnies = realloc(app->bunnies, app->bunny_capacity * sizeof(SDL_FRect));
        if (!tmpBunnies) {
            SDL_Log("Memory reallocation failed for bunnies");
            return;
        }
        app->bunnies = tmpBunnies;

        float *tmpBunnyX = realloc(app->bunny_x_speeds, app->bunny_capacity * sizeof(float));
        if (!tmpBunnyX) {
            SDL_Log("Memory reallocation failed for bunny_x_speeds");
            return;
        }
        app->bunny_x_speeds = tmpBunnyX;

        float *tmpBunnyY = realloc(app->bunny_y_speeds, app->bunny_capacity * sizeof(float));
        if (!tmpBunnyY) {
            SDL_Log("Memory reallocation failed for bunny_y_speeds");
            return;
        }
        app->bunny_y_speeds = tmpBunnyY;
    }
    app->bunnies[app->bunny_count] = rect;
    app->bunny_x_speeds[app->bunny_count] = x_speed;
    app->bunny_y_speeds[app->bunny_count] = y_speed;
    app->bunny_count++;
}

// Return a random float in [min, max].
static float random_float(float min, float max)
{
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return SDL_AppFail();
    }
    SDL_Window *window =
        SDL_CreateWindow("Window", 352, 430, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) {
        return SDL_AppFail();
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        return SDL_AppFail();
    }
    float pixel_density = SDL_GetWindowPixelDensity(window);

#ifndef __ANDROID__
    const char *basePath = SDL_GetBasePath();
    if (!basePath) {
        return SDL_AppFail();
    }
    char assets_path[512];
    SDL_snprintf(assets_path, sizeof(assets_path), "%sassets/", basePath);
    basePath = assets_path;
#else
    const char *basePath = "";
#endif

    // Initialize audio.
    SDL_AudioSpec spec;
    spec.freq = MIX_DEFAULT_FREQUENCY;
    spec.format = MIX_DEFAULT_FORMAT;
    spec.channels = MIX_DEFAULT_CHANNELS;
    if (!Mix_OpenAudio(0, &spec)) {
        SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
        return SDL_AppFail();
    }
    int loops = -1;
    Mix_QuerySpec(&spec.freq, &spec.format, &spec.channels);
    SDL_Log("Opened audio at %d Hz", spec.freq);

    // Load background music.
    char combined_path[512];
    SDL_snprintf(combined_path, sizeof(combined_path), "%s%s", basePath, "background.mp3");
    Mix_Music *music = Mix_LoadMUS(combined_path);
    if (!music) {
        SDL_Log("Couldn't load %s: %s\n", combined_path, SDL_GetError());
        return SDL_AppFail();
    }

    SDL_ShowWindow(window);

    // Load bunny texture.
    SDL_snprintf(combined_path, sizeof(combined_path), "%s%s", basePath, "bunny.png");
    SDL_Surface *surface = IMG_Load(combined_path);
    if (!surface) {
        SDL_Log("Couldn't load %s: %s\n", combined_path, SDL_GetError());
    }
    SDL_Surface *temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);
    SDL_Texture *bunny_texture = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_DestroySurface(temp);
    if (!bunny_texture) {
        SDL_Log("Couldn't create texture: %s\n", SDL_GetError());
    }
    SDL_SetTextureScaleMode(bunny_texture, SDL_SCALEMODE_NEAREST);

    // Initialize TTF.
    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize TTF: %s\n", SDL_GetError());
        return SDL_AppFail();
    }
    float ptsize = 32 * pixel_density;
    SDL_snprintf(combined_path, sizeof(combined_path), "%s%s", basePath, "monogram.ttf");
    TTF_Font *font = TTF_OpenFont(combined_path, ptsize);
    if (!font) {
        SDL_Log("Couldn't load %f pt font from %s: %s\n", ptsize, combined_path, SDL_GetError());
        return SDL_AppFail();
    }
    TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    TTF_SetFontOutline(font, 0);
    TTF_SetFontKerning(font, 1);
    TTF_SetFontHinting(font, TTF_HINTING_MONO);

    // Create initial FPS texture.
    const char *message = "FPS: 60";
    SDL_Color white = {255, 255, 255, 0};
    SDL_Surface *text = TTF_RenderText_Solid(font, message, SDL_strlen(message), white);
    if (!text) {
        SDL_Log("Couldn't render text: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        return SDL_AppFail();
    }
    SDL_Texture *font_texture = SDL_CreateTextureFromSurface(renderer, text);
    SDL_DestroySurface(text);
    SDL_SetTextureScaleMode(font_texture, SDL_SCALEMODE_NEAREST);

    // Initialize bunny dynamic arrays.
    size_t bunny_capacity = INITIAL_BUNNY_CAPACITY;
    SDL_FRect *bunnies = malloc(bunny_capacity * sizeof(SDL_FRect));
    float *bunny_x_speeds = malloc(bunny_capacity * sizeof(float));
    float *bunny_y_speeds = malloc(bunny_capacity * sizeof(float));
    if (!bunnies || !bunny_x_speeds || !bunny_y_speeds) {
        SDL_Log("Couldn't allocate bunny arrays");
        return SDL_AppFail();
    }
    size_t bunny_count = 0;
    srand((unsigned int)time(NULL));
    {
        SDL_FRect r;
        r.x = random_float(0.0f, 352.0f);
        r.y = random_float(0.0f, 430.0f);
        r.w = 26 * pixel_density;
        r.h = 37 * pixel_density;
        BunnyArrayPush(&(AppContext){.bunnies = bunnies,
                                     .bunny_count = bunny_count,
                                     .bunny_capacity = bunny_capacity,
                                     .bunny_x_speeds = bunny_x_speeds,
                                     .bunny_y_speeds = bunny_y_speeds},
                       r, random_float(-50.0f, 50.0f), random_float(-50.0f, 50.0f));
        bunny_count++; // Set initial count.
    }

    AppContext *app = malloc(sizeof(AppContext));
    if (!app) {
        return SDL_AppFail();
    }
    app->window = window;
    app->renderer = renderer;
    app->bunny_texture = bunny_texture;
    app->font_texture = font_texture;
    app->music = music;
    app->pixel_density = pixel_density;
    app->app_quit = 0;
    app->bunnies = bunnies;
    app->bunny_count = bunny_count;
    app->bunny_capacity = bunny_capacity;
    app->bunny_x_speeds = bunny_x_speeds;
    app->bunny_y_speeds = bunny_y_speeds;
    app->font = font;

    Mix_PlayMusic(music, loops);
    *appstate = app;
    return 0;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppContext *app = (AppContext *)appstate;
    if (event->type == SDL_EVENT_QUIT) {
        app->app_quit = 1;
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            float bunnyW = 26 * app->pixel_density;
            float bunnyH = 37 * app->pixel_density;
            float mouseX = (float)event->button.x * app->pixel_density - bunnyW / 2;
            float mouseY = (float)event->button.y * app->pixel_density - bunnyH / 2;
            for (int i = 0; i < 1000; i++) {
                SDL_FRect r = {mouseX, mouseY, bunnyW, bunnyH};
                float xs = random_float(-50.0f, 50.0f);
                float ys = random_float(-50.0f, 50.0f);
                BunnyArrayPush(app, r, xs, ys);
            }
        }
    }
    return 0;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppContext *app = (AppContext *)appstate;
    // Clear screen white.
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);

    int rendererWidth;
    int rendererHeight;
    SDL_GetCurrentRenderOutputSize(app->renderer, &rendererWidth, &rendererHeight);

    static Uint32 prevTime = 0;
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (prevTime == 0) ? 0 : (currentTime - prevTime) / 1000.0f;
    prevTime = currentTime;

    // Update bunny positions.
    for (size_t i = 0; i < app->bunny_count; i++) {
        app->bunnies[i].x += app->bunny_x_speeds[i] * deltaTime;
        app->bunnies[i].y += app->bunny_y_speeds[i] * deltaTime;
        if (app->bunnies[i].x < 0 || app->bunnies[i].x + app->bunnies[i].w > rendererWidth)
            app->bunny_x_speeds[i] *= -1;
        if (app->bunnies[i].y < 0 || app->bunnies[i].y + app->bunnies[i].h > rendererHeight)
            app->bunny_y_speeds[i] *= -1;
    }

    SDL_FRect bunny_srcrect = {0, 0, 26, 37};
    for (size_t i = 0; i < app->bunny_count; i++) {
        SDL_RenderTexture(app->renderer, app->bunny_texture, &bunny_srcrect, &app->bunnies[i]);
    }

    // Update FPS texture every second (similar to your current FPS text update).
    static Uint32 lastTime = 0;
    static Uint32 lastBunnyUpdate = 0;
    static int frameCount = 0;
    frameCount++;
    if (currentTime - lastTime >= 1000) {
        int fps = frameCount;
        frameCount = 0;
        lastTime = currentTime;
        char fpsText[32];
        SDL_snprintf(fpsText, sizeof(fpsText), "FPS: %d", fps);
        SDL_Color fpsColor = {0, 0, 0, 255};
        SDL_Surface *fpsSurface =
            TTF_RenderText_Blended(app->font, fpsText, SDL_strlen(fpsText), fpsColor);
        if (fpsSurface) {
            SDL_Texture *newFontTexture = SDL_CreateTextureFromSurface(app->renderer, fpsSurface);
            SDL_DestroySurface(fpsSurface);
            if (newFontTexture) {
                SDL_DestroyTexture(app->font_texture);
                app->font_texture = newFontTexture;
            }
        }
    }
    // Render FPS at bottom left.
    float txtW;
    float txtH;
    SDL_GetTextureSize(app->font_texture, &txtW, &txtH);
    int margin = 16 * app->pixel_density;
    SDL_FRect fps_dst = {margin, (float)(rendererHeight - txtH - margin), txtW, txtH};
    SDL_RenderTexture(app->renderer, app->font_texture, NULL, &fps_dst);

    // Render bunny count at top left.
    static SDL_Texture *bunnyCountTexture = NULL;
    if (currentTime - lastBunnyUpdate >= 1000) {
        lastBunnyUpdate = currentTime;
        char bunnyText[32];
        SDL_snprintf(bunnyText, sizeof(bunnyText), "Bunnies: %d", (int)app->bunny_count);
        SDL_Color black = {0, 0, 0, 255};
        SDL_Surface *bunnySurface =
            TTF_RenderText_Blended(app->font, bunnyText, SDL_strlen(bunnyText), black);
        if (bunnySurface) {
            SDL_Texture *newTexture = SDL_CreateTextureFromSurface(app->renderer, bunnySurface);
            SDL_DestroySurface(bunnySurface);
            if (newTexture) {
                if (bunnyCountTexture) {
                    SDL_DestroyTexture(bunnyCountTexture);
                }
                bunnyCountTexture = newTexture;
            }
        }
    }
    if (bunnyCountTexture) {
        float txtW2;
        float txtH2;
        SDL_GetTextureSize(bunnyCountTexture, &txtW2, &txtH2);
        SDL_FRect bunny_dst = {margin, margin, txtW2, txtH2};
        SDL_RenderTexture(app->renderer, bunnyCountTexture, NULL, &bunny_dst);
    }

    SDL_RenderPresent(app->renderer);
    return app->app_quit;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void)result;
    AppContext *app = (AppContext *)appstate;
    if (app) {
        Mix_FreeMusic(app->music);
        SDL_DestroyTexture(app->bunny_texture);
        SDL_DestroyTexture(app->font_texture);
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        TTF_CloseFont(app->font);
        free(app->bunnies);
        free(app->bunny_x_speeds);
        free(app->bunny_y_speeds);
        free(app);
    }
    TTF_Quit();
    SDL_Quit();
    SDL_Log("Application quit successfully!");
}
