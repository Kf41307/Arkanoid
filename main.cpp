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

int multiplicadorPontos = 1;
int inversorPontuacao = 1;
float tempoMultiplicador = 0.0f;
float tempoInversor = 0.0f;
float tempoPaddleLento = 0.0f;

float velocidadePaddleAtual = SPEED;

float multiplicadorVelocidade = 1.0f;

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

void colocarBolaNoPaddle(vector<Bola> &bolas, const Vector2 &posPaddle, const Vector2 tamPaddle){
    Bola b;
    b.raio = 10.0f;
    b.pos.x = posPaddle.x + tamPaddle.x / 2.0f;
    b.pos.y = posPaddle.y - b.raio - 1.0f;
    b.vel.x = 0.0f * multiplicadorVelocidade;
    b.vel.y = 0.0f * multiplicadorVelocidade;
    b.ativo = true;
    b.preso = true;
    bolas.push_back(b);
}

void movimentarPaddle(Vector2 &pos, const int lastPos, const float dt, const Vector2 &tamPaddle){
    switch(lastPos){
        case LEFT:
            pos.x -= velocidadePaddleAtual * dt;
            if (pos.x < 30) {
                pos.x = 30;
            }
            break;
        case RIGHT:
            pos.x += velocidadePaddleAtual * dt;
            if (pos.x + tamPaddle.x > 770) {
                pos.x = 770 - tamPaddle.x;
            }
            break;
    }
}

void movimentarBola(Bola &b, const float dt){
    if(!b.ativo) return;
    if(b.preso) return;

    b.pos.x += b.vel.x * dt;
    b.pos.y += b.vel.y * dt;

    if(b.pos.x - b.raio <= 30){
        b.pos.x = 30 + b.raio;
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

void checarColisoes(Bola &b, const Rectangle &paddle, vector<Bloco> &blocos, Ranking &jogoAtual, vector<ItemBonus> &itensBonus){
    if(!b.ativo) return;
    if(b.preso) return;

    if(CheckCollisionCircleRec(b.pos, b.raio, paddle)){ 
        b.pos.y = paddle.y - b.raio - 1.0f;
        b.vel.y *= -1;

        float centroPaddleX = paddle.x + paddle.width / 2.0f;
        float desloc = (b.pos.x - centroPaddleX) / (paddle.width / 2.0f); // -1 ou 1
        b.vel.x += desloc * 100.0f;
    }

    for(int i = 0; i < (int)blocos.size(); i++){
        Bloco &bl = blocos[i];
        if(!bl.ativo) continue;

        bool colisao = false;

        Rectangle hitboxEsq= {bl.esquerda.x, bl.esquerda.y, bl.esquerda.largura, bl.esquerda.altura};
        Rectangle hitboxDir = {bl.direita.x, bl.direita.y, bl.direita.largura, bl.direita.altura};
        Rectangle hitboxCima = {bl.cima.x, bl.cima.y, bl.cima.largura, bl.cima.altura};
        Rectangle hitboxBaixo = {bl.baixo.x, bl.baixo.y, bl.baixo.largura, bl.baixo.altura};

        if(CheckCollisionCircleRec(b.pos, b.raio, hitboxEsq) || CheckCollisionCircleRec(b.pos, b.raio, hitboxDir)){
            b.vel.x *= -1;

            if(CheckCollisionCircleRec(b.pos, b.raio, hitboxEsq))  b.pos.x = bl.x - b.raio;
            if(CheckCollisionCircleRec(b.pos, b.raio, hitboxDir))  b.pos.x = bl.x + bl.largura + b.raio;

            colisao = true;
        }
        if(CheckCollisionCircleRec(b.pos, b.raio, hitboxCima) || CheckCollisionCircleRec(b.pos, b.raio, hitboxBaixo)){
            b.vel.y *= -1;
            
            if(CheckCollisionCircleRec(b.pos, b.raio, hitboxCima))  b.pos.y = bl.y - b.raio;
            if(CheckCollisionCircleRec(b.pos, b.raio, hitboxBaixo)) b.pos.y = bl.y + bl.altura + b.raio;

            colisao = true;
        }

        if(colisao){
            bl.vidas--;

            if(bl.vidas == 0){
                bl.ativo = false;
                jogoAtual.pontuacao += bl.pontos * multiplicadorPontos * inversorPontuacao;

                int chance = rand() % 10;
                
                if(chance == 9 || chance == 8){
                    
                    float cx = bl.x + bl.largura / 2.0f;
                    float cy = bl.y + bl.altura / 2.0f;

                    int tipo = rand() % 8;
                    ItemBonus it;
                    it.pos = { cx, cy };
                    it.velY = 120.0f; 
                    it.tipo = tipo;
                    it.ativo = true;
                    itensBonus.push_back(it);
                }
            }

            return;
        }

    }
}

void carregarFases(vector<Bloco> &blocos, int fase, Texture2D &textura4, Texture2D &textura5, Texture2D &textura6){
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

            b.esquerda.x = 50 + j * largura;
            b.esquerda.y = 121 + i * altura;
            b.esquerda.largura = hitboxSize;
            b.esquerda.altura  = altura - 2;

            b.direita.x = 50 + j * largura + b.largura;
            b.direita.y = 121 + i * altura;
            b.direita.largura = hitboxSize;
            b.direita.altura  = altura - 2;

            b.cima.x = 51 + j * largura;
            b.cima.y = 120 + i * altura;
            b.cima.largura = largura-2;
            b.cima.altura = hitboxSize;

            b.baixo.x = 51 + j * largura;
            b.baixo.y = altura + 120 + i * altura;
            b.baixo.largura = largura-2;
            b.baixo.altura = hitboxSize;

            //b.cor = BLACK; //Cor da hitbox se precisar

            if(b.vidas == 1){
                b.pontos = 50;
                b.nomeTextura = &textura4;
            } 
            else if(b.vidas == 2){
                b.pontos = 100;
                b.nomeTextura = &textura5;
            }
            else{
                b.pontos = 200;
                b.nomeTextura = &textura6;
            }

            blocos.push_back(b);
        }
    }
}

