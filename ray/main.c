#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#ifdef RAYLIB
#if RAYLIB < 4200
#include "polyfill42.h"
#endif
#endif

const char * searchPath[] = {
    NULL,
#ifndef WIN32
    "..\\assets",
#else
    "../assets",
    "/usr/share/omfgdogs",
    "/usr/local/share/omfgdogs",
#endif
    NULL
};

#ifndef DIRECTORY_SEPARATOR
#ifdef WIN32
#define DIRECTORY_SEPARATOR '\\'
#else
#define DIRECTORY_SEPARATOR '/'
#endif
#endif

char * TextDuplicate(const char *text)
{
#ifdef WIN32
    char * result = _strdup(text);
#else
    char * result = strdup(text);
#endif
    return result;
}

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitAudioDevice();

    const char *parts[2];
    char delim[2] = { DIRECTORY_SEPARATOR, 0 };
    searchPath[0] = GetApplicationDirectory();

    int frames = 0;
    Sound song = { 0 };
    Image sprites = { 0 };
    for (int i = 0; searchPath[i]; ++i) {
        const char * filename;
        parts[0] = searchPath[i];
        if (!song.frameCount) {
            parts[1] = "omfgdogs.mp3";
            filename = TextJoin(parts, 2, delim);
            if (FileExists(filename)) {
                song = LoadSound(filename);
            }
        }
        if (frames == 0) {
            parts[1] = "omfgdogs.gif";
            filename = TextJoin(parts, 2, delim);
            if (FileExists(filename)) {
                sprites = LoadImageAnim(filename, &frames);
            }
        }
    }
    if (!song.frameCount) {
        TraceLog(LOG_WARNING, "music not found");
    }
    if (frames == 0) {
        TraceLog(LOG_FATAL, "spritesheet not found");
    }
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - colors palette");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    if (song.frameCount) {
        PlaySound(song);
    }
    Texture2D *textures = malloc(frames * sizeof(Texture2D));
    Image image = sprites;
    unsigned int frameOffset = sprites.width*sprites.height*4;
    int frame = 0;
    for (int i = 0; i != frames; ++i) {
        image.data = ((unsigned char *)sprites.data) + i * frameOffset;
        textures[i] = LoadTextureFromImage(image);
    }
    UnloadImage(sprites);

    // Main game loop
    float time = .0F;
    bool mute = false;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        unsigned int nextFrameDataOffset = sprites.width*sprites.height*4*frame;

        time += GetFrameTime();
        if (time > 0.04) {
            if (++frame == frames) frame = 0;
            time = .0F;
        }
        BeginDrawing();
        // ClearBackground(RAYWHITE);
        for (int y = 0; y < GetScreenHeight(); y += image.height)
        {
            for (int x = 0; x < GetScreenWidth(); x += image.width) {
                DrawTexture(textures[frame], x, y, WHITE);
            }
        }
        DrawFPS(0, 0);
        EndDrawing();

        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }
        if (IsKeyPressed(KEY_F)) ToggleFullscreen();
        if (IsKeyPressed(KEY_M)) {
            if (song.frameCount) {
                mute = !mute;
                if (mute) PauseSound(song);
                else ResumeSound(song);
            }
        }
    }

    for (int i = 0; i != frames; ++i)
    {
        UnloadTexture(textures[i]);
    }
    if (song.frameCount) {
        StopSound(song);
    }
    CloseAudioDevice();
    CloseWindow();                // Close window and OpenGL context

    return 0;
}

