/** @file eeprom.h
*	@brief Function and datatype declarations.
*	
*	This file contains the private functions and API
*	functions to read and write from simulated eeprom.
* 	eeprom is simulated using a text file.
*
*	@author Atharv
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>

#include <assert.h>

#ifndef EEPROM_H
#define EEPROM_H

/**
*	@brief Data structure to hold offset parameters pf the query.
*
*	@param page_number Stores the required page of the eeprom.
*	@param page_offset Stores the the start offset for the page.
*	@param end_offset Stores the end offset for the page.
*/
typedef struct position {
	uint8_t page_number;
	uint16_t page_offset;
	uint16_t end_offset;
} position;

/**	@brief API function to read from eeprom.
*
*	The functions fetches the required size from the memory offset and
*	stores the data in buffer.
*
*	@param offset The offset in bytes for the starting point to read data. 
*	@param size The size in bytes to read data from the offset.
*	@param buf Pointer to the buffer to store the read data.
*	@return Void.
*/
void eeprom_read(uint32_t offset, int size, char *buf);

/**	@brief API function to write to eeprom.
*
*	The functions writes the required size from the buffer to
*	the memory offset.
*
*	@param offset The offset in bytes for the starting point to write data. 
*	@param size The size in bytes to write data from the offset.
*	@param buf Pointer to the buffer of data to be written.
*	@return Void.
*/
void eeprom_write(uint32_t offset, int size, char *buf);

/**	@breif Private function to create .txt file for simulated eeprom.
*
*	The function creates a .txt file for simulated eeporm with given page size
*	and total pages. Initially all values are written as 0xFF. Enabling verbose flag
*	makes a new line in the .txt for a new page. 
*
*	@param page_size Number of bytes allocated for each page.
*	@param total_pages Total pages present in the simulated eeprom.
*	@verbose Set to improve readability of .txt file.
*	@return Void. 
*/
void _create_eeprom(uint32_t page_size, uint16_t total_pages, bool verbose);

/**	@brief Computes the offset parameters from memory offset and size.
*
*	The functions computes asscoiated page number, page offset and the end offset
*	and stores in the position datastructure.
*
*	@param memeory_offset The offset in bytes for the starting point for any eeprom operation.
*	@param size The size in bytes to perform an operarion from the offset. 
*	@return position data structure with all the computed values. 
*/
position _get_starting_position(uint32_t memory_offset, int size);

#endif
