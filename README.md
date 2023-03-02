# EEPROM simulator

## Specifications
EEPROM is of 64Kb with 8192 words of 8 bits. 
The memory has 32 bytes page access. There are total 256 pages.

There are 2 API functions.
- void eeprom_read(uint32_t offset, int size, char *buf)
- void eeprom_write(uint32_t offset, int size, char *buf);

The offset is the total memory offset, size is buffer size that is
to be read or written. *buf is the pointer to the start of the buffer.
- In read operation, data is read from eeprom and copied to buf.
- In write operation, data from buf is written to the eeprom.

## Compilation and Usage
Assuming we are in the directory of the project.
- Compiling the code base.
	terminal> make
- Running for above specifications.
	terminal> make run
- Clearing object files and simuated .txt file
	terminal> make clean
- Running with default values will run a total of 8 trace file simultaneously
with a dedicated thread for each file.
- The verbose flag is turned ON by default. This will print all the details of
the operations performed. It will print the buffer if the size is less than 100.
- The simulated eeprom .txt file will have new line for each page. This is meant for
the simplicity of the manual check if required.
- The code can also be executed with custom parameters for any specific trace file.
Command line flags include	-p defining page size in bytes
				-n total number of pages
				-t path of trace file
				-v verbose flag
- The following command will create eeprom with 512 pages with 16 bytes each
and run only the read_small trace file while printing all the details.
terminal> ./main -p 16 -n 512 -v 1 -t traces/read_small.trace
- Failing to specifiy any parameter in comman line options, will assume the default
value for the parameter.

## Trace files
Testing is conducted on 8 trace files. Each file tests different aspect of the code.
- toy.trace - Tests a total of 20 read/write operations with memory offset less than 127
			bytes and size less than 10 bytes.
- read_small.trace - Tests a total of 200 read operations with memory offset and size less 
			than 127 bytes.
- write_small.trace - Tests a total of 200 write operations with memory offset and size less 
			than 127 bytes.
- test_small.trace - Tests a total of 200 read/write operations with memory offset and size 
			less than 127 bytes.
- read_big.trace - Tests a total of 200 read operations with memory offset and size upto
			8192 bytes.
- write_big.trace - Tests a total of 200 write operations with memory offset and size upto
			8192 bytes.
- test_big.trace - Tests a total of 200 read/write operations with memory offset and size 
				upto 8192 bytes.
- edge.trace - Tests a total of 200 read/write operations with corner cases including memory
				offset out of bounds, memory offset in bounds but size exceed the remaining 
				capacity of the memory ,size = 0, size less than page size, size less than page size
				but operation spread across 2 pages.

## Code architecture and assumptions
- The eeprom library makes use of file read and write functions to acces one page in a single operation.
- The required page number and page offset is calculated from the input memory offset.
- If the operations is spread over multiple pages, the page is accessed one by one and data is transferred
from the buffer atmost one page per time.
- When multiple threads are accessing the simulated eeprom, critical section is defined only for a single
page operation and not the entire write or read operation.
- Autheticity of the data going in and out of the eeprom is deterimined by the successful page operations.
- The simulated eeprom is filled by 0xFF during its initialisation.
- The write operation generated arftifical data with ascii value between 97 - 123. This is chosen for the
simplicity of visualisation and the logic is idependednt of the byte value.
- Trace files are generated using random number generator with different ranges as per required.

## Testing specification
Apple clang version 11.0.3 (clang-1103.0.32.62)
Target: x86_64-apple-darwin19.6.0
Thread model: posix
