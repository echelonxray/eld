#include "elf_load.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
//#include <sys/stat.h>

char* output_filename = "a.out";

/*
// Sanitize file indexing/reading in case of buffer overflow
char* getFileString(void* restrict data, size_t datasize, size_t secindex) {
	char* data2 = data;
	size_t i = secindex;
	do {
		if (i >= datasize) {
			fprintf(stderr, "File overflow\n");
			exit(1);
		}
	} while (data2[i++] != '\0');
	return data2 + secindex;
}
ssize_t getFileData(void* restrict data, size_t datasize, 
                    void* restrict secbuf, size_t secindex, size_t secsize) {
	if (secindex >= datasize) {
		fprintf(stderr, "File overflow\n");
		exit(1);
	}
	if (secindex + secsize > datasize) {
		fprintf(stderr, "File overflow\n");
		exit(1);
	}
	memcpy(secbuf, data + secindex, secsize);
	return secsize;
}

int parse_elf(uint8_t* data, size_t datasize) {
	size_t data_index = 0;
	
	// Load ELF Header
	ELF32_Header elf_header;
	getFileData(data, datasize, &elf_header, 0, sizeof(elf_header));
	
	// Validate Header
	if (strncmp(elf_header.ei_magic, ELFH_EI_MAGIC, ELFH_EI_MAGICLEN)) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.version != 1) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.ei_version != 1) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.ehsize != 52) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.ei_class != ELFH_EI_CLASS_32) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.ei_data != ELFH_EI_DATA_LE) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.ei_osabi != ELFH_EI_OSABI_SYSTEMV) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.type != ELFH_ET_REL) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.machine != ELFH_MACHINE_RISCV) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.entry != 0) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	if (elf_header.shstrndx == 0) {
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	uint32_t shstrndx;
	uint32_t shentsize;
	uint32_t shoff;
	shstrndx  = elf_header.shstrndx;
	shentsize = elf_header.shentsize;
	shoff     = elf_header.shoff;
	
	printf("shstrndx offset: %d\n", shstrndx);
	
	ELF32_Section_Header elf_str_section_header;
	getFileData(data, datasize, &elf_str_section_header, shoff + shstrndx * shentsize, sizeof(elf_str_section_header));
	if (elf_str_section_header.type != 0x3) {
		fprintf(stderr, "TypeNum: %X\n", elf_str_section_header.type);
		fprintf(stderr, "Validation Failure - Line: %d\n", __LINE__);
		return 1;
	}
	size_t strtab_size;
	strtab_size = elf_str_section_header.size;
	char* strtab;
	strtab = malloc(strtab_size);
	if (strtab == NULL) {
		fprintf(stderr, "malloc() failed\n");
		return 1;
	}
	getFileData(data, datasize, strtab, elf_str_section_header.offset, strtab_size);
	char* secname;
	secname = getFileString(strtab, strtab_size, elf_str_section_header.name);
	printf("strtab: \"%s\"\n\n", secname);
	
	data_index = shoff;
	for (uint16_t i = 0; i < elf_header.shnum; i++) {
		ELF32_Section_Header elf_section_header;
		getFileData(data, datasize, &elf_section_header, data_index, sizeof(elf_section_header));
		data_index += elf_header.shentsize;
		
		char* secname;
		secname = getFileString(strtab, strtab_size, elf_section_header.name);
		printf("section: \"%s\"\n", secname);
		
		
	}
	
	free(strtab);
	
	return 0;
}
*/

void parse_elf(int fd) {
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
	
	printf("Sections (Count: %d): \n", elf_header.e_shnum);
	ELF32_Section_Header* section_names_header;
	void* section_names_data;
	section_names_header = elf_shs + elf_header.e_shstrndx;
	section_names_data = elf_scts[elf_header.e_shstrndx];
	for (uint16_t i = 0; i < elf_header.e_shnum; i++) {
		char* section_name;
		section_name = elf_get_section_string(section_names_data, section_names_header, elf_shs[i].sh_name);
		printf("\t[%02u] \"%s\"\n", i, section_name);
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
		parse_elf(fd);
		
		/*
		struct stat statbuf;
		signed int retval;
		retval = fstat(fd, &statbuf);
		if (retval < 0) {
			fprintf(stderr, "Failed to stat: %d \"%s\"\n", fd, argv[i]);
			return 1;
		}
		
		uint8_t* file_data;
		file_data = malloc(statbuf.st_size);
		if (file_data == NULL) {
			fprintf(stderr, "malloc() failed\n");
			return 1;
		}
		
		ssize_t read_count;
		read_count = read(fd, file_data, statbuf.st_size);
		if (read_count != statbuf.st_size) {
			fprintf(stderr, "Only read %ld/%ld bytes from file %d \"%s\"\n", read_count, statbuf.st_size, fd, argv[i]);
			return 1;
		}
		*/
		
		close(fd);
		
		/*
		int ret;
		ret = parse_elf(file_data, read_count);
		if (ret != 0) {
			fprintf(stderr, "parse_elf() returned an error\n");
			return 1;
		}
		
		free(file_data);
		*/
	}
	return 0;
}
