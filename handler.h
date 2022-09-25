#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include<math.h>

#include "in_out_put.h"

#include "symbol.h"

#include "lines.h"

#include "files_handler.h"


/* data_handler: fill the bin number of the data he gets, if its string he will transform the ascii value of the chars*/

void data_handler(char mission[MAX][MAX], int * words, char type[], int * IC, int * DC, int * flag, int L,

                  char machine_codes[MACHINE_CODES][MACHINE_CODE_ROW]);

/*job secure function search if the job is allowed*/

int job_secure(char job[MAX]);

/*this function fil the machine code matrix with the right values */

void handle_machine_code_stage_one(char mission[MAX][MAX], char command[MAX], int * words, int * IC, int * DC,

                                   char mc[MACHINE_CODES][MACHINE_CODE_ROW], int keeper[MAX_keeper], int counter);

void handle_machine_code_stage_two(char mission[MAX][MAX], int * words, int * IC, int * DC,

                                   char mc[MACHINE_CODES][MACHINE_CODE_ROW], struct symbol *symbols, int keeper[MAX_keeper],

                                   int counter, int symbol_index, int label, int * flag);

/*convert the decimal number - value to bin and write it to the given location - buffer.

 * if the value is negative we use the "mashlim le shtaim" method */

char * dec_to_bin(int value, char * buffer, int base, int size);

/* search in the given string if there is a register, and if the is if it's a known register*/

int find_register(char * string);

/*returns the operand op code of the command*/

int get_operand_op_code(char * command);

/* fill the data according the addressing method 0 */

int addressing_method_0(char string[MAX], int number, char mc[MACHINE_CODES][MACHINE_CODE_ROW], int * IC);

/* fill the data according the addressing method 2 */

int addressing_method_2(char string[MAX], char mc[MACHINE_CODES][MACHINE_CODE_ROW], int * IC, int number);

/* fill the data according the addressing method 3 */

int addressing_method_3(char string[MAX], char mc[MACHINE_CODES][MACHINE_CODE_ROW], int * IC, int number);

/*this function use switch case in order to know what is the given function, and checks what is the addressing methods*/

int handle_arguments_code(int temp, int number,

                          char string[MAX], int * IC, char mc[MACHINE_CODES][MACHINE_CODE_ROW], int *checker);

/* this function increase the machine code size, and moves the pointer also */

void machine_code_reset(char mc[MACHINE_CODES][MACHINE_CODE_ROW]);

/* Declaration of local variables  */

int op_code_array[16] = {

        0,

        1,

        2,

        2,

        4,

        5,

        5,

        5,

        5,

        9,

        9,

        9,

        12,

        13,

        14,

        15

};

char * instruction_name[] = {

        "mov",

        "cmp",

        "add",

        "sub",

        "lea",

        "clr",

        "not",

        "inc",

        "dec",

        "jmp",

        "bne",

        "jsr",

        "red",

        "prn",

        "rts",

        "stop"

};

char * register_name[] = {

        "r0",

        "r1",

        "r2",

        "r3",

        "r4",

        "r5",

        "r6",

        "r7",

        "r8",

        "r9",

        "r10",

        "r11",

        "r12",

        "r13",

        "r14",

        "r15"

};

struct symbol * symbols_increase(struct symbol * symbols,int * size){

    *size = *size * 2;

    return (struct symbol *) realloc(symbols, sizeof(struct symbol) * (*size));

}


void machine_code_reset(char mc[MACHINE_CODES][MACHINE_CODE_ROW]) {

    int i = START_OF_ICF;

    for (; i < MACHINE_CODES; i++) {

        memset(mc[i], '0', 20);

    }

}

