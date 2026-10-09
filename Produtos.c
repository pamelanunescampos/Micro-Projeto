#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define ARQUIVO "produtos.csv"
#define TEMP "temp.csv"
#define TAM 100

#ifdef _WIN32
    #define LIMPAR_TELA "cls"
#else
    #define LIMPAR_TELA "clear"
#endif

typedef struct {
	int id;
    char nome[TAM];
    char categoria[TAM];
    float preco;
} Produto;

// Protótipo das funções

// Funções de verificação
void lerTexto(char *rotulo, char *destino);
float lerPreco(char *rotulo);
int lerLinhaCSV(char *linha, Produto *produto);
int verificarProdutos(FILE *f, char *linha, Produto *produto);
int lerId(char *rotulo);
int gerarId(Produto *produto);

// Funções auxiliares
void menu(void);
void limparBuffer(void);
void pausar(void);
void mostrarProduto(Produto produto);
int confirmarRemocao(void);

// Funções CRUD
void cadastrarProduto(Produto *produto);
void listarProduto(Produto *produto);
void buscarPorNome(Produto *produto);
void buscarPorCategoria(Produto *produto);
void buscarPorFaixa(Produto *produto);
void atualizarProduto(Produto *produto);
void removerProduto(Produto *produto);

// Função principal
int main() {
	setlocale(LC_ALL, "");
    int opcao;
    Produto produto;

    do {
        system(LIMPAR_TELA);
		menu();
        if (scanf("%d", &opcao) != 1) {
    		printf("\nEntrada invalida!\n");
    		limparBuffer();
    		continue;
		}
        limparBuffer();

        printf("\n");

        switch (opcao) {
            case 1: 
				cadastrarProduto(&produto);
				pausar();
				break;
				
            case 2: 
				listarProduto(&produto);
				pausar();
				break;
				
            case 3: 
				buscarPorNome(&produto);
				pausar();
				break;
				
            case 4: 
				buscarPorCategoria(&produto); 
				pausar();
				break;
				
            case 5: 
				buscarPorFaixa(&produto);     
				pausar();
				break;
				
            case 6: 
				atualizarProduto(&produto);            
				pausar();
				break;
				
            case 7:
				removerProduto(&produto);           
				pausar();
				break;
				
            case 0: 
				printf("\nEncerrando o sistema...\n"); 
				break;
				
            default:
                printf("\nErro: opção inválida! Tente novamente.\n");
                pausar();
        }
    } while (opcao != 0);

    return 0;
}

// Função de menu
void menu(){
		printf("------------------------------------\n");
        printf("      SISTEMA LOJA DE PRODUTOS      \n");
        printf("------------------------------------\n");
        printf("1 - Cadastrar produto               \n");
        printf("2 - Listar produtos                 \n");
        printf("3 - Buscar por nome                 \n");
        printf("4 - Buscar por categoria            \n");
        printf("5 - Buscar por faixa de preços      \n");
        printf("6 - Atualizar produto               \n");
        printf("7 - Remover produto                 \n");
        printf("0 - Sair\n");
        printf("------------------------------------\n");
        printf("Escolha uma das opções: ");
}

// Limpa o '\n' que o scanf deixa no input
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Função para pausar, já que o System limpa a tela e volta pro menu
void pausar(){
    printf("\nPressione ENTER para continuar...");
    limparBuffer();
}

// Função de verificação de um texto não aceita ';' porque daria erro no csv
void lerTexto(char *rotulo, char *destino) {

    while (1) {

        printf("%s", rotulo);
        
        fgets(destino, TAM, stdin);
        destino[strcspn(destino, "\n")] = '\0';

        if (strlen(destino) == 0) {
            printf("\nErro: o campo não pode ficar vazio.\n");
            continue;
        }

        if (strchr(destino, ';') != NULL) {
            printf("\nErro: não use o caractere ';'.\n");
            continue;
        }

        return;
    }
}

