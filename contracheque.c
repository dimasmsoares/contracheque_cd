#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#define MAX_DEPENDENTES 10

/* ================= TECLADO MULTIPLATAFORMA ================= */

#ifdef _WIN32
int getch_cross() {
    return _getch();
}
#else
int getch_cross() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

/* ================= MENU INTERATIVO ================= */

int menu(const char *titulo, const char *opcoes[], int total) {
    int selecionado = 0;

    while (1) {
        system("clear||cls");
        printf("==== %s ====\n\n", titulo);

        for (int i = 0; i < total; i++) {
            if (i == selecionado)
                printf(" 👉 %s\n", opcoes[i]);
            else
                printf("    %s\n", opcoes[i]);
        }

        printf("\nUse ↑ ↓ e ENTER\n");

        int ch = getch_cross();

#ifdef _WIN32
        if (ch == 224) ch = getch_cross();
#else
        if (ch == 27) {
            getch_cross();
            ch = getch_cross();
        }
#endif

        if (ch == 72 || ch == 65) { // cima
            if (selecionado > 0) selecionado--;
        }
        else if (ch == 80 || ch == 66) { // baixo
            if (selecionado < total - 1) selecionado++;
        }
        else if (ch == 13 || ch == 10) { // enter
            return selecionado;
        }
    }
}

/* ================= ESTRUTURAS ================= */

typedef struct {
    int grau;
    int idade;
} Dependente;

typedef struct {
    int padrao;
    int idade;
    int escolaridade[5];
    int prosaude;
    int qtd_dependentes;
    Dependente dependentes[MAX_DEPENDENTES];
    int regime; // 0 RPPS | 1 RGPS
    int funpresp;
    float aliquota_funpresp;
    int fc;
    int substituto;
    int fc_substituida;
    int dias_substituicao;
    int aux_pre_escolar;
} Servidor;

/* ================= FUNÇÕES AUXILIARES ================= */

int ler_int(const char *msg) {
    int v;
    printf("%s", msg);
    scanf("%d", &v);
    return v;
}

float ler_float(const char *msg) {
    float v;
    printf("%s", msg);
    scanf("%f", &v);
    return v;
}

/* ================= PROGRAMA PRINCIPAL ================= */

int main() {
    Servidor s = {0};

    const char *padroes[] = {
        "Padrao 1","Padrao 2","Padrao 3","Padrao 4","Padrao 5",
        "Padrao 6","Padrao 7","Padrao 8","Padrao 9","Padrao 10"
    };
    s.padrao = menu("Selecione seu padrao na carreira", padroes, 10) + 1;

    system("clear||cls");
    s.idade = ler_int("Informe sua idade: ");

    const char *escolaridade[] = {
        "Possui mestrado",
        "Possui doutorado",
        "Possui 1 especializacao",
        "Possui 2 especializacoes",
        "Possui segunda graduacao"
    };

    for (int i = 0; i < 5; i++) {
        const char *op[] = {"Nao", "Sim"};
        int r = menu(escolaridade[i], op, 2);
        s.escolaridade[i] = r;
    }

    const char *op_sim_nao[] = {"Nao", "Sim"};
    s.prosaude = menu("Aderiu ao ProSaude?", op_sim_nao, 2);

    if (s.prosaude) {
        s.qtd_dependentes = ler_int("Quantidade de dependentes no ProSaude: ");

        const char *graus[] = {
            "Conjuge",
            "Companheiro(a)",
            "Filho/enteado menor 21",
            "Filho invalido maior 21",
            "Filho estudante ate 25",
            "Irmao invalido",
            "Menor sob guarda",
            "Filho 21 a 33",
            "Pai/mae dependentes",
            "Pai/mae nao dependentes"
        };

        for (int i = 0; i < s.qtd_dependentes; i++) {
            s.dependentes[i].grau = menu("Selecione o grau do dependente", graus, 10) + 1;
            s.dependentes[i].idade = ler_int("Idade do dependente: ");
        }
    }

    const char *regimes[] = {"RPPS", "RGPS"};
    s.regime = menu("Selecione o regime previdenciario", regimes, 2);

    if (s.regime == 1) {
        s.funpresp = menu("Aderiu ao Funpresp?", op_sim_nao, 2);

        if (s.funpresp) {
            const char *aliquotas[] = {"7.5%", "8%", "8.5%"};
            int a = menu("Selecione a aliquota", aliquotas, 3);
            s.aliquota_funpresp = (a == 0 ? 7.5 : (a == 1 ? 8.0 : 8.5));
        }
    }

    const char *fcs[] = {"Nenhuma", "FC1", "FC2", "FC3", "FC4", "FC5", "FC6"};
    s.fc = menu("Possui funcao comissionada?", fcs, 7);

    s.substituto = menu("Foi substituto de alguma FC neste mes?", op_sim_nao, 2);

    if (s.substituto) {
        s.fc_substituida = menu("Qual FC substituiu?", fcs, 7);
        s.dias_substituicao = ler_int("Quantos dias substituiu? ");
    }

    s.aux_pre_escolar = menu("Recebe auxilio pre-escolar?", op_sim_nao, 2);

    system("clear||cls");
    printf("✅ Dados coletados com sucesso!\n");
    printf("Programa de entrada finalizado.\n");

    return 0;
}
