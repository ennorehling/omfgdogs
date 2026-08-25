#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    MIX_Mixer* mixer;
    MIX_Track* musicTrack;
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

SDL_IOStream *OpenAsset(const char *asset) {
    static int lastMatch = 0;
    const char * paths[] = {
        "/usr/share/omfgdogs/",
        "/usr/local/share/omfgdogs/",
        "assets/",
        "../assets/",
        "../../assets/",
        "", NULL
    };
    char filename[1024];
    for (int i = lastMatch; ; ) {
        if (!paths[i]) i = 0;
        snprintf(filename, sizeof(filename), "%s%s", paths[i], asset);
        SDL_IOStream *io = SDL_IOFromFile(filename, "rw");
        if (io) {
            lastMatch = i;
            return io;
        }
        if (++i==lastMatch) break;
    }
    SDL_Log("OpenAsset() failed: Couldn't find %s", asset);
    return NULL;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_IOStream *stream;
    SDL_SetAppMetadata("SDL omfgdogs Example", "1.0", "com.example.sdl-omfgdogs");

    struct AppContext *app = SDL_calloc(sizeof(struct AppContext), 1);
    if (!app) {
        SDL_Log("SDL_malloc(AppContext) failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    *appstate = app;

    if (!MIX_Init()) {
        SDL_Log("MIX_Init() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;

    }
    if (NULL == (app->mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL)))
    {
        SDL_Log("MIX_CreateMixerDevice() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (NULL == (app->musicTrack = MIX_CreateTrack(app->mixer))) {
        SDL_Log("MIX_CreateTrack() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (NULL == (stream = OpenAsset("omfgdogs.gif"))) {
        return SDL_APP_FAILURE;
    }
    if (NULL == (app->animation = IMG_LoadAnimation_IO(stream, true))) {
        SDL_Log("IMG_LoadAnimation() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (NULL == (stream = OpenAsset("omfgdogs.mp3"))) {
        return SDL_APP_FAILURE;
    }
    if (NULL == (app->audio = MIX_LoadAudio_IO(app->mixer, stream, true, true)))
    {
        SDL_Log("IMG_LoadAnimation() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!MIX_SetTrackAudio(app->musicTrack, app->audio)) {
        SDL_Log("MIX_SetTrackAudio() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(app->musicTrack, props);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_CreateWindowAndRenderer("OMFGDOGS (SDL)", 800, 450, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    app->window = window;
    app->renderer = renderer;

    app->textures = SDL_malloc(sizeof(SDL_Texture *) * app->animation->count);
    for (int f=0; f!=app->animation->count;++f) {
        SDL_Surface *surface = app->animation->frames[f];
        app->textures[f] = SDL_CreateTextureFromSurface(renderer, surface);
    }
    app->mouseposrect.x = app->mouseposrect.y = -1000;  /* -1000 so it's offscreen at start */
    app->mouseposrect.w = app->mouseposrect.h = 50;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    struct AppContext *app = (struct AppContext *)appstate;
    if (app) {
        MIX_DestroyTrack(app->musicTrack);
        MIX_DestroyAudio(app->audio);
        MIX_DestroyMixer(app->mixer);
        if (app->animation) {
            for (int f=0;f!=app->animation->count;++f) {
                SDL_DestroyTexture(app->textures[f]);
            }
            IMG_FreeAnimation(app->animation);
        }
        SDL_free(app->textures);
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        SDL_free(app);
    }
    SDL_Quit();
}
