#ifndef __HEADER_ELF_LOAD_H
#define __HEADER_ELF_LOAD_H

#include "elf.h"
#include <stdint.h>
#include <stddef.h>

char* elf_get_section_string(void* restrict data, ELF32_Section_Header* restrict elf_section_header, size_t secindex);
int elf_load_header(int fd, ELF32_Header* elf_header);
int elf_load_program_header(int fd, ELF32_Program_Header* elf_program_header, ELF32_Header* elf_header, uint16_t program_index);
int elf_load_section_header(int fd, ELF32_Section_Header* elf_section_header, ELF32_Header* elf_header, uint16_t section_index);
int elf_load_section_body(int fd, void** restrict buffer_ptr, ELF32_Section_Header* restrict elf_section_header);

#endif
