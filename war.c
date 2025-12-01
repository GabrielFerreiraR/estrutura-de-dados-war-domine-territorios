#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define NUM_TERRITORIOS 12
#define MAX_NOME 32
#define MISSao_DESTRUIR 0
#define MISSao_CONQUISTAR 1

typedef struct {
    char nome[MAX_NOME];
    int dono;     
    int tropas;
} Territorio;

typedef struct {
    int id;           
    int alvoCor;       
    int alvoQtd;       
} Missao;

Territorio *alocarMapa(size_t n);
void inicializarTerritorios(Territorio *mapa, size_t n);
void liberarMemoria(Territorio *mapa);

void exibirMenuPrincipal(void);
void exibirMapa(const Territorio *mapa, size_t n, const char *cores[]);
void exibirMissao(const Missao *m, const char *cores[]);

void faseDeAtaque(Territorio *mapa, size_t n, int jogadorCor, const char *cores[]);
void simularAtaque(Territorio *mapa, size_t n, int origemIdx, int destinoIdx);
Missao sortearMissao(int jogadorCor);
int verificarVitoria(const Territorio *mapa, size_t n, const Missao *m, int jogadorCor);

void limparBufferEntrada(void);
int lerInteiro(const char *prompt, int min, int max);


Territorio *alocarMapa(size_t n) {
    Territorio *mapa = (Territorio *) calloc(n, sizeof(Territorio));
    return mapa;
}

void inicializarTerritorios(Territorio *mapa, size_t n) {
    const char *nomes[NUM_TERRITORIOS] = {
        "Alaska", "Vancouver", "California", "Mexico",
        "Brasil", "Argentina", "AfricaNorte", "Egito",
        "EuropaOeste", "Russia", "China", "Australia"
    };
    int donos[NUM_TERRITORIOS] = {0,1,0,1,2,2,3,3,1,0,2,3};
    int tropasInit[NUM_TERRITORIOS] = {5,4,6,3,6,4,3,5,4,5,6,3};

    for (size_t i = 0; i < n; ++i) {
        strncpy(mapa[i].nome, nomes[i % NUM_TERRITORIOS], MAX_NOME-1);
        mapa[i].nome[MAX_NOME-1] = '\0';
        mapa[i].dono = donos[i % NUM_TERRITORIOS];
        mapa[i].tropas = tropasInit[i % NUM_TERRITORIOS];
    }
}

void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

void exibirMenuPrincipal(void) {
    printf("\n--- MENU ---\n");
    printf("1 - Fase de ataque\n");
    printf("2 - Verificar missão (e status de vitória)\n");
    printf("0 - Sair\n");
    printf("Escolha: ");
}

void exibirMapa(const Territorio *mapa, size_t n, const char *cores[]) {
    printf("\n--- MAPA ---\n");
    printf("%-3s %-18s %-10s %-6s\n", "ID", "Território", "Dono", "Tropas");
    printf("------------------------------------------------\n");
    for (size_t i = 0; i < n; ++i) {
        printf("%-3zu %-18s %-10s %-6d\n", i, mapa[i].nome, cores[mapa[i].dono], mapa[i].tropas);
    }
}

void exibirMissao(const Missao *m, const char *cores[]) {
    printf("\n--- MISSÃO SECRETA ---\n");
    if (m->id == MISSao_DESTRUIR) {
        printf("Destruir o exército da cor: %s\n", cores[m->alvoCor]);
    } else if (m->id == MISSao_CONQUISTAR) {
        printf("Conquistar %d territórios (além dos que já possui).\n", m->alvoQtd);
    } else {
        printf("Missão desconhecida.\n");
    }
}

void faseDeAtaque(Territorio *mapa, size_t n, int jogadorCor, const char *cores[]) {
    printf("\n--- FASE DE ATAQUE ---\n");
    exibirMapa(mapa, n, cores);
    printf("\nObservação: você só pode atacar de um território que seja seu e tenha >1 tropa.\n");
    int origem = lerInteiro("ID do território de origem (-1 para cancelar): ", -1, (int)n-1);
    if (origem == -1) {
        printf("Ataque cancelado.\n");
        return;
    }
    int destino = lerInteiro("ID do território de destino: ", 0, (int)n-1);

    if (origem < 0 || origem >= (int)n || destino < 0 || destino >= (int)n) {
        printf("IDs inválidos.\n");
        return;
    }
    if (origem == destino) {
        printf("Origem e destino iguais. Cancelando.\n");
        return;
    }
    if (mapa[origem].dono != jogadorCor) {
        printf("Território de origem não pertence ao jogador.\n");
        return;
    }
    if (mapa[origem].tropas <= 1) {
        printf("Território de origem não possui tropas suficientes (>1) para atacar.\n");
        return;
    }

    simularAtaque(mapa, n, origem, destino);
}

static int compararDesc(const void *a, const void *b) {
    return (*(int *)b) - (*(int *)a);
}

