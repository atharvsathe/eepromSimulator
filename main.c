/** @file main.c
*	@brief Driver code for eeprom library.
*	
*	This file contains the driver code and helper functions
*	that interface with eeprom API functions.
*
*	@author Atharv
*/

#include "eeprom.h"
#include <getopt.h>

/**	@brief Generates random data to write in eeprom.
*
*	The functions computes a random char in range 97 - 123
*	and fills the buffer of specified size with that character.
*
*	@param size The size in bytes of the buffer.
*	@param buf Pointer to the buffer to store the generated data.
*	@return Void.
*/
void generate_data(int size, char* buf);

/**	@brief Reads memory traces and calls appropriate API functions.
*
*	The functions reads the operation, offset, size from the memory trace
*	and calls read/ write function accordingly.
*	When a new thread is created for each trace file, this function is 
*	assigned to the tread
*
*	@param vargp Pointer to the char that contains the name of the file.
*	@return Void.
*/
void *run_eeprom(void *vargp);

/* Buffer size limit to print on the screen */
const int print_limit = 100;

/* Verbose flag to print outputs */
static bool verbose = true;

/* Mutex to print without interruption */
static pthread_mutex_t print_mutex;

/**	@brief Main driver code.
*
*	The functions reads command line options and sets up the simulated
*	eeprom accordingly. It also runs the mentioned trace file.
*	If nothing is specifieed, 64Kb eeprom is initialised with 32bytes
*	page size. It runs all 8 traces simultaneously on dedicated thread.
*
*	@param argc Number of parameters passed from the command line.
*	@param *argv[] Array of the parameters passed from the command line.
*	@return Integer to indicate completion and exit the program.
*/
int main(int argc, char *argv[]) {
	/* Default page size set to 32 bytes */
	static uint32_t page_size = 32;

	/* Defalut number of pages set to 256 */
	static uint16_t total_pages = 256;

	pthread_mutex_init(&print_mutex, NULL);

	/* All trace files to be executed */
	char read_small[] = "traces/read_small.trace";
	char write_small[] = "traces/write_small.trace";
	char test_small[] = "traces/test_small.trace";
	char read_big[] = "traces/read_big.trace";
	char write_big[] = "traces/write_big.trace";
	char test_big[] = "traces/test_big.trace";
	char toy[] = "traces/toy.trace";
	char edge[] = "traces/edge.trace";
	char *all_traces[] = {read_small, write_small, test_small,
		read_big, write_big, test_big, toy, edge};
	static unsigned num_files = sizeof(all_traces) / sizeof(all_traces[0]);

	static int opt;
	char file[100];

	/* Parse command line options */
	while ((opt = getopt(argc, argv, "p:n:t:v:")) != -1) {
		switch (opt) {
			case 'p': page_size = (uint32_t)atoi(optarg); break;
			case 'n': total_pages = (uint16_t)atoi(optarg); break;
			case 't': memcpy(file, optarg, 99); break;
			case 'v': verbose = atoi(optarg) == 0 ? false : true;break;
			default: exit(EXIT_FAILURE);
		}
	}
	if (strstr(file, "traces") != NULL) {
		all_traces[0] = file;
		num_files = 1;
	}

	/* Initialize simulated eeprom */
	_create_eeprom(page_size, total_pages, verbose);
  	pthread_t tid[num_files];
  	
  	/* Create multiple threads for each trace file */
  	for (unsigned count = 0; count < num_files; count++) {
  		pthread_create(&tid[count], NULL, run_eeprom, (void *)all_traces[count]); 
  	}

  	/* Wait for all threads to finish */
  	for (unsigned count = 0; count < num_files; count++) {
  		pthread_join(tid[count], NULL);
  	}

  	printf("DONE\n");
	return 0;
}

void generate_data(int size, char* buf) {
	int random = 97 + rand() % 26;
	for (int count = 0; count < size; count++) {
		memcpy(buf, &random , 1);
		buf += 1;
	}
}

void *run_eeprom(void *vargp) {
	char *filename = (char *)vargp;
	
	char op, space, comma;
  	uint32_t offset;
  	int size;

	FILE* trace;
	trace = fopen(filename, "r");
	assert(trace != NULL);

	while (fscanf(trace, "%c %c %u %c %d", &space, &op, &offset, &comma, &size) == 5) {
		char data[size + 1];
		data[size] = '\0';

		/* Performs read operation */ 
		if (op == 'R') {
			eeprom_read(offset, size, data);
		} 

		/* Performs write operation */
		else if (op == 'W') {
			generate_data(size, data);
			eeprom_write(offset, size, data);
		}

		/* Print routine */
		pthread_mutex_lock(&print_mutex);
			if (verbose == true) {
				if (size < print_limit)
					printf("%s offset:%u operation:%c size:%d data: %s\n",filename, offset, op, size, data);
				else 
					printf("%s offset:%u operation:%c size:%d\n",filename, offset, op, size);
			}
		pthread_mutex_unlock(&print_mutex);
	}
	return NULL;
}
