#include "Arkanoid.h"

int main() {
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Arkanoid");
    SetTargetFPS(60);     
    srand(time(0));
    InitAudioDevice();
    carregarAudio();
    carregarImagens();
    PlayMusicStream(musicaMenu);

    Vector2 posPaddle = {SCREENWIDTH / 1.2f, SCREENHEIGHT /1.05f};
    Vector2 tamPaddle = {PLATAFORMX, PLATAFORMY};

    vector<Bola> bolas;

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

    int estadoMusica = -1;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        UpdateMusicStream(musicaMenu);
        UpdateMusicStream(musicaJogo);
    
        //Movimentos do paddle

        if((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && posPaddle.x >= 29){
            lastPos = LEFT;
        }else if((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && posPaddle.x <= 671){
            lastPos = RIGHT;
        }else{
            lastPos = 0;
        }

        if(estadoTela != estadoMusica){
            StopMusicStream(musicaMenu);
            StopMusicStream(musicaJogo);

            if(estadoTela == 0) PlayMusicStream(musicaMenu);
            else if(estadoTela == 1) PlayMusicStream(musicaJogo);
            else if(estadoTela == 4) PlaySound(somVitoria);
            else if(estadoTela == 5) PlaySound(somDerrota);

            estadoMusica = estadoTela;
        }


        if (estadoTela == 0){ //Menu principal
            BeginDrawing();
            ClearBackground(BLACK);
            estadoTela = Menu(texture2);     
            EndDrawing();
            continue;
        }

        if(estadoTela == 1){ //Jogabilidade
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

            Texture2D paddleTex = texture8;

            if(tamPaddle.x >= 160.0f) paddleTex = texture20;
            else if(tamPaddle.x >= 140.0f) paddleTex = texture19;
            else if(tamPaddle.x >= 120.0f) paddleTex = texture18;
            else if(tamPaddle.x <= 60.0f) paddleTex = texture22;
            else if(tamPaddle.x <= 80.0f) paddleTex = texture21;
            else paddleTex = texture8;

            Rectangle areaTexturaPaddle = { 0.0f, 0.0f, (float)paddleTex.width, (float)paddleTex.height };
            Rectangle areaTelaPaddle = { posPaddle.x, posPaddle.y, tamPaddle.x, tamPaddle.y };
            Vector2 origemPaddle = { 0.0f, 0.0f };
            DrawTexturePro(paddleTex, areaTexturaPaddle, areaTelaPaddle, origemPaddle, 0.0f, WHITE);
            
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

                    Rectangle areaTexturaBloco = {0, 0, (float)(*b.nomeTextura).width, (float)(*b.nomeTextura).height};
                    Rectangle areaTelaBloco = {b.x, b.y, b.largura, b.altura};           
                    Vector2 origemBloco = {0, 0};
                    DrawTexturePro(*b.nomeTextura, areaTexturaBloco, areaTelaBloco, origemBloco, 0.0f, WHITE);
                }
            }

            int bolasAtivas = 0;
            for(int i = 0; i < (int)bolas.size(); i++){
                if(bolas[i].ativo) bolasAtivas++;
            }

            if(bolasAtivas == 0){
                vidas--;
                PlaySound(somPerderVida);
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
                         texture10, // pontos_extras (usado também como default
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

        if(estadoTela == 2){ //Rankning
            mostrarRanking(ranking, texture3);

            if(IsKeyPressed(KEY_ENTER)) estadoTela = 0;

            continue;
        }

        if(estadoTela == 3){ //Fechar jogo
            CloseWindow();
        }

        if(estadoTela == 4){ //Estado de vitória
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

        if(estadoTela == 5){ //Estado de derrota
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

        if(estadoTela == 6){ //Menu dificuldade
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

    descarregarImagens();
    descarregarAudio();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}