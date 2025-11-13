#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// --- Constantes ---
#define TAM_FILA 5
#define TAM_PILHA 3
#define MAX_HISTORICO 5 // Quantos passos podemos desfazer
#define TIPOS_PECAS "IOTLJSZ"

// --- Estruturas de Dados ---

typedef struct {
    int id;
    char nome;
} Peca;

// Estrutura da FILA
typedef struct {
    Peca itens[TAM_FILA];
    int inicio, fim, quantidade;
} FilaCircular;

// Estrutura da PILHA
typedef struct {
    Peca itens[TAM_PILHA];
    int topo; 
} PilhaReserva;

// 🆕 Estrutura que guarda o ESTADO COMPLETO (Snapshot)
typedef struct {
    FilaCircular fila;
    PilhaReserva pilha;
} EstadoJogo;

// 🆕 Estrutura para gerenciar o UNDO (Pilha de Estados)
typedef struct {
    EstadoJogo estados[MAX_HISTORICO];
    int topo;
} Historico;

// --- Funções Auxiliares Básicas ---

Peca gerarPeca() {
    static int contadorId = 1;
    Peca p;
    p.id = contadorId++;
    p.nome = TIPOS_PECAS[rand() % 7];
    return p;
}

void initFila(FilaCircular *f) {
    f->inicio = 0; f->fim = 0; f->quantidade = 0;
}

void initPilha(PilhaReserva *p) {
    p->topo = -1;
}

void initHistorico(Historico *h) {
    h->topo = -1;
}

// --- Lógica de Fila e Pilha (Standard) ---