void simularAtaque(Territorio *mapa, size_t n, int origemIdx, int destinoIdx) {
    Territorio *origem = &mapa[origemIdx];
    Territorio *destino = &mapa[destinoIdx];

    int atacanteT = origem->tropas;
    int defensorT = destino->tropas;

    int maxDadosAtq = (atacanteT - 1 >= 3) ? 3 : (atacanteT - 1);
    if (maxDadosAtq < 1) {
        printf("Atacante não pode rolar dados (tropas insuficientes).\n");
        return;
    }
    int maxDadosDef = (defensorT >= 2) ? 2 : 1;

    int dadosAtq[3] = {0,0,0};
    int dadosDef[2] = {0,0};
    for (int i=0;i<maxDadosAtq;++i) dadosAtq[i] = (rand() % 6) + 1;
    for (int i=0;i<maxDadosDef;++i) dadosDef[i] = (rand() % 6) + 1;

    qsort(dadosAtq, maxDadosAtq, sizeof(int), compararDesc);
    qsort(dadosDef, maxDadosDef, sizeof(int), compararDesc);

    printf("\nResultado dos dados (Atacante %d dados): ", maxDadosAtq);
    for (int i=0;i<maxDadosAtq;++i) printf("%d ", dadosAtq[i]);
    printf("\n(Defensor %d dados): ", maxDadosDef);
    for (int i=0;i<maxDadosDef;++i) printf("%d ", dadosDef[i]);
    printf("\n");

    int pares = (maxDadosAtq < maxDadosDef) ? maxDadosAtq : maxDadosDef;
    int perdasAtq = 0, perdasDef = 0;
    for (int i=0;i<pares;++i) {
        if (dadosAtq[i] > dadosDef[i]) {
            perdasDef++;
        } else {
            perdasAtq++;
        }
    }

    origem->tropas -= perdasAtq;
    destino->tropas -= perdasDef;

    printf("Perdas: Atacante perdeu %d tropa(s). Defensor perdeu %d tropa(s).\n", perdasAtq, perdasDef);

    if (destino->tropas <= 0) {
        destino->tropas = 0;
        printf("Território %s conquistado!\n", destino->nome);
        destino->dono = origem->dono;
        if (origem->tropas > 1) {
            origem->tropas -= 1;
            destino->tropas += 1;
            printf("Uma tropa foi movida de %s para %s.\n", origem->nome, destino->nome);
        } else {
            destino->tropas += 1;
            printf("Conquista realizada com reposição mínima.\n");
        }
    }
}

Missao sortearMissao(int jogadorCor) {
    Missao m;
    int id = rand() % 2;
    m.id = id;
    if (id == MISSao_DESTRUIR) {
        int cor;
        do {
            cor = rand() % 4;
        } while (cor == jogadorCor); 
        m.alvoCor = cor;
        m.alvoQtd = 0;
    } else {
        int n = 4 + (rand() % 3); 
        m.alvoQtd = n;
        m.alvoCor = -1;
    }
    return m;
}

int verificarVitoria(const Territorio *mapa, size_t n, const Missao *m, int jogadorCor) {
    if (m->id == MISSao_DESTRUIR) {
        for (size_t i = 0; i < n; ++i) {
            if (mapa[i].dono == m->alvoCor) return 0; 
        }
        return 1; 
    } else if (m->id == MISSao_CONQUISTAR) {
        int cnt = 0;
        for (size_t i = 0; i < n; ++i) {
            if (mapa[i].dono == jogadorCor) cnt++;
        }
        if (cnt >= m->alvoQtd) return 1;
        return 0;
    }
    return 0;
}

void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int lerInteiro(const char *prompt, int min, int max) {
    int v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &v) != 1) {
            printf("Entrada inválida. Tente novamente.\n");
            limparBufferEntrada();
            continue;
        }
        limparBufferEntrada();
        if (v < min || v > max) {
            printf("Valor fora do intervalo [%d, %d]. Tente novamente.\n", min, max);
            continue;
        }
        return v;
    }
}

int main(void) {
    setlocale(LC_ALL, ""); 
    srand((unsigned int) time(NULL));

    const char *cores[] = {"Vermelho", "Azul", "Verde", "Amarelo"};

    Territorio *mapa = alocarMapa(NUM_TERRITORIOS);
    if (mapa == NULL) {
        fprintf(stderr, "Erro: falha ao alocar mapa.\n");
        return 1;
    }
    inicializarTerritorios(mapa, NUM_TERRITORIOS);

    printf("Bem-vindo ao WAR Estruturado!\n");
    printf("Escolha sua cor: 0-Vermelho 1-Azul 2-Verde 3-Amarelo\n");
    int jogadorCor = lerInteiro("Cor do jogador (0-3): ", 0, 3);

    Missao missao = sortearMissao(jogadorCor);
    printf("Missão sorteada. Boa sorte, jogador %s!\n", cores[jogadorCor]);

    int opcao;
    int venceu = 0;
    do {
        exibirMapa(mapa, NUM_TERRITORIOS, cores);
        exibirMissao(&missao, cores);
        exibirMenuPrincipal();
        if (scanf("%d", &opcao) != 1) {
            limparBufferEntrada();
            printf("Entrada inválida. Voltando ao menu.\n");
            continue;
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, NUM_TERRITORIOS, jogadorCor, cores);
                break;
            case 2:
                venceu = verificarVitoria(mapa, NUM_TERRITORIOS, &missao, jogadorCor);
                if (venceu) {
                    printf("\nPARABÉNS! Você cumpriu sua missão!\n");
                } else {
                    printf("\nAinda não cumpriu a missão. Continue jogando.\n");
                }
                break;
            case 0:
                printf("Saindo do jogo. Até a próxima!\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
        if (!venceu && opcao != 0) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
    } while (opcao != 0 && !venceu);

    liberarMemoria(mapa);

    return 0;
}
