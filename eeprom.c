/** @file eeprom.c
*	@brief Function implementation defined in eeprom.h
*	
*	This file contains the implementation of private 
*	functions and API functions to read and write from s
*	imulated eeprom.
*
*	@author Atharv
*/

#include "eeprom.h"

/* File name of simulated eeprom */
const char *FILE_NAME = "eeprom.txt";

/* Mode to open the simulated eepro file */
const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

/* Place holder for page size in bytes */
static uint32_t PAGE_SIZE = 0;

/* Place holder for total pages */
static uint16_t TOTAL_PAGES = 0;

/* Sets depending on verbose flag */
static uint8_t display_offset = 0;

/* Mutex variable to define critical sections */
static pthread_mutex_t mutex;

void _create_eeprom(uint32_t page_size, uint16_t total_pages, bool verbose) {
	if (verbose == true) 
		display_offset = 1; 
	PAGE_SIZE = page_size;
	TOTAL_PAGES = total_pages;
	int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, mode);
	pthread_mutex_init(&mutex, NULL);

	for (unsigned int page_count = 0; page_count < TOTAL_PAGES; page_count++) {
		char buf[PAGE_SIZE + display_offset];
		memset(buf, 0xff, PAGE_SIZE);
		if (display_offset == 1)
			buf[PAGE_SIZE] = '\n';

		ssize_t response = write(fd, buf, PAGE_SIZE + display_offset);
		assert(response == PAGE_SIZE + display_offset);
	}
	close(fd);
}

position _get_starting_position(uint32_t memory_offset, int size) {
	position starting_pos;
	starting_pos.page_offset =  (uint16_t)(memory_offset % PAGE_SIZE);
	starting_pos.page_number =  (uint8_t)(memory_offset / PAGE_SIZE);	
	starting_pos.end_offset = (uint16_t)((starting_pos.page_offset + ((unsigned)size % PAGE_SIZE)) % PAGE_SIZE);
	starting_pos.end_offset = (uint16_t)(PAGE_SIZE - starting_pos.end_offset);
	starting_pos.end_offset = starting_pos.end_offset % PAGE_SIZE;
	return starting_pos;
}

void eeprom_read(uint32_t offset, int size, char *buf) {
	position starting_pos = _get_starting_position(offset, size);	

	while (size > 0 && 0 <= starting_pos.page_number < TOTAL_PAGES) {
		char start[PAGE_SIZE];
		uint32_t size_offset =  0;
		const off_t offset = starting_pos.page_number * (PAGE_SIZE + display_offset);

		if ((unsigned)size <= PAGE_SIZE - starting_pos.page_offset) {
			size_offset = starting_pos.end_offset;
		}

		pthread_mutex_lock(&mutex);
		int fd = open(FILE_NAME, O_RDONLY, mode);
		ssize_t response = pread(fd, start, PAGE_SIZE, offset);
		close(fd);
		pthread_mutex_unlock(&mutex);

		assert(response = PAGE_SIZE);
		memcpy(buf, start + starting_pos.page_offset, PAGE_SIZE - starting_pos.page_offset - size_offset);

		buf += PAGE_SIZE - starting_pos.page_offset - size_offset;
		size -= PAGE_SIZE - starting_pos.page_offset - size_offset;
		starting_pos.page_number += 1;
		starting_pos.page_offset = 0;
	}
	
}

void eeprom_write(uint32_t offset, int size, char *buf) {
	position starting_pos = _get_starting_position(offset, size);

	while (size > 0 && 0 <= starting_pos.page_number < TOTAL_PAGES) {
		char start[PAGE_SIZE];
		uint32_t size_offset =  0;
		const off_t offset = starting_pos.page_number * (PAGE_SIZE + display_offset);
		if ((unsigned)size <= PAGE_SIZE - starting_pos.page_offset) {
			size_offset = starting_pos.end_offset;
		}

		pthread_mutex_lock(&mutex);
		int fd = open(FILE_NAME, O_RDWR, mode);
		if (starting_pos.page_offset != 0 || size_offset != 0) {
			ssize_t response = pread(fd, start, PAGE_SIZE, offset);
			assert(response = PAGE_SIZE);
		}
		memcpy(start + starting_pos.page_offset, buf, PAGE_SIZE - starting_pos.page_offset - size_offset);
		ssize_t response = pwrite(fd, start, PAGE_SIZE, offset);
		close(fd);
		pthread_mutex_unlock(&mutex);

		assert(response = PAGE_SIZE);

		buf += PAGE_SIZE - starting_pos.page_offset - size_offset;
		size -= PAGE_SIZE - starting_pos.page_offset - size_offset;
		starting_pos.page_number += 1;
		starting_pos.page_offset = 0;
	}
	
}



