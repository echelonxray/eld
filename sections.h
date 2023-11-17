#ifndef __HEADER_SECTIONS_H
#define __HEADER_SECTIONS_H

#include "elf.h"
#include <stddef.h>

typedef struct {
	size_t elf_data_index;
	size_t elf_data_section_index;
	size_t address;
} ELF_Section_Ref;
typedef struct {
	ELF_Section_Ref* text_refs;
	ELF_Section_Ref* rodata_refs;
	ELF_Section_Ref* data_refs;
	ELF_Section_Ref* bss_refs;
	size_t s_text_count;
	size_t s_rodata_count;
	size_t s_data_count;
	size_t s_bss_count;
} ELF_Section_Layout;

void fill_section_offsets(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf);
void build_section_layout(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf, size_t elf_count);
void order_section_layout(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf);
void free_section_layout(ELF_Section_Layout* elf_sec_lay_buf);

#endif
