#ifndef __HEADER_SYMBOLS_H
#define __HEADER_SYMBOLS_H

#include "elf.h"
#include "sections.h"
#include <elf.h>
#include <stddef.h>

typedef struct {
	size_t elf_data_index;
	size_t local_index;
	Elf32_Sym sym;
} GlobalSymbol;
typedef struct {
	GlobalSymbol* symbols;
	size_t count;
} GlobalSymbolTable;

void generate_global_symbol_table(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf, GlobalSymbolTable* g_sym);
void free_global_symbol_table(GlobalSymbolTable* g_sym);

#endif
