#Arkanoid
Visão geral

Jogo estilo Arkanoid desenvolvido em C++ com a biblioteca raylib. O projeto contém lógica de fases, vidas, itens bônus (positivos e negativos), sistema de ranking salvo em arquivo e sons/músicas.

Autores: Guilherme Krieger Ferreira, Augusto Dell'Antonia Bottamedi.

* Objetivos do projeto:

 - Implementar um clone simples de Arkanoid com três fases.

 - Trabalhar com colisões (bola com paddle, bola com blocos, itens com paddle).

 - Salvar ranking (nome, data, tempo, pontuação) em Ranking.txt.

 - Usar recursos (texturas + sons) carregados em tempo de execução.

 - Permitir seleção de dificuldade (multiplicador de velocidade das bolas).

* Estrutura do repositório (arquivos principais):

 - main.cpp — ponto de entrada, loop principal, mudança de telas (menu, jogo, ranking, vitória/derrota).

 - funcoes.cpp — implementações das funções (movimentação, colisões, carregamento de imagens e sons, etc.).

 - Arkanoid.h — declarações de structs, variaveis constates e definições de funções; também contém variáveis globais.

 - Imagens/ — pasta com as texturas (backgrounds, blocos, paddle, bola, ícones de bônus, etc.).

 - Sons/ — pasta com arquivos .wav (sons e músicas do jogo).

 - Ranking.txt — arquivo salvo/atu­alizado com os top scores.

*Bibliotecas utilizadas

 - raylib (render + áudio + entrada).

 - iostream, usda na entrada e saida de dados.

 - ctime, usada para pegar o tempo.

 - fstream, usada para a manipulação de arquivos.

 - cstdlib, usada para rand e srand.

* Como compilar (exemplo com w64devkit / MinGW)

Este projeto foi testado em Windows com w64devkit. Ajuste caminhos conforme seu ambiente.

# dentro da pasta do projeto
g++ main.cpp funcoes.cpp -o Arkanoid.exe -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

# executar
./Arkanoid.exe

* Controles

 - Menu: clique com o mouse nos botões ou use o mouse para selecionar. Enter confirma em algumas telas (ex.: gravação de nome no ranking).

 - Jogo:

Mover paddle: <- / A para esquerda; -> / D para direita.

Disparar bola (inicial): basta mover (o jogo solta a bola baseada na direção lastPos).

* Regras e mecânicas do jogo (baseadas no código)

Há 3 fases (layoutF1, layoutF2, layoutF3) com 5 linhas x 10 colunas de blocos, sendo que cada bloco de ouro tem tres vidas, o de prata tem duas vidas e o de bronze tem uma vida.

Cada bloco tem vidas (1, 2 ou 3) e dá pontos diferentes:

 - 1 vida → 50 pontos

 - 2 vidas → 100 pontos

 - 3 vidas → 200 pontos

Quando um bloco atinge 0 vidas ele é removido e pode gerar um item bônus (chance de 20%).

Pontuação final incrementa pelos pontos do bloco multiplicados por multiplicadorPontos e por inversorPontuacao (para efeitos que invertem pontuação).

Itens bônus (tipos 0..7):

 - 0 — vida extra

 - 1 — aumenta tamanho do paddle

 - 2 — pontos extras (usa jogoAtual.pontuacao += 500 * multiplicadorPontos * inversorPontuacao)

 - 3 — temporariamente dobra multiplicador de pontos (60s)

 - 4 — gera 4 bolas extras

 - 5 — diminui tamanho do paddle

 - 6 — inverte pontuação (temporariamente)

 - 7 — diminui velocidade do paddle (tempo limitado)

Itens positivos somam +50 pontos quando pegos; itens negativos subtraem 50 (implementação atual adiciona/subtrai instantaneamente quando o item colide com o paddle).

O jogador inicia com 3 vidas. Perde uma vida quando todas as bolas ficam inativas (caem abaixo da tela).

Fim de fase quando todos os blocos estiverem inativos. Se passar a fase 3, o jogador vence.

Ao perder todas as vidas, entra estado de derrota.

O ranking é salvo em Ranking.txt com as colunas Nome;Data;Tempo;Pontuação.

Sons e músicas — mapeamento (implementado)

Arquivos esperados na pasta Sons/ (formato .wav):

musica_menu.wav — música tocada no menu (carregada em musicaMenu).

musica_jogo.wav — música tocada durante a jogatina (musicaJogo).

clicar_botao_menu.wav — som de clique de botão (usado quando o jogador clica em botões de menu) — somClick.

coletar_item_positivo.wav — som ao coletar item positivo — somBonusPositivo.

coletar_item_negativo.wav — som ao coletar item negativo — somBonusNegativo.

colisao_bloco.wav — som quando a bola atinge um bloco (não necessariamente quebra) — somAcertarBloco.

colisao_bola_paredes.wav — som para colisão com parede e paddle — somAcertarParede.

quebra_bloco.wav — som quando um bloco é destruído — somQuebrarBloco.

perder_vida.wav — som quando o jogador perde uma vida — somPerderVida.

passar_mouse_botao.wav — (opcional) som ao passar o mouse sobre botão — somPassarMouse.

vitoria.wav — som de vitória (estado 4) — somVitoria.

game_over.wav — som de derrota (estado 5) — somDerrota.

* Como a detecção de colisões e desenho de itens funciona

 - Bloques têm 4 hitboxes (esquerda, direita, cima, baixo) com dimensões calculadas ao carregar a fase.

 - Colisão da bola usa CheckCollisionCircleRec para cada hitbox. Ao detectar colisão, o código inverte a componente de velocidade apropriada e decrementa vidas do bloco.

 - Itens bônus são desenhados com DrawTexturePro usando um dst de 56x20 pixels. A colisão com o paddle usa CheckCollisionRecs(hitboxItem, paddle) comparando os retângulos processados.
