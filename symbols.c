#define _DEFAULT_SOURCE

#include "symbols.h"
#include "sections.h"
#include "elf.h"
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/*
typedef struct {
	size_t elf_data_index;
	size_t elf_data_section_index;
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

typedef struct {
	size_t elf_data_index;
	size_t local_index;
	Elf32_Sym sym;
} GlobalSymbol;
typedef struct {
	GlobalSymbol* symbols;
	size_t count;
} GlobalSymbolTable;
*/

void _generate_global_symbol_table(ELF32_Data* elf_data, ELF_Section_Ref* refs, GlobalSymbolTable* g_sym, size_t count) {
	for (size_t i = 0; i < count; i++) {
		ELF32_Section_Header sec_hdr;
		elf_get_section(elf_data + refs[i].elf_data_index, &sec_hdr, ".symtab");
		size_t symcount = sec_hdr.sh_size / sec_hdr.sh_entsize;
		for (size_t j = 0; j < symcount; j++) {
			Elf32_Sym elf_sym;
			elf_get_sym(elf_data, &elf_sym, j);
			if (elf_sym.st_shndx == refs[i].elf_data_section_index) {
				g_sym->symbols = reallocarray(g_sym->symbols, g_sym->count + 1, sizeof(g_sym->symbols[0]));
				assert(g_sym->symbols != NULL);
				g_sym->symbols[g_sym->count].elf_data_index = refs[i].elf_data_index;
				g_sym->symbols[g_sym->count].local_index = j;
				g_sym->symbols[g_sym->count].sym = elf_sym;
				g_sym->count++;
			}
		}
	}
	
	return;
}
void generate_global_symbol_table(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf, GlobalSymbolTable* g_sym) {
	g_sym->symbols = NULL;
	g_sym->count = 0;
	
	ELF_Section_Ref* text_refs = elf_sec_lay_buf->text_refs;
	size_t s_text_count = elf_sec_lay_buf->s_text_count;
	_generate_global_symbol_table(elf_data, text_refs, g_sym, s_text_count);
	
	return;
}
void free_global_symbol_table(GlobalSymbolTable* g_sym) {
	free(g_sym->symbols);
}
