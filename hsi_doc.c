/*

Descrição: Código que implementa a geração de casos de teste a partir
           do método HSI, além de todo o procedimento de leitura de MEFs.
           

           A estruturas e algortimos sao explicados e exemplificados no 
           relatorio tecnico 286 da biblioteca do ICMC/USP

           
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define MAX 50

/******************** Estruturas para insercao da MEF ********************/

/* Tipo da Matriz que representa a MEF */
typedef struct {
   char estado[100]; /* estados da transicao */
   char saida[100];  /* saida gerada pela transicao */
} TipoMatriz;


/* Noh que representa as entradas inseridas */
struct _noent {    
   char elem[100]; 
   struct _noent *prox;
};
typedef struct _noent noEntrada;


/* Lista que representa as entradas inseridas.
   Utilizado na representacao matricial da MEF */
typedef struct {
   int cont;         /* contador do número de entradas da MEF*/
   noEntrada *pont;
} TipoListaEntrada;


/* Noh que representa os estados da MEF inseridos */
struct _noest {
   char elem[MAX];              /* nome do estado */               
   char HSI[MAX][200];          /* HSI do estado */      
   char transition[MAX][200];   /* transition cover do estado */
   char preamble[MAX];          /* preambule do estado */ 
   struct _noest *prox;
};
typedef struct _noest noEstado;

/* Lista que representa os estados inseridos.
   Utilizado na representacao matricial da MEF */
typedef struct {
   int cont;     /* contador do número de estados da MEF*/
   noEstado *pont;
} TipoListaEstado;

/* Lista que representa a entrada dos dados da MEF 
 Exemplo: s0 - a/0 -> s1
*/
struct _noEntradaDados {
   char estOrigem[MAX];          /* s0 */
   char estDestino[MAX];         /* s1 */
   char entrada[MAX];            /* a */
   char saida[MAX];              /* 0 */
   struct _noEntradaDados *prox;       
};
typedef struct _noEntradaDados EntradaDados;


/***** Estruturas para obtencaoo do conjunto final de teste *****/


/* Estrutura que representa o noh da arvore utilizada para 
   obtencao do preambulo */
typedef struct _no_p {
    char estado[MAX];
    char entrada[MAX];
    struct _no_p * pai;
    struct _no_p * entradas;
    struct _no_p * lista;
    int valido;
}no_p;


/* lista que armazena as entradas aceitas pela MEF.
   Utilizada na estrutura de obtencao do HSI */
typedef struct _pontHSI {
    char entrada[MAX];
    struct _noHSI * pont;
}pontHSI;


/* lista que armazena a sequencia HSI de cada par de estados
   Utilizada na estrutura de obtencao do HSI */
typedef struct _seqHSI {
    char entrada[MAX];
    struct _seqHSI * prox;
    struct _seqHSI * phsi;
}seqHSI;


/* Nó da estrutura principal de obtenção do conjunto HSI */
typedef struct _noHSI {
    char est1[MAX];
    char est2[MAX];
    char flag;
    struct _pontHSI *Phsi;
    struct _seqHSI *Shsi;
}noHSI;

/*----------------------------------------------------------------------------*/

/* funcao para adicao dos estados inseridos na lista de estados que representa
   a matriz da MEF */

void adicEstado(char estado[MAX], TipoListaEstado *p) {
   noEstado *q;
   noEstado *aux;

   q = (noEstado*) malloc(sizeof(noEstado));
   strcpy(q->elem, estado);
   q->prox = NULL;
   /* se for o primeiro elemento */   
   if (p->pont == NULL) {          
      p->pont = q;
      p->cont = 1;
   } else {
      aux = p->pont;
	  /* insercao no final da lista */
      while( aux->prox != NULL ) { 
        aux = aux->prox;
      }  
      aux->prox = q;
      (p->cont)++;    /* incremento no contador */
   }
}

/*----------------------------------------------------------------------------*/

/* funcao para adicao das entradas inseridas na lista de entradas que representa
   a matriz da MEF */

void adicEntrada(char entrada[MAX], TipoListaEntrada *p) {
   noEntrada *q;
   noEntrada *aux;

   q = (noEntrada*) malloc(sizeof(noEntrada));
   strcpy(q->elem, entrada);
   q->prox = NULL;
   /* se for o primeiro elemento */   
   if (p->pont == NULL) {
      p->pont = q;
      p->cont = 1;
   } else {
      aux = p->pont;
	  /* insercao no final da lista */
      while( aux->prox != NULL ) {
        aux = aux->prox;
      }  
      aux->prox = q;
      (p->cont)++;     /* incremento no contador */
   }
}

/*----------------------------------------------------------------------------*/

/* funcao utilizada para criar o relacionamento na matriz da MEF */