char * dec_to_bin(int value, char * buffer, int base, int size) {

    char * temp;

    char * onescomplement;

    int r, n, i = 0, carry = 1;

    onescomplement = (char * ) malloc(sizeof(char) * (size + 1));

    temp = (char * ) malloc(sizeof(char) * size);

    memset(temp, '0', size);

    if (base < 2 || base > 32) {

        return buffer;

    }

    n = abs(value);

    while (n)

    {

        r = n % base;

        if (r >= 10) {

            temp[i++] = 65 + (r - 10);

        } else {

            temp[i++] = 48 + r;

        }

        n = n / base;

    }

    if (i == 0) {

        temp[i++] = '0';

    }

    if (value < 0 && base == 10) {

        temp[i++] = '-';

    }

    if (value < 0) {

        /*        char onescomplement[size+1];*/

        for (i = 0; i < size; i++)

        {

            if (temp[i] == '0')

                onescomplement[i] = '1';

            else if (temp[i] == '1')

                onescomplement[i] = '0';

        }

        onescomplement[size] = '\0';

        for (i = 0; i < size; i++)

        {

            if (onescomplement[i] == '1' && carry == 1)

            {

                buffer[i] = '0';

            } else if (onescomplement[i] == '0' && carry == 1)

            {

                buffer[i] = '1';

                carry = 0;

            } else

            {

                buffer[i] = onescomplement[i];

            }

        }

    } else {

        for (i = 0; i < size; i++) {

            buffer[i] = temp[i];

        }

    }

    free(temp);

    return buffer;

}

int job_secure(char job[MAX]) {



    if (!strcmp(job, "mov") || !strcmp(job, "cmp") || !strcmp(job, "add") || !strcmp(job, "sub") || !strcmp(job, "lea")) {

        return 2;

    } else if (!strcmp(job, "clr") || !strcmp(job, "not") || !strcmp(job, "inc") || !strcmp(job, "dec") || !strcmp(job, "jmp")

               ||
               !strcmp(job, "bne") || !strcmp(job, "jsr") || !strcmp(job, "red") || !strcmp(job, "prn")) {

        return 1;

    } else if (!strcmp(job, "rts") || !strcmp(job, "stop")) {

        return 0;

    }

    printf("\nthere isn't any job by just name: %s\n", job);

    return -1;

}

