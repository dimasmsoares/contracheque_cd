#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

static const float VENCIMENTO_BASICO[4][10] = { {5739.22, 6041.29, 6293.01, 6555.23, 7365.42, 7672.32, 7991.98, 8324.99, 8903.77, 9274.73}, // Consultor
                                                {5739.22, 6041.29, 6293.01, 6555.23, 7365.42, 7672.32, 7991.98, 8324.99, 8903.77, 9274.73}, // Analista 
                                                {3198.89, 3496.07, 3820.83, 4175.77, 4639.75, 5070.76, 5541.81, 6056.62, 6729.59, 7354.75}, // Técnico
                                                {974.32, 1094.75, 1230.05, 1382.09, 1570.58, 1764.69, 1982.77, 2227.83, 2531.64, 2844.53}}; // Auxiliar

static const float GRAT_ATV_LEG[4][10] =    {{7460.99, 7853.68, 8180.91, 8521.80, 9575.05, 9974.02, 10389.57, 10822.49, 11574.90, 12057.15},   // Consultor 
                                            {7460.99, 7853.68, 8180.91, 8521.80, 9575.05, 9974.02, 10389.57, 10822.49, 11574.90, 12057.15},    // Analista
                                            {4158.56, 4544.89, 4967.08, 5428.50, 6031.68, 6591.99, 7204.35, 7873.61, 8748.47, 9561.18},        // Técnico
                                            {1266.62, 1423.18, 1599.07, 1796.72, 2041.75, 2294.10, 2577.60, 2896.18, 3291.13, 3697.89}};       // Auxiliar

static const float GRAT_REPRESENTACAO[4] = {16202.70, 16202.70, 12199.66, 12199.66}; // {Consultor, Analista, Técnico, Auxiliar}

static const float VPI = 59.87;
static const float VLR_AUXILIO_ALIMENTAÇAO = 1784.42;
static const float VLR_SESSAO_NOTURNA = 485.75;

typedef struct {
    int cargo_efetivo;
    int padrao_carreira;
    int funcao_comissionada;
    int substituicao_fc;
    int substituicao_fc_dias;
    int ad_ferias;
    int grat_natalina;
    int aux_natalidade;
    int aux_pre_escolar;
    int ad_especializacao_doutorado_mestrado;
    int ad_especializacao_graduacao;
    int ad_especializacao_especializacao;
    int ad_sessao_noturna;

    int regime_previdenciario;
    int aliquota_funpresp;
    
} InfoServidor;

typedef struct {
    float remuneracao_bruta;
    float remuneracao_liquida;
    float salario_participacao;
    float vencimento_basico;
    float grat_representacao;
    float grat_atv_leg;
    float vpi;
    float aux_alimentacao;
    float sessao_noturna;
    
    float base_ad_especializacao;
    float aliquota_ad_especializacao;
    float ad_especializacao;

    float base_ad_ferias;
    float ad_ferias;

    float contribuicao_rpps;
    float contribuicao_rgps;
    float contribuicao_funpresp;
    float imposto_de_renda;
} Contracheque;

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

const char *opcoes_ad_especializacao_graduacao[] = {"Não possuo graduação", "Possuo uma graduação", "Possuo duas ou mais graduações"};
int n_opcoes_ad_especializacao_graduacao = 3;
const char *desc_ad_especializacao_graduacao = "Possui graduação?\n";

const char *opcoes_ad_especializacao_especializacao[] = {"Não possuo especialização", "Possuo uma especialização",
                                                        "Possuo duas ou mais especializações"};
int n_opcoes_ad_especializacao_especializacao = 3;
const char *desc_ad_especializacao_especializacao = "Possui especializações (pós-graduações)?\n";

const char *opcoes_ad_ferias[] = {"Não", "Sim, irei receber adicional de férias"};
int n_opcoes_ad_ferias = 2;
const char *desc_ad_ferias = "Vai receber adicional de férias?\n";

const char *opcoes_aux_natalidade[] = {"Não", "1 filho(a)", "2 filhos(as)", "3 filhos(as)", "4 filhos(as)", "5 filhos(as)"};
int n_opcoes_aux_natalidade = 6;
const char *desc_aux_natalidade = "Tem direito a receber auxílio natalidade este mês? Caso positivo, indique a quantidade de filhos recém-nascidos / adotados:\n";

const char *opcoes_aux_pre_escolar[] = {"Não", "1 filho(a)", "2 filhos(as)", "3 filhos(as)", "4 filhos(as)", "5 filhos(as)"};
int n_opcoes_aux_pre_escolar = 6;
const char *desc_aux_pre_escolar = "Tem direito a receber auxílio pré-escolar? Caso positivo, indique a quantidade de filhos entre 0 (zero) e 6 (anos):\n";

const char *opcoes_grat_natalina[] = {"Não", "Sim"};
int n_opcoes_grat_natalina = 2;
const char *desc_grat_natalina = "Vai receber parcela da gratificação natalina?\nOBS: O adiantamento da primeira parcela da gratificação natalina para servidores efetivos, aposentados e pensionistas é recebido nos meses de janeiro.\n";