int Relaciona( TipoListaEstado lista_estados, TipoListaEntrada lista_entradas, TipoMatriz **mat, 
               char origem[MAX], char destino[MAX], char entrada[MAX], char saida[MAX]) {

   noEstado *aux1 = lista_estados.pont;
   noEntrada *aux2 = lista_entradas.pont;
   int l = 0;
   int c = 0;
   
   /* busca do estado de origem na lista de estados */
   while ((aux1 != NULL) && (strcmp(origem, aux1->elem) != 0)) {
      aux1 = aux1->prox;
      l++;
   }
   /* caso nao encontra */
   if (aux1 == NULL) {
      return 0;
   }
   
   /* busca da entrada na lista de entradas */
   while ((aux2 != NULL) && (strcmp(entrada, aux2->elem) != 0)) {
      aux2 = aux2->prox;
      c++;
   }
   /* caso nao encontra */
   if (aux2 == NULL) {
      return 0;
   }
   
   /* insercao do destino na matriz da MEF */
   strcpy ((mat[l][c]).estado, destino);
   /* insercao da saida na matriz da MEF */   
   strcpy ((mat[l][c]).saida, saida);    
   return 1;
}

/*----------------------------------------------------------------------------*/

/* funcao de execucao de sequencias de entradas na MEF inserida */

int executa( TipoListaEstado lista_estados, TipoListaEntrada lista_entradas, TipoMatriz **mat, 
             char est[MAX], char entrada[MAX], TipoListaEstado *hsi, int flag) {
 
   noEstado *aux1 = lista_estados.pont;
   noEntrada *aux2 = lista_entradas.pont;
   TipoListaEstado aux3;
   int l = 0;
   int c = 0;
   int j = 0;
   int i = 0;
   int k;
   char entradaAux[MAX];
   
   /* executa-se até o fim da sequencia de entrada */   
   while (entrada[i] != '\0') { 
      j = 0;
      entradaAux[0] = '\0';
      while (entrada[i] != ' ') {  /* pegando cada entrada separadamente */
         entradaAux[j] = entrada[i];
         j++;
         i++;
      }
      i++;
      entradaAux[j] = ' ';
      entradaAux[j+1] = '\0';
   
      /* buscando estado na lista de estados para buscar referencia na matriz */
      while ((aux1 != NULL) && (strcmp(est, aux1->elem) != 0)) {
         aux1 = aux1->prox;
         l++;
      }
      if (aux1 == NULL) {        /* se nao encontrou */
         return 0;
      }
   
      /* buscando entrada na lista de entradas para buscar referencia na matriz */
      while ((aux2 != NULL) && (strcmp(entradaAux, aux2->elem) != 0)) {
         aux2 = aux2->prox;
         c++;
      }
      if (aux2 == NULL) {        /* se nao encontrou */
         return 0;
      }
      
      /* caso deseja imprimir a execucao (nao usado nessa implementacao) */
      if ((flag == 1) && (strcmp(mat[l][c].estado, "*"))) {
         printf("   %s", (mat[l][c]).saida);
      }
      
      /* caso nao exista saida definida no estado */
      if (strcmp(mat[l][c].estado, "*")) {
         strcpy(est, (mat[l][c]).estado);
      }
      l = 0;   /* zerando as 'coordenadas' da matriz */
      c = 0;
      aux1 = lista_estados.pont;
      aux2 = lista_entradas.pont; 
   }
   
   /* atualizacao do ponteiro hsi */
   aux3 = lista_estados;
   for (k = 0; k < aux3.cont; k++) {
      if (strcmp(est, aux3.pont->elem) == 0) {
         *hsi = aux3;
         break;
      }
      aux3.pont = aux3.pont->prox;
   }
   return 1;
}

/*----------------------------------------------------------------------------*/

/* funcao que cria a matriz de relacionamento, representando a MEF inserida */

