#include "raylib.h"
#include <iostream>
#include <vector>

#define LEFT 2
#define RIGHT 3

#define SPEED 250.0f
#define PLATAFORMX 100.0f
#define PLATAFORMY 20.0f

#define SCREENWIDTH 800
#define SCREENHEIGHT 800
using namespace std;

const int layoutF1[5][10] =
{
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {2,2,2,2,2,2,2,2,2,2}
};

const int layoutF2[5][10] =
{
    {1,0,1,0,1,0,1,0,1,0},
    {0,2,0,2,0,2,0,2,0,2},
    {3,0,3,0,3,0,3,0,3,0},
    {0,2,0,2,0,2,0,2,0,2},
    {1,0,1,0,1,0,1,0,1,0}
};

const int layoutF3[5][10] =
{
    {3,2,1,3,2,1,3,2,1,3},
    {2,1,3,2,1,3,2,1,3,2},
    {1,3,2,1,3,2,1,3,2,1},
    {3,2,1,3,2,1,3,2,1,3},
    {2,1,3,2,1,3,2,1,3,2}
};

struct Hitbox{
    float x, y;
    float largura, altura;
    bool ativo;
};

struct Bloco{
    float x, y;
    float largura, altura;
    int vidas;
    Color cor;
    bool ativo;
    Hitbox direita, esquerda, cima, baixo;
};

struct Bola{
    Vector2 pos;
    Vector2 vel;
    float raio;
    bool ativo;
};

void colocarBolaNoPaddle(vector<Bola> &bolas, const Vector2 &posPaddle, const Vector2 tamPaddle){
    Bola b;
    b.raio = 10.0f;
    b.pos.x = posPaddle.x + tamPaddle.x / 2.0f;
    b.pos.y = posPaddle.y - b.raio - 1.0f;
    b.vel.x = 200.0f;
    b.vel.y = -200.0f;
    b.ativo = true;
    bolas.push_back(b);
}

