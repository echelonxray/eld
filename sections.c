#define _DEFAULT_SOURCE

#include "sections.h"
#include "util.h"
#include "elf.h"
#include <stdio.h>
#include <stdlib.h>

size_t _fill_section_offsets(ELF32_Data* elf_data, ELF_Section_Ref* sec_ref, size_t count, size_t start_offset) {
	for (size_t i = 0; i < count; i++) {
		Elf32_Shdr sec_hdr;
		sec_hdr = elf_data[sec_ref[i].elf_data_index].elf_shs[sec_ref[i].elf_data_section_index];
		size_t addralign = sec_hdr.sh_addralign;
		if (start_offset & (addralign - 1)) {
			addralign = ~addralign;
			start_offset &= addralign;
			start_offset += sec_hdr.sh_addralign;
		}
		sec_ref[i].address = start_offset;
		start_offset += sec_hdr.sh_size;
	}
	return start_offset;
}
void fill_section_offsets(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf) {
	size_t start_offset = 0;
	
	ELF_Section_Ref* text_refs = elf_sec_lay_buf->text_refs;
	size_t s_text_count = elf_sec_lay_buf->s_text_count;
	start_offset = _fill_section_offsets(elf_data, text_refs, s_text_count, start_offset);
	
	ELF_Section_Ref* rodata_refs = elf_sec_lay_buf->rodata_refs;
	size_t s_rodata_count = elf_sec_lay_buf->s_rodata_count;
	start_offset = _fill_section_offsets(elf_data, rodata_refs, s_rodata_count, start_offset);
	
	ELF_Section_Ref* data_refs = elf_sec_lay_buf->data_refs;
	size_t s_data_count = elf_sec_lay_buf->s_data_count;
	start_offset = _fill_section_offsets(elf_data, data_refs, s_data_count, start_offset);
	
	ELF_Section_Ref* bss_refs = elf_sec_lay_buf->bss_refs;
	size_t s_bss_count = elf_sec_lay_buf->s_bss_count;
	start_offset = _fill_section_offsets(elf_data, bss_refs, s_bss_count, start_offset);
	
	return;
}

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

