#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VERDE  "\033[0;32m"
#define AMARELO "\033[0;33m"
#define VERMELHO "\033[0;31m"
#define RESET  "\033[0m"

#define MAX 100
#define TAM 6

int main(){

    FILE *arq = fopen("palavras.txt", "r");
    if(arq == NULL){
        printf("Erro ao abrir arquivo\n");
        return 1;
    }
    char banco[MAX][TAM];
    int total = 0;
    while(total< MAX && fscanf(arq, "%5s", banco[total]) == 1){
        total++;
    }
    fclose(arq);
    printf("Foram carregadas %d palavras\n", total);
    srand(time(NULL));
    if(total == 0){
        printf("Nenhuma palavra carregada\n");
        return 1;
    }
    char *palavra = banco[rand() % total];
    //_________________________________________________________________
    char tentativa[6];
    char correc[6];
    char uso[5] = {0};
    int i = 1;


    printf("-----------------------------------------------------------------------\n");
    printf("------------------------------ADIVINHE A-------------------------------\n");
    printf("--------------------------------PALAVRA--------------------------------\n");
    printf("-----------------------------------------------------------------------\n");

    printf("\nVoce tem 6 tentativas para acertar uma palavra secreta de 5 letras.\n");
    printf("\n- Vermelho significa que a letra nao existe na palavra.\n");
    printf("- Amarelo significa que a letra esta na palavra, mas na posicao errada.\n");
    printf("- Verde significa que a letra esta na posicao certa.\n");
    printf("-----------------------------------------------------------------------\n");

    while(i<=6){ //(tentativa numero i)
        printf("\n%d: ", i);
        scanf("%5s", tentativa);//"tentativa" já é um endereço;
                
        for(int k = 0; k < 5; k++){//deixa uso[] e correc[] zerados.
            uso[k] = 0;
            correc[k] = 'X';
        }

        for(int j=0; j<5; j++){ //1a passada
            if(tentativa[j]==palavra[j]){
                correc[j]='O';
                uso[j]=1;
                
            }
        }

        for(int j=0; j<5; j++){ //2a passada
            if(correc[j]=='O'){
                continue;
            }
            for(int k=0; k<5; k++){//aqui verifica se tentativa[j] está contido em palavra[], passando por todas as letras a partir do loop k.
                if(!uso[k] && tentativa[j]==palavra[k]){ //palavra[k] é a letra na posição onde uso[k] é 0, ou seja, a letra não foi usada. Se tentativa[j] for igual a palavra[k], então (amarelo)
                correc[j]='/';
                uso[k]=1;
                break;//se a condição é satisfeita, a procura para e segue para outra letra de tentativa      
                }
            }    
        }
        correc[5]='\0';

        for(int j = 0; j < 5; j++){
            if(correc[j] == 'O'){
                printf(VERDE "%c " RESET, tentativa[j]);
            }
            else if(correc[j] == '/'){
                printf(AMARELO "%c " RESET, tentativa[j]);
            }
            else{
                printf(VERMELHO "%c " RESET, tentativa[j]);
            }
        }

        if(strcmp(tentativa, palavra)==0){
            printf("\nParabens! Voce ganhou o jogo.");
            break;
        }

        printf("\n");
        i++;
    }
    if(i>6){
        printf("\nVoce perdeu o jogo. A palavra era '%s'.", palavra);
    }

    return 0;
}