#include "elf.h"
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

char* elf_get_rela_type(uint32_t type) {
	#define SWITCH_CASE(arg) case arg: return #arg;
	switch (type) {
		SWITCH_CASE(R_RISCV_NONE)
		SWITCH_CASE(R_RISCV_32)
		SWITCH_CASE(R_RISCV_64)
		SWITCH_CASE(R_RISCV_RELATIVE)
		SWITCH_CASE(R_RISCV_COPY)
		SWITCH_CASE(R_RISCV_JUMP_SLOT)
		SWITCH_CASE(R_RISCV_TLS_DTPMOD32)
		SWITCH_CASE(R_RISCV_TLS_DTPMOD64)
		SWITCH_CASE(R_RISCV_TLS_DTPREL32)
		SWITCH_CASE(R_RISCV_TLS_DTPREL64)
		SWITCH_CASE(R_RISCV_TLS_TPREL32)
		SWITCH_CASE(R_RISCV_TLS_TPREL64)
		SWITCH_CASE(R_RISCV_TLSDESC)
		SWITCH_CASE(R_RISCV_BRANCH)
		SWITCH_CASE(R_RISCV_JAL)
		SWITCH_CASE(R_RISCV_CALL)
		SWITCH_CASE(R_RISCV_CALL_PLT)
		SWITCH_CASE(R_RISCV_GOT_HI20)
		SWITCH_CASE(R_RISCV_TLS_GD_HI20)
		SWITCH_CASE(R_RISCV_PCREL_HI20)
		SWITCH_CASE(R_RISCV_PCREL_LO12_I)
		SWITCH_CASE(R_RISCV_PCREL_LO12_S)
		SWITCH_CASE(R_RISCV_HI20)
		SWITCH_CASE(R_RISCV_LO12_I)
		SWITCH_CASE(R_RISCV_LO12_S)
		SWITCH_CASE(R_RISCV_TPREL_HI20)
		SWITCH_CASE(R_RISCV_TPREL_LO12_I)
		SWITCH_CASE(R_RISCV_TPREL_LO12_S)
		SWITCH_CASE(R_RISCV_TPREL_ADD)
		SWITCH_CASE(R_RISCV_ADD8)
		SWITCH_CASE(R_RISCV_ADD16)
		SWITCH_CASE(R_RISCV_ADD32)
		SWITCH_CASE(R_RISCV_ADD64)
		SWITCH_CASE(R_RISCV_SUB8)
		SWITCH_CASE(R_RISCV_SUB16)
		SWITCH_CASE(R_RISCV_SUB32)
		SWITCH_CASE(R_RISCV_SUB64)
		SWITCH_CASE(R_RISCV_ALIGN)
		SWITCH_CASE(R_RISCV_RVC_BRANCH)
		SWITCH_CASE(R_RISCV_RVC_JUMP)
		SWITCH_CASE(R_RISCV_GPREL_LO12_I)
		SWITCH_CASE(R_RISCV_GPREL_LO12_S)
		SWITCH_CASE(R_RISCV_GPREL_HI20)
		SWITCH_CASE(R_RISCV_RELAX)
		SWITCH_CASE(R_RISCV_SUB6)
		SWITCH_CASE(R_RISCV_SET6)
		SWITCH_CASE(R_RISCV_SET8)
		SWITCH_CASE(R_RISCV_SET16)
		SWITCH_CASE(R_RISCV_SET32)
		SWITCH_CASE(R_RISCV_32_PCREL)
		SWITCH_CASE(R_RISCV_IRELATIVE)
		SWITCH_CASE(R_RISCV_PLT32)
		SWITCH_CASE(R_RISCV_SET_ULEB128)
		SWITCH_CASE(R_RISCV_SUB_ULEB128)
		SWITCH_CASE(R_RISCV_TLSDESC_HI20)
		SWITCH_CASE(R_RISCV_TLSDESC_LOAD_LO12)
		SWITCH_CASE(R_RISCV_TLSDESC_ADD_LO12)
		SWITCH_CASE(R_RISCV_TLSDESC_CALL)
		default:
	}
	return "UNKNOWN_RELOC";
}

char* elf_get_str(ELF32_Data* elf_data, uint32_t string_index) {
	char* names_sec_dat;
	names_sec_dat = elf_data->elf_sda[elf_data->elf_hdr.e_shstrndx];
	for (uint16_t i = 0; i < elf_data->elf_hdr.e_shnum; i++) {
		char* sec_name;
		sec_name = names_sec_dat + elf_data->elf_shs[i].sh_name;
		if (!strcmp(sec_name, ".strtab")) {
			char* str;
			uint32_t j;
			str = elf_data->elf_sda[i];
			j = string_index;
			do {
				if (j >= elf_data->elf_shs[i].sh_size) {
					fprintf(stderr, "string out of .strtab bounds.\n");
					exit(1);
				}
			} while (str[j++] != '\0');
			return str + string_index;
		}
	}
	fprintf(stderr, ".strtab section not found.\n");
	exit(1);
	return NULL;
}

char* elf_get_shstr(ELF32_Data* elf_data, uint32_t section_index) {
	if (section_index >= elf_data->elf_hdr.e_shnum) {
		fprintf(stderr, "section_index out of range.\n");
		exit(1);
	}
	char* names_sec_dat;
	names_sec_dat = elf_data->elf_sda[elf_data->elf_hdr.e_shstrndx];
	return names_sec_dat + elf_data->elf_shs[section_index].sh_name;
}

void elf_get_sym(ELF32_Data* elf_data, Elf32_Sym* elf_sym, uint32_t symbol_index) {
	char* names_sec_dat;
	names_sec_dat = elf_data->elf_sda[elf_data->elf_hdr.e_shstrndx];
	for (uint16_t i = 0; i < elf_data->elf_hdr.e_shnum; i++) {
		char* sec_name;
		sec_name = names_sec_dat + elf_data->elf_shs[i].sh_name;
		if (!strcmp(sec_name, ".symtab")) {
			Elf32_Sym* elf_sym_ent;
			elf_sym_ent = elf_data->elf_sda[i];
			// TODO: Validate that symbol index is in range.
			memcpy(elf_sym, elf_sym_ent + symbol_index, sizeof(*elf_sym));
			return;
		}
	}
	fprintf(stderr, ".symtab section not found.\n");
	exit(1);
	return;
}

void elf_get_section(ELF32_Data* elf_data, Elf32_Shdr* elf_sec, char* section_name) {
	char* names_sec_dat;
	names_sec_dat = elf_data->elf_sda[elf_data->elf_hdr.e_shstrndx];
	for (uint16_t i = 0; i < elf_data->elf_hdr.e_shnum; i++) {
		char* sec_name;
		sec_name = names_sec_dat + elf_data->elf_shs[i].sh_name;
		if (!strcmp(sec_name, section_name)) {
			*elf_sec = elf_data->elf_shs[i];
			return;
		}
	}
	fprintf(stderr, "%s section not found.\n", section_name);
	exit(1);
	return;
}
