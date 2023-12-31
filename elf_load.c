#include "elf_load.h"
#include "elf.h"
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

// Sanitize file indexing/reading in case of buffer overflow
char* elf_get_section_string(void* restrict data, Elf32_Shdr* restrict elf_section_header, size_t secindex) {
	if (elf_section_header->sh_type == SHT_NOBITS) {
		fprintf(stderr, "[File: \"%s\", Line: %d] Invalid section for reading string contents.\n", __FILE__,  __LINE__);
		exit(1);
	}
	
	char* data2 = data;
	uint64_t data_size = elf_section_header->sh_size;
	size_t i = secindex;
	do {
		if (i >= data_size) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Section overflow error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	} while (data2[i++] != '\0');
	
	return data2 + secindex;
}

void elf_load_header(int fd, ELF32_Header* elf_header) {
	{
		off_t retval;
		retval = lseek(fd, 0, SEEK_SET);
		if (retval == (off_t)-1) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File seek error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	ELF32_Header elf_header_tmp;
	{
		ssize_t retval;
		retval = read(fd, &elf_header_tmp, sizeof(elf_header_tmp));
		if (retval != sizeof(elf_header_tmp)) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File read error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	// Validate Header
	{
		if (strncmp(elf_header_tmp.ei_magic, ELFH_EI_MAGIC, ELFH_EI_MAGICLEN)) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.ei_version != 1) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_version != 1) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_ehsize != 52) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_ehsize != sizeof(elf_header_tmp)) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.ei_class != ELFH_EI_CLASS_32) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.ei_data != ELFH_EI_DATA_LE) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.ei_osabi != ELFH_EI_OSABI_SYSTEMV) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_type != ELFH_ET_REL) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_machine != ELFH_MACHINE_RISCV) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_entry != 0) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_shstrndx >= elf_header_tmp.e_shnum) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_phentsize != sizeof(Elf32_Phdr) && elf_header_tmp.e_phnum > 0) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_shentsize != sizeof(Elf32_Shdr) && elf_header_tmp.e_shnum > 0) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		if (elf_header_tmp.e_phnum > 0) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	memcpy(elf_header, &elf_header_tmp, sizeof(elf_header_tmp));
	
	return;
}

void elf_load_program_header(int fd, Elf32_Phdr* elf_program_header, ELF32_Header* elf_header, uint16_t program_index) {
	if (program_index >= elf_header->e_phnum) {
		fprintf(stderr, "[File: \"%s\", Line: %d] Program header index out of bounds.\n", __FILE__,  __LINE__);
		exit(1);
	}
	
	{
		off_t p_index;
		off_t p_headersize;
		off_t p_headeroff;
		off_t file_off;
		
		p_index = program_index;
		p_headersize = elf_header->e_phentsize;
		p_headeroff = elf_header->e_phoff;
		file_off = (p_headersize * p_index) + p_headeroff;
		
		off_t retval;
		retval = lseek(fd, file_off, SEEK_SET);
		if (retval == (off_t)-1) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File seek error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	Elf32_Phdr elf_program_header_tmp;
	{
		ssize_t retval;
		retval = read(fd, &elf_program_header_tmp, sizeof(elf_program_header_tmp));
		if (retval != sizeof(elf_program_header_tmp)) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File read error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	memcpy(elf_program_header, &elf_program_header_tmp, sizeof(elf_program_header_tmp));
	
	return;
}
void elf_load_section_header(int fd, Elf32_Shdr* elf_section_header, ELF32_Header* elf_header, uint16_t section_index) {
	if (section_index >= elf_header->e_shnum) {
		fprintf(stderr, "[File: \"%s\", Line: %d] Section header index out of bounds.\n", __FILE__,  __LINE__);
		exit(1);
	}
	
	{
		off_t s_index;
		off_t s_headersize;
		off_t s_headeroff;
		off_t file_off;
		
		s_index = section_index;
		s_headersize = elf_header->e_shentsize;
		s_headeroff = elf_header->e_shoff;
		file_off = (s_headersize * s_index) + s_headeroff;
		
		off_t retval;
		retval = lseek(fd, file_off, SEEK_SET);
		if (retval == (off_t)-1) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File seek error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	Elf32_Shdr elf_section_header_tmp;
	{
		ssize_t retval;
		retval = read(fd, &elf_section_header_tmp, sizeof(elf_section_header_tmp));
		if (retval != sizeof(elf_section_header_tmp)) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File read error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	memcpy(elf_section_header, &elf_section_header_tmp, sizeof(elf_section_header_tmp));
	
	return;
}

void elf_load_section_body(int fd, void** restrict buffer_ptr, Elf32_Shdr* restrict elf_section_header) {
	if (elf_section_header->sh_type == SHT_NOBITS) {
		*buffer_ptr = NULL;
		return;
	}
	
	{
		off_t retval;
		retval = lseek(fd, elf_section_header->sh_offset, SEEK_SET);
		if (retval == (off_t)-1) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File seek error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	void* buffer;
	{
		buffer = malloc(elf_section_header->sh_size);
		if (buffer == NULL) {
			fprintf(stderr, "[File: \"%s\", Line: %d] malloc() error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	{
		ssize_t retval;
		retval = read(fd, buffer, elf_section_header->sh_size);
		if (retval != elf_section_header->sh_size) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File read error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	*buffer_ptr = buffer;
	
	return;
}

void elf_load(int fd, ELF32_Data* elf_data) {
	ELF32_Header elf_header;
	Elf32_Phdr* elf_phs;
	Elf32_Shdr* elf_shs;
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
	Elf32_Shdr* names_sec_hdr;
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
	
	return;
}
void elf_unload(ELF32_Data* elf_data) {
	ELF32_Header elf_header;
	Elf32_Phdr* elf_phs;
	Elf32_Shdr* elf_shs;
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
