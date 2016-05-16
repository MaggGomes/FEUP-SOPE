#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define NUM_CONTROLLER 4
#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

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




int main (int argc, char * argv[]){

  if (argc != 3 ){
		fprintf(stderr, "Usage: %s <N_LUGARES> <T_ABERTURA>\n", argv[0]);
		exit(1);
	}




  return 0;
}
