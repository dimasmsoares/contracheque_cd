# contracheque_cd

Calculadora do contracheque da Câmara dos Deputados para o cargo de Analista Legislativo.

## Estrutura do projeto

O código foi separado por camadas para facilitar manutenção:

- `entrada.c` / `entrada.h`: leitura e validação de dados de entrada.
- `negocio.c` / `negocio.h`: regras de cálculo de rendimentos e descontos.
- `relatorio.c` / `relatorio.h`: geração de saída em tela e em `contracheque.txt`.
- `servidor.h`: estrutura de dados compartilhada entre os módulos.
- `constantes.h`: tabelas e constantes legais usadas nos cálculos.
- `main.c`: orquestra o fluxo da aplicação.

## Como compilar e executar

```bash
gcc -Wall -Wextra -pedantic -std=c11 main.c entrada.c negocio.c relatorio.c -o contracheque
./contracheque
```
