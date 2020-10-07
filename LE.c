#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

/*Variables Globales*/

	int rc=0;
	int wc=0;
	
	typedef struct archivo{
		int numero;
	}Archivo;

	unsigned int num_escritores, num_lectores;
	FILE * baseDatos;

/*Semaforos*/

	sem_t mutex1,mutex2,rdb,wdb; //incluye exclusion mutua de semaforos en las primeras dos variables

/*Funciones*/

	void *lector(void *p);
	void *escritor(void *p);
	void escribirenbasedatos();
	void leerenbasedatos();


int main(int argc, char *argv[]){

	int estado,i;
	extern FILE * baseDatos;
	baseDatos=fopen("baseDatos.txt","a");
	fclose(baseDatos);
	
	if ( argc < 3 ) {
    	printf("Numero de parametros incorrecto. Indicar el numero de datos a producir.\n") ;
    	exit(1);              
    }
	
	num_escritores = atoi(argv[1]);
	num_lectores = atoi(argv[2]);
	
	int eid[num_escritores],lid[num_lectores];

	/*Declaración de las dos hilos*/

	pthread_t escritora[num_escritores],lectora[num_lectores];

	/*Inicialización de Semáforos*/

	sem_init(&mutex1,0,1);
	sem_init(&mutex2,0,1);
	sem_init(&rdb,0,1);
	sem_init(&wdb,0,1);
	
	/*Creaccion de los hilos*/
	
	for(i=0;i<num_lectores;i++){
		lid[i]=i;
		estado=pthread_create(&lectora[i],NULL,lector,(void *) &lid[i]);
			if(estado!=0)
				printf("Error al cargar la hebra lectora\n");
	}

	for(i=0;i<num_escritores;i++){
		eid[i]=i;
		estado=pthread_create(&escritora[i],NULL,escritor,(void *) &eid[i]);
		if(estado!=0)
			printf("Error al cargar la hebra escritora\n");
	}
	
	/*Ejecucion con los hilos*/

	for(i=0;i<num_lectores;i++)
		pthread_join(lectora[i],NULL);
		
	for(i=0;i<num_escritores;i++)
		pthread_join(escritora[i],NULL);
	

	/*Destrucción de los semaforos*/

	sem_destroy(&mutex1);
	sem_destroy(&mutex2);
	sem_destroy(&rdb);
	sem_destroy(&wdb);

	return 0;

}

void escribirenbasedatos(){

	extern FILE *baseDatos;
	char c='0';
	int aux= rand() % 11;
	baseDatos=fopen("baseDatos.txt","w");
	c=aux+47; //conversion ASCII
	printf("El caracter es: %c \n",c);
	fputc(c,baseDatos);
	
	fclose(baseDatos);

}

void leerenbasedatos(){
	
	extern FILE *baseDatos;
    Archivo p;
                                                        
	baseDatos=fopen("baseDatos.txt","r");

	while (fscanf(baseDatos,"%d\n",&p.numero)==1){

		printf("Numero:\n");
		printf("%d\n",p.numero);
	}

	fclose(baseDatos);

}

void *escritor(void *p){

	//Saber cual es el escritor por su ID
	int *id_es;
    id_es=(int *)p;

	sem_wait(&mutex1);
	wc++;

	//Si hay lector espera a que termine
	if(wc == 1)
	sem_wait(&rdb);

	sem_post(&mutex1);
	sem_wait(&wdb);
	printf("\nEscritor %d Escribiendo...\n",*id_es);
	escribirenbasedatos(*id_es);
	sem_post(&wdb);
	sem_wait(&mutex1);
	wc--;
	if(wc == 0)
		sem_post(&rdb);
	
	sem_post(&mutex1);
	pthread_exit(NULL);
}

void *lector(void *p){

	//Saber cual es el lector por su ID
	int *id_le;
    id_le=(int *)p;	

	sem_wait(&rdb);
	sem_wait(&mutex2);
	rc++;
	
	//Si hay escritor espera a que termine
	if(rc == 1)
	sem_wait(&wdb);

	sem_post(&rdb);
	printf("\nLector %d Leyendo...\n",*id_le);
	leerenbasedatos(*id_le);
	sem_post(&mutex2);
	sem_wait(&mutex2);
	rc--;
	if(rc == 0)
	sem_post(&wdb);

	sem_post(&mutex2);
	pthread_exit(NULL);
	
}

