#ifndef ARKANOID_H
#define AKANOID_H

#include "raylib.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <cstdlib>

#define LEFT 2
#define RIGHT 3

#define SPEED 250.0f
#define PLATAFORMX 100.0f
#define PLATAFORMY 20.0f

#define SCREENWIDTH 800
#define SCREENHEIGHT 800

const float VelocidadeBola = 200.0f;
using namespace std;

extern int multiplicadorPontos;
extern int inversorPontuacao;
extern float tempoMultiplicador;
extern float tempoInversor;
extern float tempoPaddleLento;
extern float velocidadePaddleAtual;
extern float multiplicadorVelocidade;

// Parte das texturas:

extern Texture2D texture;      // background jogo
extern Texture2D texture2;     // background menu
extern Texture2D texture3;     // background ranking
extern Texture2D texture4;     // bloco1
extern Texture2D texture5;     // bloco2
extern Texture2D texture6;     // bloco3
extern Texture2D texture7;     // bola
extern Texture2D texture8;     // paddle normal
extern Texture2D texture9;     // vida

extern Texture2D texture10;    // pontos extras
extern Texture2D texture11;    // multiplicador
extern Texture2D texture13;    // aumentar paddle
extern Texture2D texture14;    // diminuir paddle
extern Texture2D texture15;    // inverter pontuação
extern Texture2D texture16;    // diminuir velocidade
extern Texture2D texture17;    // vida extra

extern Texture2D texture18;    // paddle_120
extern Texture2D texture19;    // paddle_140
extern Texture2D texture20;    // paddle_160

extern Texture2D texture21;    // paddle_80
extern Texture2D texture22;    // paddle_60

//Parte dos sons:

extern Music musicaMenu;
extern Music musicaJogo;

extern Sound somClick;
extern Sound somBonusPositivo;
extern Sound somBonusNegativo;
extern Sound somAcertarBloco;
extern Sound somAcertarParede;
extern Sound somPerderVida;
extern Sound somPassarMouse;
extern Sound somQuebrarBloco;
extern Sound somVitoria;
extern Sound somDerrota;

//Layouts das fases:

const int layoutF1[5][10] =
{
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {2,2,2,2,2,2,2,2,2,2},
    {2,2,2,2,2,2,2,2,2,2},
    {3,3,3,3,3,3,3,3,3,3}
};

const int layoutF2[5][10] =
{
    {0,0,1,0,1,0,1,0,1,0},
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

const char separador = ';';

struct Hitbox{
    float x, y;
    float largura, altura;
    bool ativo;
};

struct Bloco{
    float x, y;
    float largura, altura;
    int vidas;
    //Color cor;
    bool ativo;
    Hitbox direita, esquerda, cima, baixo;
    Texture2D* nomeTextura;
    int pontos;
};

struct Bola{
    Vector2 pos;
    Vector2 vel;
    float raio;
    bool ativo;
    bool preso;
};

struct Ranking{
    string nome; 
    string data;
    string tempo;
    int pontuacao;
    string hora_inicio;
    string hora_fim;
};

struct ItemBonus{
    Vector2 pos;
    float velY;
    int tipo;
    bool ativo;
};

void colocarBolaNoPaddle(vector<Bola> &bolas, const Vector2 &posPaddle, const Vector2 tamPaddle);

void movimentarPaddle(Vector2 &pos, const int lastPos, const float dt, const Vector2 &tamPaddle);

void movimentarBola(Bola &b, const float dt);

void checarColisoes(Bola &b, const Rectangle &paddle, vector<Bloco> &blocos, Ranking &jogoAtual, vector<ItemBonus> &itensBonus);

void carregarFases(vector<Bloco> &blocos, int fase, Texture2D &textura4, Texture2D &textura5, Texture2D &textura6);

int Menu(Texture2D texture2);

int MenuDificuldade(Texture2D texture2);

void inicializarVetorRanking(vector<Ranking> &ranking);

void ordenarVetorRanking(vector<Ranking> &ranking);

void atualizarArquivos(vector<Ranking> &ranking);

void mostrarRanking(vector<Ranking> &ranking, Texture2D texture3);

void salvarRanking(bool &rankingSalvo, Ranking &jogoAtual, vector<Ranking> &ranking, time_t jogoInicio, bool &inserindoNome, string &nomeDigitado);

void salvarTempo(Ranking &jogoAtual, bool &tempoRegistrado, time_t jogoInicio);

void aplicarEfeitosBonus(int tipo, Ranking &jogoAtual, Vector2 &tamPaddle, vector<Bola> &bolas, int &vidas);

void movimentarEDesenharBonus(float dt, vector<ItemBonus> &bonusItens, const Rectangle &paddle, Ranking &jogoAtual, Vector2 &tamPaddle, vector<Bola> &bolas, int &vidas, const Vector2 &posPaddle,
                              Texture2D texturePontos, Texture2D textureMultiplicador, Texture2D textureAumPaddle, Texture2D textureDimPaddle, Texture2D textureInverter, Texture2D textureDimVel, Texture2D textureVida);

void desenharInterface(int faseAtual, const Ranking &jogoAtual, int vidas, time_t jogoInicio, Texture2D texture9);

void carregarImagens();

void descarregarImagens();

void carregarAudio();

void descarregarAudio();

#endif
