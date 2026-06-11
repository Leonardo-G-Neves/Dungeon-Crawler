#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <windows.h>

/* ============================================================================
   ESTRUTURAS E DADOS
   ============================================================================ */
#define MAX_DIM 25

typedef struct {
    int x, y;
    char simbolo;       
    int vidas;          
    int chaves;         
    int arma_equipada;  
    int arma_dano;      
    char arma_nome[30]; 
    int arma_upada;     
} Padawan;

typedef struct {
    int x, y;
    char tipo;          
    int hp;             
    int ativo;          
} Monstro;

char mapa[MAX_DIM][MAX_DIM];
char mapa_backup[MAX_DIM][MAX_DIM]; 
char overlay_ataque[MAX_DIM][MAX_DIM]; /* NOVO: Matriz temporaria para mostrar o ataque */
int dim_atual = 10;                 
int andar_atual = 0;                
int turnos_boss = 0;

Padawan player;
Monstro inimigos[10]; /* Array suporta confortavelmente o limite de 7 inimigos simultaneos */
int qtd_inimigos = 0;

void exibir_historia(void);
void pausa_dramatica(const char* mensagem, int milissegundos);
void aplicar_tema_visual(void);
void inicializar_fase(void);
void desenhar_mapa(void);
void processar_turno(char tecla);
void mover_inimigos(void);
void processar_ataque(void);
void aplicar_dano(int x, int y);
int movimento_valido(int prox_x, int prox_y);
int inimigos_vivos(void);

/* ============================================================================
   FUNCAO MAIN
   ============================================================================ */
int main(void) {
    char opcao;
    srand((unsigned int)time(NULL));
    system("color 0F");

    while (1) {
        system("cls");
        printf("=========================================\n");
        printf("        DUNGEON CRAWLER                   \n");
        printf("=========================================\n\n");
        printf("1. Iniciar a Jornada (Jogar)\n");
        printf("2. Ler as Escrituras (Tutorial)\n");
        printf("3. Abandonar (Sair)\n\n");
        printf("Qual o seu destino? ");
        opcao = getch();

        if (opcao == '1') {
            break; 
        } else if (opcao == '2') {
            system("cls");
            printf("=== AS ESCRITURAS ===\n");
            printf("Comandos: W, A, S, D (Andar) | I (Interagir) | O (Atacar)\n\n");
            printf("Os perigos do mundo:\n");
            printf(" * Parede impenetravel\n");
            printf(" k Caixa fragil (Destrua atacando)\n");
            printf(" # Espinhos letais\n");
            printf(" @ Chave da esperanca\n");
            printf(" D Porta trancada\n");
            printf(" O Botao de Ascensao (Revela o verdadeiro poder de sua arma)\n");
            printf(" L Escadaria para o proximo andar\n\n");
            printf("Pressione qualquer tecla para retornar...");
            getch();
        } else if (opcao == '3') {
            system("cls");
            printf("Desenvolvido pelo trio: Leonardo Augusto & Adan Melo & Arthur Moraes.\n");
            exit(0);
        }
    }

    exibir_historia();

    player.vidas = 3;
    player.chaves = 0;
    player.arma_equipada = 0; 
    player.arma_dano = 1;
    player.arma_upada = 0;
    strcpy(player.arma_nome, "Desarmado");
    andar_atual = 0; 
    
    inicializar_fase();

    while (player.vidas > 0 && andar_atual <= 3) {
        char tecla;
        desenhar_mapa();
        
        tecla = getch();
        if (tecla >= 'A' && tecla <= 'Z') {
            tecla = tecla + 32; 
        }

        if (tecla == 'w' || tecla == 'a' || tecla == 's' || tecla == 'd' || tecla == 'i' || tecla == 'o') {
            processar_turno(tecla);
        }
        
        if (andar_atual == 3 && qtd_inimigos > 0 && inimigos[0].tipo == 'Z' && inimigos[0].ativo == 0) {
            pausa_dramatica("Krauser da seu ultimo suspiro. A profecia se cumpriu...", 3000);
            break;
        }
    }

    system("color 0F");
    system("cls");
    if (player.vidas <= 0) {
        printf("=========================================\n");
        printf("               GAME OVER                 \n");
        printf("=========================================\n\n");
        printf("O caos consumiu o escolhido. Krauser reinara para sempre...\n");
    } else {
        printf("=========================================\n");
        printf("           O EQUILIBRIO RESTAURADO!      \n");
        printf("=========================================\n\n");
        printf("Com a forca do seu %s, voce dissipou as trevas.\n", player.arma_nome);
        printf("Entre o ceu e a terra, voce provou ser o mais honrado.\n");
        printf("A vila respira livre mais uma vez.\n");
    }
    printf("\nPressione qualquer tecla para encerrar sua lenda...");
    getch();
    return 0;
}

