#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <cjson/cJSON.h>

/* Essa estrutura armazena os dados extraidos do JSON dos 
funcionários da Câmara, disponível no Dados Abertos */
typedef struct {
    char ponto[7];
    char nome[80];
    char cargo[80];
    char funcao[120];
    char nivel_carreira[15];
    char doutorado[10];
    char mestrado[10];
    char graduacao_1[10];
    char graduacao_2[10];
    char especializacao_1[10];
    char especializacao_2[10];

} InfosServidor;

/* PROTÓTIPO DAS FUNÇÕES */
char *obter_ponto();
int pesquisar_infos_servidor(InfosServidor *info_servidor);
int menu(const char *titulo, const char *opcoes[], int n_opçoes, const char *descricao);
int tecla_pressionada();
int tecla_pressionada_com_echo();
char *ler_arquivo(const char *nome);
void simular_contracheque(InfosServidor *info_servidor);


/* OPÇÕES */
char *opcoes_nivel[10] = {"A1", "A2", "A3", "A4", "B5", "B6", "B7", "B8" , "Especial 9", "Especial 10"};


int main(int argc, char *argv[]){
    InfosServidor info_servidor = {0};  // Inicializa a struct info_servidor

    char *p = obter_ponto();   // Obtem o ponto do servidor
    strcpy(info_servidor.ponto, p);     // Grava o ponto na struct info_servidor
    free(p);                            

    pesquisar_infos_servidor(&info_servidor);   // A partir do ponto, pesquisa as informaçoes do servidor
                                                // no JSON e armazena elas na struct info_servidor

    
    const char *titulo = "CÂMARA DOS DEPUTADOS";
    const char *opcoes[] = {"Simular contrachaque", 
                            "Ver minhas informações"};
    int n_opcoes = 2;
    char descricao[80] = "Olá, ";
    int pos = strcspn(info_servidor.nome, " ");
    char nome[20] = {'\0'};    
    strncpy(nome, info_servidor.nome, pos);
    strcat(descricao, nome);
    strcat(descricao, ". O que deseja?");

    int opcao = menu(titulo, opcoes, n_opcoes, descricao);

    switch(opcao){
        case 0:
            simular_contracheque(&info_servidor);
            break;
        case 1:
            printf("%s\n", opcoes[opcao]);
            break;
        default:
            printf("%s\n", opcoes[opcao]);
    }
    return 0;

}

int menu(const char *titulo, const char *opcoes[], int n_opçoes, const char *descricao){
    int selecionado = 0;

    while(1){
        //Limpa a tela
        system("clear");
        
        //Imprime o título do menu
        printf("########## %s ##########\n\n", titulo);

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

int pesquisar_infos_servidor(InfosServidor *info_servidor){
    char *conteudo = ler_arquivo("funcionarios.json");
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
        cJSON *funcao = cJSON_GetObjectItem(item, "funcao");

        if (cJSON_IsString(ponto) &&
            strcmp(ponto->valuestring, info_servidor->ponto) == 0) {
            
            strcpy(info_servidor->nome, nome->valuestring);
            strcpy(info_servidor->cargo, cargo->valuestring);
            strcpy(info_servidor->funcao, funcao->valuestring);
            //printf("Nome: %s\n", nome ? nome->valuestring : "(sem nome)");
            //printf("Cargo: %s\n", cargo ? cargo->valuestring : "(sem cargo)");
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

int tecla_pressionada_com_echo(){
    /*No Linux/Unix, a função _getch() existente no Windows não existe por padrão. O terminal funciona em modo "canônico" 
    (espera uma linha inteira e o "Enter"). Para simular o comportamento do Windows, precisamos manipular as configurações 
    do terminal manualmente usando a estrutura "termios".*/

    // Cria duas variáveis para guardar as configurações do terminal: oldt (configuração antiga/original) e newt (nova 
    // configuração que vamos criar).
    struct termios oldt, newt;  
    
    // Variável para armazenar o caractere pressionado
    int ch;
    
    // Pega (get) os atributos atuais do terminal e salva em oldt. Esses atributos definem o comportamento do terminal.
    // Ex: (ICANON) Esperar o ENTER para a execução do comando.
    tcgetattr(STDIN_FILENO, &oldt);
    
    // Copia as configurações originais para a variável newt. Vamos modificar essa cópia.
    newt = oldt;
    
    // Desligando duas "bandeiras" (flags) de configuração usando operações bit a bit:
    newt.c_lflag &= ~(ICANON);
    
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
    /* Essa função abre um arquivo JSON, ler seu conteúdo 
    e o copia para um buffer que é retornado */
    
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

char *obter_ponto(){
    /* Esta função imprime uma mensagem na tela, solicitando que o usuário digite seu numéro de ponto*/
    
    char c; // Armazena oa caractere pressionado

    char *ponto = malloc(7);    // Armazena o ponto
    strcpy(ponto, "P_0000");

    system("clear");    //Limpa a tela
    
    // Imprimindo a mensagem
    printf("########## CÂMARA DOS DEPUTADOS ##########");
    printf("\nInsira seu ponto: P_");
    
    for(int i = 2; i < 6; i++){
        c = tecla_pressionada_com_echo();
        if(c < '0' || c > '9'){
            printf("\033[D"); // move cursor para a esquerda
            printf("\033[K"); // apaga até o fim da linha
            i--;
        }
        else{
            ponto[i] = c;
        }    
    }

    ponto[6] = '\0';
    
    return ponto;
}

void simular_contracheque(InfosServidor *info_servidor){
    int selecionado = 0;
    int modo_edicao = 0;
    char *campos[][2] = {{"Nome", info_servidor->nome}, 
                        {"Cargo", info_servidor->cargo}, 
                        {"Nível na carreira", info_servidor->nivel_carreira}, 
                        {"Função comissionada", "---"}, 
                        {"Substituição de FC", "---"}, 
                        {"Doutorado", "---"},
                        {"Mestrado", "---"},
                        {"1ª Graduação", "---"},
                        {"2ª Graduação", "---"}, 
                        {"1ª Especialização", "---"}, 
                        {"2ª Especialização", "---"}};
    int n_campos = 11;

    while(1){
        system("clear");
        printf("########## SIMULAR CONTRACHEQUE ##########\n");
        printf("Verifique as informações abaixo.\nPara editar, selecione o campo e tecle ENTER.\nUse as setas ↑ ↓\n");

        for(int i = 0; i < n_campos; i++){
            if (selecionado == i) printf(" ✏️  \033[31m%s:\033[0m %s\n", campos[i][0], campos[i][1]);
            else printf("    %s: %s\n", campos[i][0], campos[i][1]);
        }

        int ch = tecla_pressionada();

        if(ch == 27){   // Escape (ESC)
            ch = tecla_pressionada();
            if(ch == 91){
                ch = tecla_pressionada();
                if(ch == 65){   // Seta para cima
                    if(selecionado > 0){
                        selecionado --;
                    }
                    else{
                        selecionado = n_campos - 1;
                    }
                } // if(ch == 65)
                else if(ch == 66){  // Seta para baixo
                    if(selecionado < (n_campos -1)){
                        selecionado ++;
                    }
                    else{
                        selecionado = 0;
                    }
                } // else if(ch == 66)
            } // if(ch == 91)
        } // if(ch == 27)

        if(ch == 10){   // ENTER
            if(modo_edicao == 0) modo_edicao = 1;
            else modo_edicao = 0; 
            printf("%d\n", selecionado);
            break;
        }
    }
}   // simular_contracheque