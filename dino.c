#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>//verifica se houve tecla

#define VERDE  "\033[0;32m"
#define AMARELO "\033[0;33m"
#define VERMELHO "\033[0;31m"
#define AZUL "\033[34m"
#define CINZA "\033[90m"
#define RESET  "\033[0m"

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

//o cacto só vai se mover x--; nunca mexe Y
//a ave só vai se mover x--, alternando tipos; nunca mexe Y
//o player vai ficar parado, alternando tipos, e podendo pular y-- nunca mexe X

char morto[4][7]={
"   _  ",
" _| X>", 
"\\<__/ ",
" ! '  ",
};
char esqP[4][7]={
"   _  ",
" _| '>", 
"\\<__/ ",
" ! '  ",
};
char dirP[4][7]={
"   _  ",
" _| '>", 
"\\<__/ ",
" ' !  ",
};
char mapa[10][46]={
    "+++++++++++++++++++++++++++++++++++++++++++++",
    "                                             ",
    "                                             ",
    "                                             ",
    "                                             ",
    "                                             ",
    "                                             ",
    "                                             ",
    "                                             ",
    "_____________________________________________",
};
char cactosprite[3][4]={
    "\\| ",
    " |/",
    " | ",
};
char aveBaixo[2][7]={
    "<\\.___",
    "  'v-'"
};
char aveCima[2][7]={
    "<\\.^__",
    "  '--'"
};

struct entidade{//o player, o cacto e as aves.
    int x;
    int y;
    int altura, largura;
    int tipo;
    int pulo;
    char (*sprite)[7];
};
struct entidade spawnAve(){
    struct entidade n;
    n.x=44;
    n.y=7;
    n.altura=2;
    n.largura=6;
    n.tipo=1;
    return n;
};
struct entidade spawnCacto(){
    struct entidade n;
    n.x=44;
    n.y=7;
    n.altura=3;
    n.largura=3;
    return n;
};
struct entidade spawnPlayer(){
    struct entidade n;
    n.x=1;
    n.y=6;
    n.altura=4;
    n.largura=6;
    n.pulo=10;
    n.tipo=1;
    return n;
}


int colisao(struct entidade a, struct entidade b){
    if(
        a.x < b.x + b.largura &&
        a.x + a.largura > b.x &&
        a.y< b.y + b.altura &&
        a.y + a.altura > b.y
    ){return 1;}
    else{return 0;}
};

int main(){
    srand(time(NULL));
    struct entidade player;
    struct entidade ave[100];
    struct entidade cacto[100];

    int perdeu=0;
    int a=-1; //conta aves ativas
    int c=-1; //conta cactos ativos

    player=spawnPlayer();
    int cooldown=10;
    int pulo=0;
    int frames=0;
    int col=0;
    mudar_terminal();
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

while(1){
    printf("\033[H");
    cooldown--;
    frames++;

    char comando;
    //char temp;
    if(read(STDIN_FILENO, &comando, 1) > 0){
        //comando = temp;
        if(comando=='w' && player.y>=6) {pulo=1;}
    }

    

    if(pulo){//pulo do personagem
        if(player.pulo>5){player.y--;}
        if(player.pulo<5 && player.pulo>=0){player.y++;}
        player.pulo--;
    }
        if(player.pulo<0){player.pulo=10; pulo=0;}

    if(player.tipo==1){player.sprite=esqP; player.tipo=2;}//anima player
    else if(player.tipo==2){player.sprite=dirP; player.tipo=1;}

    if(cooldown<=0){
        int m = rand()%3;
        if(m==0){a++; ave[a]=spawnAve();}
        if(m==1){c++; cacto[c]=spawnCacto();}
    }

    for(int i=0; i<=a; i++){//movimenta e anima ave
        ave[i].x=ave[i].x-3;
        if(ave[i].tipo==1){ave[i].sprite=aveCima; ave[i].tipo=2;}
        else if(ave[i].tipo==2){ave[i].sprite=aveBaixo; ave[i].tipo=1;}
    }
    for(int i=0; i<=c; i++){//movimenta cacto
        cacto[i].x=cacto[i].x-3;
    } 
    
    for(int i=0; i<=a; i++){
        if(colisao(player, ave[i])){perdeu=1;}
    }
    for(int i=0; i<=c; i++){
        if(colisao(player, cacto[i])){perdeu=1;}
    }
    if(perdeu){player.sprite=morto;}

    for(int y=0; y<10; y++){//impressão
        for(int x=0; x<45; x++){
            int imp=0;
            if(x>=player.x && x<player.x+6 && y>=player.y && y<player.y+4){
                printf("%c", player.sprite[y-player.y][x-player.x]);
                imp=1;
            }
            for(int i=0; i<=a; i++){//aves
                if(x>=ave[i].x && x<ave[i].x+6 && y>=7 && y<7+2){
                    printf("%c", ave[i].sprite[y-7][x-ave[i].x]);
                    imp=1;
                }
            }
            for(int i=0; i<=c; i++){//cactos
                if(x>=cacto[i].x && x<cacto[i].x+3 && y>=7 && y<7+3){
                    printf("%c", cactosprite[y-7][x-cacto[i].x]);
                    imp=1;
                }
            }
            if(!imp){
                printf("%c", mapa[y][x]);
            }
        }
        printf("\n");
    }
    usleep(100000);


    printf("\nPontuacao: %d", frames);

    if(cooldown<=0){cooldown=20;}
    if(perdeu){break;}


}//while
    //restaurar_terminal();
    printf("\nVoce perdeu! X.X\n");

    return 0;
}