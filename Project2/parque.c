#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time>

#define NUM_CONTROLLERS 4
#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

// Global variables
const char* LOG_FILENAME = "parque.log";
const char* LOG_FULL = "cheio";
const char* LOG_CLOSED = "encerrado"
const char* LOG_PARKING = "estacionamento";
const char* LOG_EXIT= "saida";
const char* FIFO_CONTROLLERS[NUM_CONTROLLERS] = {"fifoN", "fifoS", "fifoE", "fifoO"};

static int numPlaces;
static int openTime;
static int occupiedPlaces = 0;
static clock_t startT;
static pthread_mutex_t mutexPark = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	clock_t parkingTime;
	char entryName;
	int idParkingPlace;
	int id;	
} vehiculeInfo


/*
///APAGAR
2 níveis de threads: num nível, dá-se o atendimento das chegadas
pelos 4 threads “controladores”, cada um associado a um dos pontos de acesso (entrada/saída); a outro
nível, dá-se o atendimento “personalizado” de cada viatura, mediante a criação “na hora” de um thread
”arrumador” que a acompanha durante a sua permanência no Parque: ou a rejeita logo de início ou a deixa
estacionar durante o tempo solicitado. Cada evento relevante, e.g. pedido de acesso de uma viatura ou
saída de outra, serão anotados num ficheiro de registos, e sempre associados ao instante de ocorrência
(em clock ticks).
*/

/*
// APAGAR - COMUNICAÇÃO ENTRE PARQUE E GERADOR
A comunicação entre os processos Parque e Gerador é efetuada por intermédio de pipes com nome
(FIFOs). Basicamente, cada thread “viatura” coloca no acesso estipulado na geração da viatura um pedido
de estacionamento, que será recebido pelo thread “controlador” desse acesso e respondido (positiva ou
negativamente) pelo respetivo thread “arrumador” do Parque. O conjunto dos threads "arrumador” do
Parque (um por viatura que chega ao Parque), coordenam-se na gestão do número de lugares disponíveis
por meio de primitivas de sincronização apresentadas nas aulas (mutexes, variáveis de condição,
semáforos). Sempre que possível, todas as atividades ligadas à simulação deverão decorrer sem “esperas
ativas” (busy waiting).
*/

// TODO - colocar na makefile -pthread

// TODO - IMPLEMENTAR ESTAS FUNÇÕES QUE VAI SER USADAS PARA A EXECUÇÃO DOS THREADS

void* controller(void* arg);

void* carSaver(void* arg);




int main (int argc, char * argv[]){
	
	/*
	 inicializa as variáveis globais necessárias, incluindo o temporizador geral que controla o tempo de
abertura do Parque;
 cria os 4 threads “controlador”, um para cada acesso, e aguarda que eles terminem;
 finalmente, efetua e publica estatísticas globais. 
*/


	
	pthread_t controllerThreads[NUM_CONTROLLERS];
	int i;

  if (argc != 3 ){
		fprintf(stderr, "Usage: %s <N_LUGARES> <T_ABERTURA>\n", argv[0]);
		exit(1);
	}
	
	// TODO - LER ARGUMENTOS PASSADOS
	
	// TODO - ABRIR O FICHEIRO LOGGER


// TODO - CRIAR UM THREAD PARA CADA CONTROLADOR E NO FINAL CHAMAR SLEEP(OPENTIME), DE MODO
// A O PROGRAMA(PARQUE) ESTEJA ABERTO O TEMPO REQUERIDO

startT = clock();

	for (i = 0; i  < NUM_CONTROLLER; i++){
		pthread_create(controllerThreads[i], NULL, controller, FIFO_CONTROLLERS[i]);
	}

	// TODO - USAR ALARM?
			sleep(openTime);
	
	
	// Waits for the thread of each park's controller
	for (i = 0; i < NUM_CONTROLLERS; i++){
		pthread_join(controllerThreads[i], NULL);
	}
	
  pthread_exit(0);
}

// TODO - FAZER
void* controller(void* arg){
	
	/*
	criar o seu FIFO próprio (identificado por “fifo?”, onde '?' será ou N ou E, ou S ou O);
 receber pedidos de acesso através do seu FIFO; cada pedido inclui os seguintes dados:
◦ porta de entrada;
◦ tempo de estacionamento (em clock ticks);
◦ número identificador único da viatura (inteiro);
◦ nome do FIFO privado para a comunicação com o thread “viatura” do programa Gerador.
 criar um thread “arrumador” para cada pedido de entrada recebido e passar-lhe a informação
correspondente a esse pedido;
 estar atento a uma condição de terminação (correspondendo à passagem do T_ABERTURA do
Parque) e, nessa altura, ler todos os pedidos pendentes no seu FIFO e fechá-lo para que potenciais
novos clientes de estacionamento sejam notificados do encerramento do Parque; encaminhar os
últimos pedidos a correspondentes thread “arrumador”. 
*/
	
	
	
}

// TODO - FAZER
void* carSaver(void* arg){
	pthread_t selfThread = pthread_self();
	
	if (pthread_detach(selfThread) != 0){
		perror("Failed to make thread number % detached.\n", selfThread);
	exit(1);
	}
	
	/*
	recolher a informação sobre a viatura de que está encarregue;
 verificar se há lugar para o estacionamento da viatura ou não (nota: esta operação deverá ser
executada de forma sincronizada com todos os outros threads do mesmo tipo, não havendo
competição por um lugar concreto do Parque!)
◦ se não houver vaga (Parque cheio), nega à viatura o acesso ao Parque, mediante a colocação
de uma mensagem disso indicativa (“cheio!”) no FIFO privado da viatura, que será lida pelo
respetivo thread “viatura” do programa Gerador;
◦ se houver vaga no Parque, reserva-a (evitando condições de competição – race conditions –
com todos os outros threads do mesmo tipo) e envia, pelo FIFO privado da viatura, uma
mensagem esclarecedora do estacionamento (“entrada”) ao respetivo thread “viatura”; neste
caso a viatura ‘desaparece’.
 na sequência da admissão de uma viatura ao Parque, ligar um temporizador local, para controlar o
tempo de estacionamento da viatura, e quando o prazo terminar, “acompanhar a viatura à saída do
Parque”, colocando uma mensagem indicativa ("saida") no FIFO privado da viatura, que será lida
pelo respetivo thread “viatura”;
 na sequência da saída de uma viatura do
Parque, dar baixa do lugar ocupado, atualizando
(de forma sincronizada com todos os outros
threads do mesmo tipo) a contagem do número
de lugares vagos no Parque. 
*/
	
	
}