int CriaMatrizMEF(TipoListaEstado **listaEstado, TipoListaEntrada **listaEntrada,
                  TipoMatriz ***mat, EntradaDados *lDados) {
 
   noEstado *listaEstadoAux;
   noEntrada *listaEntradaAux;
   EntradaDados *auxDados;
   int insere;
   int i;
   int j;
   int k;                        
   
   auxDados = lDados;
   /* executa o laco enquanto houver elementos na lista de entrada */
   while (auxDados != NULL) {
      /* se nao existem elementos na lista de estados e entrada */   
      if ((*listaEstado)->pont == NULL) { 
         adicEstado(auxDados->estOrigem, *listaEstado);
         adicEntrada(auxDados->entrada, *listaEntrada);
		 /* se os dois estados forem diferentes insere-os na lista de estado */
         if ( strcmp(auxDados->estOrigem, auxDados->estDestino) != 0) {
            adicEstado(auxDados->estDestino, *listaEstado);
         }
		/* se nao for o primeiro elemento */
      } else {
	     listaEstadoAux = (*listaEstado)->pont;
         insere = 1; /* indica se o estado de origem ja foi inserido */
         while (listaEstadoAux != NULL) { /* verifica se o estado de origem ja foi inserido */
            if ( strcmp(listaEstadoAux->elem, auxDados->estOrigem) == 0) {
               insere = 0;
               break;
            }
            listaEstadoAux = listaEstadoAux->prox;  
         }
		 /* se nao existe na lista entao eh inserido */
         if (insere) {
            adicEstado(auxDados->estOrigem, *listaEstado);
         }
      
         listaEstadoAux = (*listaEstado)->pont;
         insere  = 1; /* indica se o estado de destino ja foi inserido */
         while (listaEstadoAux != NULL) { /* verifica se o estado de destino ja foi inserido */
            if ( strcmp(listaEstadoAux->elem, auxDados->estDestino) == 0) {
               insere = 0;
               break;
            }
            listaEstadoAux = listaEstadoAux->prox;  
         }
         /* se nao existe na lista entao eh inserido */
         if (insere) {
            adicEstado(auxDados->estDestino, *listaEstado); 
         }
         
         listaEntradaAux = (*listaEntrada)->pont;
         insere = 1; /* indica se a entrada ja foi inserida */
         while (listaEntradaAux != NULL) { /* verifica se a entrada ja foi inserida */
            if ( strcmp(listaEntradaAux->elem, auxDados->entrada) == 0) {
               insere = 0;
               break;
            }
            listaEntradaAux = listaEntradaAux->prox;    
         }
         /* se nao existe na lista entao eh inserida */
         if (insere) {
            adicEntrada(auxDados->entrada, *listaEntrada);
         }
         
      } 
      auxDados = auxDados->prox;
   }
      
   /* Matriz é alocada de acordo com o numero de estados e entradas inseridas */
   *mat = (TipoMatriz **) malloc(sizeof(TipoMatriz*)*(*listaEstado)->cont);
   for (i = 0; i < (*listaEstado)->cont; i++) {
      (*mat)[i] = (TipoMatriz *) malloc(sizeof(TipoMatriz)*(*listaEntrada)->cont);
   }
   /* carregamento inicial da matriz */
   for (j = 0; j < (*listaEstado)->cont; j++) {
      for (k = 0; k < (*listaEntrada)->cont; k++) {
         strcpy((*mat)[j][k].saida, "#");  /* indica saida vazia na  matriz*/
      	 strcpy((*mat)[j][k].estado, "*"); /* idica estado vazia na matriz */
      }
   }
   
   /* realiza o relacionamento na matriz alocada atraves da funcao Relaciona*/
   auxDados = lDados;
   while(auxDados != NULL) {
      Relaciona(**listaEstado, **listaEntrada, *mat, auxDados->estOrigem, 
                auxDados->estDestino, auxDados->entrada, auxDados->saida);
      auxDados = auxDados->prox;
   }
         
   return 0;   
}

/*----------------------------------------------------------------------------*/

/* funcao que interpreta a entrada inserida pelo usuario 

estadoOrigem - entrada/saida -> estadoDestino

retorna 0 sempre que encontrar um erro de sintaxe na insercao
se retornar 1 nenhum erro de sintaxe foi encontrado

*/

int Interpreta(char op[MAX], char estOrigem[MAX], char estDestino[MAX], char entrada[MAX],
               char saida[MAX]) {
   
   int i = 0;
   int j = 0;
   
   /* atribui o estado de origem  */
   while(op[i] != '-') {
	  /* indica erro na insercao dos dados */
      if (i > (strlen(op) - 6)) {
         return 0;
      }
	  /* atribuindo o estado de origem */
      estOrigem[j] = op[i];
      i++;
      j++;
   }
   estOrigem[j] = '\0';
   
   i++;
   j = 0;
   while(op[i] != '/') {
      /* indica erro na insercao dos dados */
      if (i > (strlen(op) - 4)) {
         return 0;
      }
	  /* atribuindo a entrada */
      entrada[j] = op[i];
      i++;
      j++; 
   }
   entrada[j] = '\0';
   
   i++;
   j = 0;
   while(op[i] != '-') {
      /* indica erro na insercao dos dados */
      if (i > (strlen(op) - 2)) {
         return 0;
      }
	  /* atribuindo a saida */
      saida[j] = op[i];
      i++;
      j++; 
   }
   saida[j] = '\0';
   
   /* indica erro na insercao */
   if (op[++i] != '>') {
      return 0;
   }
   
   i++;
   j = 0;
   /* atribuicao do estado de destino   */
   while(op[i] != '\0') {
      estDestino[j] = op[i];
      i++;
      j++; 
   }
   estDestino[j] = '\0';
   
   return 1;
}

/*----------------------------------------------------------------------------*/

/* Funcao que insere os dados inseridos na lista de entrada de 
   dados. 

  Utiliza a funcao interpreta para validar a entrada e a funcao
  CriaMatrizMEF	para criar a matriz da mef

*/

