#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>//desativa enter
#include <stdlib.h>
#include <fcntl.h>//verifica se houve tecla

#define VERDE  "\033[0;32m"
#define AMARELO "\033[0;33m"
#define VERMELHO "\033[0;31m"
#define RESET  "\033[0m"

struct corpo{
    int posx;
    int posy;
};

struct termios velho_terminal, novo_terminal;
void mudar_terminal(){
    tcgetattr(STDIN_FILENO, &velho_terminal);
    novo_terminal = velho_terminal;

    novo_terminal.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &novo_terminal);
}
void restaurar_terminal(){
    tcsetattr(STDIN_FILENO, TCSANOW, &velho_terminal);
}

void gerarMaca(char mapa[17][18]) {
    int ax, ay;

    do {
        ax = rand() % 17;
        ay = rand() % 17;
    } while (
        mapa[ax][ay] == '|' ||
        mapa[ax][ay] == '#' ||
        mapa[ax][ay] == '_' ||
        mapa[ax][ay] == '+' ||
        mapa[ax][ay] == '>' ||
        mapa[ax][ay] == '<' ||
        mapa[ax][ay] == '^' ||
        mapa[ax][ay] == 'v' 
    );

    mapa[ax][ay] = '@';
}


int main(){
    mudar_terminal();
    printf("--------------------------\n");
    printf("-----JOGO DA COBRINHA-----\n");
    printf("--------------------------\n");
    printf("\nComa o maximo de macas (@) que conseguir.\nCada maca aumenta seu corpo um pouquinho\nCuidado para nao esbarrar nas paredes ou em voce mesmo!\n");
    printf("Aperte qualquer tecla para continuar.\n");
    char qualquer = getchar();
    system("clear");

    srand(time(NULL));
    int contador = 0;

    int j=0;
    int cresceu=0;
    char move = 'd';
    char mapa[17][18] = { //linha/coluna+1

        "_________________",//pos 0
        "|0              |",//(1,1)
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|               |",
        "|###############|",
    };        
    int ax, ay;
    do {//randomizar spawn das macas
        ax = rand() % 17;  
        ay = rand() % 17;  
    } while(mapa[ax][ay] != ' ');

    mapa[ax][ay] = '@';
    
    for(int i = 0; i <17; i++){//primeira impressao do mapa
        printf("%s\n", mapa[i]);
    }
    struct corpo c[50];
    int px=1, py=1;

    char d;
    c[0].posx = px;
    c[0].posy = py;

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
//---------------------------------------------------------
    while(1){
        printf("\nPontuacao: %d\n", contador);

        char temp;
        if(read(STDIN_FILENO, &temp, 1) > 0){
            move = temp;
        }
        usleep(500000);        

        int oldx = px;
        int oldy= py;
        //antiga posição, antes do movimento

        switch(move){ //movimentação
            case 'a': //esquerda, mantem linha, -coluna
                py--;
                d='<';
                break;
            case 'w': //cima, -linha, mantem coluna
                px--;
                d='^';
                break;
            case 's': //baixo, +linha, mantem coluna
                px++;
                d='v';
                break;
            case 'd': //direita, mantem linha, +coluna
                py++;
                d='>';
                break;
        }//setados novos valores para coordenadas de 0
        
        if(mapa[px][py]=='|' || mapa[px][py]=='_' || mapa[px][py]=='+' || mapa[px][py]=='#'){ 
            mapa[oldx][oldy]='X';
            system("clear");
            for(int i = 0; i <17; i++){
                printf("%s\n", mapa[i]);
            }

            break; 
        } 
        //se esbarrar, morre

        if(mapa[px][py]=='@'){
            cresceu = 1;
            gerarMaca(mapa);
            contador++;
            j++;
        } 

        for(int k=j; k>0;k--){//transferencia de coordenadas para todas as partes, de tras para frente.
            c[k]=c[k-1];
        }

        c[0].posx = px;//ponto de partida para o for de desenho, com coord atualizada de 0
        c[0].posy= py;

        for(int i=0; i<17; i++){//limpa todo o mapa
            for(int l=0; l<17; l++){
                if(mapa[i][l]== '>' || mapa[i][l]== '<' ||mapa[i][l]== '^' ||mapa[i][l]== 'v' || mapa[i][l]=='+' || mapa[i][l]=='0'){
                    mapa[i][l]=' ';
                }
            }
        }
        for(int k=0; k<=j; k++){//depois redesenha com as novas coordenadas
            if(k==0)
                mapa[c[k].posx][c[k].posy] = d;
            else
                mapa[c[k].posx][c[k].posy] = '+';
        
        }

        printf("\033[H");

        for(int i=0; i<17; i++){//CORES!
            for(int l=0; l<17; l++){
                if(mapa[i][l]=='+'||mapa[i][l]==0||mapa[i][l]== '>' || mapa[i][l]== '<' ||mapa[i][l]== '^' ||mapa[i][l]== 'v'){
                    printf(VERDE "%c" RESET, mapa[i][l]);
                }
                else if(mapa[i][l]=='@'){
                    printf(VERMELHO "%c" RESET, mapa[i][l]);
                }
                else{
                    printf("%c", mapa[i][l]);
                }
            }
            printf("\n");
        }

    }
    printf("\nFim de jogo.");
    printf("\nVoce comeu %d macas.\n", contador);

    restaurar_terminal();
    return 0;
}

/*LOGICA
Do ultimo elemento para o primeiro, as partes do corpo recebem a coordenada do seu posterior, assim:
c[5] passa a ser c[4], c[4] passa a ser c[3].....c[1] passa ser c[0], sendo c[0] a cabeça do corpo, que tem coordenadas px,py;

O mapa é apagado
Então reescrito com as novas coordenadas, onde para j != 0, imprime +, e para j=0, imprime seta.


*/