int Menu(Texture2D texture2){
    Rectangle botaoJogar = {250, 250, 300, 80};
    Rectangle botaoDificuldade = {250, 350, 300, 80};
    Rectangle botaoRanking = {250, 450, 300, 80};
    Rectangle botaoSair = {250, 550, 300, 80};

    Vector2 mouse = GetMousePosition();

    Color corJogar = CheckCollisionPointRec(mouse, botaoJogar) ? RED : YELLOW;
    Color corDificuldade = CheckCollisionPointRec(mouse, botaoDificuldade) ? RED : YELLOW;
    Color corRanking = CheckCollisionPointRec(mouse, botaoRanking) ? RED : YELLOW;
    Color corSair = CheckCollisionPointRec(mouse, botaoSair) ? RED : YELLOW;

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(CheckCollisionPointRec(mouse, botaoJogar)) return 1;
        if(CheckCollisionPointRec(mouse, botaoDificuldade)) return 6;
        if (CheckCollisionPointRec(mouse, botaoRanking)) return 2;
        if (CheckCollisionPointRec(mouse, botaoSair)) return 3;                 
    }

    DrawTexture(texture2, SCREENWIDTH/2 - texture2.width/2, SCREENHEIGHT/2 - texture2.height/2, WHITE);

    DrawRectangleRec(botaoJogar, corJogar);
    DrawRectangleRec(botaoDificuldade, corDificuldade);
    DrawRectangleRec(botaoRanking, corRanking);
    DrawRectangleRec(botaoSair, corSair);

    DrawText("JOGAR", botaoJogar.x + 100, botaoJogar.y + 25, 30, WHITE); 
    DrawText("DIFICULDADE", botaoDificuldade.x + 40, botaoDificuldade.y + 25, 30, WHITE);
    DrawText("RANKING", botaoRanking.x + 85, botaoRanking.y + 25, 30, WHITE);
    DrawText("SAIR", botaoSair.x + 120, botaoSair.y + 25, 30, WHITE);

    return 0;
}