int InsereMEF(TipoListaEstado *lista_estados, TipoListaEntrada *lista_entradas, 
              TipoMatriz ***mat) {
   
   char opAux[100];                         
   char op[100];
   char opAux2[MAX];
   char estadoOrigem[MAX];
   char estadoDestino[MAX];
   char entrada[MAX];
   char saida[MAX];
   EntradaDados *lDados = NULL;
   EntradaDados *ultDados;
   EntradaDados *auxDados;
   int j = 0;
   int k = 0;
   
   op[0] = '\0';
   opAux2[0] = '\0';
   /* percorre ate o fim do arquivo com as entradas */
   while (! feof(stdin)) {
      /* leitura da linha do arquivo e atribuicao desta a variaval opAux*/
      fgets(opAux, 100, stdin);
	  /* se chegou o final do arquivo para a execucao */
      if (feof(stdin)) {
         break;
      }
      
	  /* retirando os espacos da entrada */
      j = 0;
      for (k = 0; k < strlen(opAux) - 1; k++) {
	     /* verificacao dos espacamentos */
         if (opAux[k] != ' ') {
            op[j] = opAux[k];
            j++;
         }
      }
      op[j] = '\0';
      
	  /* se a entrada for valida */
      if (Interpreta(op, estadoOrigem, estadoDestino, entrada, saida)) {
	     /* se a lista de entrada de dados estiver vazia */
         if (lDados == NULL) {
            lDados = (EntradaDados*) malloc(sizeof(EntradaDados));
            ultDados = lDados;
         } else { /* se ja existerem elementos na lista */
            auxDados = (EntradaDados*) malloc(sizeof(EntradaDados));
            ultDados->prox = auxDados;
            ultDados = auxDados;       
         }
		 /* atribuicoes na lista */
         strcpy(ultDados->estOrigem, estadoOrigem);
         strcpy(ultDados->estDestino, estadoDestino);
         strcat(entrada, " ");
         strcpy(ultDados->entrada, entrada);
         strcpy(ultDados->saida, saida);
         ultDados->prox = NULL;
      } else { /* se houver erro de sintaxe na entrada */
         printf("error: formato de entrada invalido\n");
         return 0;
      }
      
   }
   /* cria matriz a partir da lista de dados lDados */
   CriaMatrizMEF(&lista_estados, &lista_entradas, mat, lDados);
   
   return 1;           
}

/*----------------------------------------------------------------------------*/

/* funcao que encontra o Transition Cover de uma MEF */

int Cria_Transition_Cover(TipoListaEstado lista_estados, TipoListaEntrada lista_entradas, 
                          TipoMatriz **mat ) {

   int i;
   int j;
   int k = 0;
   char transition_cover[MAX];
   TipoListaEstado auxEstado = lista_estados;
   TipoListaEntrada auxEntrada = lista_entradas;
   
   /* percorrendo a matriz toda */
   for (i = 0; i < auxEstado.cont; i++) {
      for (j = 0; j < auxEntrada.cont; j++) {
         /* se for uma transicao existente */	  
         if (strcmp(mat[i][j].estado, "*") != 0) {
		    /* transition_cover recebe o preambulo do estado */
            strcpy(transition_cover, auxEstado.pont->preamble);
			/* verifica se a MEF eh atingivel */
            if ((transition_cover[0] == '\0') && (strcmp(lista_estados.pont->elem, auxEstado.pont->elem) != 0)) {
			   /* se a MEF nao eh atingivel retorna 0 */
               return 0;                       
            }
			/* concatena a entrada aceita pelo estado */
            strcat(transition_cover, auxEntrada.pont->elem);
			/* atribuicao ao transition cover do estado */
            strcpy(auxEstado.pont->transition[k], transition_cover);
            k++;
         }
         auxEntrada.pont = auxEntrada.pont->prox;
      }
	  /* reiniciando laco de outro estado */
      strcpy(auxEstado.pont->transition[k], "##");
      auxEstado.pont = auxEstado.pont->prox;
      auxEntrada = lista_entradas;
      k = 0;                     
   }
   /* se obteve sucesso retorna 1 */
   return 1;                       
}

/*----------------------------------------------------------------------------*/

/* funcao que calcula o caminho da raiz ateh os nohs folha */

void calc_caminho_p( no_p * no, no_p * raiz, char * caminho ){
   no_p * aux = no;
   int i;
   int j;
   char str[MAX];
   char strAux[MAX];

   /* calculando caminho do noh folha ateh a raiz */
   str[0] = '\0';
   while( aux != raiz ) {
      strcat( str, aux->entrada );
      aux = aux->pai; /* calcula caminho atraves do ponteiro pai */
   }
   
   /* procedimento para inverter str1 */
   caminho[0] = '\0';
   i = 0;
   while(str[i] != '\0') {
      j = 0;
      while (str[i] != ' ') {
         strAux[j] = str[i];
         j++;
         i++;
      }
      i++;
      strAux[j] = ' ';
      strAux[j+1] = '\0';
	  /* invertendo e jogando em caminho */
      strcat(strAux, caminho);
      strcpy(caminho, strAux);
      strAux[0] = '\0';
   }
}

