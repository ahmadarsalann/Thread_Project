#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int main(){
	int min = 0;
	int max = 3;
	srand(time(NULL));

	for(int i = 0; i < 10; i++){
		int random = (rand() % (max - min + 1) + min);
		printf("r= %d\n", random);
	}
}

