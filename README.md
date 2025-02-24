# Virtual Memory Management System

This project is a **virtual memory management system** implemented in C, which simulates **paging**, **page replacement algorithms**, and **memory accesses**.

## Features
- **Paging System**: Virtual memory (128 bytes) and main memory (32 bytes) with 8-byte pages.
- **Page Table**: Tracks valid/dirty bits and mappings between virtual and main memory pages.
- **Page Replacement Algorithms**:
  - **FIFO (First-In-First-Out)**
  - **LRU (Least Recently Used)**
- **Command Interface**:
  - `read <virtual_address>`: Reads a value from a virtual memory address.
  - `write <virtual_address> <value>`: Writes a value to a virtual memory address.
  - `showmain <page_number>`: Displays the contents of a specific page in main memory.
  - `showptable`: Displays the current state of the page table.
  - `quit`: Exits the program.

## Compilation & Execution
### Compile:
```sh
gcc -o vm vm.c
```
### Run with FIFO (default):
```sh
./vm FIFO
```
### Run with LRU:
```sh
./vm LRU
```

## Example Usage
```sh
> write 10 42  # Write 42 to virtual address 10
> read 10      # Read value from virtual address 10
> showptable   # Show the page table
> showmain 1   # Show contents of main memory page 1
> quit         # Exit
```

## Author
**Bhuvan Chandra**