//------------------------------------------------------------------------------

/* funcao que atualiza os filhos de cada noh */

int atualiza_vetor_arvore_p ( TipoListaEstado lista_estados, TipoListaEntrada lista_entradas, 
                               TipoMatriz **mat,  no_p * vetor,  no_p * raiz, 
                               no_p ** ultimo, no_p * pai ) {
   int i;
   int j;
   int quant_no = 0; 
   noEstado *aux1 ;
   no_p * aux_lista = raiz;
   noEntrada * aux_lista_entradas = lista_entradas.pont;
   
   /* percorrendo os filhos do noh*/
   for (i = 0; i < lista_entradas.cont; i++) {
      j = 0;
      aux1 = lista_estados.pont;
	  /* procurando o pai de cada noh */
	  while (((aux1 != NULL) && (strcmp(pai->estado, aux1->elem) != 0))) {
         aux1 = aux1->prox;
         j++;
      }
      
	  /* atribuindo o estado ao vetor de filhos */
      strcpy( vetor[i].estado, mat[j][i].estado );
	  /* atribuindo a entrada que representa  */
      strcpy( vetor[i].entrada, aux_lista_entradas->elem );
	  /* atribuindo o pai de cada noh */
      vetor[i].pai = pai;
      aux_lista_entradas = aux_lista_entradas->prox;
      
	  /* verificando se o noh eh valido, ou seja, se existe transicao na MEF entre o pai e filho
	          verifica tambem se o filho eh igual ao pai
	     */ 
      if( (strcmp( vetor[i].estado , pai->estado ) == 0) || (strcmp(vetor[i].estado, "*") == 0)) {
	     /* nao valido */
         vetor[i].valido = 0;
      } else {
	     /* valido */
         vetor[i].valido = 1;
      }   

	  /* valor nulo para os filhos do vetor */
      vetor[i].entradas = NULL;
	  /* atribuicao do campo lista dos nohs do vetor */
      if( (i+1) < lista_entradas.cont ) {
         vetor[i].lista = vetor+(i+1);
      } else {
         vetor[i].lista = NULL;
         (*ultimo) = vetor+i;
      }
      
	  /* verificacao se todos elementos estao na arvore */
      aux_lista = raiz;
      while( (aux_lista != vetor+i) && (aux_lista != NULL) ) {
         if (!strcmp(aux_lista->estado, mat[j][i].estado)) {
            vetor[i].valido = 0;
            quant_no ++;
            break;
         }
         aux_lista = aux_lista->lista;
      }
   }
   
   /* se todos elementos estiverem na arvore retorna 1 */
   if ( lista_entradas.cont == quant_no ) {
      return 1;
   } else {
      return 0;
   }
}

//------------------------------------------------------------------------------

/* funcao que aloca o vetor p que representa os filhos de cada noh */

no_p * aloca_vetor_arvore_p( TipoListaEstado lista_estados, TipoListaEntrada lista_entradas,  
                             TipoMatriz **mat, no_p * no, no_p * raiz, 
                             no_p ** ultimo, int * fim ) {
   int i;	
   no_p * vetor;
   
   /* alocando o vetor do tipo no_p */
   vetor = (no_p *) malloc (sizeof(no_p)* lista_entradas.cont) ;
   
   /* se o no for a raizm entao a propria raiz aponta para vetor */
   if( no == raiz ) {
     no->lista = vetor+0;
   /* senao o ultimo aponta para vetor */
   } else {
     (*ultimo)->lista = vetor+0;
   }

   /* atribuindo os ponteiros lista */
   for (i = 0; i < (lista_entradas.cont-1); i++){
      vetor[i].lista = vetor+i+1;
   }
   vetor[lista_entradas.cont-1].lista = NULL;
   
   /* atualiza o vetor p alocado */
   *fim = atualiza_vetor_arvore_p( lista_estados, lista_entradas, mat, vetor,  
                                   raiz, ultimo, no );
   return vetor;
}

// ----------------------------------------------------------------------------

/* funcao que cria a arvore p para obtencao do preambulo */

