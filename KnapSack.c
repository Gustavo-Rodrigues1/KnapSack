#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char codigo[14];
    float valor;
    int peso;
    int volume;
    int atribuida;
} Encomenda;

typedef struct {
    char placa[8];
    int peso;
    int pesoAtual;
    int volume;
    int volumeAtual;
    float valorDasEncomendas;
    Encomenda *encomendasAtribuidas;
    int qtdEncomendasAtribuidas;
} Caminhao;

void merge(Encomenda *vetor, int inicio, int meio, int fim) {
    Encomenda *aux = (Encomenda *)malloc((fim - inicio + 1) * sizeof(Encomenda));
    if (aux == NULL) {
        fprintf(stderr, "Falha ao alocar memória para merge.\n");
        exit(EXIT_FAILURE);
    }

    int primeiroInicio = inicio, segundoInicio = meio + 1, posicaoAux = 0;
    while (primeiroInicio <= meio && segundoInicio <= fim) {
        if (vetor[primeiroInicio].atribuida <= vetor[segundoInicio].atribuida) {
            aux[posicaoAux++] = vetor[primeiroInicio++];
        } else {
            aux[posicaoAux++] = vetor[segundoInicio++];
        }
    }
    while (primeiroInicio <= meio) {
        aux[posicaoAux++] = vetor[primeiroInicio++];
    }
    while (segundoInicio <= fim) {
        aux[posicaoAux++] = vetor[segundoInicio++];
    }
    for (primeiroInicio = inicio, posicaoAux = 0; primeiroInicio <= fim; primeiroInicio++, posicaoAux++) {
        vetor[primeiroInicio] = aux[posicaoAux];
    }
    free(aux);
}

void mergeSort(Encomenda *vetor, int inicio, int fim) {
    if (inicio < fim) {
        int meio = (inicio + fim) / 2;
        mergeSort(vetor, inicio, meio);
        mergeSort(vetor, meio + 1, fim);
        merge(vetor, inicio, meio, fim);
    }
}
float max(float a, float b) {
    return (a > b) ? a : b;
}

void knapSackMemo(Caminhao *caminhao, Encomenda *encomenda, int numeroDeEncomendas) {
    // Alocação dinâmica para a tabela de memoização
    float ***dp = (float ***)malloc((numeroDeEncomendas + 1) * sizeof(float **));
    for (int i = 0; i <= numeroDeEncomendas; i++) {
        dp[i] = (float **)malloc((caminhao->peso + 1) * sizeof(float *));
        for (int j = 0; j <= caminhao->peso; j++) {
            dp[i][j] = (float *)malloc((caminhao->volume + 1) * sizeof(float));
            for (int k = 0; k <= caminhao->volume; k++) {
                if (i == 0 || j == 0 || k == 0) {
                    dp[i][j][k] = 0; // Caso base
                } else {
                    dp[i][j][k] = -1; // Inicializa o restante com -1
                }
            }
        }
    }

    // Preenche a tabela
    for (int i = 1; i <= numeroDeEncomendas; i++) {
        for (int j = 0; j <= caminhao->peso; j++) {
            for (int k = 0; k <= caminhao->volume; k++) {
                if (encomenda[i - 1].atribuida == 0 && encomenda[i - 1].peso <= j && encomenda[i - 1].volume <= k) {
                    // Escolhe o máximo entre incluir ou não o pacote
                    dp[i][j][k] = max(
                        dp[i - 1][j][k], // Não inclui a encomenda
                        dp[i - 1][j - encomenda[i - 1].peso][k - encomenda[i - 1].volume] + encomenda[i - 1].valor // Inclui a encomenda
                    );
                } else {
                    // Não inclui o pacote
                    dp[i][j][k] = dp[i - 1][j][k];
                }
            }
        }
    }

    int i = numeroDeEncomendas, j = caminhao->peso, k = caminhao->volume;

    while (i > 0 && j > 0 && k > 0) {
        if (dp[i][j][k] != dp[i - 1][j][k]) {
            // A encomenda foi incluída
            caminhao->pesoAtual += encomenda[i - 1].peso;
            caminhao->volumeAtual += encomenda[i - 1].volume;
            caminhao->valorDasEncomendas += encomenda[i - 1].valor;
            caminhao->encomendasAtribuidas[caminhao->qtdEncomendasAtribuidas] = encomenda[i - 1];
            caminhao->qtdEncomendasAtribuidas++;
            encomenda[i - 1].atribuida = 1;
            j -= encomenda[i - 1].peso;
            k -= encomenda[i - 1].volume;
        }
        i--;
    }
}