int MenuDificuldade(Texture2D texture2){
    Rectangle botaoFacil = {250, 300, 300, 80};
    Rectangle botaoMedio = {250, 400, 300, 80};
    Rectangle botaoDificil = {250, 500, 300, 80};

    Vector2 mouse = GetMousePosition();

    Color corFacil = CheckCollisionPointRec(mouse, botaoFacil) ? RED : YELLOW;
    Color corMedio = CheckCollisionPointRec(mouse, botaoMedio) ? RED : YELLOW;
    Color corDificil = CheckCollisionPointRec(mouse, botaoDificil) ? RED : YELLOW;

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(CheckCollisionPointRec(mouse, botaoFacil)) return 1;
        if(CheckCollisionPointRec(mouse, botaoMedio)) return 2;
        if(CheckCollisionPointRec(mouse, botaoDificil)) return 3;
    }

    DrawTexture(texture2, SCREENWIDTH/2 - texture2.width/2, SCREENHEIGHT/2 - texture2.height/2, WHITE);


    DrawRectangleRec(botaoFacil, corFacil);
    DrawRectangleRec(botaoMedio, corMedio);
    DrawRectangleRec(botaoDificil, corDificil);

    DrawText("FACIL", botaoFacil.x + 110, botaoFacil.y + 25, 30, WHITE); 
    DrawText("MEDIO", botaoMedio.x + 110, botaoMedio.y + 25, 30, WHITE);
    DrawText("DIFICIL", botaoDificil.x + 95, botaoDificil.y + 25, 30, WHITE);

    return 0;
}

void inicializarVetorRanking(vector<Ranking> &ranking){
    ranking.clear();

    ifstream arqRank("../Arkanoid/Ranking.txt");
    if(arqRank.is_open()){
        string linha;

        getline(arqRank, linha);

        while(getline(arqRank, linha)){
            if(linha.empty()) continue;

            int p1 = linha.find(';');
            int p2 = linha.find(';', p1 + 1);
            int p3 = linha.find(';', p2 + 1);

            if (p1 == -1 || p2 == -1 || p3 == -1) continue;

            string snome = linha.substr(0,p1);
            string sData = linha.substr(p1 + 1, p2 - p1 - 1);
            string sTempo = linha.substr(p2 + 1, p3 - p2 - 1);
            string sPontuacao = linha.substr(p3 + 1);

            Ranking rank;
            rank.nome = snome;
            rank.data = sData;
            rank.tempo = sTempo;
            rank.pontuacao = stoi(sPontuacao);

            ranking.push_back(rank);
        }
        arqRank.close();
    }
}

void ordenarVetorRanking(vector<Ranking> &ranking){
    for(int i = 0; i < (int)ranking.size(); i++){
        for(int j = i + 1; j < (int)ranking.size(); j++){
            if(ranking[j].pontuacao > ranking[i].pontuacao){
                swap(ranking[i], ranking[j]);
            }
        }
    }
}

void atualizarArquivos(vector<Ranking> &ranking){
    ofstream rank;
    rank.open("../Arkanoid/Ranking.txt");

    if(!rank.is_open()){
        cout<<"Erro ao tentar abrir o arquivo."<<endl;
        return;
    }else{
        rank<<"Nome;Data;Tempo;Pontuação"<<endl;

        for(int i = 0; i < (int)ranking.size(); i++){
            rank<<ranking[i].nome << separador
                <<ranking[i].data << separador
                <<ranking[i].tempo <<separador
                <<ranking[i].pontuacao <<endl;
        }
        rank.close();
    }
}

void mostrarRanking(vector<Ranking> &ranking, Texture2D texture3){
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(texture3, SCREENWIDTH/2 - texture3.width/2, SCREENHEIGHT/2 - texture3.height/2, WHITE);
    DrawText("RANKING", 285, 40, 45, YELLOW);
    
    DrawText("POS", 80, 130, 22, WHITE);
    DrawText("NOME", 160, 130, 22, WHITE);
    DrawText("DATA", 340, 130, 22, WHITE);
    DrawText("TEMPO", 480, 130, 22, WHITE);
    DrawText("PONTOS", 600, 130, 22, WHITE);

    DrawLine(60, 160, 740, 160, WHITE);

    int y = 180;

    for(int i=0; i < (int)ranking.size() && i < 10; i++){
        char posStr[5];
        sprintf(posStr, "%dº", i+1);

        DrawText(posStr, 80, y, 20, YELLOW);
        DrawText(ranking[i].nome.c_str(),  160,  y, 20, WHITE);
        DrawText(ranking[i].data.c_str(),  340,  y, 20, WHITE);
        DrawText(ranking[i].tempo.c_str(), 480, y, 20, WHITE);

        char pts[10];
        sprintf(pts, "%d", ranking[i].pontuacao);
        DrawText(pts, 600, y, 20, WHITE);

        y += 32;
    }

    DrawText("Pressione ENTER para voltar",230, 720, 25, YELLOW);
    EndDrawing();
}