// Função de verificação de um preço e só aceita número válido e não negativo
float lerPreco(char *rotulo) {
    char entrada[50];
    int reais, centavos = 0;

    while (1) {
        printf("%s", rotulo);
        fgets(entrada, sizeof(entrada), stdin);

        if (sscanf(entrada, "%d%*[.,]%d", &reais, &centavos) >= 1 && reais >= 0) {
            return reais + centavos / 100.0f;
        }
        printf("\nErro: digite um preço válido.\n");
    }
}

// Função para manipular o arquivo produtos e temp(temporario)
// Transforma uma linha "nome;categoria;preco" em um Produto usando strtok.
// Devolve 1 se deu certo e 0 se a linha estiver incompleta
int lerLinhaCSV(char *linha, Produto *produto) {
	char *id = strtok(linha, ";");
    char *nome = strtok(NULL, ";");       
    char *categoria = strtok(NULL, ";");    
    char *preco = strtok(NULL, ";\n");

    if (id == NULL || nome == NULL || categoria == NULL || preco == NULL) {
        return 0;
    }
	
	produto->id = atoi(id);
    strncpy(produto->nome, nome, TAM - 1);
    produto->nome[TAM - 1] = '\0';
    strncpy(produto->categoria, categoria, TAM - 1);
    produto->categoria[TAM - 1] = '\0';
    produto->preco = (float) atof(preco);         // texto vira numero numero
    return 1;
}

// Função para confirmar se o usuario quer remover o produto
int confirmarRemocao() {
    char resposta;

    printf("\nTem certeza que deseja remover este produto? (S/N): ");
    scanf(" %c", &resposta);
    limparBuffer();

    if (resposta == 'S' || resposta == 's') {
        return 1;
    }

    return 0;
}

// Função para verificar se tem produtos
int verificarProdutos(FILE *f, char *linha, Produto *produto) {

    int total = 0;

    while (fgets(linha, 250, f) != NULL) {
        if (lerLinhaCSV(linha, produto)) {
            total++;
        }
    }

    if (total == 0) {
        printf("\nNenhum produto cadastrado.\n");
        return 0;
    }

    rewind(f);

    return 1;
}

// Função para ler um ID
int lerId(char *rotulo){
	
	int valor;
	char entrada[50];
	
	while(1){
		printf("%s", rotulo);
		
		if(fgets(entrada, 50 , stdin) == NULL){
        	entrada[0] = '\0';
		}
		
		if(sscanf(entrada, "%d", &valor) != 1 || valor <= 0){
        	printf("\nErro: digite um ID válido.\n");
		}
		
		else{
			return valor;
		}
	}
}

// Função de gerar o Id
int gerarId(Produto *produto){
	char linha[250];
	int maior = 0;

	FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        return 1;
    }
	
    while (fgets(linha, 250, f) != NULL) {
        if (lerLinhaCSV(linha, produto) && produto->id > maior) {
            maior = produto->id;
        }
    }

    fclose(f);

    return maior + 1;	
	
}

// Função de cadastrar produtos
void cadastrarProduto(Produto *produto) {
	
	int novoId = gerarId(produto);
    
	printf("----- CADASTRAR PRODUTO -----\n"); 
    lerTexto("\nNome do produto: ", produto->nome);
    lerTexto("Categoria: ", produto->categoria);
    produto->preco = lerPreco("Preço: ");
    produto->id = novoId;

    // "a" (append) acrescenta no final sem apagar os produtos existentes
    FILE *f = fopen(ARQUIVO, "a");
    if (f == NULL) {
        perror("\nErro ao abrir o arquivo!");
        return;
    }

    fprintf(f, "%d;%s;%s;%.2f\n", produto->id, produto->nome, produto->categoria, produto->preco);
    fclose(f);
    printf("\nProduto cadastrado com sucesso!\n");
}

