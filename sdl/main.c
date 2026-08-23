#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    IMG_Animation *animation;
    SDL_Texture **textures;
    MIX_Audio* audio;
    SDL_FRect mouseposrect;
};

SDL_AppResult SDL_AppIterate(void *appstate)
{
    struct AppContext *app = (struct AppContext *)appstate;
    /* draw image, loop animation every 3 seconds */
    int w, h;
    if (SDL_GetRenderOutputSize(app->renderer, &w, &h)) {
        Uint8 frame = (Uint8) ((((float) (SDL_GetTicks() % 3000)) / 3000.0f) * app->animation->count);
        SDL_Texture *texture = app->textures[frame];
        SDL_FRect dstrect = {
            .x = 0.0f,
            .y = 0.0f,
            .w = texture->w,
            .h = texture->h
        };
        for (dstrect.y = .0f; dstrect.y < h; dstrect.y += dstrect.h) {
            for (dstrect.x = .0f; dstrect.x < w; dstrect.x += dstrect.w) {
                SDL_RenderTexture(app->renderer, texture, NULL, &dstrect);
            }
        }
    }

    /* set the color to white */
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);

    /* draw a square where the mouse cursor currently is. */
    SDL_RenderFillRect(app->renderer, &app->mouseposrect);

    /* put everything we drew to the screen. */
    SDL_RenderPresent(app->renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    struct AppContext *app = (struct AppContext *)appstate;
    switch (event->type) {
        case SDL_EVENT_QUIT:  /* triggers on last window close and other things. End the program. */
            return SDL_APP_SUCCESS;

        case SDL_EVENT_KEY_DOWN:  /* quit if user hits ESC key */
            if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
                return SDL_APP_SUCCESS;
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:  /* keep track of the latest mouse position */
            /* center the square where the mouse is */
            app->mouseposrect.x = event->motion.x - (app->mouseposrect.w / 2);
            app->mouseposrect.y = event->motion.y - (app->mouseposrect.h / 2);
            break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_SetAppMetadata("SDL omfgdogs Example", "1.0", "com.example.sdl-omfgdogs");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Hello SDL", 800, 450, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    struct AppContext *app = SDL_malloc(sizeof(struct AppContext));
    if (!app) {
        SDL_Log("SDL_malloc(AppContext) failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    app->mouseposrect.x = app->mouseposrect.y = -1000;  /* -1000 so it's offscreen at start */
    app->mouseposrect.w = app->mouseposrect.h = 50;
    app->window = window;
    app->renderer = renderer;
    app->textures = NULL;
    app->animation = IMG_LoadAnimation("omfgdogs.gif");
    if (!app->animation) {
        SDL_Log("IMG_LoadAnimation() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    app->textures = SDL_malloc(sizeof(SDL_Texture *) * app->animation->count);
    for (int f=0; f!=app->animation->count;++f) {
        SDL_Surface *surface = app->animation->frames[f];
        app->textures[f] = SDL_CreateTextureFromSurface(app->renderer, surface);
    }

    *appstate = app;
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    struct AppContext *app = (struct AppContext *)appstate;
    for (int f=0;f!=app->animation->count;++f) {
        SDL_DestroyTexture(app->textures[f]);
    }
    SDL_free(app->textures);
    IMG_FreeAnimation(app->animation);
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    SDL_free(app);
    SDL_Quit();
}