void enfileirar(FilaCircular *f, Peca p) {
    if (f->quantidade == TAM_FILA) return;
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

void push(PilhaReserva *p, Peca peca) {
    if (p->topo < TAM_PILHA - 1) {
        p->topo++;
        p->itens[p->topo] = peca;
    }
}

Peca pop(PilhaReserva *p) {
    if (p->topo >= 0) {
        return p->itens[p->topo--];
    }
    Peca vazia = {0, ' '};
    return vazia;
}

// --- ⚡ LÓGICA AVANÇADA (MESTRE) ---

// 1. Salvar Estado (Para o Undo)
void salvarEstado(Historico *h, FilaCircular f, PilhaReserva p) {
    if (h->topo < MAX_HISTORICO - 1) {
        h->topo++;
    } else {
        // Se cheio, movemos tudo um para trás (perde o mais antigo) - Opcional, aqui simplificado sobrescreve
        // Para simplificar no nível Mestre, apenas sobrescrevemos se cheio ou limitamos
        // Vamos usar lógica circular simples ou travar:
        // Neste exemplo: Se cheio, sobrescreve o topo (não ideal, mas simples para C puro)
        // Melhor: Shift array.
        for(int i = 0; i < MAX_HISTORICO - 1; i++) {
            h->estados[i] = h->estados[i+1];
        }
    }
    // Grava o estado atual no topo
    h->estados[h->topo].fila = f;
    h->estados[h->topo].pilha = p;
}

// 2. Desfazer (Undo)
int desfazer(Historico *h, FilaCircular *f, PilhaReserva *p) {
    if (h->topo == -1) return 0; // Nada para desfazer
    
    // Recupera o último estado
    *f = h->estados[h->topo].fila;
    *p = h->estados[h->topo].pilha;
    h->topo--;
    return 1;
}

// 3. Troca Estratégica (Swap Topo Pilha <-> Frente Fila)
void trocarFilaPilha(FilaCircular *f, PilhaReserva *p) {
    if (f->quantidade == 0 || p->topo == -1) {
        printf("⚠️  Impossível trocar: Fila vazia ou Pilha vazia.\n");
        return;
    }
    
    // Acesso direto aos ponteiros para troca
    int idxFila = f->inicio; // Índice real da frente da fila
    int idxPilha = p->topo;  // Índice real do topo da pilha
    
    Peca temp = f->itens[idxFila];
    f->itens[idxFila] = p->itens[idxPilha];
    p->itens[idxPilha] = temp;
    
    printf("♻️  Peças trocadas: Fila recebeu [%c], Pilha recebeu [%c]\n", 
           f->itens[idxFila].nome, p->itens[idxPilha].nome);
}

// 4. Inversão em Massa (Conteúdo da Pilha vira Fila e vice-versa - para os N primeiros)
void inverterMassa(FilaCircular *f, PilhaReserva *p) {
    if (p->topo == -1) {
        printf("⚠️  Pilha vazia, nada para inverter.\n");
        return;
    }
    
    // Troca os itens da pilha com os primeiros itens da fila
    for (int i = 0; i <= p->topo; i++) {
        int idxFila = (f->inicio + i) % TAM_FILA; // Calcula índice circular
        int idxPilha = p->topo - i; // Do topo para baixo
        
        Peca temp = f->itens[idxFila];
        f->itens[idxFila] = p->itens[idxPilha];
        p->itens[idxPilha] = temp;
    }
    printf("🌀 Inversão em Massa realizada!\n");
}

// --- Visualização ---

void visualizar(FilaCircular f, PilhaReserva p, Historico h) {
    printf("\n========================================\n");
    printf("⏪ Undo Disponíveis: %d\n", h.topo + 1);
    
    printf("📦 RESERVA: ");
    if (p.topo == -1) printf("[ Vazia ]");
    else {
        for (int i = 0; i <= p.topo; i++) printf("(%c) ", p.itens[i].nome);
        printf("<- TOPO");
    }
    
    printf("\n🚀 FILA:    [ SAÍDA ] <--- ");
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
    Historico historico;
    int opcao;
    
    srand(time(NULL));
    initFila(&fila);
    initPilha(&pilha);
    initHistorico(&historico);
    
    for(int i=0; i<TAM_FILA; i++) enfileirar(&fila, gerarPeca());

    do {
        visualizar(fila, pilha, historico);
        
        printf("1. JOGAR (Dequeue + Repor)\n");
        printf("2. RESERVAR (Enqueue -> Pilha)\n");
        printf("3. USAR RESERVA (Pop Pilha)\n");
        printf("4. 🔁 TROCAR (Frente Fila <-> Topo Pilha)\n");
        printf("5. ⏪ DESFAZER última ação\n");
        printf("6. 🌀 INVERSÃO EM MASSA (Troca Pilha com início Fila)\n");
        printf("0. Sair\n");
        printf("👉 Opção: ");
        scanf("%d", &opcao);
        
        // Salva o estado ANTES de qualquer ação modificadora (1, 2, 3, 4, 6)
        if (opcao >= 1 && opcao <= 6 && opcao != 5) {
            salvarEstado(&historico, fila, pilha);
        }

        printf("\n--- Ação ---\n");

        switch (opcao) {
            case 1: {
                Peca p = desenfileirar(&fila);
                printf("🎮 Jogou: %c\n", p.nome);
                enfileirar(&fila, gerarPeca());
                break;
            }
            case 2: {
                if (pilha.topo < TAM_PILHA - 1) {
                    push(&pilha, desenfileirar(&fila));
                    enfileirar(&fila, gerarPeca());
                    printf("💾 Reservada com sucesso.\n");
                } else {
                    printf("❌ Pilha cheia! (Ação cancelada, Undo não necessário)\n");
                    historico.topo--; // Remove o save state pois nada mudou
                }
                break;
            }
            case 3: {
                if (pilha.topo != -1) {
                    Peca p = pop(&pilha);
                    printf("♻️ Usou reserva: %c\n", p.nome);
                } else {
                    printf("❌ Pilha vazia!\n");
                    historico.topo--;
                }
                break;
            }
            case 4:
                trocarFilaPilha(&fila, &pilha);
                break;
            case 5:
                if (desfazer(&historico, &fila, &pilha)) {
                    printf("⏪ Voltamos no tempo!\n");
                } else {
                    printf("❌ Nada para desfazer.\n");
                }
                break;
            case 6:
                inverterMassa(&fila, &pilha);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }

    } while (opcao != 0);

    return 0;
}