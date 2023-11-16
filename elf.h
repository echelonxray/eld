#ifndef __HEADER_ELF_H
#define __HEADER_ELF_H

#include <elf.h>
#include <stdint.h>

typedef struct __attribute__((__packed__)) {
	char       ei_magic[4];
	uint8_t    ei_class;
	uint8_t    ei_data;
	uint8_t    ei_version;
	uint8_t    ei_osabi;
	uint8_t    ei_abiversion;
	uint8_t    __ei_pad[7]; // Padding
	uint16_t   e_type;
	uint16_t   e_machine;
	uint32_t   e_version;
	uint32_t   e_entry; // Size: ELF Native RL
	uint32_t   e_phoff; // Size: ELF Native RL
	uint32_t   e_shoff; // Size: ELF Native RL
	uint32_t   e_flags;
	uint16_t   e_ehsize;
	uint16_t   e_phentsize;
	uint16_t   e_phnum;
	uint16_t   e_shentsize;
	uint16_t   e_shnum;
	uint16_t   e_shstrndx;
} ELF32_Header;
typedef struct __attribute__((__packed__)) {
	uint32_t   p_type;
	//uint32_t   p_flags;    // Only on ELF64
	uint32_t   p_offset;   // Size: ELF Native RL
	uint32_t   p_vaddr;    // Size: ELF Native RL
	uint32_t   p_paddr;    // Size: ELF Native RL
	uint32_t   p_filesz;   // Size: ELF Native RL
	uint32_t   p_memsz;    // Size: ELF Native RL
	uint32_t   p_flags;
	uint32_t   p_align;    // Size: ELF Native RL
} ELF32_Program_Header;
typedef struct __attribute__((__packed__)) {
	uint32_t   sh_name;
	uint32_t   sh_type;
	uint32_t   sh_flags;      // Size: ELF Native RL
	uint32_t   sh_addr;       // Size: ELF Native RL
	uint32_t   sh_offset;     // Size: ELF Native RL
	uint32_t   sh_size;       // Size: ELF Native RL
	uint32_t   sh_link;
	uint32_t   sh_info;
	uint32_t   sh_addralign;  // Size: ELF Native RL
	uint32_t   sh_entsize;    // Size: ELF Native RL
} ELF32_Section_Header;

typedef struct {
	ELF32_Header elf_hdr;
	ELF32_Program_Header* elf_phs;
	ELF32_Section_Header* elf_shs;
	void** elf_sda;
} ELF32_Data;

#define ELFH_EI_MAGIC               "\x7F" "ELF"
#define ELFH_EI_MAGICLEN            4

#define ELFH_EI_CLASS_32            1
#define ELFH_EI_CLASS_64            2

#define ELFH_EI_DATA_LE             1
#define ELFH_EI_DATA_BE             2

#define ELFH_EI_OSABI_SYSTEMV       0x00
#define ELFH_EI_OSABI_HPUX          0x01
#define ELFH_EI_OSABI_NETBSD        0x02
#define ELFH_EI_OSABI_LINUX         0x03
#define ELFH_EI_OSABI_GNUHURD       0x04
#define ELFH_EI_OSABI_SOLARIS       0x06
#define ELFH_EI_OSABI_AIX           0x07
#define ELFH_EI_OSABI_IRIX          0x08
#define ELFH_EI_OSABI_FREEBSD       0x09
#define ELFH_EI_OSABI_TRU64         0x0A
#define ELFH_EI_OSABI_MODESTO       0x0B
#define ELFH_EI_OSABI_OPENBSD       0x0C
#define ELFH_EI_OSABI_OPENVMS       0x0D
#define ELFH_EI_OSABI_NONSTOPKERNEL 0x0E
#define ELFH_EI_OSABI_AROS          0x0F
#define ELFH_EI_OSABI_FENIXOS       0x10
#define ELFH_EI_OSABI_CLOUNDABI     0x11
#define ELFH_EI_OSABI_OPENVOS       0x12

#define ELFH_ET_NONE                0x0000
#define ELFH_ET_REL                 0x0001
#define ELFH_ET_EXEC                0x0002
#define ELFH_ET_DYN                 0x0003
#define ELFH_ET_CORE                0x0004
#define ELFH_ET_LOOS                0xFE00
#define ELFH_ET_HIOS                0xFEFF
#define ELFH_ET_LOPROC              0xFF00
#define ELFH_ET_HIPROC              0xFFFF

#define ELFH_MACHINE_RISCV          0xF3

#define R_RISCV_NONE                0

#define R_RISCV_32                  1
#define R_RISCV_64                  2

#define R_RISCV_RELATIVE            3
#define R_RISCV_COPY                4
#define R_RISCV_JUMP_SLOT           5
#define R_RISCV_TLS_DTPMOD32        6
#define R_RISCV_TLS_DTPMOD64        7
#define R_RISCV_TLS_DTPREL32        8
#define R_RISCV_TLS_DTPREL64        9
#define R_RISCV_TLS_TPREL32         10
#define R_RISCV_TLS_TPREL64         11
#define R_RISCV_TLSDESC             12

#define R_RISCV_BRANCH              16
#define R_RISCV_JAL                 17
#define R_RISCV_CALL                18
#define R_RISCV_CALL_PLT            19
#define R_RISCV_GOT_HI20            20
#define R_RISCV_TLS_GOT_HI20        21
#define R_RISCV_TLS_GD_HI20         22
#define R_RISCV_PCREL_HI20          23
#define R_RISCV_PCREL_LO12_I        24
#define R_RISCV_PCREL_LO12_S        25
#define R_RISCV_HI20                26
#define R_RISCV_LO12_I              27
#define R_RISCV_LO12_S              28
#define R_RISCV_TPREL_HI20          29
#define R_RISCV_TPREL_LO12_I        30
#define R_RISCV_TPREL_LO12_S        31
#define R_RISCV_TPREL_ADD           32
#define R_RISCV_ADD8                33
#define R_RISCV_ADD16               34
#define R_RISCV_ADD32               35
#define R_RISCV_ADD64               36
#define R_RISCV_SUB8                37
#define R_RISCV_SUB16               38
#define R_RISCV_SUB32               39
#define R_RISCV_SUB64               40
#define R_RISCV_ALIGN               43
#define R_RISCV_RVC_BRANCH          44
#define R_RISCV_RVC_JUMP            45
#define R_RISCV_GPREL_LO12_I        47
#define R_RISCV_GPREL_LO12_S        48
#define R_RISCV_GPREL_HI20          49
#define R_RISCV_RELAX               51
#define R_RISCV_SUB6                52
#define R_RISCV_SET6                53
#define R_RISCV_SET8                54
#define R_RISCV_SET16               55
#define R_RISCV_SET32               56
#define R_RISCV_32_PCREL            57

#define R_RISCV_IRELATIVE           58

#define R_RISCV_PLT32               59
#define R_RISCV_SET_ULEB128         60
#define R_RISCV_SUB_ULEB128         61
#define R_RISCV_TLSDESC_HI20        62
#define R_RISCV_TLSDESC_LOAD_LO12   63
#define R_RISCV_TLSDESC_ADD_LO12    64
#define R_RISCV_TLSDESC_CALL        65

char* elf_get_rela_type(uint32_t type);
char* elf_get_str(ELF32_Data* elf_data, uint32_t string_index);
char* elf_get_shstr(ELF32_Data* elf_data, uint32_t section_index);
void elf_get_sym(ELF32_Data* elf_data, Elf32_Sym* elf_sym, uint32_t symbol_index);

#endif