void movimentarPaddle(Vector2 &pos, const int lastPos, const float dt){
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

void movimentarBola(Bola &b, const float dt){
    if(!b.ativo) return;

    b.pos.x += b.vel.x * dt;
    b.pos.y += b.vel.y * dt;

    if(b.pos.x - b.raio <= 30){
        b.pos.x = 47 + b.raio;
        b.vel.x *= -1;
    }

    if(b.pos.x + b.raio >= 770){
        b.pos.x = 770 - b.raio;
        b.vel.x *= -1;
    }

    if(b.pos.y - b.raio <= 26){
        b.pos.y = 26 + b.raio;
        b.vel.y *= -1;
    }

    if(b.pos.y - b.raio > SCREENHEIGHT){
        b.ativo = false;
    }
}

void checarColisoes(Bola &b, const Rectangle &paddle, vector<Bloco> &blocos){
    if(!b.ativo) return;

    if(CheckCollisionCircleRec(b.pos, b.raio, paddle)){ 
        b.pos.y = paddle.y - b.raio - 1.0f;
        b.vel.y *= -1;
    }

    for(int i = 0; i < (int)blocos.size(); i++){
        Bloco &bl = blocos[i];
        if(!bl.ativo) continue;

        Rectangle r = {bl.x, bl.y, bl.largura, bl.altura};
        Rectangle hitboxEsq= {bl.esquerda.x, bl.esquerda.y, bl.esquerda.largura, bl.esquerda.altura};
        Rectangle hitboxDir = {bl.direita.x, bl.direita.y, bl.direita.largura, bl.direita.altura};
        Rectangle hitboxCima = {bl.cima.x, bl.cima.y, bl.cima.largura, bl.cima.altura};
        Rectangle hitboxBaixo = {bl.baixo.x, bl.baixo.y, bl.baixo.largura, bl.baixo.altura};

        if(CheckCollisionCircleRec(b.pos, b.raio, hitboxEsq) || CheckCollisionCircleRec(b.pos, b.raio, hitboxDir)){
            b.vel.x *= -1;
            bl.vidas--;

            if(bl.vidas == 2) bl.cor = GREEN;
            else if(bl.vidas == 1) bl.cor = YELLOW;
            if(bl.vidas == 0) bl.ativo = false;
        }
        if(CheckCollisionCircleRec(b.pos, b.raio, hitboxCima) || CheckCollisionCircleRec(b.pos, b.raio, hitboxBaixo)){
            b.vel.y *= -1;
            bl.vidas--;

            if(bl.vidas == 2) bl.cor = GREEN;
            else if(bl.vidas == 1) bl.cor = YELLOW;
            if(bl.vidas == 0) bl.ativo = false;
        }

        //if(CheckCollisionCircleRec(b.pos, b.raio, r)){
//
 //           b.vel.y *= -1;
  //          bl.vidas--;
//

//
   //         if(bl.vidas == 2) bl.cor = GREEN;
    //        else if(bl.vidas == 1) bl.cor = YELLOW;

        //   if(bl.vidas == 0) bl.ativo = false;
        //}

    }
}

void carregarFases(vector<Bloco> &blocos, int fase){
    blocos.clear();

    int linhas = 5;
    int colunas = 10;

    float largura = 70;
    float altura = 30;
    float hitboxSize = 1;

    for(int i = 0; i < linhas; i++){
        for(int j = 0; j < colunas; j++){

            int valor;

            if(fase == 1) valor = layoutF1[i][j];
            else if(fase == 2) valor = layoutF2[i][j];
            else valor = layoutF3[i][j];


            if(valor == 0) continue;

            Bloco b;
            b.x = 50 + j * largura;
            b.y = 120 + i * altura;
            b.largura = largura - 4;
            b.altura = altura - 4;
            b.ativo = true;
            b.vidas = valor;

            b.direita.x = 50 + j * largura;
            b.direita.y = 121 + i * altura;
            b.direita.largura = hitboxSize;
            b.direita.altura = altura-2;

            b.esquerda.x = largura + 50 + j * largura;
            b.esquerda.y = 121 + i * altura;
            b.esquerda.largura = hitboxSize;
            b.esquerda.altura = altura-2;

            b.cima.x = 51 + j * largura;
            b.cima.y = 120 + i * altura;
            b.cima.largura = largura-2;
            b.cima.altura = hitboxSize;

            b.baixo.x = 51 + j * largura;
            b.baixo.y = altura + 120 + i * altura;
            b.baixo.largura = largura-2;
            b.baixo.altura = hitboxSize;

            if(b.vidas == 1) b.cor = YELLOW;
            else if(b.vidas == 2) b.cor = GREEN;
            else b.cor = RED;

            blocos.push_back(b);
        }
    }
}

int Menu(){
    Rectangle botaoJogar   = {300, 300, 200, 60};
    Rectangle botaoRanking = {300, 400, 200, 60};
    Rectangle botaoSair    = {300, 500, 200, 60};

    Vector2 mouse = GetMousePosition();

    Color corJogar   = CheckCollisionPointRec(mouse, botaoJogar)   ? RED : YELLOW;
    Color corRanking = CheckCollisionPointRec(mouse, botaoRanking) ? RED : YELLOW;
    Color corSair    = CheckCollisionPointRec(mouse, botaoSair)    ? RED : YELLOW;

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(CheckCollisionPointRec(mouse, botaoJogar)) return 1;
        if (CheckCollisionPointRec(mouse, botaoRanking)) return 2;
        if (CheckCollisionPointRec(mouse, botaoSair)) return 3;                 
    }

    DrawRectangleRec(botaoJogar, corJogar);
    DrawRectangleRec(botaoRanking, corRanking);
    DrawRectangleRec(botaoSair, corSair);

    DrawText("JOGAR",   botaoJogar.x + 40,   botaoJogar.y + 15,   30, WHITE); 
    DrawText("RANKING", botaoRanking.x + 30, botaoRanking.y + 15, 30, WHITE);
    DrawText("SAIR",    botaoSair.x + 60,    botaoSair.y + 15,    30, WHITE);

    return 0;
}

