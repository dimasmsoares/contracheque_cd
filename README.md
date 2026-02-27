# contracheque_cd

Calculadora do contracheque da Câmara dos Deputados para o cargo de Analista Legislativo.

## Como executar

```bash
gcc -Wall -Wextra -pedantic -std=c11 main.c -o contracheque
./contracheque
```

## Melhorias aplicadas neste ciclo

- Remoção de "flags" temporárias em campos de desconto para armazenar o regime previdenciário.
- Unificação do cálculo progressivo (IR, RGPS e RPPS) em uma função reutilizável.
- Proteção para evitar cálculo de imposto com base negativa.
- Relatório em arquivo alinhado ao relatório exibido no terminal (incluindo sessões noturnas quando houver).

## Melhorias sugeridas para próximos passos

1. **Separar camadas**: mover entrada de dados, regras de negócio e geração de relatório para arquivos distintos.
2. **Adicionar testes automatizados**: criar testes de regressão para cenários de RPPS, RGPS e FUNPRESP.
3. **Centralizar parâmetros legais**: versionar tabelas por competência (ano/mês) em arquivo externo.
4. **Suporte a dependentes e pensão**: incluir deduções de IR e descontos opcionais que hoje não são considerados.
5. **Internacionalização e formatação monetária**: padronizar saída monetária para locale `pt_BR`.
