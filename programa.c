#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <cjson/cJSON.h>

int menu(const char *titulo, const char *opcoes[], int n_opçoes, const char *descricao);
int formulario_infos_servidor();
int tecla_pressionada();
char *ler_arquivo(const char *nome);


int main(int argc, char *argv[]){
    formulario_infos_servidor();
    /*const char *opcoes[] =    {"Simular contracheque", 
                                "Visualizar estruturas remuneratórias", 
                                "Projetar evolução na carreira", 
                                "Sair"};
    int n_opcoes = 4;
    const char *desc = "Selecione uma opção abaixo:\n";

    menu("CÂMARA DOS DEPUTADOS", opcoes, n_opcoes, desc);
    system("clear");
    */
    return 0;
}




int menu(const char *titulo, const char *opcoes[], int n_opçoes, const char *descricao){
    int selecionado = 0;

    while(1){
        //Limpa a tela
        system("clear");
        
        //Imprime o título do menu
        printf("##### %s #####\n\n", titulo);

        printf("%s\n", descricao);

        for(int i = 0; i < n_opçoes; i++){
            if(selecionado == i){
                printf(" 👉 %s\n", opcoes[i]);
            }
            else{
                printf("    %s\n", opcoes[i]);
            } 
        }   // for

        int ch = tecla_pressionada();
        /*Quando pressionamos uma tecla que não pertence a tabela ASCII, o computador envia não um, mas 3 bytes em sequência para
        para nosso programa:
        27 (ESC): É o caractere "Escape". Ele serve como um alerta.
        91 ([): Na tabela ASCII, o 91 é o abre colchetes. Nos padrões de terminal (ANSI/VT100), o ESC seguido de [ inicia a maioria 
        dos comandos de movimentação de cursor.
        XX (): Um byte, que especifica o comando. Ex: (66) = seta para baixo ↓ | (65) = seta para cima ↑*/

        // Verifica se é uma seta de es uma seta "↑" "↓" ou "ENTER".
        if(ch == 27){   // Escape (ESC)
            ch = tecla_pressionada();
            if(ch == 91){
                ch = tecla_pressionada();
                if(ch == 65){   // Seta para cima
                    if(selecionado > 0){
                        selecionado --;
                    }
                    else{
                        selecionado = n_opçoes - 1;
                    }
                } // if(ch == 65)
                else if(ch == 66){  // Seta para baixo
                    if(selecionado < (n_opçoes -1)){
                        selecionado ++;
                    }
                    else{
                        selecionado = 0;
                    }
                } // else if(ch == 66)
            } // if(ch == 91)
        } // if(ch == 27)

        if(ch == 10){   // ENTER
            return selecionado;
        }
    }   // while
}   // menu

int formulario_infos_servidor(){
    const char *nome = "funcionarios.json";
    const char *p = "P_6843";

    char *conteudo = ler_arquivo(nome);
    if (!conteudo) {
        printf("Erro ao ler arquivo\n");
        return 1;
    }

    cJSON *json = cJSON_Parse(conteudo);
    if (!json) {
        printf("Erro ao parsear JSON\n");
        free(conteudo);
        return 1;
    }

    // 1️⃣ pega o array "dados"
    cJSON *dados = cJSON_GetObjectItem(json, "dados");
    if (!cJSON_IsArray(dados)) {
        printf("Campo 'dados' não é array\n");
        goto fim;
    }

    // 2️⃣ percorre o array
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, dados) {

        cJSON *ponto = cJSON_GetObjectItem(item, "ponto");
        cJSON *nome  = cJSON_GetObjectItem(item, "nome");
        cJSON *cargo = cJSON_GetObjectItem(item, "cargo");

        if (cJSON_IsString(ponto) &&
            strcmp(ponto->valuestring, p) == 0) {

            printf("✅ Encontrado!\n");
            printf("Nome: %s\n", nome ? nome->valuestring : "(sem nome)");
            printf("Cargo: %s\n", cargo ? cargo->valuestring : "(sem cargo)");
            break;
        }
    }

fim:
    cJSON_Delete(json);
    free(conteudo);
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

char *ler_arquivo(const char *nome) {
    FILE *f = fopen(nome, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    rewind(f);

    char *buffer = malloc(tamanho + 1);
    fread(buffer, 1, tamanho, f);
    buffer[tamanho] = '\0';

    fclose(f);
    return buffer;
}