/* ============================================================================
   FEEDBACKS E CORES
   ============================================================================ */
void exibir_historia(void) {
    system("cls");
    printf("\n\nO sombrio Krauser aterroriza toda a vila, espalhando uma nevoa de morte...\n");
    Sleep(2500);
    printf("Muitos tentaram invadir seu dominio, mas todos pereceram.\n");
    Sleep(2500);
    printf("\nMas as profecias sempre falaram de um Escolhido.\n");
    printf("Alguem destinado a trazer o equilibrio a este mundo caotico.\n");
    Sleep(3000);
    printf("\nApenas aquele que transcender os limites de seu poder podera empunhar as armas lendarias.\n");
    printf("Lute. Sobreviva. E prove ser o guerreiro mais honrado de nossa era.\n");
    Sleep(3500);
    while(kbhit()) getch(); 
}

void pausa_dramatica(const char* mensagem, int milissegundos) {
    system("cls");
    printf("\n\n\n\t====================================\n");
    printf("\t  %s\n", mensagem);
    printf("\t====================================\n");
    Sleep(milissegundos);
    while(kbhit()) getch(); 
}

void aplicar_tema_visual(void) {
    if (andar_atual == 0) system("color 0F");      
    else if (andar_atual == 1) system("color 08"); 
    else if (andar_atual == 2) system("color 0D"); 
    else if (andar_atual == 3) system("color 0C"); 
}

/* ============================================================================
   LEVEL DESIGN (SISTEMA DE LOCKS OBRIGATORIOS)
   ============================================================================ */
