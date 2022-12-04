#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

FILE *file, *saida;

int tamanho;

typedef struct node {
  char nome[20];
  int periodo;
  int burstOriginal;
  int burstAtual;
  int countDeadLine;
  int countExecution;
  int countKilled;
  int flag;
  struct node *prox;
} node;

node *head, *current, *tail;

int *addmdc(node *le, int length, int time, int *m);
void imprima(node *le);
void insert(char *neWnome, int neWperiodo, int neWburstOriginal, node *p,
            int time);
void swap(int *a, int *b);
void bubbleSort(int array[], int n);
void rate(node *le, int *f, int *m, int t);

int main(int argc, char const *argv[]) {
  int tempo, periodo, burst;
  char nome[10];

  tamanho = 1;

  head = (node *)malloc(sizeof(node));
  tail = (node *)malloc(sizeof(node));
  head->prox = NULL;

  file = fopen(argv[1], "r");

  if (file == NULL) {
    printf("ERRO: ARQUIVO NÃO PODE SER ABERTO\n");
    return 0;
  }

  if (argc > 2) {
    printf("ERRO: DIGITE APENAS UM ARQUIVO DE ENTRADA\n");
    return 0;
  }

  fscanf(file, "%d", &tempo);

  while (fscanf(file, "%s %d %d", nome, &periodo, &burst) != EOF) {
    current = head;
    insert(nome, periodo, burst, head, tempo);
  }

  tamanho++;
  int m[tamanho];

  int *mult = addmdc(head, tamanho, tempo, m);

  bubbleSort(mult, tamanho);

  int flag[tamanho];
  for (int i = 0; i < tamanho; i++) {
    flag[i] = 0;
  }
  rate(head, flag, mult, tempo);

  return 0;
}

void rate(node *le, int *f, int *m, int t) {
  saida = fopen("rate_popg.out", "w");
  node *p;
  int temp, exec, can = TRUE, acul = 0;
  fprintf(saida, "EXECUTION BY RATE\n");
  for (int i = 0; i < tamanho - 1; i++) {
    int dif = m[i + 1] - m[i];
    int all;
    int try = t - m[i];
    int try1;
    for (p = le->prox; p != NULL; p = p->prox) {
      if (m[i] % p->periodo == 0) {

        p->burstAtual = p->burstOriginal;
      }

      if (try < p->burstAtual && try > try1) {
        can = FALSE;
        try1 = try;
      }

      if (p->burstAtual > 0 && dif > 0) {
        if (dif > p->burstAtual) {
          temp = dif;
          dif -= p->burstAtual;
          exec = temp - dif;
          p->burstAtual = p->burstAtual - exec;
        } else if (dif <= p->burstAtual) {
          temp = p->burstAtual;
          p->burstAtual -= dif;
          exec = temp - p->burstAtual;
          dif = dif - exec;
        }

        for (node *q = le->prox; q != NULL; q = q->prox) {
          all = TRUE;
          if (q->burstAtual > 0)
            all = FALSE;
        }

        if (p->burstAtual == 0) {
          p->countExecution++;
          fprintf(saida, "[%s] for %d units - F\n", p->nome, exec);
          if (all == TRUE && p->prox == NULL)
            fprintf(saida, "idle for %d units\n", dif);
        } else if (p->burstAtual > 0 && m[i + 1] % p->periodo != 0) {
          if (can == FALSE)
            acul += exec;
          if (can)
            fprintf(saida, "[%s] for %d units - H\n", p->nome, exec);
          else if (acul == try1)
            fprintf(saida, "[%s] for %d units - K\n", p->nome, acul);

        } else if (m[i + 1] % p->periodo == 0) {
          fprintf(saida, "[%s] for %d units - L\n", p->nome, exec);
          p->countDeadLine++;
        }
      }
    }
    for (p = le->prox; p != NULL; p = p->prox) {
      if (p->burstAtual > 0) {
        p->countKilled = 1;
      }
    }
  }
  fprintf(saida, "\nLOST DEADLINES\n");
  for (p = le->prox; p != NULL; p = p->prox) {
    fprintf(saida, "[%s] %d\n", p->nome, p->countDeadLine);
  }
  fprintf(saida, "\nCOMPLETE EXECUTION\n");
  for (p = le->prox; p != NULL; p = p->prox) {
    fprintf(saida, "[%s] %d\n", p->nome, p->countExecution);
  }
  fprintf(saida, "\nKILLED\n");
  for (p = le->prox; p != NULL; p = p->prox) {
    fprintf(saida, "[%s] %d\n", p->nome, p->countKilled);
  }
  fclose(saida);
}

void insert(char *neWnome, int neWperiodo, int neWburstOriginal, node *p,
            int time) {
  node *nova, *aux;
  aux = (node *)malloc(sizeof(node));
  nova = (node *)malloc(sizeof(node));
  strcpy(nova->nome, neWnome);
  nova->periodo = neWperiodo;
  nova->burstOriginal = neWburstOriginal;
  nova->burstAtual = 0;
  nova->countDeadLine = 0;
  nova->countExecution = 0;
  nova->countKilled = 0;
  nova->flag = 0;

  if (p->prox == NULL) {
    nova->prox = p->prox;
    p->prox = nova;
  } else if (nova->periodo < p->prox->periodo) {
    nova->prox = p->prox;
    p->prox = nova;
  } else {
    aux = p->prox;
    while (aux->prox != NULL && nova->periodo > aux->prox->periodo) {
      aux = aux->prox;
    }
    nova->prox = aux->prox;
    aux->prox = nova;
  }
  tamanho += time / neWperiodo;
}

void imprima(node *le) {
  node *p;
  for (p = le->prox; p != NULL; p = p->prox) {
    printf("\n%s %d %d", p->nome, p->periodo, p->burstOriginal);
  }
}

int *addmdc(node *le, int length, int time, int *m) {
  node *p;
  int flag = 0;
  int in = 0;

  for (size_t i = 0; i < length; i++) {
    m[i] = -1;
  }

  for (p = le->prox; p != NULL; p = p->prox) {
    int N = time / p->periodo;

    for (int i = 0; i < N + 1; i++) {
      for (int j = 0; j < length; j++) {
        if (m[j] == p->periodo * i) {
          flag = 1;
        }
      }
      if (flag == 0) {
        m[in] = p->periodo * i;
        in++;
      }
      flag = 0;
    }
  }
  m[in] = time;
  return m;
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void bubbleSort(int array[], int n) {
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (array[j] > array[j + 1]) {
        swap(&array[j], &array[j + 1]);
      }
    }
  }
}