void salvarRanking(bool &rankingSalvo, Ranking &jogoAtual, vector<Ranking> &ranking, time_t jogoInicio, bool &inserindoNome, string &nomeDigitado){
    if(rankingSalvo) return;

    if(!inserindoNome){
        inserindoNome = true;
        nomeDigitado = "";
    }

    DrawText("Digite seu nome (max 12 caracteres):", 200, 470, 26, YELLOW);
    DrawRectangle(250, 510, 300, 38, WHITE);
    DrawRectangleLines(250, 510, 300, 38, BLACK);

    int key = GetCharPressed();
        
    while(key > 0){
        if((key >= 32 && key <= 125) && nomeDigitado.size() < 12){
            nomeDigitado.push_back((char)key);
        }
        key = GetCharPressed();
    }

    if(IsKeyPressed(KEY_BACKSPACE) && !nomeDigitado.empty()){
        nomeDigitado.pop_back();
    }

    DrawText(nomeDigitado.c_str(), 260, 515, 28, BLACK);
    DrawText("Pressione ENTER para confirmar", 250, 560, 20, YELLOW);

    if(IsKeyPressed(KEY_ENTER) && nomeDigitado.size() > 0){
        jogoAtual.nome = nomeDigitado;

        ranking.push_back(jogoAtual);
        ordenarVetorRanking(ranking);
        atualizarArquivos(ranking);

        rankingSalvo = true;
        inserindoNome = false;
    }
}

void salvarTempo(Ranking &jogoAtual, bool &tempoRegistrado, time_t jogoInicio){
    if(tempoRegistrado) return;

    time_t agora = time(0);
    tm *tempo = localtime(&agora);

    char dataStr[11];
    char horaFim[9];
    strftime(dataStr, sizeof(dataStr), "%d/%m/%Y", tempo);
    strftime(horaFim, sizeof(horaFim), "%H:%M:%S", tempo);

    jogoAtual.data = dataStr;
    jogoAtual.hora_fim = horaFim;

    double duracaoSegs = 0;
    if(jogoInicio != 0) duracaoSegs = difftime(agora, jogoInicio);

    char tempoStr[10];
    sprintf(tempoStr, "%02d:%02d", (int)(duracaoSegs / 60), (int)(duracaoSegs) % 60);

    jogoAtual.tempo = tempoStr;

    tempoRegistrado = true;
}

void aplicarEfeitosBonus(int tipo, Ranking &jogoAtual, Vector2 &tamPaddle, vector<Bola> &bolas, int &vidas){
    if(tipo == 0){
        vidas++;
    }else if(tipo == 1){
        tamPaddle.x += 20;
        if(tamPaddle.x > 300) tamPaddle.x = 300;
    }else if(tipo == 2){
        jogoAtual.pontuacao += 500 * multiplicadorPontos * inversorPontuacao;
    }else if(tipo == 3){
        multiplicadorPontos = 2;
        tempoMultiplicador = 60.0f;
    }else if(tipo == 5){
        tamPaddle.x -= 20;
        if(tamPaddle.x < 40) tamPaddle.x = 40;
    }else if(tipo == 6){
        inversorPontuacao = -1;
        tempoInversor = 60.0f;
    }else if(tipo == 7){
        velocidadePaddleAtual = velocidadePaddleAtual * 0.7f;
        tempoPaddleLento = 20.0f;
        if(velocidadePaddleAtual < 80.0f) velocidadePaddleAtual = 80.0f;
    }
}