void inicializar_fase(void) {
    int i, j;

    for (i = 0; i < MAX_DIM; i++) {
        for (j = 0; j < MAX_DIM; j++) {
            mapa[i][j] = ' ';
            overlay_ataque[i][j] = 0; /* Zera a tela de ataque no inicio da fase */
        }
    }
    qtd_inimigos = 0;
    
    /* Resetar array de inimigos */
    for(i=0; i<10; i++) inimigos[i].ativo = 0;

    aplicar_tema_visual();

    if (andar_atual == 0) {
        dim_atual = 10;
        player.x = 8; player.y = 5; player.simbolo = '^';
        for(i=0; i<10; i++) { mapa[0][i] = '*'; mapa[9][i] = '*'; mapa[i][0] = '*'; mapa[i][9] = '*'; }
        
        mapa[3][4] = '*'; mapa[3][6] = '*';
        mapa[4][4] = '*'; mapa[4][6] = '*';
        mapa[5][4] = '*'; mapa[5][6] = '*';
        
        mapa[2][5] = 'N'; 
    }
    
    else if (andar_atual == 1) { 
        dim_atual = 10;
        player.x = 8; player.y = 8;
        for(i=0; i<10; i++) { mapa[0][i] = '*'; mapa[9][i] = '*'; mapa[i][0] = '*'; mapa[i][9] = '*'; }
        
        for(i=3; i<9; i++) mapa[i][6] = '*'; 
        mapa[3][5] = '*'; mapa[3][4] = '*'; mapa[3][3] = '*';
        
        mapa[8][4] = 'k'; 
        mapa[2][8] = '@'; 
        mapa[5][8] = 'D'; 

        inimigos[0].x = 2; inimigos[0].y = 2; inimigos[0].tipo = 'X'; inimigos[0].hp = 3; inimigos[0].ativo = 1;
        qtd_inimigos = 1;
    }
    
    else if (andar_atual == 2) { 
        dim_atual = 15;
        player.x = 13; player.y = 7;
        for(i=0; i<15; i++) { mapa[0][i] = '*'; mapa[14][i] = '*'; mapa[i][0] = '*'; mapa[i][14] = '*'; }
        
        for(i=1; i<14; i++) mapa[11][i] = '*'; mapa[11][7] = 'D';
        for(i=1; i<14; i++) mapa[5][i] = '*';  mapa[5][7] = 'D';
        
        mapa[12][2] = '#'; mapa[12][12] = '#'; 
        mapa[13][2] = '@'; mapa[13][12] = 'O'; /* Botao para melhorar arma */
        mapa[6][12] = '@';
        mapa[8][12] = 'k'; 

        inimigos[0].x = 8; inimigos[0].y = 7; inimigos[0].tipo = 'Y'; inimigos[0].hp = 5; inimigos[0].ativo = 1;
        qtd_inimigos = 1;
    }
    
    else if (andar_atual == 3) { 
        dim_atual = 25;
        player.x = 23; player.y = 12;
        for(i=0; i<25; i++) { mapa[0][i] = '*'; mapa[24][i] = '*'; mapa[i][0] = '*'; mapa[i][24] = '*'; }
        
        for(i=5; i<20; i++) { mapa[i][5] = '*'; mapa[i][19] = '*'; }
        for(i=5; i<20; i++) { mapa[5][i] = '*'; mapa[19][i] = '*'; }
        mapa[19][12] = 'D'; 
        
        mapa[22][2] = '@'; mapa[22][22] = '@'; mapa[2][2] = '@';
        mapa[4][12] = 'D'; mapa[3][12] = 'D';

        inimigos[0].x = 12; inimigos[0].y = 12; inimigos[0].tipo = 'Z'; inimigos[0].hp = 8; inimigos[0].ativo = 1;
        inimigos[1].x = 10; inimigos[1].y = 10; inimigos[1].tipo = 'Y'; inimigos[1].hp = 5; inimigos[1].ativo = 1;
        qtd_inimigos = 2;
    }

    memcpy(mapa_backup, mapa, sizeof(mapa));
}

/* ============================================================================
   COLISAO E INTERACAO (LOGICA SOLIDA)
   ============================================================================ */
int movimento_valido(int prox_x, int prox_y) {
    char bloco;
    if (prox_x < 0 || prox_x >= dim_atual || prox_y < 0 || prox_y >= dim_atual) return 0; 
    
    bloco = mapa[prox_x][prox_y];
    if (bloco == '*' || bloco == 'k' || bloco == 'D' || bloco == 'N') return 0; 
    return 1; 
}

int inimigos_vivos(void) {
    int i, vivos = 0;
    for (i = 0; i < qtd_inimigos; i++) {
        if (inimigos[i].ativo) vivos++;
    }
    return vivos;
}

