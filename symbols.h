#ifndef __HEADER_SYMBOLS_H
#define __HEADER_SYMBOLS_H

#include "elf.h"
#include "sections.h"
#include <elf.h>
#include <stddef.h>

typedef struct {
	size_t elf_data_index;
	size_t local_index;
	size_t address;
	Elf32_Sym sym;
	ELF_Section_Ref* in_section;
} GlobalSymbol;
typedef struct {
	GlobalSymbol* symbols;
	size_t count;
} GlobalSymbolTable;

void fill_symbol_offsets(GlobalSymbolTable* g_sym);
void generate_global_symbol_table(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf, GlobalSymbolTable* g_sym);
void free_global_symbol_table(GlobalSymbolTable* g_sym);

#endif
