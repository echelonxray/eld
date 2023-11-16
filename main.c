#define _DEFAULT_SOURCE

#include "util.h"
#include "sections.h"
#include "elf.h"
#include "elf_load.h"
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

char* output_filename = "a.out";

void print_sections(ELF32_Data* elf_data) {
	printf("Sections (Count: %d): \n", elf_data->elf_hdr.e_shnum);
	char* names_sec_dat;
	names_sec_dat = elf_data->elf_sda[elf_data->elf_hdr.e_shstrndx];
	for (uint16_t i = 0; i < elf_data->elf_hdr.e_shnum; i++) {
		char* sec_name;
		sec_name = names_sec_dat + elf_data->elf_shs[i].sh_name;
		printf("\t[%02u] \"%s\"\n", i, sec_name);
	}
	printf("\n");
	return;
}
void print_relocs(ELF32_Data* elf_data) {
	printf("Relocs: \n");
	char* names_sec_dat;
	names_sec_dat = elf_data->elf_sda[elf_data->elf_hdr.e_shstrndx];
	for (uint16_t i = 0; i < elf_data->elf_hdr.e_shnum; i++) {
		char* sec_name;
		sec_name = names_sec_dat + elf_data->elf_shs[i].sh_name;
		if (!strcmp_prefix(sec_name, ".rela")) {
			printf("\t[%02u] \"%s\"\n", i, sec_name);
			void* elf_rela_ent_v;
			elf_rela_ent_v = elf_data->elf_sda[i];
			for (size_t j = 0; j < elf_data->elf_shs[i].sh_size; j += elf_data->elf_shs[i].sh_entsize) {
				Elf32_Rela* elf_rela_ent;
				elf_rela_ent = elf_rela_ent_v + j;
				Elf32_Sym elf_sym_ent;
				elf_get_sym(elf_data, &elf_sym_ent, ELF32_R_SYM(elf_rela_ent->r_info));
				printf("\t\tr_offset: %08X, r_info: %08X (Type %3u: %-25s, Sym %3u->%-3u: %25s->%-25s), r_addend: %+04d + %08X\n",
				       elf_rela_ent->r_offset,
				       elf_rela_ent->r_info,
				       ELF32_R_TYPE(elf_rela_ent->r_info),
				       elf_get_rela_type(ELF32_R_TYPE(elf_rela_ent->r_info)),
				       elf_sym_ent.st_shndx,
				       ELF32_R_SYM(elf_rela_ent->r_info),
				       elf_get_shstr(elf_data, elf_sym_ent.st_shndx),
				       elf_get_str(elf_data, elf_sym_ent.st_name),
				       elf_rela_ent->r_addend,
				       elf_sym_ent.st_value);
			}
		}
	}
	printf("\n");
	return;
}

int main(int argc, char* argv[]) {
	ELF32_Data* elf_data;
	size_t elf_data_len;
	elf_data = NULL;
	elf_data_len = 0;
	for (int i = 1; i < argc; i++) {
		signed int fd;
		fd = open(argv[i], O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "Failed to open: \"%s\"\n", argv[i]);
			return 1;
		}
		
		printf("File: \"%s\"\n", argv[i]);
		elf_data = reallocarray(elf_data, sizeof(*elf_data), elf_data_len + 1);
		if (elf_data == NULL) {
			fprintf(stderr, "[File: \"%s\", Line: %d] malloc() error.\n", __FILE__,  __LINE__);
			exit(1);
		}
		elf_load(fd, elf_data + elf_data_len);
		close(fd);
		
		//print_sections(elf_data + elf_data_len);
		//print_relocs(elf_data + elf_data_len);
		
		elf_data_len++;
	}
	
	ELF_Section_Layout elf_layout;
	build_section_layout(elf_data, &elf_layout, elf_data_len);
	free_section_layout(&elf_layout);
	
	for (size_t i = 0; i < elf_data_len; i++) {
		elf_unload(elf_data + i);
	}
	return 0;
}