void handle_machine_code_stage_one(char mission[MAX][MAX], char command[MAX], int * words, int * IC, int * DC, char mc[MACHINE_CODES][MACHINE_CODE_ROW],

                                   int keeper[MAX_keeper], int counter) {

    int unknown = 0, checker = 0;

    int j = 0, op_code = -1;

    int function_index;

    if (mission[j][strlen(mission[j]) - 1] == ':') {

        j++;

    }

    mc[*IC][18] = '1';

    /* entering the op code */

    function_index = get_operand_op_code(command);

    op_code = op_code_array[function_index];

    mc[*IC][op_code] = '1';

    *IC = *IC + 1;

    j++;

    if (function_index <= 4) {

        unknown = unknown + handle_arguments_code(function_index, 0, mission[j], IC, mc, &checker);

        if (unknown > 0) {

            keeper[counter] = *IC + 1;

        }

        unknown = unknown + handle_arguments_code(function_index, 1, mission[j + 1], IC, mc, &checker);

            if (unknown > 0) {

                keeper[counter] = * IC + 1;

            }
    } else if (function_index < 14) {

        mc[*IC][18] = '1';

        unknown = unknown + handle_arguments_code(function_index, 1, mission[j], IC, mc, &checker);

        if (unknown > 0) {

            keeper[counter] = *IC + 1;

        }
    }else {
        *IC = *IC -1;
    }

    if(checker) {
        if (checker == 1) {
            mc[*IC + 1][18] = '1';
            dec_to_bin(atoi(mission[j] + 1), mc[*IC + 1], 2, 16);
            *IC = *IC + 1;
        } else if (checker == 2) {
            dec_to_bin(atoi(mission[j + 1] + 1), mc[*IC + 1 + unknown], 2, 16);
            mc[*IC + 1 + unknown][18] = '1';
            *IC = *IC + 1;
        } else {
            dec_to_bin(atoi(mission[j] + 1), mc[*IC + 1], 2, 16);
            dec_to_bin(atoi(mission[j + 1] + 1), mc[*IC + 2], 2, 16);
            mc[*IC + 1][18] = '1';
            mc[*IC + 2][18] = '1';
            *IC = *IC + 1;
        }
    }
    *IC = *IC + unknown + 1;
}
void handle_machine_code_stage_two(char mission[MAX][MAX], int * words, int * IC, int * DC,

                                   char mc[MACHINE_CODES][MACHINE_CODE_ROW], struct symbol *symbols, int keeper[MAX_keeper], int counter,

                                   int symbol_index, int label, int * flag) {

    int j = 0, function_index, temp = 0;

    int first = keeper[counter];

    int secund = first;

    function_index = get_operand_op_code(mission[label]);

    if (first == 0) {

        return;

    }

    if(function_index < 4 && mission[label + 1][0] == '#'){
        temp = secund;
        secund--;
    }

    if ((mc[first - 1][6] == '1' && mc[first - 1][7] == '0') || (mc[first - 1][6] == '0' && mc[first - 1][7] == '1')) {

        j = check_if_symbol(mission[1 + label], symbols, * words, symbol_index, label);
        if (j == -1) {

            * flag = 1;

            return;

        }

        if (symbols[j]._isExt == 1) {

            mc[first][16] = '1';

            mc[first + 1][16] = '1';

        } else {

            mc[first][17] = '1';

            mc[first + 1][17] = '1';

        }

        dec_to_bin(symbols[j]._base, mc[first], 2, 16);

        dec_to_bin(symbols[j]._offset, mc[first + 1], 2, 4);

        secund++;

    }

    if ((mc[secund - 1][0] == '1' && mc[secund - 1][1] == '0') || (mc[secund - 1][0] == '0' && mc[secund - 1][1] == '1')) {

        if(temp){
            secund = temp + 1;
        }

	if((*words - label) == 2){
	    j = check_if_symbol(mission[1 + label], symbols, * words, symbol_index, label);
	
	}

	else {

	    j = check_if_symbol(mission[2 + label], symbols, * words, symbol_index, label);
	
	}

        if (j == -1) {

            * flag = 1;

            return;

        }

        if (symbols[j]._isExt == 1) {

            mc[secund][16] = '1';

            mc[secund + 1][16] = '1';

        } else {

            mc[secund][17] = '1';

            mc[secund + 1][17] = '1';

        }

        dec_to_bin(symbols[j]._base, mc[secund], 2, 16);

        dec_to_bin(symbols[j]._offset, mc[secund + 1], 2, 4);

    }

}

