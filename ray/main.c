#include "raylib.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
	
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - colors palette");
    int frames, frame = 0;
    Image sprites = LoadImageAnim("../assets/omfgdogs.gif", &frames);
    Texture2D texture = LoadTextureFromImage(sprites);
    SetTargetFPS(120);               // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        unsigned int nextFrameDataOffset = sprites.width*sprites.height*4*frame;
        if (++frame == frames) frame = 0;
        UpdateTexture(texture, ((unsigned char *)sprites.data) + nextFrameDataOffset);

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("Hello World", 28, 42, 20, BLACK);
	    DrawRectangleLines(20, 20, screenWidth - 40, screenHeight - 40, BLACK);

        DrawTexture(texture, GetScreenWidth()/2 - texture.width/2, GetScreenHeight()/2 - texture.height/2, WHITE);
        DrawFPS(0, 0);
        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadImage(sprites);
    CloseWindow();                // Close window and OpenGL context

    return 0;
}