int main() {
    // Arquivos
    FILE *input = fopen("TransportadoraEntrada.txt", "r");
    FILE *output = fopen("TransportadoraSaida.txt", "w");
    if (input == NULL || output == NULL) {
        fprintf(stderr, "Falha ao abrir o arquivo!\n");
        return EXIT_FAILURE;
    }

    // Leitura do número de caminhões
    int numeroDeCaminhoes, numeroDeEncomendas;
    fscanf(input, "%d", &numeroDeCaminhoes);
    Caminhao *vetorDeCaminhoes = (Caminhao *)malloc(numeroDeCaminhoes * sizeof(Caminhao));

    // Leitura dos caminhões
    for (int i = 0; i < numeroDeCaminhoes; i++) {
        fscanf(input, "%s %d %d", vetorDeCaminhoes[i].placa, &vetorDeCaminhoes[i].peso, &vetorDeCaminhoes[i].volume);
        vetorDeCaminhoes[i].pesoAtual = 0;
        vetorDeCaminhoes[i].volumeAtual = 0;
        vetorDeCaminhoes[i].valorDasEncomendas = 0;
        vetorDeCaminhoes[i].qtdEncomendasAtribuidas = 0;
        vetorDeCaminhoes[i].encomendasAtribuidas = NULL;
    }

    // Leitura do número de encomendas
    fscanf(input, "%d", &numeroDeEncomendas);

    for (int i = 0; i < numeroDeCaminhoes; i++) {
        vetorDeCaminhoes[i].encomendasAtribuidas = (Encomenda *)malloc(numeroDeEncomendas * sizeof(Encomenda));
    }

    Encomenda *vetorDeEncomenda = (Encomenda *)malloc(numeroDeEncomendas * sizeof(Encomenda));

    // Leitura das encomendas
    for (int i = 0; i < numeroDeEncomendas; i++) {
        fscanf(input, "%s %f %d %d", vetorDeEncomenda[i].codigo, &vetorDeEncomenda[i].valor, &vetorDeEncomenda[i].peso, &vetorDeEncomenda[i].volume);
        vetorDeEncomenda[i].atribuida = 0;
    }

    //imprimir caminhoes//
    double porcentagemPeso, porcentagemVolume;
    for (int i = 0; i < numeroDeCaminhoes; i++) {
        knapSackMemo(&vetorDeCaminhoes[i], vetorDeEncomenda, numeroDeEncomendas);
        if (vetorDeCaminhoes[i].peso != 0) {
            porcentagemPeso = round((vetorDeCaminhoes[i].pesoAtual * 100.0) / vetorDeCaminhoes[i].peso);
        } else {
            porcentagemPeso = 0;
        }

        if (vetorDeCaminhoes[i].volume != 0) {
            porcentagemVolume = round((vetorDeCaminhoes[i].volumeAtual * 100.0) / vetorDeCaminhoes[i].volume);
        } else {
            porcentagemVolume = 0;
        }
        fprintf(output, "[%s]R$%.02f,%dKG(%.0f%%),%dL(%.0f%%)->", vetorDeCaminhoes[i].placa, vetorDeCaminhoes[i].valorDasEncomendas, vetorDeCaminhoes[i].pesoAtual,porcentagemPeso, vetorDeCaminhoes[i].volumeAtual,porcentagemVolume);
        for (int c = vetorDeCaminhoes[i].qtdEncomendasAtribuidas - 1; c >= 0; c--)
        {
            fprintf(output,"%s",vetorDeCaminhoes[i].encomendasAtribuidas[c].codigo);
            if (c > 0) {
                fprintf(output, ",");
            }
        }
        fprintf(output,"\n");

    }

    // Inicializa a variável para armazenar o peso total das encomendas pendentes
int pesoTotalPendentes = 0, volumeTotalPendentes = 0;
float valorTotalPendentes = 0;

    mergeSort(vetorDeEncomenda, 0, numeroDeEncomendas - 1);
// Percorre o vetor de encomendas
for (int i = 0; i < numeroDeEncomendas; i++) {
    // Verifica se a encomenda está pendente
    if (vetorDeEncomenda[i].atribuida == 0) {
        // Soma o peso da encomenda pendente
        pesoTotalPendentes += vetorDeEncomenda[i].peso;
        volumeTotalPendentes += vetorDeEncomenda[i].volume;
        valorTotalPendentes += vetorDeEncomenda[i].valor;
    } else {
        break;
    }
}

    // Imprime as encomendas pendentes
    fprintf(output, "PENDENTE:R$%.02f,%dKG,%dL->",valorTotalPendentes,pesoTotalPendentes,volumeTotalPendentes);
    int contador = 0;
    while (contador < numeroDeEncomendas && vetorDeEncomenda[contador].atribuida == 0) {
        if (contador > 0) {
            fprintf(output, ",");
        }
        fprintf(output, "%s", vetorDeEncomenda[contador].codigo);
        contador++;
    }

    fclose(input);
    fclose(output);

    return 0;
}