void processar_turno(char tecla) {
    int prox_x = player.x;
    int prox_y = player.y;
    int fx, fy;
    char alvo;

    if (kbhit()) getch(); 

    if (tecla == 'w') { player.simbolo = '^'; prox_x--; }
    else if (tecla == 's') { player.simbolo = 'v'; prox_x++; }
    else if (tecla == 'a') { player.simbolo = '<'; prox_y--; }
    else if (tecla == 'd') { player.simbolo = '>'; prox_y++; }
    
    else if (tecla == 'o') {
        if (player.arma_equipada == 0) {
            pausa_dramatica("Aviso: Voce esta desarmado!", 1500);
            return;
        }
        processar_ataque();
        mover_inimigos();
        return;
    }
    
    else if (tecla == 'i') {
        fx = player.x; fy = player.y;
        if (player.simbolo == '^') fx--;
        else if (player.simbolo == 'v') fx++;
        else if (player.simbolo == '<') fy--;
        else if (player.simbolo == '>') fy++;

        if (fx >= 0 && fx < dim_atual && fy >= 0 && fy < dim_atual) {
            alvo = mapa[fx][fy];
            
            if (andar_atual == 0 && alvo == 'N') {
                char op;
                system("cls");
                printf("=== ANCIAO DA VILA ===\n");
                printf("A profecia se inicia... Escolha seu instrumento:\n");
                printf("1. Wooden Sword (Dano: 1)\n");
                printf("2. Patched Bow (Dano: 1)\n");
                printf("3. Hickory Staff (Dano: 1)\n");
                op = getch();
                if (op == '1') { player.arma_equipada = 1; strcpy(player.arma_nome, "Wooden Sword"); }
                else if (op == '2') { player.arma_equipada = 2; strcpy(player.arma_nome, "Patched Bow"); }
                else if (op == '3') { player.arma_equipada = 3; strcpy(player.arma_nome, "Hickory Staff"); }
                else { return; } 
                
                mapa[fx][fy] = 'L'; 
                pausa_dramatica("As escadas para os dominios profanados se abrem...", 2500);
                return;
            }
            
            if (alvo == 'D' && player.chaves > 0) {
                player.chaves--;
                mapa[fx][fy] = '='; 
                pausa_dramatica("A porta cede revelando o caminho...", 1200);
                return;
            }
            
            if (alvo == 'O') {
                if (player.arma_dano == 1) {
                    player.arma_dano = 3;
                    player.arma_upada = 1;
                    if (player.arma_equipada == 1) strcpy(player.arma_nome, "Master Sword");
                    else if (player.arma_equipada == 2) strcpy(player.arma_nome, "Galadhrim Bow");
                    else if (player.arma_equipada == 3) strcpy(player.arma_nome, "Void Staff");
                    
                    mapa[fx][fy] = '=';
                    pausa_dramatica("Sua arma atinge o maximo potencial (DANO 3)! O Escolhido desperta!", 2500);
                }
                return;
            }
        }
        return;
    }

    if (movimento_valido(prox_x, prox_y)) {
        char bloco_pisado = mapa[prox_x][prox_y];
        
        player.x = prox_x;
        player.y = prox_y;

        if (bloco_pisado == '@') {
            player.chaves++;
            mapa[player.x][player.y] = ' ';
            pausa_dramatica("Voce encontrou uma Chave Ferrujada!", 1000);
        }

        if (bloco_pisado == '#') {
            player.vidas--;
            pausa_dramatica("AAARRGH! Espinhos rasgam sua armadura...", 2000);
            if (player.vidas > 0) {
                player.chaves = 0; 
                memcpy(mapa, mapa_backup, sizeof(mapa));
                inicializar_fase(); 
            }
            return;
        }

        if (bloco_pisado == 'L') {
            andar_atual++;
            player.chaves = 0; 
            pausa_dramatica("A descida rumo ao covil de Krauser continua...", 2000);
            inicializar_fase();
            return;
        }
    }

    mover_inimigos();
}

/* ============================================================================
   MOVIMENTACAO E COMBATE
   ============================================================================ */
