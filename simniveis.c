//-----------------------------------------------------------------------------
// Simulador de cache multinível: DOIS NÍVEIS DE CACHE, split no nível 1 (I1 e D1) e unificada no nível 2 (L2)
//
// Para compilar: gcc cache.c simniveis.c -o simniveis -Wall
// Para executar: simniveis arquivoConfiguracao arquivoAcessos
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h" // Módulo que implementa cache genérica

//-----------------------------------------------------------------------------
// Variaveis

              // Medidas de desempenho
int nAcessosI;     // Número total de acessos a instruções
int nAcessosD;     // Número total de acessos a dados
int nFalhasI1;     // Número de falhas na cache I1
int nFalhasD1;     // Número de falhas na cache D1
int nFalhasL2;     // Número de falhas na cache L2

tCache cacheI1,    // Cache de instruções nível 1
    cacheD1,    // Cache de dados nível 1
    cacheL2;    // Cache unificada nível 2

FILE *arqAcessos;  // Arquivo de trace com acessos à memória

//-----------------------------------------------------------------------------
// Protótipos de funções

void inicializa(int argc, char **);
void finaliza(void);

//-----------------------------------------------------------------------------
// Programa principal

int main(int argc, char **argv)
{
  char tipoAcesso;  // 'I' (leitura de instrução), 'L' (LOAD = leitura de dado) ou 'S' (STORE = escrita de dado)
  int result,
      resultAcessoI1, 
      resultAcessoD1,
      resultAcessoL2,
      endereco;     // Endereço de memória acessado

  inicializa(argc, argv);

  // Enquanto não termina arquivo de acessos
  while (!feof(arqAcessos))
  {
    // Lê um endereço do arquivo de acessos
    result = fscanf(arqAcessos, " %c %d", &tipoAcesso, &endereco);

    if ((result != 0) && (result != EOF))
    {
      if (tipoAcesso == 'I') { // Acesso é leitura de uma instrução
        nAcessosI++;
        // Procura bloco na cache e, se não encontra, insere bloco na cache
        // Valor de retorno = 0 (acerto), 1 (falha sem substituição) ou 2 (falha com substituição)
        resultAcessoI1 = buscaInsereCache(&cacheI1, endereco);
        if ((resultAcessoI1 == 1) || (resultAcessoI1 == 2)) { // Se houve falha na cache
          nFalhasI1++;
          resultAcessoL2 = buscaInsereCache(&cacheL2, endereco); // Procura na L2
          if ((resultAcessoL2 == 1) || (resultAcessoL2 == 2)) // Se houve falha na L2
            nFalhasL2++;
        }
      }
      else if (tipoAcesso == 'L' || tipoAcesso == 'S')  // Acesso é leitura de dado ('L') ou escrita de dado ('S')
      {
        nAcessosD++;
        // Procura bloco na cache e, se não encontra, insere bloco na cache
        // Valor de retorno = 0 (acerto), 1 (falha sem substituição) ou 2 (falha com substituição)
        resultAcessoD1 = buscaInsereCache(&cacheD1, endereco);
        if ((resultAcessoD1 == 1) || (resultAcessoD1 == 2)) // Se houve falha na cache D1
        {
          nFalhasD1++;
          resultAcessoL2 = buscaInsereCache(&cacheL2, endereco); // Procura na L2
          if ((resultAcessoL2 == 1) || (resultAcessoL2 == 2)) // Se houve falha na L2
            nFalhasL2++;
        }
      }
    }
  }

  finaliza();
  return 0;
}

//-----------------------------------------------------------------------------
void inicializa(int argc, char **argv)
{
  char nomeArqConfig[100], 
       nomeArqAcessos[100];

  unsigned int nBlocosI1,           // Número total de blocos da cache I1
               associatividadeI1,   // Número de blocos por conjunto da cache I1
               nPalavrasBlocoI1;    // Número de palavras do bloco da cache I1

  unsigned int nBlocosD1,           // Número total de blocos da cache D1
               associatividadeD1,   // Número de blocos por conjunto da cache D1
               nPalavrasBlocoD1;    // Número de palavras do bloco da cache D1

  unsigned int nBlocosL2,           // Número total de blocos da cache L2
               associatividadeL2,   // Número de blocos por conjunto da cache L2
               nPalavrasBlocoL2;    // Número de palavras do bloco da cache L2

  FILE *arqConfig;  // Arquivo com configuração da cache

  if (argc != 3)
  {
    printf("\nUso: simniveis arquivoConfiguracao arquivoAcessos\n");
    exit(0);
  }

  // Lê arquivo de configuracao e obtem nBlocos, associatividade e nPalavrasBloco das caches
  strcpy(nomeArqConfig, argv[1]);
  arqConfig = fopen(nomeArqConfig, "rt");
  if (arqConfig == NULL)
  {
    printf("\nArquivo de configuração não encontrado\n");
    exit(0);
  }

  fscanf(arqConfig, "%d %d %d", &nBlocosI1, &associatividadeI1, &nPalavrasBlocoI1);
  fscanf(arqConfig, "%d %d %d", &nBlocosD1, &associatividadeD1, &nPalavrasBlocoD1);
  fscanf(arqConfig, "%d %d %d", &nBlocosL2, &associatividadeL2, &nPalavrasBlocoL2);
  fclose(arqConfig);

  // Abre arquivo de acessos
  strcpy(nomeArqAcessos, argv[2]);
  arqAcessos = fopen(nomeArqAcessos, "rt");
  if (arqAcessos == NULL)
  {
    printf("\nArquivo de acessos não encontrado\n");
    exit(0);
  }

  // Inicializa medidas de desempenho
  nAcessosI = 0;
  nAcessosD = 0;
  nFalhasI1 = 0;
  nFalhasD1 = 0;
  nFalhasL2 = 0;

  
  // Aloca e inicializa estrutura de dados da cache T1
  alocaCache(&cacheI1, nBlocosI1, associatividadeI1, nPalavrasBlocoI1);
  // Aloca e inicializa estrutura de dados da cache D1
  alocaCache(&cacheD1, nBlocosD1, associatividadeD1, nPalavrasBlocoD1);
  // Aloca e inicializa estrutura de dados da cache L2
  alocaCache(&cacheL2, nBlocosL2, associatividadeL2, nPalavrasBlocoL2);
}

//-----------------------------------------------------------------------------
// Função de finalização

void finaliza(void)
{
  // Fecha arquivo de acessos
  fclose(arqAcessos);

  // Imprime medidas de desempenho
  printf("nAcessosI: %d\n", nAcessosI);
  printf("nAcessosD: %d\n", nAcessosD);
  printf("nFalhasI1: %d\n", nFalhasI1);
  printf("nFalhasD1: %d\n", nFalhasD1);
  printf("nFalhasL2: %d\n", nFalhasL2);

  // Libera estrutura de dados da cache
  liberaCache(&cacheI1);
  liberaCache(&cacheD1);
  liberaCache(&cacheL2);
}