int Cria_p( TipoListaEstado lista_estados, TipoListaEntrada lista_entradas, 
                        TipoMatriz **mat ) {

   no_p *raiz;
   no_p *aux;
   no_p *ultimo;
   TipoListaEstado aux2;
   int fim;
   int i; 
   int j;
   char caminho[MAX];

   /* alocacao da Raiz da arvore */
   raiz = (no_p *) malloc (sizeof(no_p));
   /* noh raiz recebe o estado inicial da mef */
   strcpy( raiz->estado, lista_estados.pont->elem  );
   raiz->pai = NULL; /* ponteiro pai aponta para null */
   raiz->valido = 1; /* raiz marcada como valida */
   /* alocacao do vetor que representa os filhos do noh raiz */
   raiz->entradas = aloca_vetor_arvore_p( lista_estados, lista_entradas, mat, raiz, raiz, &ultimo, &fim );
   /* lista da raiz aponta para a lista de entradas suas entradas */
   raiz->lista = raiz->entradas;
   
   /* percorre o vetor de entradas alocando os filhos de cada noh */
   aux = raiz->lista;
   while(  (aux != NULL)  ||  (fim == 0)  ) {
      if( aux->valido != 0 ) {
	     /* alocando os filhos de cada noh */
         aux->entradas = aloca_vetor_arvore_p( lista_estados, lista_entradas, mat, aux, raiz, 
                                               &ultimo, &fim );
      }
      aux = aux->lista;	
   }
   
   aux = raiz;
   j = 1;
   
   /* calculando o caminho da raiz ateh determinado noh */
   while( aux != NULL ) {
      for(i = 0 ; i < 20 ; i++) {
         caminho[i] = '\0';
      }
	  /* se for um noh valido */
      if (aux->valido == 1) {
         /* calculando caminho da raiz ateh o noh */	  
         calc_caminho_p( aux, raiz, caminho );
         
         aux2 = lista_estados;
         while (strcmp(aux->estado, aux2.pont->elem) != 0) {
            aux2.pont = aux2.pont->prox;
         }
		 /* atribuicao do caminho ao campo do preambulo do estado */
         strcpy(aux2.pont->preamble, caminho);
      }
      aux = aux->lista;
      j++;
   }
   
   /* cria o conjunto transition cover */
   i = Cria_Transition_Cover(lista_estados, lista_entradas, mat);
   
   /* se transition cover retorna 0 indica uma MEF nao atingivel */
   if (i == 0) {
      printf("error: a MEF inserida nao eh atingivel\n");
      return 0;   
   }
   
   return 1;
}

/*----------------------------------------------------------------------------*/

/* funcao que indica se str2 eh prefixo de str1 */

int eh_prefixo(char str1[MAX], char str2[MAX]) {
   int i;
   char strAux[MAX];

   for (i = 0; i < strlen(str2); i++) {
      strAux[i] = str2[i];
      strAux[i+1] = '\0';
	  /* verificando se eh prefixo */
      if (strcmp(strAux, str1) == 0) {
         return 1;
      }
   }
   return 0;
}

/*----------------------------------------------------------------------------*/

/* Encontra o conjunto final de testes da MEF */

int Conjunto_teste(TipoListaEstado lista_estados, TipoListaEntrada lista_entradas, 
                   TipoMatriz **mat) {

   TipoListaEstado auxEst = lista_estados;
   TipoListaEstado hsi;
   int k = 0;
   int i;
   int j;
   TipoMatriz **matAux = mat;
   char estado[MAX];
   char hsiAux[MAX];
   
   /* percorrendo a lista de estados */
   for (i = 0; i < auxEst.cont; i++) {
	  /* enquanto o estado for valido */
      while (strcmp(auxEst.pont->transition[k], "##") != 0) {
         strcpy(estado, lista_estados.pont->elem);
		 /* executa-se a mef com o transition cover do estado */
         executa(lista_estados, lista_entradas, matAux, estado, auxEst.pont->transition[k], &hsi, 0);
         j = 0;
		 /* enquanto hsi estiver vazio */
		 /* impresssao do conjunto final de teste */
         while (strcmp(hsi.pont->HSI[j], "##") != 0) {
            strcpy(hsiAux, hsi.pont->HSI[j]);
			/* Exemplo de formato de impressao: 
			   seq: a b b b a b a
               seq: b a b b			   
			*/
            printf("seq: ");
            printf("%s", auxEst.pont->transition[k]);
            printf("%s", hsiAux);
            printf("\n");
            j++;
         }
         k++;
      }     
      k = 0;
      auxEst.pont = auxEst.pont->prox;  
   }
   return 0;
}


/*----------------------------------------------------------------------------*/

/* funcao que encontra o conjunto HSI para cada estado da MEF */

