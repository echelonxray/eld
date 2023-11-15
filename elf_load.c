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
/*
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
*/
/*
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
*/
char* elf_get_section_string(void* restrict data, ELF32_Section_Header* restrict elf_section_header, size_t secindex) {
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

int elf_load_header(int fd, ELF32_Header* elf_header) {
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
	
	{
		// Validate Header
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
		/*
		if (elf_header_tmp.e_phentsize != sizeof(ELF32_Program_Header)) {
			fprintf(stderr, "elf_header_tmp.e_phentsize: %d\n", elf_header_tmp.e_phentsize);
			fprintf(stderr, "sizeof(ELF32_Program_Header): %d\n", (int)sizeof(ELF32_Program_Header));
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
		*/
		if (elf_header_tmp.e_shentsize != sizeof(ELF32_Section_Header)) {
			fprintf(stderr, "[File: \"%s\", Line: %d] Validation failure.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	memcpy(elf_header, &elf_header_tmp, sizeof(elf_header_tmp));
	
	return 0;
}

int elf_load_program_header(int fd, ELF32_Program_Header* elf_program_header, ELF32_Header* elf_header, uint16_t program_index) {
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
	
	ELF32_Program_Header elf_program_header_tmp;
	{
		ssize_t retval;
		retval = read(fd, &elf_program_header_tmp, sizeof(elf_program_header_tmp));
		if (retval != sizeof(elf_program_header_tmp)) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File read error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	memcpy(elf_program_header, &elf_program_header_tmp, sizeof(elf_program_header_tmp));
	
	return 0;
}

int elf_load_section_header(int fd, ELF32_Section_Header* elf_section_header, ELF32_Header* elf_header, uint16_t section_index) {
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
	
	ELF32_Section_Header elf_section_header_tmp;
	{
		ssize_t retval;
		retval = read(fd, &elf_section_header_tmp, sizeof(elf_section_header_tmp));
		if (retval != sizeof(elf_section_header_tmp)) {
			fprintf(stderr, "[File: \"%s\", Line: %d] File read error.\n", __FILE__,  __LINE__);
			exit(1);
		}
	}
	
	memcpy(elf_section_header, &elf_section_header_tmp, sizeof(elf_section_header_tmp));
	
	return 0;
}

int elf_load_section_body(int fd, void** restrict buffer_ptr, ELF32_Section_Header* restrict elf_section_header) {
	if (elf_section_header->sh_type == SHT_NOBITS) {
		*buffer_ptr = NULL;
		return 0;
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
	
	return 0;
}

/*
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