void movimentarEDesenharBonus(float dt, vector<ItemBonus> &bonusItens, const Rectangle &paddle, Ranking &jogoAtual, Vector2 &tamPaddle, vector<Bola> &bolas, int &vidas, const Vector2 &posPaddle,
                              Texture2D texturePontos, Texture2D textureMultiplicador, Texture2D textureAumPaddle, Texture2D textureDimPaddle, Texture2D textureInverter, Texture2D textureDimVel, Texture2D textureVida)
{
    for(int i = 0; i < (int)bonusItens.size(); ++i){
        ItemBonus &it = bonusItens[i];
        if(!it.ativo) continue;

        it.pos.y += it.velY * dt;

        Texture2D tex = texturePontos;
        if(it.tipo == 0) tex = textureVida;            // vida++
        else if(it.tipo == 1) tex = textureAumPaddle;  // aumento paddle
        else if(it.tipo == 2) tex = texturePontos;     // pontos extras
        else if(it.tipo == 3) tex = textureMultiplicador; // multiplicador
        else if(it.tipo == 4) tex = texturePontos;     // bolas extras (reusar pontos se não tiver textura específica)
        else if(it.tipo == 5) tex = textureDimPaddle;  // diminuir paddle
        else if(it.tipo == 6) tex = textureInverter;   // inversor
        else if(it.tipo == 7) tex = textureDimVel;     // diminuir velocidade paddle

        Rectangle src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        float size = 24.0f;
        Rectangle dst = { it.pos.x - size/2.0f, it.pos.y - size/2.0f, size, size };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);

        if(it.pos.y > SCREENHEIGHT + 20){
            it.ativo = false;
            continue;
        }

        if(CheckCollisionCircleRec(it.pos, 8.0f, paddle)){
            if(it.tipo == 0 || it.tipo == 1 || it.tipo == 2 || it.tipo == 3 || it.tipo == 4){
                jogoAtual.pontuacao += 50;
            }else if(it.tipo == 5 || it.tipo == 6 || it.tipo == 7){
                jogoAtual.pontuacao -= 50;
            }

            if(it.tipo == 4){
                for(int k = 0 ; k < 4; k++){
                    Bola nb;
                    nb.raio = 10.0f;
                    nb.pos.x = posPaddle.x + tamPaddle.x / 2.0f;
                    nb.pos.y = posPaddle.y - nb.raio - 1.0f;
                    nb.ativo = true;
                    nb.preso = false;
                    if(k < 2) nb.vel.x = -200.0f * multiplicadorVelocidade;
                    else nb.vel.x = 200.0f * multiplicadorVelocidade;

                    nb.vel.y = -200.0f * multiplicadorVelocidade;
                    bolas.push_back(nb);
                }
            }else{
                aplicarEfeitosBonus(it.tipo, jogoAtual, tamPaddle, bolas, vidas);
            }

            it.ativo = false;
        }
    }
}

void desenharInterface(int faseAtual, const Ranking &jogoAtual, int vidas, time_t jogoInicio, Texture2D texture9){
    char textoRound[20];
    sprintf(textoRound, "ROUND %d", faseAtual);

    int larguraTextoRound = MeasureText(textoRound, 30);
    DrawText(textoRound, SCREENWIDTH/2 - larguraTextoRound/2, 35, 30, WHITE);


    char textoPontuacao[30];
    sprintf(textoPontuacao, "Score: %d", jogoAtual.pontuacao);

    DrawText(textoPontuacao, 520, 35, 25, YELLOW);


    DrawText("Vidas:", 520, 65, 20, WHITE);

    for (int i = 0; i < vidas; i++) {
        Rectangle src = { 0.0f, 0.0f, (float)texture9.width, (float)texture9.height };
        Rectangle dst = { 590.0f + i * 22.0f, 65.0f, 16.0f, 16.0f };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(texture9, src, dst, origin, 0.0f, WHITE);
    }

    time_t agora = time(0);
    int tempoSegundos = difftime(agora, jogoInicio);

    int minutos = tempoSegundos / 60;
    int segundos = tempoSegundos % 60;

    char textoTempo[20];
    sprintf(textoTempo, "%02d:%02d", minutos, segundos);

    DrawText(textoTempo, 520, 95, 25, WHITE);
}

