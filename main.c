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
		close(fd);
		
		print_sections(&elf_data);
		
		elf_unload(&elf_data);
	}
	return 0;
}
