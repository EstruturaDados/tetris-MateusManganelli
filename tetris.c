#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Constantes ---
#define TAM_FILA 5
#define TAM_PILHA 3
#define TIPOS_PECAS "IOTLJSZ"

// --- Estruturas ---

typedef struct {
    int id;
    char nome;
} Peca;

// Estrutura da FILA (Circular)
typedef struct {
    Peca itens[TAM_FILA];
    int inicio, fim, quantidade;
} FilaCircular;

// Estrutura da PILHA (Linear - Reserva)
typedef struct {
    Peca itens[TAM_PILHA];
    int topo; // Índice do elemento no topo (-1 se vazia)
} PilhaReserva;

// --- Funções Auxiliares ---

Peca gerarPeca() {
    static int contadorId = 1;
    Peca p;
    p.id = contadorId++;
    p.nome = TIPOS_PECAS[rand() % 7];
    return p;
}

// --- Gerenciamento da FILA ---

void initFila(FilaCircular *f) {
    f->inicio = 0;
    f->fim = 0;
    f->quantidade = 0;
}

void enfileirar(FilaCircular *f, Peca p) {
    if (f->quantidade == TAM_FILA) return; // Fila cheia (segurança)
    f->itens[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->quantidade++;
}

Peca desenfileirar(FilaCircular *f) {
    Peca p = {0, ' '};
    if (f->quantidade == 0) return p;
    p = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->quantidade--;
    return p;
}

// --- Gerenciamento da PILHA (NOVIDADE) ---

void initPilha(PilhaReserva *p) {
    p->topo = -1; // Indica pilha vazia
}

// Push: Adicionar no topo
int push(PilhaReserva *p, Peca peca) {
    if (p->topo == TAM_PILHA - 1) {
        return 0; // Erro: Pilha cheia (Overflow)
    }
    p->topo++;
    p->itens[p->topo] = peca;
    return 1; // Sucesso
}

// Pop: Remover do topo
Peca pop(PilhaReserva *p) {
    Peca vazia = {0, ' '};
    if (p->topo == -1) {
        return vazia; // Erro: Pilha vazia (Underflow)
    }
    Peca pecaRemovida = p->itens[p->topo];
    p->topo--;
    return pecaRemovida;
}

// --- Visualização ---

void visualizarEstado(FilaCircular f, PilhaReserva p) {
    printf("\n========================================\n");
    
    // Mostra a PILHA
    printf("📦 RESERVA (Pilha - Max 3): ");
    if (p.topo == -1) {
        printf("[ Vazia ]");
    } else {
        // Percorre do topo até a base
        printf("[ Topo: ");
        for (int i = p.topo; i >= 0; i--) {
            printf("(%c) ", p.itens[i].nome);
        }
        printf("Base ]");
    }
    
    // Mostra a FILA
    printf("\n🚀 PRÓXIMAS (Fila):      [ SAÍDA ] <--- ");
    for (int i = 0; i < f.quantidade; i++) {
        int idx = (f.inicio + i) % TAM_FILA;
        printf("%c ", f.itens[idx].nome);
    }
    printf("<--- [ ENTRADA ]\n");
    printf("========================================\n");
}

// --- Main ---

int main() {
    FilaCircular fila;
    PilhaReserva pilha;
    int opcao;
    
    srand(time(NULL));
    initFila(&fila);
    initPilha(&pilha);
    
    // Enche a fila inicial
    for(int i=0; i<TAM_FILA; i++) enfileirar(&fila, gerarPeca());

    do {
        visualizarEstado(fila, pilha);
        
        printf("1. JOGAR peça da fila (Dequeue)\n");
        printf("2. RESERVAR peça da fila (Push na Pilha)\n");
        printf("3. USAR peça da reserva (Pop da Pilha)\n");
        printf("0. Sair\n");
        printf("👉 Escolha: ");
        scanf("%d", &opcao);
        
        printf("\n----------------------------------------\n");

        if (opcao == 1) {
            // JOGAR: Remove da fila e repõe
            Peca p = desenfileirar(&fila);
            printf("🎮 Você jogou a peça: [%c] (ID: %d)\n", p.nome, p.id);
            enfileirar(&fila, gerarPeca()); // Reposição automática
        }
        else if (opcao == 2) {
            // RESERVAR: Tira da fila -> Coloca na Pilha
            // Primeiro verificamos se a pilha aguenta
            if (pilha.topo < TAM_PILHA - 1) {
                Peca p = desenfileirar(&fila);
                push(&pilha, p);
                printf("💾 Peça [%c] guardada na reserva!\n", p.nome);
                enfileirar(&fila, gerarPeca()); // Reposição automática na fila
            } else {
                printf("⚠️  ALERTA: A Reserva está cheia! Jogue ou use uma peça.\n");
            }
        }
        else if (opcao == 3) {
            // USAR RESERVA: Tira da Pilha e Joga
            if (pilha.topo >= 0) {
                Peca p = pop(&pilha);
                printf("♻️  Reserva recuperada! Jogando peça: [%c] (ID: %d)\n", p.nome, p.id);
                // Nota: Aqui NÃO enfileiramos nada, pois a peça veio da pilha, não da fila.
            } else {
                printf("⚠️  ALERTA: A Reserva está vazia!\n");
            }
        }

    } while (opcao != 0);

    return 0;
}