int main() {
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Arkanoid");
    SetTargetFPS(60);     
    srand(time(0));

    Vector2 posPaddle = {SCREENWIDTH / 1.2f, SCREENHEIGHT /1.05f};
    Vector2 tamPaddle = {PLATAFORMX, PLATAFORMY};

    vector<Bola> bolas;
    
    Image backgound_jogo = LoadImage("Imagens/background.png");
    Texture2D texture = LoadTextureFromImage(backgound_jogo);
    UnloadImage(backgound_jogo);

    Image background_menu = LoadImage("Imagens/background_menu.png");
    Texture2D texture2 = LoadTextureFromImage(background_menu);
    UnloadImage(background_menu);

    Image background_ranking = LoadImage("Imagens/background_ranking.png");
    Texture2D texture3 = LoadTextureFromImage(background_ranking);
    UnloadImage(background_ranking);

    Image bloco1 = LoadImage("Imagens/bloco1.png");
    Texture2D texture4 = LoadTextureFromImage(bloco1);
    UnloadImage(bloco1);

    Image bloco2 = LoadImage("Imagens/bloco2.png");
    Texture2D texture5 = LoadTextureFromImage(bloco2);
    UnloadImage(bloco2);

    Image bloco3 = LoadImage("Imagens/bloco3.png");
    Texture2D texture6 = LoadTextureFromImage(bloco3);
    UnloadImage(bloco3);

    Image bola = LoadImage("Imagens/bola.png");
    Texture2D texture7 = LoadTextureFromImage(bola);
    UnloadImage(bola);

    Image paddle = LoadImage("Imagens/paddle.png");
    Texture2D texture8 = LoadTextureFromImage(paddle);
    UnloadImage(paddle);

    Image vida = LoadImage("Imagens/vida.png");
    Texture2D texture9 = LoadTextureFromImage(vida);
    UnloadImage(vida);

    Image pontos_extras = LoadImage("Imagens/pontos_extras.png");
    Texture2D texture10 = LoadTextureFromImage(pontos_extras);
    UnloadImage(pontos_extras);

    Image multiplicador_pontos = LoadImage("Imagens/multiplicador_pontos.png");
    Texture2D texture11 = LoadTextureFromImage(multiplicador_pontos);
    UnloadImage(multiplicador_pontos);

    Image aumentarTamPaddle = LoadImage("Imagens/tam_paddle++.png");
    Texture2D texture13 = LoadTextureFromImage(aumentarTamPaddle);
    UnloadImage(aumentarTamPaddle);

    Image diminuirTamPaddle = LoadImage("Imagens/tam_paddle--.png");
    Texture2D texture14 = LoadTextureFromImage(diminuirTamPaddle);
    UnloadImage(diminuirTamPaddle);

    Image inverterPontuacao = LoadImage("Imagens/inverter_pontuacao.png");
    Texture2D texture15 = LoadTextureFromImage(inverterPontuacao);
    UnloadImage(inverterPontuacao);

    Image diminuirVelPaddle = LoadImage("Imagens/vel_paddle--.png");
    Texture2D texture16 = LoadTextureFromImage(diminuirVelPaddle);
    UnloadImage(diminuirVelPaddle);

    Image vidaExtra = LoadImage("Imagens/vida++.png");
    Texture2D texture17 = LoadTextureFromImage(vidaExtra);
    UnloadImage(vidaExtra);

    Image aumentoPaddle1 = LoadImage("Imagens/paddle_120x20.png");
    Texture2D texture18 = LoadTextureFromImage(aumentoPaddle1);
    UnloadImage(aumentoPaddle1);

    Image aumentoPaddle2 = LoadImage("Imagens/paddle_140x20.png");
    Texture2D texture19 = LoadTextureFromImage(aumentoPaddle2);
    UnloadImage(aumentoPaddle2);

    Image aumentoPaddle3 = LoadImage("Imagens/paddle_160x20.png");
    Texture2D texture20 = LoadTextureFromImage(aumentoPaddle3);
    UnloadImage(aumentoPaddle3);

    Image paddleDiminuido1 = LoadImage("Imagens/paddle_80x20.png");
    Texture2D texture21 = LoadTextureFromImage(paddleDiminuido1);
    UnloadImage(paddleDiminuido1);

    Image paddleDiminuido2 = LoadImage("Imagens/paddle_60x20.png");
    Texture2D texture22 = LoadTextureFromImage(paddleDiminuido2);
    UnloadImage(paddleDiminuido2);


    int estadoTela = 0;

    vector<Bloco> blocos;

    int faseAtual = 1;
    bool resetFase = true;
    int vidas = 3;
    int lastPos = RIGHT;

    vector<Ranking> ranking;
    Ranking jogoAtual;

    inicializarVetorRanking(ranking);

    jogoAtual.nome = "PLAYER";
    time_t jogoInicio = 0;
    bool rankingSalvo = false;
    string nomeDigitado = "";
    bool inserindoNome = false;
    bool tempoRegistrado = false;

    vector<ItemBonus> itensBonus;

    velocidadePaddleAtual = SPEED;
    multiplicadorPontos = 1;
    inversorPontuacao = 1;
    tempoMultiplicador = tempoInversor = tempoPaddleLento = 0.0f;

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
            estadoTela = Menu(texture2);     
            EndDrawing();
            continue;
        }

        if(estadoTela == 1){
            if(resetFase){
                if (faseAtual == 1 && vidas == 3) {
                    jogoAtual.pontuacao = 0;

                    bolas.clear();
                    colocarBolaNoPaddle(bolas, posPaddle, tamPaddle);
                    
                    time_t agora = time(0);
                    jogoInicio = agora;

                    tm *tempo = localtime(&agora);
                    char horaInicio[9];
                    strftime(horaInicio, sizeof(horaInicio), "%H:%M:%S", tempo);

                    jogoAtual.hora_inicio = horaInicio;

                    rankingSalvo = false;
                    tempoRegistrado = false;
                    inserindoNome = false;
                    nomeDigitado.clear();

                    jogoAtual.data = "";
                    jogoAtual.hora_fim = "";
                    jogoAtual.tempo = "00:00";
                    jogoAtual.nome = "PLAYER";

                    multiplicadorPontos = 1;
                    inversorPontuacao = 1;
                    tempoMultiplicador = 0;
                    tempoInversor = 0;
                    tempoPaddleLento = 0;
                    velocidadePaddleAtual = SPEED;
                }
                
                carregarFases(blocos, faseAtual, texture4, texture5, texture6);
                resetFase = false;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(texture, SCREENWIDTH/2 - texture.width/2, SCREENHEIGHT/2 - texture.height/2, WHITE);


            movimentarPaddle(posPaddle, lastPos, dt, tamPaddle);

            Texture2D paddleTex = texture8; // padrão

            if(tamPaddle.x >= 160.0f) paddleTex = texture20;
            else if(tamPaddle.x >= 140.0f) paddleTex = texture19;
            else if(tamPaddle.x >= 120.0f) paddleTex = texture18;
            else if(tamPaddle.x <= 60.0f) paddleTex = texture22;
            else if(tamPaddle.x <= 80.0f) paddleTex = texture21;
            else paddleTex = texture8;

            Rectangle srcP = { 0.0f, 0.0f, (float)paddleTex.width, (float)paddleTex.height };
            Rectangle dstP = { posPaddle.x, posPaddle.y, tamPaddle.x, tamPaddle.y };
            Vector2 originP = { 0.0f, 0.0f };
            DrawTexturePro(paddleTex, srcP, dstP, originP, 0.0f, WHITE);
            
            Rectangle paddle = {posPaddle.x, posPaddle.y, tamPaddle.x, tamPaddle.y };

            bool bolaPresa = false;

            for(int i = 0; i < (int)bolas.size(); i++){

                if(bolas[i].preso){
                    bolaPresa = true;

                    bolas[i].pos.x = posPaddle.x + tamPaddle.x / 2.0f;
                    bolas[i].pos.y = posPaddle.y - bolas[i].raio - 1.0f;

                    if (lastPos == LEFT) {
                        bolas[i].preso = false;
                        bolas[i].vel.x = -200.0f * multiplicadorVelocidade;
                        bolas[i].vel.y = -200.0f * multiplicadorVelocidade;
                    }else if (lastPos == RIGHT) {
                        bolas[i].preso = false;
                        bolas[i].vel.x =  200.0f * multiplicadorVelocidade;
                        bolas[i].vel.y = -200.0f * multiplicadorVelocidade;
                    }
                }
            }

            for(int i = 0; i < (int)bolas.size(); i++){
                movimentarBola(bolas[i], dt);
                checarColisoes(bolas[i], paddle, blocos, jogoAtual, itensBonus);
            }

            for(int i = 0; i < (int)bolas.size(); i++){
                if(bolas[i].ativo){
                    Rectangle areaTexturaBola = {0, 0, (float)texture7.width, (float)texture7.height};
                    Rectangle areaTelaBola = {bolas[i].pos.x - bolas[i].raio, bolas[i].pos.y - bolas[i].raio, bolas[i].raio*2, bolas[i].raio*2};
                    Vector2 pontoDeRotacaoBola = {0, 0};
                    DrawTexturePro(texture7, areaTexturaBola, areaTelaBola, pontoDeRotacaoBola, 0.0f, WHITE);

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
                    
                    //DrawRectangleRec(hitboxEsq, b.cor); //Funcoes para desenhar as hitbox caso seja preciso
                    //DrawRectangleRec(hitboxDir, b.cor);
                    //DrawRectangleRec(hitboxCima, b.cor);
                    //DrawRectangleRec(hitboxBaixo, b.cor);

                    Rectangle areaDaTextura = {0, 0, (float)(*b.nomeTextura).width, (float)(*b.nomeTextura).height};
                    Rectangle areaNaTela = {b.x, b.y, b.largura, b.altura};           
                    Vector2 pontoDeRotacao = {0, 0};

                    DrawTexturePro(*b.nomeTextura, areaDaTextura, areaNaTela, pontoDeRotacao, 0.0f, WHITE);
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
                    salvarTempo(jogoAtual, tempoRegistrado, jogoInicio);
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
                if(faseAtual > 3){
                    estadoTela = 4; // Vitoria
                    salvarTempo(jogoAtual, tempoRegistrado, jogoInicio);
                }else resetFase = true;
            }

            if(tempoMultiplicador > 0.0f){
                tempoMultiplicador -= dt;
                if(tempoMultiplicador <= 0.0f){
                    multiplicadorPontos = 1;
                    tempoMultiplicador = 0.0f;
                }
            }

            if(tempoInversor > 0.0f){
                tempoInversor -= dt;
                if(tempoInversor <= 0.0f){
                    inversorPontuacao = 1;
                    tempoInversor = 0.0f;
                }
            }

            if(tempoPaddleLento > 0.0f){
                tempoPaddleLento -= dt;
                if(tempoPaddleLento <= 0.0f){
                    velocidadePaddleAtual = SPEED;
                    tempoPaddleLento = 0.0f;
                }
            }

            movimentarEDesenharBonus(dt, itensBonus, paddle, jogoAtual, tamPaddle, bolas, vidas, posPaddle,
                         texture10, // pontos_extras (usado também como default / bolas extras caso não tenha específica)
                         texture11, // multiplicador_pontos
                         texture13, // aumentarTamPaddle
                         texture14, // diminuirTamPaddle
                         texture15, // inverterPontuacao
                         texture16, // diminuirVelPaddle
                         texture17  // vidaExtra
                        );
            desenharInterface(faseAtual, jogoAtual, vidas, jogoInicio, texture9);

            DrawFPS(40, 30);
            EndDrawing();
        }

        if(estadoTela == 2){
            mostrarRanking(ranking, texture3);

            if(IsKeyPressed(KEY_ENTER)) estadoTela = 0;

            continue;
        }

        if(estadoTela == 3){
            CloseWindow();
        }

        if(estadoTela == 4){            
            BeginDrawing();

            DrawRectangle(75, 275, 650, 340, BLACK);

            DrawText("VOCê VENCEU", 190, 330, 60, GREEN);

            if(IsKeyPressed(KEY_ENTER)){
                faseAtual = 1;
                vidas = 3;
                blocos.clear();
                bolas.clear();
                resetFase = true;

                estadoTela = 0;
            }

            salvarRanking(rankingSalvo, jogoAtual, ranking, jogoInicio, inserindoNome, nomeDigitado);
            EndDrawing();
            continue;
        }

        if(estadoTela == 5){
            BeginDrawing();

            DrawRectangle(75, 275, 650, 340, BLACK);

            DrawText("GAME OVER", 230, 330, 60, RED);

            if(IsKeyPressed(KEY_ENTER)){
                faseAtual = 1;
                vidas = 3;
                blocos.clear();
                bolas.clear();
                resetFase = true;

                estadoTela = 0;
            }

            salvarRanking(rankingSalvo, jogoAtual, ranking, jogoInicio, inserindoNome, nomeDigitado);
            EndDrawing();
            continue;
        }

        if(estadoTela == 6){
            BeginDrawing();
            ClearBackground(BLACK);

            int escolha = MenuDificuldade(texture2);
            
            if(escolha == 1){
                multiplicadorVelocidade = 1.0f;
                estadoTela = 0;
            }
            else if(escolha == 2){
                multiplicadorVelocidade = 1.25f;
                estadoTela = 0;
            }
            else if(escolha == 3){
                multiplicadorVelocidade = 1.50f;
                estadoTela = 0;
            }

            EndDrawing();
            continue;
        }
    }
    
    CloseWindow();
    return 0;
}