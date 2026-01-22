#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

typedef struct {
    int cargo_efetivo;
    int padrao_carreira;
    int funcao_comissionada;
    int substituicao_fc;
    int substituicao_fc_dias;
    int aux_pre_escola;
    int ad_especializacao_doutorado_mestrado;
    int ad_especializacao_seg_graduacao;
    int ad_especializacao_especializacao;
    int regime_previdenciario;
    int aliquota_funpresp;
    
} InfoServidor;

/*OPÇÕES PARA INFOS SERVIDOR*/
const char *opcoes_cargo_efetivo[] =    {"Consultor Legislativo", 
                                        "Analista Legislativo", 
                                        "Técnico Legislativo", 
                                        "Auxiliar Legislativo"};
int n_opcoes_cargo_efetivo = 4;
const char *desc_cargo_efetivo = "Selecione sua carreira:\n";

const char *opcoes_padrao_carreira[] = {"1 (Classe A)", "2 (Classe A)", "3 (Classe A)", 
                                        "4 (Classe A)", "5 (Classe B)", "6 (Classe B)", 
                                        "7 (Classe B)", "8 (Classe B)", "9 (Especial)", 
                                        "10 (Especial)"};
int n_opcoes_padrao_carreira = 10;
const char *desc_padrao_carreira = "Selecione seu padrão atual na carreira:\n";

const char *opcoes_funcao_comissionada[] = {"Não Possuo", "FC 1", "FC 2", "FC 3",
                                            "FC 4", "FC 5", "FC 6"};
int n_opcoes_funcao_comissionada = 7;
const char *desc_funcao_comissionada = "Possui função comissionda? Caso possua, indique:\n";

const char *opcoes_substituicao_fc[] =    {"Não substitui", "Substitui FC 1", "Substitui FC 2", 
                                        "Substitui FC 3", "Substitui FC 4", "Substitui FC 5",
                                        "Substitui FC 6"};
int n_opcoes_substituicao_fc = 7;
const char *desc_substituicao_fc = "Você substituiu alguém com função comissionda?\n";

const char *opcoes_ad_especializacao_doutorado_mestrado[] = {"Não possuo doutorado nem mestrado", "Apenas mestrado", 
                                                    "Apenas doutorado", "Mestrado e doutorado"};
int n_opcoes_ad_especializacao_doutorado_mestrado = 4;
const char *desc_ad_especializacao_doutorado_mestrado = "Possui mestrado e/ou doutorado?\n";

const char *opcoes_ad_especializacao_seg_graduacao[] = {"Não possuo 2ª graduação", "Possuo 2ª graduação"};
int n_opcoes_ad_especializacao_seg_graduacao = 2;
const char *desc_ad_especializacao_seg_graduacao = "Possui uma segunda graduação?\n";

const char *opcoes_ad_especializacao_especializacao[] = {"Não possuo especialização", "Possuo uma especialização",
                                                        "Possuo duas especializações"};
int n_opcoes_ad_especializacao_especializacao = 3;
const char *desc_ad_especializacao_especializacao = "Possui especializações (pós-graduações)?\n";

const char *opcoes_regime_previdenciario[] = {"RPPS", "RGPS - Sem FUNPRESP", "RGPS - Com FUNPRESP"};
int n_opcoes_regime_previdenciario = 3;
const char *desc_regime_previdenciario = "A qual regime previdenciário você está sujeito?\n";

const char *opcoes_aliquota_funpresp[] = {"7,5%", "8,0%", "8,5%"};
int n_opcoes_aliquota_funpresp = 3;
const char *desc_aliquota_funpresp = "Qual alíquota sobre o salário participação da FUNPRESP você escolheu?\n";

// PROTÓTIPOS
int tecla_pressionada();
int menu(const char *titulo, const char *opcoes[], int n_opçoes, const char *descricao);


