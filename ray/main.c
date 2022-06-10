#include "raylib.h"
#include <stdlib.h>

const char * soundFiles[] = {
    "omfgdogs.mp3",
    "/usr/share/omfgdogs/omfgdogs.mp3",
    "/usr/local/share/omfgdogs/omfgdogs.mp3",
    "../assets/omfgdogs.mp3",
    NULL
};
const char * imageFiles[] = {
    "omfgdogs.gif",
    "/usr/share/omfgdogs/omfgdogs.gif",
    "/usr/local/share/omfgdogs/omfgdogs.gif",
    "../assets/omfgdogs.gif",
    NULL
};

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitAudioDevice();
    const char * soundFile = NULL;
    const char * imageFile = NULL;
    for (int i = 0; soundFiles[i]; ++i) {
        if (FileExists(soundFiles[i])) {
            soundFile = soundFiles[i];
            imageFile = imageFiles[i];
            break;
        }
    }
    if (!soundFile || !imageFile) {
        TraceLog(LOG_ERROR, "required assets not found");
        return -1;
    }
    Sound song = LoadSound(soundFile);
    int frames, frame = 0;
    Image sprites = LoadImageAnim(imageFile, &frames);
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - colors palette");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    PlaySound(song);

    Texture2D *textures = malloc(frames * sizeof(Texture2D));
    Image image = sprites;
    unsigned int frameOffset = sprites.width*sprites.height*4;
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
            mute = !mute;
            if (mute) PauseSound(song);
            else ResumeSound(song);
        }
    }

    for (int i = 0; i != frames; ++i)
    {
        UnloadTexture(textures[i]);
    }
    StopSound(song);
    CloseAudioDevice();
    CloseWindow();                // Close window and OpenGL context

    return 0;
}