/*
void _print_sections(ELF32_Data* elf_data, ELF_Section_Ref* sec_ref, size_t count) {
	printf("[Sections]\n");
	for (size_t i = 0; i < count; i++) {
		size_t elf_data_index;
		size_t elf_sect_index;
		elf_data_index = sec_ref[i].elf_data_index;
		elf_sect_index = sec_ref[i].elf_data_section_index;
		
		char* str;
		unsigned int size;
		unsigned int align;
		size  = (unsigned int)elf_data[elf_data_index].elf_shs[elf_sect_index].sh_size;
		align = (unsigned int)elf_data[elf_data_index].elf_shs[elf_sect_index].sh_addralign;
		str   = elf_get_shstr(elf_data + elf_data_index, elf_sect_index);
		printf("\t%2u->%15s, size: %4X, align: %4X\n", (unsigned int)elf_data_index, str, size, align);
	}
	printf("\n");
	return;
}
*/
void _order_section_layout(ELF32_Data* elf_data, ELF_Section_Ref* sec_ref, size_t count) {
	//printf("<A>\n");
	//_print_sections(elf_data, sec_ref, count);
	
	// Order the sections most align constrained to least aligned constrained
	for (size_t i = 0; i < count; i++) {
		size_t idx = 0;
		
		// Find the index of the next most align constrained section and store it in idx
		ssize_t sh_addralign_max = -1;
		for (size_t j = i; j < count; j++) {
			size_t elf_data_index = sec_ref[j].elf_data_index;
			size_t elf_sect_index = sec_ref[j].elf_data_section_index;
			uint32_t sh_addralign = elf_data[elf_data_index].elf_shs[elf_sect_index].sh_addralign;
			if (sh_addralign > sh_addralign_max) {
				sh_addralign_max = sh_addralign;
				idx = j;
			}
		}
		
		// Swap the section in the current array index with the section we just found
		ELF_Section_Ref tmp_sec_ref;
		tmp_sec_ref = sec_ref[i];
		sec_ref[i] = sec_ref[idx];
		sec_ref[idx] = tmp_sec_ref;
	}
	
	//printf("<B>\n");
	//_print_sections(elf_data, sec_ref, count);
	
	// Attempt to reorder the sections by fitting the less align constrained sections
	// between the more align constrained sections for size optimization
	int swap_preformed;
	do {
		swap_preformed = 0;
		for (size_t i = 2; i < count; i++) {
			size_t end_of_prev_section = elf_data[sec_ref[0].elf_data_index].elf_shs[sec_ref[0].elf_data_section_index].sh_size;
			for (size_t j = 1; j < i; j++) {
				size_t align_mask;
				
				size_t elf_data_index;
				size_t elf_sect_index;
				
				//uint32_t prev_sh_size;
				//uint32_t prev_sh_addralign;
				uint32_t next_sh_size;
				uint32_t next_sh_addralign;
				
				//elf_data_index = sec_ref[j + 0].elf_data_index;
				//elf_sect_index = sec_ref[j + 0].elf_data_section_index;
				//prev_sh_size = elf_data[elf_data_index].elf_shs[elf_sect_index].sh_size;
				//prev_sh_addralign = elf_data[elf_data_index].elf_shs[elf_sect_index].sh_addralign;
				
				elf_data_index = sec_ref[j].elf_data_index;
				elf_sect_index = sec_ref[j].elf_data_section_index;
				next_sh_size = elf_data[elf_data_index].elf_shs[elf_sect_index].sh_size;
				next_sh_addralign = elf_data[elf_data_index].elf_shs[elf_sect_index].sh_addralign;
				// Handle the case of this being zero as 1 and 0 are the same meaning.
				// However, a value of zero will break the math and logic.
				if (next_sh_addralign == 0) {
					next_sh_addralign = 1;
				}
				
				size_t start_of_next_section = end_of_prev_section;
				align_mask = next_sh_addralign - 1;
				if (start_of_next_section & align_mask) {
					align_mask = ~align_mask;
					start_of_next_section &= align_mask;
					start_of_next_section += next_sh_addralign;
				}
				
				uint32_t curr_sh_size;
				uint32_t curr_sh_addralign;
				
				elf_data_index = sec_ref[i].elf_data_index;
				elf_sect_index = sec_ref[i].elf_data_section_index;
				curr_sh_size = elf_data[elf_data_index].elf_shs[elf_sect_index].sh_size;
				curr_sh_addralign = elf_data[elf_data_index].elf_shs[elf_sect_index].sh_addralign;
				// Handle the case of this being zero as 1 and 0 are the same meaning.
				// However, a value of zero will break the math and logic.
				if (curr_sh_addralign == 0) {
					curr_sh_addralign = 1;
				}
				
				size_t start_of_curr_section = end_of_prev_section;
				align_mask = curr_sh_addralign - 1;
				if (start_of_curr_section & align_mask) {
					align_mask = ~align_mask;
					start_of_curr_section &= align_mask;
					start_of_curr_section += curr_sh_addralign;
				}
				
				size_t size_between = start_of_next_section - start_of_curr_section;
				if (size_between >= curr_sh_size) {
					// It fits! Swap the section in.
					ELF_Section_Ref tmp_sec_ref;
					tmp_sec_ref = sec_ref[i];
					sec_ref[i] = sec_ref[j];
					sec_ref[j] = tmp_sec_ref;
					end_of_prev_section = start_of_curr_section + curr_sh_size;
					swap_preformed = 1;
				} else {
					end_of_prev_section = start_of_next_section + next_sh_size;
				}
			}
		}
	} while (swap_preformed);
	
	//printf("<C>\n");
	//_print_sections(elf_data, sec_ref, count);
	
	return;
}
void order_section_layout(ELF32_Data* elf_data, ELF_Section_Layout* elf_sec_lay_buf) {
	ELF_Section_Ref* text_refs = elf_sec_lay_buf->text_refs;
	size_t s_text_count = elf_sec_lay_buf->s_text_count;
	_order_section_layout(elf_data, text_refs, s_text_count);
	
	ELF_Section_Ref* rodata_refs = elf_sec_lay_buf->rodata_refs;
	size_t s_rodata_count = elf_sec_lay_buf->s_rodata_count;
	_order_section_layout(elf_data, rodata_refs, s_rodata_count);
	
	ELF_Section_Ref* data_refs = elf_sec_lay_buf->data_refs;
	size_t s_data_count = elf_sec_lay_buf->s_data_count;
	_order_section_layout(elf_data, data_refs, s_data_count);
	
	ELF_Section_Ref* bss_refs = elf_sec_lay_buf->bss_refs;
	size_t s_bss_count = elf_sec_lay_buf->s_bss_count;
	_order_section_layout(elf_data, bss_refs, s_bss_count);
	
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