void mover_inimigos(void) {
    int i, nx, ny, dir, dx, dy, tx, ty;
    char bloco;

    for (i = 0; i < qtd_inimigos; i++) {
        if (!inimigos[i].ativo) continue;

        nx = inimigos[i].x;
        ny = inimigos[i].y;

        if (inimigos[i].tipo == 'X') {
            dir = rand() % 4;
            if (dir == 0) nx--;
            else if (dir == 1) nx++;
            else if (dir == 2) ny--;
            else if (dir == 3) ny++;
        }
        else if (inimigos[i].tipo == 'Y') {
            dx = player.x - inimigos[i].x;
            dy = player.y - inimigos[i].y;
            if (abs(dx) > abs(dy)) nx += (dx > 0) ? 1 : -1;
            else ny += (dy > 0) ? 1 : -1;
        }
        else if (inimigos[i].tipo == 'Z') {
            turnos_boss++;
            if (turnos_boss % 3 == 0) {
                /* Krauser: Teleporte e Invocacao */
                do { tx = rand() % dim_atual; ty = rand() % dim_atual; } while (mapa[tx][ty] != ' ');
                nx = tx; ny = ty; /* Teleporte */

                /* Tenta invocar minion respeitando o limite maximo de 7 em campo */
                if (inimigos_vivos() < 7) {
                    int slot = -1;
                    int j;
                    /* Acha um slot inativo para reaproveitar no array */
                    for(j = 0; j < 10; j++) {
                        if(!inimigos[j].ativo) {
                            slot = j;
                            break;
                        }
                    }
                    if (slot != -1) {
                        do { tx = rand() % dim_atual; ty = rand() % dim_atual; } while (mapa[tx][ty] != ' ');
                        inimigos[slot].x = tx;
                        inimigos[slot].y = ty;
                        inimigos[slot].tipo = (rand() % 2 == 0) ? 'X' : 'Y';
                        inimigos[slot].hp = (inimigos[slot].tipo == 'X') ? 3 : 5;
                        inimigos[slot].ativo = 1;
                        if(slot >= qtd_inimigos) qtd_inimigos = slot + 1;
                    }
                }
            } else {
                /* Movimentacao padrao perseguindo o heroi */
                dx = player.x - inimigos[i].x; dy = player.y - inimigos[i].y;
                if (abs(dx) > abs(dy)) nx += (dx > 0) ? 1 : -1;
                else ny += (dy > 0) ? 1 : -1;
            }
        }

        if (nx >= 0 && nx < dim_atual && ny >= 0 && ny < dim_atual) {
            bloco = mapa[nx][ny];
            if (bloco != '*' && bloco != 'k' && bloco != 'D' && bloco != 'L' && bloco != 'O') {
                inimigos[i].x = nx;
                inimigos[i].y = ny;
            }
        }

        if (inimigos[i].x == player.x && inimigos[i].y == player.y) {
            player.vidas--;
            pausa_dramatica("GOLPE FATAL! A escuridao o consome...", 2500);
            if (player.vidas > 0) {
                player.chaves = 0; 
                memcpy(mapa, mapa_backup, sizeof(mapa));
                inicializar_fase();
            }
            return;
        }
    }
}

void aplicar_dano(int x, int y) {
    int i;
    if (x < 0 || x >= dim_atual || y < 0 || y >= dim_atual) return;
    
    /* NOVO: Registra o ataque na matriz temporaria para feedback visual */
    overlay_ataque[x][y] = 'z';

    if (mapa[x][y] == 'k') {
        mapa[x][y] = ' '; 
        return;
    }

    for (i = 0; i < qtd_inimigos; i++) {
        if (inimigos[i].ativo && inimigos[i].x == x && inimigos[i].y == y) {
            inimigos[i].hp -= player.arma_dano;
            if (inimigos[i].hp <= 0) {
                inimigos[i].ativo = 0;
                if (inimigos[i].tipo != 'Z') pausa_dramatica("A criatura sombria foi expurgada!", 1000);
                
                if (inimigos_vivos() == 0 || (andar_atual == 3 && inimigos[0].ativo == 0)) {
                    if (andar_atual == 1) {
                        mapa[2][1] = 'L'; mapa[3][1] = 'L'; mapa[4][1] = 'L';
                        pausa_dramatica("Um tremor ecoa... As escadas para a Torre Maldita surgiram!", 2000);
                    }
                    else if (andar_atual == 2) {
                        mapa[1][7] = 'L'; mapa[2][7] = 'L'; mapa[3][7] = 'L'; mapa[4][7] = 'L';
                        pausa_dramatica("Um tremor ecoa... Os portoes do Castelo de Krauser foram abertos!", 2000);
                    }
                }
            }
        }
    }
}

