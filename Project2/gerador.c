#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
O programa Gerador, de forma pseudo-aleatória, “cria” viaturas e associa um novo thread a cada uma.
Cada thread associado a uma viatura (thread “viatura”) acompanha-la-á no seu “ciclo de vida”, desde que é
“criada”, passando pelo acesso e eventual estacionamento no Parque, até que sai do Parque se nele tiver
estacionado e “desaparece”.
*/

void* viatura(void* arg);

int main (int argc, char * argv[]){

  if (argc != 3 ){
		fprintf(stderr, "Usage: %s <T_GERACAO> <U_RELOGIO>\n", argv[0]);
		exit(1);
	}


  return 0;
}

//TODO - IMPLEMENTAR

void* viatura(void* arg) {
	pthread_t selfThread = pthread_self();
	
	if (pthread_detach(selfThread) != 0){
		perror("Failed to make thread number % detached.\n", selfThread);
	exit(1);
	}
	
	
}
