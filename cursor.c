#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int tecla_pressionada();

char *opcoes_padrao[10] = {"A1", "A2", "A3", "A4", "B5", "B6", "B7", "B8", "Especial 9", "Especial 10"};
int n_opcoes = 10;

int main(){
    
    int selecionado = 0;
    char buffer[50];
    int numero_caracteres[3] = {0};
    system("clear");
    numero_caracteres[0] = printf("Nome Completo: Dimas Moreira Soares\n");
    numero_caracteres[1] = printf("Padrão:\n");
    numero_caracteres[2] = printf("Função Comissionada:\n");
    printf("\033[2;%dH", numero_caracteres[1] + 1); // Posiciona o cursor
    
    while(1){
        int ch = tecla_pressionada();
        // Verifica se é uma seta "↑" "↓" ou "ENTER".
        if(ch == 27){   // Escape (ESC)
            ch = tecla_pressionada();
            if(ch == 91){
                ch = tecla_pressionada();
                if(ch == 65){   // Seta para cima
                    if(selecionado > 0){
                        selecionado --;
                    }
                    else{
                        selecionado = n_opcoes - 1;
                    }
                } // if(ch == 65)
                else if(ch == 66){  // Seta para baixo
                    if(selecionado < (n_opcoes -1)){
                        selecionado ++;
                    }
                    else{
                        selecionado = 0;
                    }
                } // else if(ch == 66)
                printf("\033[K");
                printf("\033[31m%s\033[0m", opcoes_padrao[selecionado]);
                printf("\033[2;%dH", numero_caracteres[1] + 1);
            } // if(ch == 91)
        } // if(ch == 27)

        if(ch == 10){   // ENTER
            printf("\033[3;%dH", numero_caracteres[2] + 1); // Posiciona o cursor
            break;
        }
    }

    return 0;
}

int tecla_pressionada(){
    /*No Linux/Unix, a função _getch() existente no Windows não existe por padrão. O terminal funciona em modo "canônico" 
    (espera uma linha inteira e o "Enter"). Para simular o comportamento do Windows, precisamos manipular as configurações 
    do terminal manualmente usando a estrutura "termios".*/

    // Cria duas variáveis para guardar as configurações do terminal: oldt (configuração antiga/original) e newt (nova 
    // configuração que vamos criar).
    struct termios oldt, newt;  
    
    // Variável para armazenar o caractere pressionado
    int ch;
    
    // Pega (get) os atributos atuais do terminal e salva em oldt. Esses atributos definem o comportamento do terminal.
    // Ex: (ICANON) Esperar o ENTER para a execução do comando | (ECHO) Mostra na tela o caractere da tecla pressionada.
    tcgetattr(STDIN_FILENO, &oldt);
    
    // Copia as configurações originais para a variável newt. Vamos modificar essa cópia.
    newt = oldt;
    
    // Desligando duas "bandeiras" (flags) de configuração usando operações bit a bit:
    newt.c_lflag &= ~(ICANON | ECHO);
    
    // Aplica (set) as novas configurações ao terminal agora (TCSANOW). A partir deste momento, o terminal para de 
    // esperar o Enter e de mostrar letras.
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    // Lê o caractere pressionado e guarda na variável ch. 
    ch = getchar();

    // Restaura as configurações originais (oldt)
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    // Retorna o caractere lido para quem chamou a função.
    return ch;
}   // tecla_pressionada