int achaHSI(TipoListaEstado lista_estados, TipoListaEntrada lista_entradas, 
            TipoMatriz **mat, noHSI *vetHSI) {

   char straux1[MAX]; 
   char straux2[MAX];
   char D[200];
   seqHSI *auxSeq;
   TipoListaEstado aux1 = lista_estados;
   TipoListaEstado aux2 = aux1;
   seqHSI *VetorSeqHSI;
   seqHSI *AuxSeqHSI, *AntAuxSeqHSI;
   TipoListaEntrada auxEnt1;
   int i;
   int j;
   int m;
   int n;
   int o;
   int f;
   int terminou = 0;
   int passou, achou, flag;
   int k = -1;
   int l1 = 0;
   int l2 = 0;
   
   /* preenchendo os estados no vetor HSI */
   for (i = 0; i < lista_estados.cont; i++) {
      for (j = i + 1; j < lista_estados.cont; j++) {
         k++;
		 /* estado 1 */
         strcpy(vetHSI[k].est1, aux1.pont->elem);
         aux2.pont = aux2.pont->prox;
		 /* estado 2 */
         strcpy(vetHSI[k].est2, aux2.pont->elem);  
      }
      aux1.pont = aux1.pont->prox;
      aux2 = aux1;
   }
   /* tamanho do vetor HSI */
   k = lista_estados.cont*(lista_estados.cont - 1)/2;
   passou = 0;
   
   /* atribuicao null para o ponteiro Shsi de cada noh do vetor HSI */
   for (i = 0; i < k; i++) {
      vetHSI[i].Shsi = NULL;
   }
   
   /* enquanto nao atribui HSI para todo par de estado */
   while (terminou < k) {
      /* percorrendo todo o vetor HSI */
      for (i = 0; i < k; i++) {
	     /* na primeira passada pelo vetor aloca-se o pontHSI para todo noh do vetor HSI */
         if (! passou) {
		    /* alocacao de pontHSI */
            vetHSI[i].Phsi = (pontHSI *) malloc(sizeof(pontHSI)*lista_entradas.cont);
            for (n = 0; n < lista_entradas.cont; n++) {
			   /* atribuicao null para o vetor de pontHsi de cada noh do vetor HSI */
               vetHSI[i].Phsi[n].pont = NULL;
            }
         }
         f = 0;
         for(j = 0; j < lista_entradas.cont; j++) {
            l1 = 0;
            aux1 = lista_estados;
			/* buscando estado1 do vetor HSI na lista de estados */
            while (strcmp(vetHSI[i].est1, aux1.pont->elem) != 0) {
               aux1.pont = aux1.pont->prox;
               l1++;
            }         
            l2 = 0;
            aux1 = lista_estados;
			/* buscando estado2 do vetor HSI na lista de estados */
            while (strcmp(vetHSI[i].est2, aux1.pont->elem) != 0) {
               aux1.pont = aux1.pont->prox;
               l2++;
            }
            /* atribuicao da transicao dos dois estados de vetHSI com determinada entrada */	
            strcpy(straux1, mat[l1][j].estado);
            strcpy(straux2, mat[l2][j].estado);
         
            achou = 0;
			/* percorrendo vetor HSI */
            for (m = 0; m < k; m++) { /* procurando par de estado da execucao no vetorHSI */
               /* se par de estados da transicao existir no vetor HSI */
			   if (((strcmp(vetHSI[m].est1, straux1) == 0) || (strcmp(vetHSI[m].est2, straux1) == 0)) && 
                  (((strcmp(vetHSI[m].est2, straux2) == 0) || (strcmp(vetHSI[m].est1, straux2) == 0)))) {
                  /* se as saidas forem diferentes indica um par de estado "fail" */
                  if (strcmp(mat[l1][j].saida, mat[l2][j].saida) != 0) {
                     vetHSI[i].flag = 'f'; /* par de estado marcado como fail */
					 /* alocacao do vetor para armazenamento da entrada identificadora */
                     VetorSeqHSI = (seqHSI *) malloc(sizeof(seqHSI));
                     auxEnt1 = lista_entradas;
                     for (o = 0; o < j; o++) {
                        auxEnt1.pont = auxEnt1.pont->prox;
                     }
					 /* atribuicao da entrada identificadora no fim da lista de armazenamento */
                     strcpy(VetorSeqHSI->entrada, auxEnt1.pont->elem);
                     VetorSeqHSI->prox = NULL;
                     VetorSeqHSI->phsi = NULL;
                     AuxSeqHSI = vetHSI[i].Shsi;
                     AntAuxSeqHSI = AuxSeqHSI;
                     /* organizando os ponteiros */
                     while (AuxSeqHSI != NULL) {   
                        AntAuxSeqHSI = AuxSeqHSI;   
                        AuxSeqHSI = AuxSeqHSI->prox;
                     }
                     /* se o par de estados nao contem HSI entao o valor eh atribuido no inicio */
                     if (vetHSI[i].Shsi == NULL) {
                        vetHSI[i].Shsi = VetorSeqHSI;
                     } else {
					    /* senao eh inserido no fim da lista */
                        AntAuxSeqHSI->prox = VetorSeqHSI;
                     } 
                     terminou++;
                  } else { /* se saidas nao forem diferentes */
				     /* percorrendo a lista de entrada */
                     for (n = j; n < lista_entradas.cont; n++) {
					    /* se o noh apontar para algum outro */
                        if (vetHSI[i].Phsi[n].pont != NULL) {
						   /* se esse outro noh for fail ou caminho para algum fail */
                           if ((vetHSI[i].Phsi[n].pont->flag == 'f') || (vetHSI[i].Phsi[n].pont->flag == 'r')) {
						      /* aloca-se seqHSI */
                              VetorSeqHSI = (seqHSI *) malloc(sizeof(seqHSI));
                              auxEnt1 = lista_entradas;
                              for (o = 0; o < j; o++) {
                                 auxEnt1.pont = auxEnt1.pont->prox;
                              }
							  /* atribuicao da entrada identificadora no fim da lista de armazenamento */
                              strcpy(VetorSeqHSI->entrada, auxEnt1.pont->elem);
                              VetorSeqHSI->phsi = vetHSI[i].Phsi[n].pont->Shsi;
                              vetHSI[i].Shsi = VetorSeqHSI;
							  /* noh marcado como caminho para algum noh fail */
                              vetHSI[i].flag = 'r';
                              break;
                           }
                        } else {
                           vetHSI[i].flag = 's';
                           break;
                        }  
                     }        
                  }
				  /* atribuicao de Phsi ao noh dos estados obtidos na transicao */
                  vetHSI[i].Phsi[j].pont = vetHSI + m;
                  achou = 1;
                  break;  
               }
            }
			/* se nao achou */
            if (achou == 0) {
               vetHSI[i].Phsi[j].pont = NULL;
               f++;
            }
			/* se terminou a execucao */
            if (f == lista_entradas.cont) {
               vetHSI[i].flag = 'n';
               terminou++;
            }
         }      
      }
      passou = 1;
   } 
   /* zerando o campo HSI na lista de estados */ 
   aux1 = lista_estados;
   for (j = 0; j < lista_estados.cont; j++) {
      /* ## indica null */
      strcpy(aux1.pont->HSI[0], "##");
      aux1.pont = aux1.pont->prox;
   }
  
   /* atribuicao final  */
   aux1 = lista_estados;
   j = 0;
   /* percorrendo a lista de estado */
   while (aux1.pont != NULL) {
      /* percorrendo o vetor HSI */
      for (i = 0; i < k; i++) {
         auxSeq = vetHSI[i].Shsi;
		 /* variavel auxiliar para tratanento do conjunto hsi */
         D[0] = '\0';
		 /* obtendo D de cada noh de vetHSI */
         while (auxSeq != NULL) {
            strcat(D, auxSeq->entrada);
            auxSeq = auxSeq->phsi;
         }
		 /* se encontrar algum dos estados do par na lista de estados */
         if ((strcmp(aux1.pont->elem, vetHSI[i].est1) == 0) ||
             (strcmp(aux1.pont->elem, vetHSI[i].est2) == 0)) {
            flag = 1;
            f = 0;
            /* enquanto hsi for diferente de "null" */	
            while (strcmp(aux1.pont->HSI[f], "##") != 0) {
			   /* verificacao de prefixo */
               if (eh_prefixo(D, aux1.pont->HSI[f])) {
                  flag = 0;
               } else if (eh_prefixo(aux1.pont->HSI[f], D)) {
                  strcpy(aux1.pont->HSI[f], D);
                  flag = 0;
               }
               f++;
            }
            /* se hsi for nulo */
            if (flag && (D[0] != '\0')) {
               strcpy(aux1.pont->HSI[j], D);
               j++;
               strcpy(aux1.pont->HSI[j], "##"); 
            } 
         }
      }
      strcpy(aux1.pont->HSI[j], "##");
      j = 0;
      aux1.pont = aux1.pont->prox;
   }
   
   return 1;
}