const char *opcoes_ad_sessao_noturna[] = {"Não", "1 sessão (2h)", "2 sessões (4h)", "3 sessões (6h)", "4 sessões (8h)", "5 sessões (10h)",
                                            "6 sessões (12h)",  "7 sessões (14h)",  "8 sessões (16h)",  "9 sessões (18h)",  "10 sessões (20h)"};
int n_opcoes_ad_sessao_noturna = 11;
const char *desc_ad_sessao_noturna = "Tem direito a receber adicional de sessões noturna? Caso positivo, indique a quantidade:\n";

const char *opcoes_regime_previdenciario[] = {"RPPS", "RGPS - Sem FUNPRESP", "RGPS - Com FUNPRESP"};
int n_opcoes_regime_previdenciario = 3;
const char *desc_regime_previdenciario = "A qual regime previdenciário você está sujeito?\n";

const char *opcoes_aliquota_funpresp[] = {"7,5%", "8,0%", "8,5%"};
int n_opcoes_aliquota_funpresp = 3;
const char *desc_aliquota_funpresp = "Qual alíquota sobre o salário participação da FUNPRESP você escolheu?\n";

// PROTÓTIPOS
int tecla_pressionada();
int menu(const char *titulo, const char *opcoes[], int n_opçoes, const char *descricao);
void printInfoServidor(InfoServidor info);
Contracheque *calcular_contracheque(InfoServidor info, Contracheque *cc);