// Função de mostrar os produtos
void mostrarProduto(Produto produto) {
	printf("\n==============================\n");
	printf("| ID: %d                        \n",produto.id);
    printf("| Nome: %s                      \n",produto.nome);
    printf("| Categoria: %s                 \n",produto.categoria);
	printf("| Preço: R$ %.2f                  ",produto.preco);
	printf("\n==============================\n");
}

// Função de listar os produto
void listarProduto(Produto *produto) {
    
    char linha[250];
    int total = 0;

    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    printf("--- PRODUTOS CADASTRADOS ---\n");

    // fgets() le uma linha do arquivo por vez, ate chegar ao fim (NULL)
    while (fgets(linha, 250, f) != NULL) {
        if (lerLinhaCSV(linha, produto)) {
            total++;
            printf("\nProduto %d\n", total);
            mostrarProduto(*produto);
        }
    }
    fclose(f);

    if (total == 0) {
        printf("\nNenhum produto cadastrado.\n");
    }
    else {
        printf("\nTotal: %d produto(s).\n", total);
    }
}

// Função de buscar produto pelo nome
void buscarPorNome(Produto *produto) {
    
    char linha[250];
    char nomeBusca[TAM];
    int encontrou = 0;
    
    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\nProduto não encontrado.\n");
        return;
    }
    
    if (!verificarProdutos(f, linha, produto)) {
    	fclose(f);
    	return;
	}

    printf("--- BUSCAR POR NOME ---\n");
    lerTexto("\nDigite o nome do produto: ", nomeBusca);

    while (fgets(linha, 250, f) != NULL && encontrou == 0) {
        if (lerLinhaCSV(linha, produto) && strcmp(produto->nome, nomeBusca) == 0) {
            printf("\nProduto(s) encontrado(s):\n");
			printf("[%d] %s - R$ %.2f\n", produto->id, produto->nome, produto->preco);            
			encontrou = 1;
        }
    }
    fclose(f);

    if (encontrou == 0) {
        printf("\nProduto não encontrado.\n");
    }
    
}

// Função de buscar produto pela categoria
void buscarPorCategoria(Produto *produto) {
    
    char linha[250];
    char categoriaBusca[TAM];
    int encontrou = 0;

    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\nProduto não encontrado.\n");
        return;
    }
    
    if (!verificarProdutos(f, linha, produto)) {
    	fclose(f);
    	return;
	}
    
    printf("--- BUSCAR POR CATEGORIA ---\n");
    lerTexto("\nDigite a categoria desejada: ", categoriaBusca);

    while (fgets(linha, 250, f) != NULL) {
        if (lerLinhaCSV(linha, produto) && strcmp(produto->categoria, categoriaBusca) == 0) {
            if (encontrou == 0) {
                printf("\nProduto(s) encontrado(s):\n");
            }
            printf("[%d] %s - R$ %.2f\n", produto->id, produto->nome, produto->preco);
            encontrou = 1;
        }
    }
    fclose(f);

    if (encontrou == 0) {
        printf("\nNenhum produto encontrado na categoria \"%s\".\n", categoriaBusca);
    }
}

// Função de buscar produto pela faixa de preçon entre o minimo e o máximo
void buscarPorFaixa(Produto *produto) {

    char linha[250];
    float minimo, maximo;
    int encontrou = 0;

    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\nProduto não encontrado.\n");
        return;
    }
    
    if (!verificarProdutos(f, linha, produto)) {
    	fclose(f);
    	return;
	}
    
    printf("--- BUSCAR POR FAIXA DE PRECOS ---\n");
    minimo = lerPreco("\nPreco minimo: ");   // ja recusa valores negativos
    maximo = lerPreco("Preco maximo: ");

    if (minimo > maximo) {
        printf("\nErro: o preço minimo não pode ser maior que o preço máximo.\n");
        fclose(f);
		return;
    }

    while (fgets(linha, 250, f) != NULL) {
        
        if (lerLinhaCSV(linha, produto) && produto->preco >= minimo && produto->preco <= maximo) {
            if (encontrou == 0) {
                printf("\nProduto(s) encontrado(s):\n");
            }
            printf("[%d] - %s - R$ %.2f\n", produto->id, produto->nome, produto->preco);
            encontrou = 1;
        }
    }
    fclose(f);

    if (encontrou == 0) {
        printf("\nNenhum produto encontrado na faixa informada.\n");
    }
}

