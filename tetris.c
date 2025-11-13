#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Definições e Constantes ---
#define TAMANHO_FILA 5
#define TIPOS_PECAS "IOTLJSZ" // Tipos possíveis de peças do Tetris

// --- Estruturas de Dados ---

// Estrutura para representar uma Peça
typedef struct {
    int id;      // Identificador único (ex: 1, 2, 3...)
    char nome;   // Tipo da peça (ex: 'T', 'I')
} Peca;

// Estrutura para a Fila Circular
typedef struct {
    Peca itens[TAMANHO_FILA];
    int inicio;
    int fim;
    int quantidade; // Controla quantos itens existem na fila
} FilaCircular;

// --- Funções Auxiliares ---

// Função para gerar uma nova peça aleatória
Peca gerarPeca() {
    static int contadorId = 1; // Variável estática mantém valor entre chamadas
    Peca p;
    
    p.id = contadorId++;
    // Escolhe uma letra aleatória da string TIPOS_PECAS
    p.nome = TIPOS_PECAS[rand() % 7]; 
    
    return p;
}

// Inicializa a fila (zera os índices)
void inicializarFila(FilaCircular *f) {
    f->inicio = 0;
    f->fim = 0;
    f->quantidade = 0;
}

// Insere uma peça no final da fila (Enqueue)
void enfileirar(FilaCircular *f, Peca p) {
    if (f->quantidade == TAMANHO_FILA) {
        printf("⚠️  Erro: A fila está cheia!\n");
        return;
    }
    
    f->itens[f->fim] = p;
    // Lógica Circular: se chegar no fim do array, volta para o índice 0
    f->fim = (f->fim + 1) % TAMANHO_FILA; 
    f->quantidade++;
}

// Remove a peça da frente (Dequeue) e retorna ela
Peca desenfileirar(FilaCircular *f) {
    Peca p = {0, ' '}; // Peça vazia para erro
    
    if (f->quantidade == 0) {
        printf("⚠️  Erro: A fila está vazia!\n");
        return p;
    }
    
    p = f->itens[f->inicio];
    // Lógica Circular: move o início para frente
    f->inicio = (f->inicio + 1) % TAMANHO_FILA; 
    f->quantidade--;
    
    return p;
}

// Exibe o estado atual da fila
void visualizarFila(FilaCircular f) {
    printf("\n=== 🧩 PRÓXIMAS PEÇAS (Fila) ===\n");
    printf("[ SAÍDA ] <--- ");
    
    int i, idx;
    for (i = 0; i < f.quantidade; i++) {
        // Calcula o índice real no array circular
        idx = (f.inicio + i) % TAMANHO_FILA;
        printf("(%d: %c) ", f.itens[idx].id, f.itens[idx].nome);
    }
    
    printf("<--- [ ENTRADA ]\n");
    printf("================================\n");
}

// --- Função Principal ---
int main() {
    FilaCircular fila;
    int opcao;
    
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));
    
    // 1. Inicialização do Sistema
    inicializarFila(&fila);
    
    printf("⚡ Sistema Iniciado. Gerando as 5 primeiras peças...\n");
    for(int i = 0; i < TAMANHO_FILA; i++) {
        enfileirar(&fila, gerarPeca());
    }

    // Loop do Menu
    do {
        visualizarFila(fila);
        
        printf("\n🎮 MENU TETRIS STACK:\n");
        printf("1. Jogar peça atual (Dequeue)\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            // Passo A: Jogar a peça da frente
            Peca jogada = desenfileirar(&fila);
            printf("\n🚀 Você jogou a peça: [%c] (ID: %d)\n", jogada.nome, jogada.id);
            
            // Passo B: Repor automaticamente
            printf("🔄 Gerando nova peça para o final da fila...\n");
            enfileirar(&fila, gerarPeca());
            
            // Pequena pausa para leitura (opcional)
            // system("pause"); // No Windows
            // getchar(); 
        }

    } while (opcao != 0);

    printf("Fim do jogo!\n");
    return 0;
}