/*----------------------------------------------------------------------------*/

/* Funcao principal main */

int main() {
   noHSI *VetorHSI;
   TipoListaEstado lista_estados;   
   TipoListaEntrada lista_entradas; 
   TipoMatriz **matriz;  
   lista_estados.pont = NULL;
   lista_entradas.pont = NULL;
   lista_estados.cont = 0;
   lista_entradas.cont = 0;
   
   /* insere MEF no sistema
      se for inserida um entrada com erro de sintaxe retorna 0 e para a execucao   
   */
   if (! InsereMEF(&lista_estados, &lista_entradas, &matriz)) {
      return 0;
   }
   
   /* Se a MEF for atingivel cria a arvore p */
   if (Cria_p( lista_estados, lista_entradas, matriz)) {         

      /* criacao do vetor HSI */   
      VetorHSI = (noHSI *) malloc(sizeof(noHSI)*(lista_estados.cont*(lista_estados.cont - 1)/2));
      /* algoritmo para obtencao do conjunto HSI */   
      achaHSI(lista_estados, lista_entradas, matriz, VetorHSI);
      /* obtencao do conjunto final de teste */            
      Conjunto_teste(lista_estados, lista_entradas, matriz);
    
   } else {
      return 0;
   }
   /* se obteve sucesso durante toda a operacao retorna 0 */
   return 1;
}
