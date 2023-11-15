#include "elf_load.h"
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
		printf("\t[%02u] \"%s\"\n", i, names_sec_dat + elf_data->elf_shs[i].sh_name);
	}
	printf("\n");
}

void elf_load(int fd, ELF32_Data* elf_data) {
	ELF32_Header elf_header;
	ELF32_Program_Header* elf_phs;
	ELF32_Section_Header* elf_shs;
	void** elf_scts;
	
	elf_load_header(fd, &elf_header);
	
	elf_phs = malloc(elf_header.e_phnum * sizeof(*elf_phs));
	if (elf_header.e_phnum > 0 && elf_phs == NULL) {
		fprintf(stderr, "[File: \"%s\", Line: %d] malloc() error.\n", __FILE__,  __LINE__);
		exit(1);
	}
	elf_shs = malloc(elf_header.e_shnum * sizeof(*elf_shs));
	if (elf_header.e_shnum > 0 && elf_shs == NULL) {
		fprintf(stderr, "[File: \"%s\", Line: %d] malloc() error.\n", __FILE__,  __LINE__);
		exit(1);
	}
	elf_scts = malloc(elf_header.e_shnum * sizeof(*elf_scts));
	if (elf_header.e_shnum > 0 && elf_scts == NULL) {
		fprintf(stderr, "[File: \"%s\", Line: %d] malloc() error.\n", __FILE__,  __LINE__);
		exit(1);
	}
	
	for (uint16_t i = 0; i < elf_header.e_phnum; i++) {
		elf_load_program_header(fd, elf_phs + i, &elf_header, i);
	}
	for (uint16_t i = 0; i < elf_header.e_shnum; i++) {
		elf_load_section_header(fd, elf_shs + i, &elf_header, i);
	}
	for (uint16_t i = 0; i < elf_header.e_shnum; i++) {
		elf_load_section_body(fd, elf_scts + i, elf_shs + i);
	}
	
	// Validate section name strings
	//printf("Sections (Count: %d): \n", elf_header.e_shnum);
	ELF32_Section_Header* names_sec_hdr;
	void* names_sec_dat;
	names_sec_hdr = elf_shs + elf_header.e_shstrndx;
	names_sec_dat = elf_scts[elf_header.e_shstrndx];
	for (uint16_t i = 0; i < elf_header.e_shnum; i++) {
		//char* sec_name;
		//sec_name = elf_get_section_string(names_sec_dat, names_sec_hdr, elf_shs[i].sh_name);
		//printf("\t[%02u] \"%s\"\n", i, sec_name);
		elf_get_section_string(names_sec_dat, names_sec_hdr, elf_shs[i].sh_name);
	}
	//printf("\n");
	
	elf_data->elf_hdr = elf_header;
	elf_data->elf_phs = elf_phs;
	elf_data->elf_shs = elf_shs;
	elf_data->elf_sda = elf_scts;
	
	print_sections(elf_data);
	
	return;
}

void elf_unload(ELF32_Data* elf_data) {
	ELF32_Header elf_header;
	ELF32_Program_Header* elf_phs;
	ELF32_Section_Header* elf_shs;
	void** elf_scts;
	
	elf_header = elf_data->elf_hdr;
	elf_phs    = elf_data->elf_phs;
	elf_shs    = elf_data->elf_shs;
	elf_scts   = elf_data->elf_sda;
	
	for (uint16_t i = 0; i < elf_header.e_shnum; i++) {
		free(elf_scts[i]);
	}
	free(elf_scts);
	free(elf_shs);
	free(elf_phs);
	
	return;
}

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		signed int fd;
		fd = open(argv[i], O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "Failed to open: \"%s\"\n", argv[i]);
			return 1;
		}
		
		printf("File: \"%s\"\n", argv[i]);
		ELF32_Data elf_data;
		elf_load(fd, &elf_data);
		elf_unload(&elf_data);
		
		close(fd);
	}
	return 0;
}