int main(int argc, char *argv[]){

    InfoServidor info_servidor = {0};
    Contracheque contracheque = {0};

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

    // Adcional de férias
    info_servidor.ad_ferias = menu("ADCIONAL DE FÉRIAS", opcoes_ad_ferias, n_opcoes_ad_ferias, desc_ad_ferias);
    system("clear");

    // Gratificação Natalina
    info_servidor.grat_natalina = menu("PARCELA DA GRATIFICAÇÃO NATALINA", opcoes_grat_natalina, n_opcoes_grat_natalina, desc_grat_natalina);
    system("clear");

    // Auxílio natalidade
    info_servidor.aux_natalidade = menu("AUXÍLIO NATALIDADE", opcoes_aux_natalidade, n_opcoes_aux_natalidade, desc_aux_natalidade);
    system("clear");

    // Auxílio Pré-Escolar
    info_servidor.aux_pre_escolar = menu("AUXÍLIO PRÉ-ESCOLAR", opcoes_aux_pre_escolar, n_opcoes_aux_pre_escolar, desc_aux_pre_escolar);
    system("clear");

    // Adicional de especialização
    info_servidor.ad_especializacao_doutorado_mestrado = menu("ADCIONAL DE ESPECIALIZAÇÃO - MESTRADO E DOUTORADO", opcoes_ad_especializacao_doutorado_mestrado, n_opcoes_ad_especializacao_doutorado_mestrado, desc_ad_especializacao_doutorado_mestrado);
    system("clear");

    info_servidor.ad_especializacao_graduacao = menu("ADCIONAL DE ESPECIALIZAÇÃO - GRADUAÇÃO", opcoes_ad_especializacao_graduacao, n_opcoes_ad_especializacao_graduacao, desc_ad_especializacao_graduacao);
    system("clear");

    info_servidor.ad_especializacao_especializacao = menu("ADCIONAL DE ESPECIALIZAÇÃO - ESPECIALIZAÇÕES", opcoes_ad_especializacao_especializacao, n_opcoes_ad_especializacao_especializacao, desc_ad_especializacao_especializacao);
    system("clear");

    // Sessões Noturnas
    info_servidor.ad_sessao_noturna = menu("SESSÕES NOTURNAS", opcoes_ad_sessao_noturna, n_opcoes_ad_sessao_noturna, desc_ad_sessao_noturna);
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

    //printInfoServidor(info_servidor);

    calcular_contracheque(info_servidor, &contracheque);
    
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

void printInfoServidor(InfoServidor info){
    printf("CARGO EFETIVO: %s\n", opcoes_cargo_efetivo[info.cargo_efetivo]);
    printf("PADRÃO DA CARREIRA: %s\n", opcoes_padrao_carreira[info.padrao_carreira]);
    printf("FUNÇÃO_COMISSIONADA: %s\n", opcoes_funcao_comissionada[info.funcao_comissionada]);
    printf("SUBSTITUIÇÃO DE FC: %s\n", opcoes_substituicao_fc[info.substituicao_fc]);
    printf("DIAS NA SUBSTITUIÇÃO DE FC: %d\n", info.substituicao_fc_dias);
    printf("ADICIONAL DE FÉRIAS: %s\n", opcoes_ad_ferias[info.ad_ferias]);
    printf("PARCELA DA GRATIFICAÇÃO NATALINA: %s\n", opcoes_grat_natalina[info.grat_natalina]);
    printf("AUXÍLIO NATALIDADE: %s\n", opcoes_aux_natalidade[info.aux_natalidade]);
    printf("AUXÍLIO PRÉ-ESCOLAR: %s\n", opcoes_aux_pre_escolar[info.aux_pre_escolar]);
    printf("MESTRADO E DOUTORADO: %s\n", opcoes_ad_especializacao_doutorado_mestrado[info.ad_especializacao_doutorado_mestrado]);
    printf("GRADUAÇÃO: %s\n", opcoes_ad_especializacao_graduacao[info.ad_especializacao_graduacao]);
    printf("ESPECIALIZAÇÕES: %s\n", opcoes_ad_especializacao_especializacao[info.ad_especializacao_especializacao]);
    printf("ADICIONAL DE SESSÕES NOTURNAS: %s\n", opcoes_ad_sessao_noturna[info.ad_sessao_noturna]);
    printf("REGIME PREVIDENCIÁRIO: %s\n", opcoes_regime_previdenciario[info.regime_previdenciario]);
    printf("ALÍQUOTA FUNPRESP: %s\n", opcoes_aliquota_funpresp[info.aliquota_funpresp]);
}

Contracheque *calcular_contracheque(InfoServidor info, Contracheque *cc){
    cc->vencimento_basico = VENCIMENTO_BASICO[info.cargo_efetivo][info.padrao_carreira];
    cc->grat_atv_leg = GRAT_ATV_LEG[info.cargo_efetivo][info.padrao_carreira];
    cc->grat_representacao = GRAT_REPRESENTACAO[info.cargo_efetivo];
    cc->vpi = VPI;
    cc->aux_alimentacao = VLR_AUXILIO_ALIMENTAÇAO;

    cc->base_ad_especializacao = VENCIMENTO_BASICO[info.cargo_efetivo][9];
    if(info.ad_especializacao_graduacao == 1){                  // 1ª graduação
        cc->aliquota_ad_especializacao += 15;
    }
    else if(info.ad_especializacao_graduacao == 2){             // 2ª graduação
        cc->aliquota_ad_especializacao += 21;
    }

    if(info.ad_especializacao_especializacao == 1){             // 1ª especialização lato sensu
        cc->aliquota_ad_especializacao += 6;
    }
    else if(info.ad_especializacao_especializacao == 2){        // 1ª especialização lato sensu
        cc->aliquota_ad_especializacao += 9;
    }

    if(info.ad_especializacao_doutorado_mestrado == 1){    // Apenas mestrado
        cc->aliquota_ad_especializacao += 8;
    }
    else if(info.ad_especializacao_doutorado_mestrado == 2){    // Apenas doutorado
        cc->aliquota_ad_especializacao += 10;
    }
    else if(info.ad_especializacao_doutorado_mestrado == 3){    // Mestrado e doutorado
        cc->aliquota_ad_especializacao += 18;
    }

    if(cc->aliquota_ad_especializacao > 30){
        cc->aliquota_ad_especializacao = 30;
    }
    cc->ad_especializacao = cc->base_ad_especializacao * cc->aliquota_ad_especializacao /100;

    if(info.ad_ferias == 1){
        cc->base_ad_ferias = cc->vencimento_basico  + 
                            cc->grat_atv_leg        + 
                            cc->grat_representacao  + 
                            cc->vpi                 +
                            cc->ad_especializacao;
        cc->ad_ferias = cc->base_ad_ferias/3;
    }
    
    if(info.ad_sessao_noturna >0){
        cc->sessao_noturna = VLR_SESSAO_NOTURNA * info.ad_sessao_noturna;
    }
    cc->remuneracao_bruta = cc->vencimento_basico   + 
                            cc->grat_atv_leg        + 
                            cc->grat_representacao  + 
                            cc->vpi                 + 
                            cc->aux_alimentacao     +
                            cc->sessao_noturna      +
                            cc->ad_ferias           +
                            cc->ad_especializacao;
    

    printf("(+) Vencimento básico ................... R$ %10.2f\n", cc->vencimento_basico);
    printf("(+) Gratificação de Representação ....... R$ %10.2f\n", cc->grat_representacao);
    printf("(+) Grat. Atividade Legislativa ......... R$ %10.2f\n", cc->grat_atv_leg);
    printf("(+) VPI ................................. R$ %10.2f\n", cc->vpi);
    printf("(+) Sessões Noturnas .................... R$ %10.2f\n", cc->sessao_noturna);
    printf("(+) Adicional de férias ................. R$ %10.2f\n", cc->ad_ferias);
    printf("(+) Auxílio alimentação ................. R$ %10.2f\n", cc->aux_alimentacao);
    printf("(+) Auxílio especialização .............. R$ %10.2f\n", cc->ad_especializacao);
    printf("(=) REMUNERAÇÃO BRUTA ................... R$ %10.2f\n", cc->remuneracao_bruta);




    return NULL;
}



