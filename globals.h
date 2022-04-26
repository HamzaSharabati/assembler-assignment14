

#ifndef _GLOBALS_H
#define _GLOBALS_H

#define MAX_MACRO_NAME 40
#define MAX_MACRO_CONTENT 1000
/** bool difinition */
typedef enum booleans {
	FALSE = 0, TRUE = 1
} bool;

/** Max size of code image  */
#define CODE_ARR_IMG_LENGTH 1200

/** Maxlength of line  */
#define MAX_LINE_LENGTH 80

/** Initial IC value */
#define IC_INIT_VALUE 100

/* Note: many enum declaration contains NONE_X value - which is a flag for not found during parsing. */

/** Operand addressing type */
typedef enum addressing_types {
	/** Immediate addressing (0) */
	IMMEDIATE_ADDR = 0,
	/** Direct addressing (1) */
	DIRECT_ADDR = 1,
	/** Index addressing (2) */
	INDEX_ADDR = 2,
	/** Register addressing */
	REGISTER_ADDR = 3,
	/** Failed/Not detected addressing */
	NONE_ADDR = -1
} addressing_type;

/**  opcodes  2 powers  to light the bit for each op code */
typedef enum opcodes {
	MOV_OP = 0x1, /* 2^0 */
	CMP_OP = 0x2, /* 2^1 */
	ADD_OP = 0x4, /* 2^2 */
	SUB_OP = 0x4,
	LEA_OP = 0x10, /* 2^4 */
	CLR_OP = 0x20, /* 2^5 */
	NOT_OP = 0x20,
	INC_OP = 0x20,
	DEC_OP = 0x20,
	JMP_OP = 0x200,  /* 2^9 */
	BNE_OP = 0x200,
	JSR_OP = 0x200,
	RED_OP = 0x1000, /* 2^12 */
	PRN_OP = 0x2000, /* 2^13 */
	RTS_OP = 0x4000, /* 2^14 */
	STOP_OP = 0x8000, /* 2^15 */
	

	
	NONE_OP = -1
} opcode;


typedef enum funct {
	/* OPCODE 2 */
	ADD_FUNCT = 10,
	SUB_FUNCT = 11,

	/* OPCODE 5 */
	CLR_FUNCT = 10,
	NOT_FUNCT = 11,
	INC_FUNCT = 12,
	DEC_FUNCT = 13,

	/* OPCODE 9 */
	JMP_FUNCT = 10,
	BNE_FUNCT = 11,
	JSR_FUNCT = 12,

	/** Default (No need/Error) */
	NONE_FUNCT = 0
} funct;


typedef enum registers {
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15,
	NONE_REG = -1
} reg;
/** Represents the second word of code */
typedef struct code_prop {
	unsigned int dest_addressing: 2 ;
	unsigned int dest_register: 4;
	unsigned int src_addressing: 2 ;
	unsigned int src_register: 4 ;
	unsigned int funct: 4 ;
	unsigned int ARE: 3;
	unsigned int lastbit: 1;

} code_prop;

/** Represents a single data or the first code -opp- word. */
typedef struct data_op_word {
	unsigned int data: 16 ;
	unsigned int ARE: 3;
	unsigned int lastbit: 1 ;
} data_op_word;



/** Represents a general machine code word contents */
typedef struct machine_word {
	/* if it represents code (not additional data), this field contains the total length required by the code. if it's data, this field is 0. */
	short length;
	/* The content can be code or data or op*/
	short flag ; /*data_op ->0  code_prop->1 */
	union word {
		data_op_word *data;
		code_prop *code;
	} word;
} machine_word;

/** Instruction type (.data, .entry, etc.) */
typedef enum instruction {
	
	DATA_INST,

	EXTERN_INST,

	ENTRY_INST,

	STRING_INST,

	NONE_INST,

	ERROR_INST
} instruction;

/**
 * Represents line
 */
typedef struct line_info {
	/** Line number in file */
	long line_number;
	/** File name */
	char *file_name;
	/** Line content (source) */
	char *content;
} line_info;

 

#endif
