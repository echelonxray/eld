#define _DEFAULT_SOURCE

#include "sections.h"
#include "util.h"
#include "elf.h"
#include <stdlib.h>

void build_section_layout(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf, size_t elf_count) {
	ELF_Section_Ref* text_refs   = NULL;
	ELF_Section_Ref* rodata_refs = NULL;
	ELF_Section_Ref* data_refs   = NULL;
	ELF_Section_Ref* bss_refs    = NULL;
	size_t s_text_count   = 0;
	size_t s_rodata_count = 0;
	size_t s_data_count   = 0;
	size_t s_bss_count    = 0;
	
	for (size_t i = 0; i < elf_count; i++) {
		uint16_t e_shnum = elf_data[i].elf_hdr.e_shnum;
		for (uint32_t j = 0; j < e_shnum; j++) {
			char* section_name;
			section_name = elf_get_shstr(elf_data + i, j);
			
			if        (!strcmp_prefix(section_name, ".text")) {
				text_refs   = reallocarray(text_refs  , s_text_count + 1  , sizeof(*text_refs));
				text_refs[s_text_count].elf_data_index         = i;
				text_refs[s_text_count].elf_data_section_index = j;
				s_text_count++;
			} else if (!strcmp_prefix(section_name, ".rodata")) {
				rodata_refs = reallocarray(rodata_refs, s_rodata_count + 1, sizeof(*rodata_refs));
				rodata_refs[s_rodata_count].elf_data_index         = i;
				rodata_refs[s_rodata_count].elf_data_section_index = j;
				s_rodata_count++;
			} else if (!strcmp_prefix(section_name, ".data")) {
				data_refs   = reallocarray(data_refs  , s_data_count + 1  , sizeof(*data_refs));
				data_refs[s_data_count].elf_data_index         = i;
				data_refs[s_data_count].elf_data_section_index = j;
				s_data_count++;
			} else if (!strcmp_prefix(section_name, ".bss")) {
				bss_refs    = reallocarray(bss_refs   , s_bss_count + 1   , sizeof(*bss_refs));
				bss_refs[s_bss_count].elf_data_index         = i;
				bss_refs[s_bss_count].elf_data_section_index = j;
				s_bss_count++;
			}
		}
	}
	
	elf_sec_lay_buf->text_refs   = text_refs;
	elf_sec_lay_buf->rodata_refs = rodata_refs;
	elf_sec_lay_buf->data_refs   = data_refs;
	elf_sec_lay_buf->bss_refs    = bss_refs;
	elf_sec_lay_buf->s_text_count   = s_text_count;
	elf_sec_lay_buf->s_rodata_count = s_rodata_count;
	elf_sec_lay_buf->s_data_count   = s_data_count;
	elf_sec_lay_buf->s_bss_count    = s_bss_count;
	
	return;
}

void free_section_layout(ELF_Section_Layout* elf_sec_lay_buf) {
	free(elf_sec_lay_buf->text_refs);
	free(elf_sec_lay_buf->rodata_refs);
	free(elf_sec_lay_buf->data_refs);
	free(elf_sec_lay_buf->bss_refs);
	elf_sec_lay_buf->text_refs   = NULL;
	elf_sec_lay_buf->rodata_refs = NULL;
	elf_sec_lay_buf->data_refs   = NULL;
	elf_sec_lay_buf->bss_refs    = NULL;
	elf_sec_lay_buf->s_text_count   = 0;
	elf_sec_lay_buf->s_rodata_count = 0;
	elf_sec_lay_buf->s_data_count   = 0;
	elf_sec_lay_buf->s_bss_count    = 0;
	return;
}