int handle_arguments_code(int temp, int number, char * string, int * IC, char mc[MACHINE_CODES][MACHINE_CODE_ROW], int *checker) {

    switch (temp) {

        {

            case 0:

                mc[ * IC][18] = '1';

            if (!number) {

                if (addressing_method_0(string, number, mc, IC)) {
                    *checker = 1;
                    return 0;
                }
                else if (addressing_method_3(string, mc, IC, number))

                    return 0;

                else if (addressing_method_2(string, mc, IC, number))

                    return 2;

                else

                    mc[ * IC][6] = '1';

                return 2;

            } else {

                if (addressing_method_3(string, mc, IC, number))

                    return 0;

                else if (addressing_method_2(string, mc, IC, number))

                    return 2;

                else

                    mc[ * IC][0] = '1';

                return 2;

            }

            case 1:

                mc[ * IC][18] = '1';

            if (addressing_method_0(string, number, mc, IC)) {

                if(number && *checker == 1){
                    *checker = 5;
                }
                else if(number){

                    *checker = 2;
                } else{
                    *checker += 1;
                }

                return 0;
            }

            else if (addressing_method_3(string, mc, IC, number))

                return 0;

            else if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][6] = '1';

            return 2;

            case 2:

                mc[ * IC][18] = '1';

            if (!number) {

                dec_to_bin(10, & mc[ * IC][12], 2, 4); /* to start from here*/

                if (addressing_method_0(string, number, mc, IC)) {
                    *checker = 1;
                    return 0;
                }

                else if (addressing_method_3(string, mc, IC, number))

                    return 0;

                else if (addressing_method_2(string, mc, IC, number))

                    return 2;

                else

                    mc[ * IC][6] = '1';

                return 2;

            } else {


                if (addressing_method_3(string, mc, IC, number))

                    return 0;

                else if (addressing_method_2(string, mc, IC, number))

                    return 2;

                else

                    mc[ * IC][0] = '1';

                return 2;

            }

            case 3:

                mc[ * IC][18] = '1';

            if (!number) {

                dec_to_bin(11, & mc[ * IC][12], 2, 4);

                if (addressing_method_0(string, number, mc, IC)) {
                    *checker += 1;
                    return 0;
                }

                else if (addressing_method_3(string, mc, IC, number))

                    return 0;

                else if (addressing_method_2(string, mc, IC, number))

                    return 2;

                else

                    mc[ * IC][6] = '1';

                return 2;

            } else {



                if (addressing_method_3(string, mc, IC, number))

                    return 0;

                else if (addressing_method_2(string, mc, IC, number))

                    return 2;

                else

                    mc[ * IC][0] = '1';

                return 2;

            }

            case 4:

                mc[ * IC][18] = '1';

            if (!number) {

                if (addressing_method_2(string, mc, IC, number))

                    return 2;

                else

                    mc[ * IC][6] = '1';

                return 2;

            } else {

                if (addressing_method_3(string, mc, IC, number))

                    return 0;

                else if (addressing_method_2(string, mc, IC, number))

                    return 2;

                else

                    mc[ * IC][0] = '1';

                return 2;

            }

            case 5:

                mc[ * IC][18] = '1';

            dec_to_bin(10, & mc[ * IC][12], 2, 4);

            if (addressing_method_3(string, mc, IC, number))

                return 0;

            else if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][6] = '1';

            return 2;

            case 6:

                mc[ * IC][18] = '1';

            dec_to_bin(11, & mc[ * IC][12], 2, 4);

            if (addressing_method_3(string, mc, IC, number))

                return 0;

            else if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][6] = '1';

            return 2;

            case 7:

                mc[ * IC][18] = '1';

            dec_to_bin(12, & mc[ * IC][12], 2, 4);

            if (addressing_method_3(string, mc, IC, 1))

                return 0;

            else if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][6] = '1';

            return 2;

            case 8:

                mc[ * IC][18] = '1';

            dec_to_bin(13, & mc[ * IC][12], 2, 4);

            if (addressing_method_3(string, mc, IC, number))

                return 0;

            else if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][0] = '1';

            return 2;

            case 9:

                mc[ * IC][18] = '1';

            dec_to_bin(10, & mc[ * IC][12], 2, 4);

            if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][6] = '1';

            return 2;

            case 10:

                mc[ * IC][18] = '1';

            dec_to_bin(11, & mc[ * IC][12], 2, 4);

            if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][0] = '1';

            return 2;

            case 11:

                mc[ * IC][18] = '1';

            dec_to_bin(12, & mc[ * IC][12], 2, 4);

            if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][6] = '1';

            return 2;

            case 12:

                mc[ * IC][18] = '1';

            if (addressing_method_3(string, mc, IC, number))

                return 0;

            else if (addressing_method_2(string, mc, IC, number))

                return 2;

            else

                mc[ * IC][6] = '1';

            return 2;

            case 13:

                mc[ * IC][18] = '1';

            if (addressing_method_0(string, number, mc, IC)) {
                *checker += 1;
                return 0;
            }

            else if (addressing_method_3(string, mc, IC, 0))

                return 0;

            else if (addressing_method_2(string, mc, IC, 0))

                return 2;

            else

                mc[ * IC][6] = '1';

            return 2;

            default:

                printf("unexpected op code");

            return 0;

        }

    }

}