void processar_ataque(void) {
    int i, j;
    
    /* Zera a tela de ataque antes de calcular as posicoes */
    for (i = 0; i < MAX_DIM; i++) {
        for (j = 0; j < MAX_DIM; j++) {
            overlay_ataque[i][j] = 0;
        }
    }

    if (player.arma_equipada == 1) {
        if (player.simbolo == '^') {
            for(i=-1; i<=1; i++) { aplicar_dano(player.x-1, player.y+i); aplicar_dano(player.x-2, player.y+i); }
        } else if (player.simbolo == 'v') {
            for(i=-1; i<=1; i++) { aplicar_dano(player.x+1, player.y+i); aplicar_dano(player.x+2, player.y+i); }
        } else if (player.simbolo == '<') {
            for(i=-1; i<=1; i++) { aplicar_dano(player.x+i, player.y-1); aplicar_dano(player.x+i, player.y-2); }
        } else if (player.simbolo == '>') {
            for(i=-1; i<=1; i++) { aplicar_dano(player.x+i, player.y+1); aplicar_dano(player.x+i, player.y+2); }
        }
    } 
    else if (player.arma_equipada == 2) {
        for (i = 1; i <= 4; i++) {
            if (player.simbolo == '^') aplicar_dano(player.x-i, player.y);
            else if (player.simbolo == 'v') aplicar_dano(player.x+i, player.y);
            else if (player.simbolo == '<') aplicar_dano(player.x, player.y-i);
            else if (player.simbolo == '>') aplicar_dano(player.x, player.y+i);
        }
    } 
    else if (player.arma_equipada == 3) {
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                aplicar_dano(player.x+i, player.y+j);
            }
        }
    }

    /* NOVO: Renderiza a tela rapidamente para mostrar os z's e dar feedback pro jogador */
    desenhar_mapa();
    Sleep(250); /* O tempo da interrupção (250 milissegundos) */
    
    /* Limpa a tela de ataque para a proxima acao */
    for (i = 0; i < MAX_DIM; i++) {
        for (j = 0; j < MAX_DIM; j++) {
            overlay_ataque[i][j] = 0;
        }
    }
}

/* ============================================================================
   RENDERIZACAO E VISUAL DOURADO
   ============================================================================ */
void desenhar_mapa(void) {
    int i, j, k;
    int inimigo_impresso;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    system("cls");
    
    printf("=== O ESCOLHIDO ===\n");
    printf("Vidas: %d | Chaves: %d | Andar: %d\n", player.vidas, player.chaves, andar_atual);
    printf("Arma: %s | DANO ATUAL: %d\n", player.arma_nome, player.arma_dano);
    printf("==============================\n\n");

    for (i = 0; i < dim_atual; i++) {
        for (j = 0; j < dim_atual; j++) {
            
            if (i == player.x && j == player.y) {
                if (player.arma_upada) {
                    /* Aplicar visual dourado para o guerreiro mais honrado */
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                }
                printf("%c ", player.simbolo);
                if (player.arma_upada) {
                    /* Restaurar cor base */
                    SetConsoleTextAttribute(hConsole, saved_attributes);
                }
            } 
            else if (overlay_ataque[i][j] == 'z') {
                /* NOVO: Imprime a zona atingida pelo ataque em destaque! */
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); 
                printf("z ");
                SetConsoleTextAttribute(hConsole, saved_attributes);
            }
            else {
                inimigo_impresso = 0;
                for (k = 0; k < qtd_inimigos; k++) {
                    if (inimigos[k].ativo && inimigos[k].x == i && inimigos[k].y == j) {
                        printf("%c ", inimigos[k].tipo);
                        inimigo_impresso = 1;
                        break;
                    }
                }
                
                if (!inimigo_impresso) {
                    printf("%c ", mapa[i][j]);
                }
            }
        }
        printf("\n");
    }
}
