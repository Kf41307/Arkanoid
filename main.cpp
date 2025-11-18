#include "raylib.h"
#include <iostream>

#define LEFT 2
#define RIGHT 3

#define SPEED 250.0f
#define PLATAFORMX 100.0f
#define PLATAFORMY 20.0f

#define SCRENWIDTH 800
#define SCREENHEIGHT 900
using namespace std;

void Movimentacao(Vector2 &pos, const int lastPos, const float dt){
    switch(lastPos){
        case LEFT:
            pos.x -= SPEED * dt;
            if (pos.x < 29) {
                pos.x = 29;
            }
            break;
        case RIGHT:
            pos.x += SPEED * dt;
            if (pos.x > 671) {
                pos.x = 671;
            }
            break;
    }
}

int main() {



    InitWindow(SCRENWIDTH, SCREENHEIGHT, "raylib basic window");
    SetTargetFPS(60);     
    
    Vector2 pos = {SCRENWIDTH / 1.2f, SCREENHEIGHT /1.05f};
    Vector2 posBola {SCRENWIDTH /2.0f, SCREENHEIGHT /2.0f};
    Vector2 size = {PLATAFORMX, PLATAFORMY};
    int lastPos = RIGHT;

    Image image = LoadImage("C:/Users/8463832/Pictures/background.png");
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
  
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
    
        if((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && pos.x >= 29){
            pos.x -= SPEED * dt;
            lastPos = LEFT;
        }if((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && pos.x <= 671){
            pos.x += SPEED * dt;
            lastPos = RIGHT;
        }

        DrawTexture(texture, SCRENWIDTH/2 - texture.width/2, SCREENHEIGHT/2 - texture.height/2, WHITE);

        Movimentacao(pos, lastPos, dt);
        lastPos = 0;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangleV(pos, size, RED);
        DrawFPS(SCRENWIDTH - 90, 10);

        DrawCircleV(posBola, 50, RED);


        EndDrawing();
    }
    CloseWindow();
    return 0;
}