// Função de atualizar os dados de um produto
void atualizarProduto(Produto *produto) {
    Produto novo;
    char linha[250], copia[250];
    int idBusca;
    int encontrou = 0;

	FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\nProduto nao encontrado.\n");
        return;
    }
	    
    if (!verificarProdutos(f, linha, produto)) {
    	fclose(f);
    	return;
	}
	
	listarProduto(produto);
    printf("\n--- ATUALIZAR PRODUTO ---\n");
    idBusca = lerId("\nDigite o Id produto que deseja atualizar: ");
	
	FILE *temp = fopen(TEMP, "w");
    if (temp == NULL) {
        printf("\nErro ao criar arquivo temporario.\n");
        fclose(f);
        return;
    }
	
    while (fgets(linha, 250, f) != NULL) {
        strcpy(copia, linha);

        if (encontrou == 0 && lerLinhaCSV(copia, produto) && produto->id == idBusca) {
            encontrou = 1;

            printf("\nProduto atual:\n");
            mostrarProduto(*produto);
            printf("\n");
			
			novo.id = produto->id;
            lerTexto("\nNovo nome: ", novo.nome);
            lerTexto("Nova categoria: ", novo.categoria);
            novo.preco = lerPreco("Novo preço: ");

            // grava os dados novos no lugar da linha antiga
            fprintf(temp, "%d;%s;%s;%.2f\n", novo.id, novo.nome, novo.categoria, novo.preco);
        }
        else {
            fprintf(temp, "%s", linha);
        }
    }

    fclose(f);
    fclose(temp);

    if (encontrou == 1) {
        remove(ARQUIVO);
        rename(TEMP, ARQUIVO);
        printf("\nProduto atualizado com sucesso!\n");
    }
    else {
        remove(TEMP);
        printf("\nProduto com ID %d não encontrado.\n", idBusca);
    }
}

// Função de remover produto
void removerProduto(Produto *produto) {

    char linha[250], copia[250];
    int idBusca;
    int encontrou = 0;
    int confirmou = 0;

	FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        printf("\nProduto não encontrado.\n");
        return;
    }
    
    
	if (!verificarProdutos(f, linha, produto)) {
    	fclose(f);
    	return;
	}
    
	printf("--- REMOVER PRODUTO ---\n");
    idBusca = lerId("\nDigite o ID do produto que deseja remover: ");

    FILE *temp = fopen(TEMP, "w");
    if (temp == NULL) {
        printf("\nErro ao criar arquivo temporario.\n");
        fclose(f);
        return;
    }
	
	while (fgets(linha, 250, f) != NULL) {

        strcpy(copia, linha);

        if (encontrou == 0 &&
            lerLinhaCSV(copia, produto) &&
            produto->id == idBusca){

            encontrou = 1;

            printf("\nProduto(s) encontrado(s):\n");
            mostrarProduto(*produto);

            confirmou = confirmarRemocao();

            if (!confirmou) {
                fprintf(temp, "%s", linha);
            }

        }
        else {
            fprintf(temp, "%s", linha);
        }
    }

    fclose(f);
    fclose(temp);

    if (encontrou == 1 && confirmou == 1) {

        remove(ARQUIVO);
        rename(TEMP, ARQUIVO);

        printf("\nProduto removido com sucesso!\n");
    }
    else {
		if (encontrou == 1 && confirmou == 0) {

        remove(TEMP);

        printf("\nRemoção cancelada.\n");
    	}
    	
    	else {

        remove(TEMP);

        printf("\nProduto com ID %d não encontrado.\n", idBusca);
    	}
    }    
}