int get_operand_op_code(char command[MAX]) {

    int i = 0;

    remove_spaces(command);

    for (; i < 16; i++) {

        if (!strcmp(command, instruction_name[i]))

            return i;

    }

    return 16;

}

int find_register(char * string) {

    int i = 0;

    remove_spaces(string);

    for (; i < 16; i++) {

        if (!strcmp(string, register_name[i]))

            return i;

    }

    return i;

}

int check_string(char * string, int len) {

    if (string[0] == 34 && string[len - 1] == 34) {

        return 1;

    }

    return 0;

}

int check_number(char * number) {

    int i;

    for (i = 0; i < strlen(number); i++) {

        if (number[i] == '-' || number[i] == '+') {

            continue;

        }

        if (number[i] < '0' || number[i] > '9') {

            return 0;

        }

    }

    return 1;

}

int addressing_method_0(char string[MAX], int number, char mc[MACHINE_CODES][MACHINE_CODE_ROW], int * IC) {

    if ( * string == '#' && check_number(string + 1)) {


        return 1;

    }

    return 0;

}

int addressing_method_3(char string[MAX], char mc[MACHINE_CODES][MACHINE_CODE_ROW], int * IC, int number) {

    int k;

    if ((k = find_register(string)) != 16) {

        if (number) {

            dec_to_bin(k, & mc[ * IC][2], 2, 4);

            mc[ * IC][0] = '1';

            mc[ * IC][1] = '1';
        } else {

            dec_to_bin(k, & mc[ * IC][8], 2, 4);

            mc[ * IC][6] = '1';

            mc[ * IC][7] = '1';
        }

        return 1;

    }

    return 0;

}

int addressing_method_2(char string[MAX], char mc[MACHINE_CODES][MACHINE_CODE_ROW], int * IC, int number) {

    int k;

    char * r;

    int index;

    if (string[strlen(string) - 1] == ']') {

        r = strchr(string, 'r');

        index = (int)(r - string);

        k = atoi(string + index + 1);

        if (k < 16) {

            if (number) {

                dec_to_bin(k, & mc[ * IC][2], 2, 4);

                mc[ * IC][1] = '1';

            } else {

                dec_to_bin(k, & mc[ * IC][8], 2, 4);

                mc[ * IC][7] = '1';

            }

            return 1;

        }

        return 0;

    }

    return 0;

}

void data_handler(char mission[MAX][MAX], int * words, char type[], int * IC, int * DC, int * flag, int L, char mc[MACHINE_CODES][MACHINE_CODE_ROW]) {

    int j = 0, len = 0;

    char * string = mission[ * words - 1];

    if (!strcmp(type, ".string")) {

        if (L > 1) {

            printf("where is too many arguments for the command .string");

            return;

        }

        len = strlen(string);

        if (!check_string(string, len)) {

            printf("\nthe string missing "
                   " \n");

            return;

        }

        string[len - 1] = '\0';

        string = & string[1];

        while (string[j] != '\0') {

            mc[ * IC][18] = '1';

            dec_to_bin(string[j], & mc[ * IC][0], 2, 16);

            * IC = * IC + 1;

            * DC = * DC + 1;

            j++;

        }

        mc[ * IC][18] = '1';

        * IC = * IC + 1;

    } else if (!strcmp(type, ".data")) {

        j = * words - L;

        while (mission[j][0] != '\0') {

            if (!check_number(mission[j])) {

                printf("the number: %s isn't allowed.\n", mission[j]);

                j++;

                continue;

            }

            mc[ * IC][18] = '1';

            dec_to_bin(atoi(mission[j]), & mc[ * IC][0], 2, 16);

            * IC = * IC + 1;

            * DC = * DC + 1;

            j++;

        }

    }

}
