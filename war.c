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
    return (Territorio *) calloc(n, sizeof(Territorio));
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
        strncpy(mapa[i].nome, nomes[i], MAX_NOME-1);
        mapa[i].nome[MAX_NOME-1] = '\0';
        mapa[i].dono = donos[i];
        mapa[i].tropas = tropasInit[i];
    }
}

void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

void exibirMenuPrincipal(void) {
    printf("\n--- MENU ---\n");
    printf("1 - Fase de ataque\n");
    printf("2 - Verificar missão\n");
    printf("0 - Sair\n");
    printf("Escolha: ");
}

void exibirMapa(const Territorio *mapa, size_t n, const char *cores[]) {
    printf("\n--- MAPA ---\n");
    printf("%-3s %-18s %-10s %-6s\n", "ID", "Território", "Dono", "Tropas");
    for (size_t i = 0; i < n; ++i) {
        printf("%-3zu %-18s %-10s %-6d\n",
               i, mapa[i].nome, cores[mapa[i].dono], mapa[i].tropas);
    }
}

void exibirMissao(const Missao *m, const char *cores[]) {
    printf("\n--- MISSÃO ---\n");
    if (m->id == MISSao_DESTRUIR)
        printf("Destruir o exército da cor: %s\n", cores[m->alvoCor]);
    else
        printf("Conquistar %d territórios.\n", m->alvoQtd);
}

int lerInteiro(const char *prompt, int min, int max) {
    int v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &v) != 1) {
            limparBufferEntrada();
            continue;
        }
        limparBufferEntrada();
        if (v >= min && v <= max) return v;
    }
}

void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void faseDeAtaque(Territorio *mapa, size_t n, int jogadorCor, const char *cores[]) {
    int origem = lerInteiro("Origem (-1 cancela): ", -1, n-1);
    if (origem == -1) return;

    int destino = lerInteiro("Destino: ", 0, n-1);

    if (mapa[origem].dono != jogadorCor) {
        printf("Origem não pertence ao jogador.\n");
        return;
    }
    if (mapa[origem].tropas <= 1) {
        printf("Tropas insuficientes.\n");
        return;
    }

    simularAtaque(mapa, n, origem, destino);
}

static int compararDesc(const void *a, const void *b) {
    return (*(int *)b) - (*(int *)a);
}

void simularAtaque(Territorio *mapa, size_t n, int origemIdx, int destinoIdx) {
    Territorio *o = &mapa[origemIdx];
    Territorio *d = &mapa[destinoIdx];

    int maxAtq = (o->tropas - 1 >= 3) ? 3 : (o->tropas - 1);
    int maxDef = (d->tropas >= 2) ? 2 : 1;

    int atq[3] = {0}, def[2] = {0};

    for (int i=0; i<maxAtq; i++) atq[i] = rand()%6 +1;
    for (int i=0; i<maxDef; i++) def[i] = rand()%6 +1;

    qsort(atq, maxAtq, sizeof(int), compararDesc);
    qsort(def, maxDef, sizeof(int), compararDesc);

    int pares = (maxAtq < maxDef ? maxAtq : maxDef);
    int perdasA=0, perdasD=0;

    for (int i=0;i<pares;i++) {
        if (atq[i] > def[i]) perdasD++;
        else perdasA++;
    }

    o->tropas -= perdasA;
    d->tropas -= perdasD;

    if (d->tropas <= 0) {
        d->dono = o->dono;
        d->tropas = 1;
        o->tropas--;
    }
}

Missao sortearMissao(int jogadorCor) {
    Missao m;
    m.id = rand() % 2;
    if (m.id == MISSao_DESTRUIR) {
        do { m.alvoCor = rand()%4; } while (m.alvoCor == jogadorCor);
    } else {
        m.alvoQtd = 4 + (rand()%3);
    }
    return m;
}

int verificarVitoria(const Territorio *mapa, size_t n, const Missao *m, int jogadorCor) {
    if (m->id == MISSao_DESTRUIR) {
        for (size_t i=0;i<n;i++)
            if (mapa[i].dono == m->alvoCor) return 0;
        return 1;
    } else {
        int cnt=0;
        for (size_t i=0;i<n;i++)
            if (mapa[i].dono == jogadorCor) cnt++;
        return cnt >= m->alvoQtd;
    }
}

int main(void) {
    setlocale(LC_ALL, "");
    srand(time(NULL));

    const char *cores[] = {"Vermelho", "Azul", "Verde", "Amarelo"};

    Territorio *mapa = alocarMapa(NUM_TERRITORIOS);
    inicializarTerritorios(mapa, NUM_TERRITORIOS);

    printf("Escolha sua cor (0-3): ");
    int jogadorCor = lerInteiro("", 0, 3);

    Missao missao = sortearMissao(jogadorCor);

    int op, venceu = 0;

    do {
        exibirMapa(mapa, NUM_TERRITORIOS, cores);
        exibirMissao(&missao, cores);
        exibirMenuPrincipal();

        scanf("%d", &op);
        limparBufferEntrada();

        if (op == 1) faseDeAtaque(mapa, NUM_TERRITORIOS, jogadorCor, cores);
        else if (op == 2) {
            venceu = verificarVitoria(mapa, NUM_TERRITORIOS, &missao, jogadorCor);
            if (venceu) printf("Você venceu!\n");
            else printf("Missão ainda não concluída.\n");
        }

        if (!venceu && op != 0) {
            printf("Enter...\n");
            getchar();
        }

    } while (op != 0 && !venceu);

    liberarMemoria(mapa);
    return 0;
}