int main() {
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Arkanoid");
    SetTargetFPS(60);     
    
    Vector2 posPaddle = {SCREENWIDTH / 1.2f, SCREENHEIGHT /1.05f};
    Vector2 tamPaddle = {PLATAFORMX, PLATAFORMY};

    vector<Bola> bolas;
    colocarBolaNoPaddle(bolas, posPaddle, tamPaddle);
    
    Image image = LoadImage("Imagens/background.png");
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    int estadoTela = 0;

    vector<Bloco> blocos;

    int faseAtual = 1;
    bool resetFase = true;
    int vidas = 3;
    int lastPos = RIGHT;
  
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
    
        if((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && posPaddle.x >= 29){
            lastPos = LEFT;
        }else if((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && posPaddle.x <= 671){
            lastPos = RIGHT;
        }else{
            lastPos = 0;
        }


        if (estadoTela == 0){
            BeginDrawing();
            ClearBackground(BLACK);
            estadoTela = Menu();     
            EndDrawing();
            continue;
        }

        if(estadoTela == 1){

            if(resetFase){
                carregarFases(blocos, faseAtual);
                resetFase = false;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(texture, SCREENWIDTH/2 - texture.width/2, SCREENHEIGHT/2 - texture.height/2, WHITE);

            movimentarPaddle(posPaddle, lastPos, dt);
            lastPos = 0;

            Rectangle paddle = {posPaddle.x, posPaddle.y, tamPaddle.x, tamPaddle.y };

            for(int i = 0; i < (int)bolas.size(); i++){
                movimentarBola(bolas[i], dt);
                checarColisoes(bolas[i], paddle, blocos);
            }

            DrawRectangleV(posPaddle, tamPaddle, RED);

            for(int i = 0; i < (int)bolas.size(); i++){
                if(bolas[i].ativo){
                    DrawCircleV(bolas[i].pos, bolas[i].raio, RED);
                }
            }

            for(int i = 0; i < (int)blocos.size(); i++){
                Bloco b = blocos[i];

                if(b.ativo){
                    Rectangle r = {b.x, b.y, b.largura, b.altura};
                    Rectangle hitboxEsq= {b.esquerda.x, b.esquerda.y, b.esquerda.largura, b.esquerda.altura};
                    Rectangle hitboxDir = {b.direita.x, b.direita.y, b.direita.largura, b.direita.altura};
                    Rectangle hitboxCima = {b.cima.x, b.cima.y, b.cima.largura, b.cima.altura};
                    Rectangle hitboxBaixo = {b.baixo.x, b.baixo.y, b.baixo.largura, b.baixo.altura};
                    
                    DrawRectangleRec(hitboxEsq, b.cor);
                    DrawRectangleRec(hitboxDir, b.cor);
                    DrawRectangleRec(hitboxCima, b.cor);
                    DrawRectangleRec(hitboxBaixo, b.cor);
                    DrawRectangleRec(r, b.cor);

                }
            }

            int bolasAtivas = 0;
            for(int i = 0; i < (int)bolas.size(); i++){
                if(bolas[i].ativo) bolasAtivas++;
            }

            if(bolasAtivas == 0){
                vidas--;
                if(vidas <= 0){
                    estadoTela = 5; //Derrota
                }else{
                    bolas.clear();
                    colocarBolaNoPaddle(bolas, posPaddle, tamPaddle);
                }
            }

            bool terminouFase = true;

            for(int i = 0; i < (int)blocos.size(); i++){
                if(blocos[i].ativo){
                    terminouFase = false;
                    break;
                }
            }

            if(terminouFase){
                faseAtual++;
                if(faseAtual > 3) estadoTela = 4; // Vitoria
                else resetFase = true;
            }
            DrawFPS(40, 30);
            EndDrawing();
        }

        if(estadoTela == 2){
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("RANKING", 300, 200, 50, YELLOW);
            DrawText("Pressione ENTER para voltar ao menu", 150, 400, 30, WHITE);

            if(IsKeyPressed(KEY_ENTER)) estadoTela = 0;

            EndDrawing();
            continue;
        }

        if(estadoTela == 3){
            CloseWindow();
        }

        if(estadoTela == 4){
            BeginDrawing();

            DrawText("VOCê VENCEU", 230, 300, 50, GREEN);
            DrawText("Pressione ENTER para voltar ao menu", 150, 400, 30, WHITE);

            if(IsKeyPressed(KEY_ENTER)){
                faseAtual = 1;
                vidas = 3;
                blocos.clear();
                bolas.clear();
                colocarBolaNoPaddle(bolas, posPaddle, tamPaddle);
                resetFase = true;

                estadoTela = 0;
            }
            
            EndDrawing();
            continue;
        }

        if(estadoTela == 5){
            BeginDrawing();

            DrawRectangle(70, 350, 650, 250, BLACK);
            DrawText("GAME OVER", 250, 420, 60, RED);
            DrawText("Pressione ENTER para voltar ao menu", 130, 500, 30, WHITE);

            if(IsKeyPressed(KEY_ENTER)){
                faseAtual = 1;
                vidas = 3;
                blocos.clear();
                bolas.clear();
                colocarBolaNoPaddle(bolas, posPaddle, tamPaddle);
                resetFase = true;

                estadoTela = 0;
            }

            EndDrawing();
            continue;
        }
    }
    
    CloseWindow();
    return 0;
}