int main(int argc, char *argv[]){

    InfoServidor info_servidor = {0};

    char aux[10] = {0};
    
    // Qual o cargo efetivo?
    info_servidor.cargo_efetivo = menu("CARGO EFETIVO", opcoes_cargo_efetivo, n_opcoes_cargo_efetivo, desc_cargo_efetivo);
    system("clear");

    // Qual o padrão da carreira?
    info_servidor.padrao_carreira = menu("PADRÃO DA CARREIRA", opcoes_padrao_carreira, n_opcoes_padrao_carreira, desc_padrao_carreira);
    system("clear");

    // Função Comissionada?
    if(info_servidor.cargo_efetivo != 0){   
        info_servidor.funcao_comissionada = menu("FUNÇÃO_COMISSIONADA", opcoes_funcao_comissionada, n_opcoes_funcao_comissionada, desc_funcao_comissionada);
        system("clear");
    }
    else {
        info_servidor.funcao_comissionada = 3;
    }

    // Substituiu alguém com FC?
    info_servidor.substituicao_fc = menu("SUBSTITUIÇÃO DE FC", opcoes_substituicao_fc, n_opcoes_substituicao_fc, desc_substituicao_fc);
    system("clear");

    // Se substituiu alguém com FC, foi por quantos dias?
    if(info_servidor.substituicao_fc != 0){   
        printf("Quantos dias na substituição da FC: ");
        fgets(aux, 10, stdin);
        info_servidor.substituicao_fc_dias = atoi(aux);
    }
    else {
        info_servidor.substituicao_fc_dias = 0;
    }

    // Adicional de especialização
    info_servidor.ad_especializacao_doutorado_mestrado = menu("ADCIONAL DE ESPECIALIZAÇÃO - MESTRADO E DOUTORADO", opcoes_ad_especializacao_doutorado_mestrado, n_opcoes_ad_especializacao_doutorado_mestrado, desc_ad_especializacao_doutorado_mestrado);
    system("clear");

    info_servidor.ad_especializacao_seg_graduacao = menu("ADCIONAL DE ESPECIALIZAÇÃO - 2ª GRADUAÇÃO", opcoes_ad_especializacao_seg_graduacao, n_opcoes_ad_especializacao_seg_graduacao, desc_ad_especializacao_seg_graduacao);
    system("clear");

    info_servidor.ad_especializacao_especializacao = menu("ADCIONAL DE ESPECIALIZAÇÃO - ESPECIALIZAÇÕES", opcoes_ad_especializacao_especializacao, n_opcoes_ad_especializacao_especializacao, desc_ad_especializacao_especializacao);
    system("clear");

    // Regime Previdenciário?
    info_servidor.regime_previdenciario = menu("REGIME PREVIDENCIÁRIO", opcoes_regime_previdenciario, n_opcoes_regime_previdenciario, desc_regime_previdenciario);
    system("clear");

    // Aliquota FUNPRESP
    if(info_servidor.regime_previdenciario == 2){   
        info_servidor.aliquota_funpresp = menu("ALÍQUOTA FUNPRESP", opcoes_aliquota_funpresp, n_opcoes_aliquota_funpresp, desc_aliquota_funpresp);
    }
    else {
        info_servidor.aliquota_funpresp = 0;
    }
    system("clear");




    printf("SUBSTITUIÇÃO DE FC: %s\n", opcoes_substituicao_fc[info_servidor.substituicao_fc]);
    printf("DIAS NA SUBSTITUIÇÃO DE FC: %d\n", info_servidor.substituicao_fc_dias);
    printf("REGIME PREVIDENCIÁRIO: %s\n", opcoes_regime_previdenciario[info_servidor.regime_previdenciario]);
    printf("ALÍQUOTA FUNPRESP: %s\n", opcoes_aliquota_funpresp[info_servidor.aliquota_funpresp]);
    printf("MESTRADO E DOUTORADO: %s\n", opcoes_ad_especializacao_doutorado_mestrado[info_servidor.ad_especializacao_doutorado_mestrado]);
    printf("2ª GRADUAÇÃO: %s\n", opcoes_ad_especializacao_seg_graduacao[info_servidor.ad_especializacao_seg_graduacao]);
    printf("ESPECIALIZAÇÕES: %s\n", opcoes_ad_especializacao_especializacao[info_servidor.ad_especializacao_especializacao]);


    printf("CARGO EFETIVO: %s\n", opcoes_cargo_efetivo[info_servidor.cargo_efetivo]);
    printf("PADRÃO DA CARREIRA: %s\n", opcoes_padrao_carreira[info_servidor.padrao_carreira]);
    printf("FUNÇÃO_COMISSIONADA: %s\n", opcoes_funcao_comissionada[info_servidor.funcao_comissionada]);
    
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
}