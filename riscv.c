#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "riscv.h"


const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int U_TYPE = 3;
const int B_TYPE = 4;
const int UNKNOWN_TYPE = 5;

/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, U_TYPE, B_TYPE, UNKNOWN_TYPE
 */
static int get_op_type(char *op)
{
    const char *r_type_op[] = {"add", "sub", "and", "slt", "sll", "sra"};
    const char *i_type_op[] = {"addi", "andi"};
    const char *mem_type_op[] = {"lw", "lb", "sw", "sb"};
    const char *u_type_op[] = {"lui"};
    const char *b_type_op[] = {"beq"};
    for (int i = 0; i < (int)(sizeof(r_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(r_type_op[i], op) == 0)
        {
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(i_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(i_type_op[i], op) == 0)
        {
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(mem_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(mem_type_op[i], op) == 0)
        {
            return MEM_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(u_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(u_type_op[i], op) == 0)
        {
            return U_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(b_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(b_type_op[i], op) == 0)
        {
            return B_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}


registers_t *registers;
char **program;
int no_of_instructions;
hashtable_t *memory;

int pc;

void init(registers_t *starting_registers, char **input_program, int given_no_of_instructions)
{
    registers = starting_registers;
    program = input_program;
    no_of_instructions = given_no_of_instructions;

    memory = ht_init(500);
    pc = 0;
}

void end()
{
    ht_free(memory);
}

int is_space(char *character)
{
    return *character == ' ';
}

// Helper used to identify tokens, removes leading and trailing space
char *trim_whitespace(char *str)
{
    char *last;

    while (is_space(str))
        str++;
    if (*str == 0)
    {
        return str;
    }

    last = str + strlen(str) - 1;
    while (last > str && is_space(last))
        last--;

    last[1] = '\0';
    return str;
}
// Helper to convert a string of hex or decimal to an int
int string_to_int(char *str)
{
    return (int)strtol(str, NULL, 0);
}

void step(char *instruction)
{

    char *op = strsep(&instruction, " "); 
 
    int op_type = get_op_type(op);
    printf("Made it to step");
    if (op_type == 5)
    {
        return;
    }
    if (op_type == 0)
    {
        printf("inside optype 0");
        char *dest = trim_whitespace(strsep(&instruction, ","));
        char *rs1 = trim_whitespace(strsep(&instruction, ","));
        char *rs2 = trim_whitespace(strsep(&instruction, ","));
        int numdest = atoi(&dest[1]);
        int numrs1 = atoi(&rs1[1]);
        int numrs2 = atoi(&rs2[1]);

        if (strcmp(op, "add") == 0)
        {
            registers->r[numdest] = registers->r[numrs1] + registers->r[numrs2];
        }
        if (strcmp(op, "sub") == 0)
        {
            registers->r[numdest] = registers->r[numrs1] - registers->r[numrs2];
        }
        if (strcmp(op, "and") == 0)
        {
            registers->r[numdest] = registers->r[numrs1] & registers->r[numrs2];
        }
        if (strcmp(op, "slt") == 0)
        {
            if (registers->r[numrs1] < registers->r[numrs2])
            {
                registers->r[numdest] = 1;
            }
            else
            {
                registers->r[numdest] = 0;
            }
        }
        if (strcmp(op, "sll") == 0)
        {
            int last_five_bits = 31 & registers->r[numrs2];
            registers->r[numdest] = registers->r[numrs1] << last_five_bits;
        }
        if (strcmp(op, "sra") == 0)
        {
            int last_five_bits = 31 & registers->r[numrs2];
            registers->r[numdest] = ((int)registers->r[numrs1]) >> last_five_bits;
        }
    }
    if (op_type == 1)
    {
        char *dest = trim_whitespace(strsep(&instruction, ","));
        char *rs1 = trim_whitespace(strsep(&instruction, ","));
        int imm = string_to_int(trim_whitespace(strsep(&instruction, ",")));
        int numdest = atoi(&dest[1]);
        int numrs1 = atoi(&rs1[1]);

        if (strcmp(op, "addi") == 0)
        {
            registers->r[numdest] = registers->r[numrs1] + imm;
        }
        if (strcmp(op, "andi") == 0)
        {
            registers->r[numdest] = registers->r[numrs1] & imm;
        }
    }
    if (op_type == 2)
    {
        char *first_reg = trim_whitespace(strsep(&instruction, ","));
        int offset = string_to_int(trim_whitespace(strsep(&instruction, "(")));
        char *second_reg = trim_whitespace(strsep(&instruction, ")"));
        int numreg1 = atoi(&first_reg[1]);
        int numreg2 = atoi(&second_reg[1]);
        int address = registers->r[numreg2] + offset;

        if (strcmp(op, "lw") == 0)
        {
            int word = 0;
            for (int i = 0; i < 4; i++)
            {
                int byte = ht_get(memory, address + i);
                word = word | (byte << (i * 8));
            }
            registers->r[numreg1] = word;
        }
        if (strcmp(op, "lb") == 0)
        {
            int byte = ht_get(memory, address);
            if (byte & 128)
            {
                registers->r[numreg1] = byte | 0xFFFFFF00;
            }
            else
            {
                registers->r[numreg1] = byte;
            }
        }
        if (strcmp(op, "sw") == 0)
        {
            int word = registers->r[numreg1];
            for (int i = 0; i < 4; i++)
            {
                int byte = (word >> (i * 8)) & 0xFF;
                ht_add(memory, address + i, byte);
            }
        }
        if (strcmp(op, "sb") == 0)
        {
            int byte = registers->r[numreg1] & 0xFF;
            ht_add(memory, address, byte);
        }
    }
    if (op_type == 3)
    {
        char *dest = trim_whitespace(strsep(&instruction, ","));
        int imm = string_to_int(trim_whitespace(strsep(&instruction, ",")));
        int numdest = atoi(&dest[1]);

        registers->r[numdest] = imm << 12;
    }
    if (op_type == 4)
    {
        char *rs1 = trim_whitespace(strsep(&instruction, ","));
        char *rs2 = trim_whitespace(strsep(&instruction, ","));
        int offset = string_to_int(trim_whitespace(strsep(&instruction, ",")));
        int numrs1 = atoi(&rs1[1]);
        int numrs2 = atoi(&rs2[1]);

        if (registers->r[numrs1] == registers->r[numrs2])
        {
            pc = (pc - 1) + (offset / 4);
        }
    }
}

void evaluate_program()
{
    while ((pc) < no_of_instructions)
    {
        step(program[pc]);
        pc++;
    }
}
