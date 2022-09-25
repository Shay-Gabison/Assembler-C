#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <string.h>

#include "lines.h"


/* remove all the in the row */

void remove_spaces(char * str);

extern char * strtok_r(char * ,
                       const char * , char ** );

void build_output_files(struct symbol *symbols, int symbols_index, char mc[MACHINE_CODES][MACHINE_CODE_ROW], int ICF, int DCF, char * filename);

void true_macro(char * asd, char * line, char * line1, char * line2, FILE * fpin);

void check_macro(int macros_counter, char * macros, char * token_command, char * line2, FILE * fpout);

/* macro_handler: opens all the macros in the given files */

void macro_handler(int argc, char * argv[], char files[MAX][MAX]);

typedef struct {
    char * key;
    int val;
}
        bin_to_hex_dict;

static bin_to_hex_dict look_up_table[] = {

        {
                "0000",
                '0'
        },
        {
                "0001",
                '8'
        },
        {
                "0010",
                '4'
        },

        {
                "0011",
                'c'
        },
        {
                "0100",
                '2'
        },
        {
                "0101",
                'a'
        },

        {
                "0110",
                '6'
        },
        {
                "0111",
                'e'
        },
        {
                "1000",
                '1'
        },

        {
                "1001",
                '9'
        },
        {
                "1010",
                '5'
        },
        {
                "1011",
                'd'
        },

        {
                "1100",
                '3'
        },
        {
                "1101",
                'b'
        },
        {
                "1110",
                '7'
        },

        {
                "1111",
                'f'
        }

};

void remove_spaces(char * str) {

    int i;

    /* To keep track of non-space character count */

    int count = 0;

    /* Traverse the given string. If current character

     is not space, then place it at index 'count++' */

    for (i = 0; str[i]; i++) {

        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')

            str[count++] = str[i]; /* here count is incremented */

    }

    str[count] = '\0'; /* finish the string */

}

void macro_handler(int argc, char * argv[], char files[MAX][MAX]) {

    int i = 1, f = 0, j;

    int macros_counter = 0;

    char macros[MAX][MAX];

    char line[MAX];

    char line2[MAX];

    char * line1 = & line[0];

    char * file_name;

    char * token_command;

    for (; i < argc; i++) {

        FILE * fpin;

        FILE * fpout;

        if ((fpin = fopen(argv[i], "r")) == NULL) {

            printf("\nCan't open %s\n", argv[i]);

            continue;

        }

        token_command = strtok_r(argv[i], ".", & argv[i]);

        file_name = strcat(token_command, ".am");

        strcpy(files[f], file_name);

        f++;

        fpout = fopen(file_name, "w");

        while (fgets(line, MAX, fpin) != NULL) {

            strcpy(line2, line);

            line1 = & line[0];

            token_command = strtok_r(NULL, " ", & line1);

            remove_spaces(token_command);

            if (!strcmp(token_command, "macro")) {
                /* recognize macro definition */

                /* check if the word is macro*/

                token_command = strtok_r(NULL, " ", & line1); /* gets macro name into macro array */

                remove_spaces(token_command);

                strcpy(macros[macros_counter], token_command);

                macros_counter++;

                true_macro(token_command, line, line1, line2, fpin);

                continue;

            }

            check_macro(macros_counter, macros[0], token_command, line2, fpout);

        }

        for (j = 0; j < macros_counter; j++) {

            remove(macros[j]);

            strcpy(macros[j], "");

        }

        macros_counter = 0;

        fclose(fpin);

        fclose(fpout);

    }

}

/*this function saves the macro actuall actions into file*/

void true_macro(char * asd, char * line, char * line1, char * line2, FILE * fpin) {

    FILE * fp_help;


    fp_help = fopen(asd, "w");



    while (fgets(line, MAX, fpin) != NULL) {

        strcpy(line2, line);


        if (line[0] == 'e' && line[1] == 'n' && line[2] == 'd' && line[3] == 'm') {

            break;

        }

        fputs(line2, fp_help);

    }

    fclose(fp_help);

}

/* check_macro is check_if_symbolng the lines in the .as file where is the macro name is metioned and replace it by macro actions*/

void check_macro(int macros_counter, char * macros, char * token_command, char * line2, FILE * fpout) {

    int j;

    for (j = 0; j < macros_counter; j++) {

        if (!strcmp(token_command, & macros[j])) {

            FILE * fpHelp;

            fpHelp = fopen(token_command, "r");

            while (fgets(line2, MAX, fpHelp) != NULL) {

                fputs(line2, fpout);

            }

            fclose(fpHelp);

            break;

        }

    }

    if ((j == macros_counter)) {

        fputs(line2, fpout);

    }

}

void build_external(struct symbol *symbols, int symbols_index, char mc[MACHINE_CODES][MACHINE_CODE_ROW], int ICF, int DCF,
                    char * filename) {

    FILE * fptr;
    int i ,j;

    for (i = 0; i < symbols_index; i++) {

        if (symbols[i]._isExt) {

            fptr = fopen(strcat(filename, ".ext"), "w+");
		for(j=100;j<ICF;j++){

		if(mc[j][16]=='1')
			break;}

            fprintf(fptr, "%s BASE %d\n%s OFFSET %d\n\n", symbols[i]._name, j, symbols[i]._name,
                    j+1);

            i++;
		j+=2;
            for (; i < symbols_index; i++) {

                if (symbols[i]._isExt) {
		for(;j<ICF;j++){

		if((mc[j][16]=='1'))
			break;}
                    fprintf(fptr, "%s BASE %d\n%s OFFSET %d\n\n", symbols[i]._name, j, symbols[i]._name,

                    j+1);
                }

            }

            fclose(fptr);

        }

    }
}

void build_entry(struct symbol *symbols, int symbols_index, char mc[MACHINE_CODES][MACHINE_CODE_ROW], int ICF, int DCF,
                 char * filename) {

    FILE * fptr;

    int i = 0;

    for (; i < symbols_index; i++) {

        if (symbols[i]._isEnt) {

            fptr = fopen(strcat(filename, ".ent"), "w+");

            fprintf(fptr, "%s, %d, %d\n", symbols[i]._name, symbols[i]._base, symbols[i]._offset);

            i++;

            for (; i < symbols_index; i++) {

                if (symbols[i]._isEnt) {

                    fprintf(fptr, "%s, %d, %d\n", symbols[i]._name, symbols[i]._base, symbols[i]._offset);

                }

            }

            fclose(fptr);

        }

    }

}

char dictFind(char * s) {

    int i = 0;

    for (; i < 16; i++) {

        if (!strcmp(look_up_table[i].key, s)) {

            return look_up_table[i].val;

        }

    }

    return '-';

}

void bin_to_hexfunc(char machine_codes[MACHINE_CODES][MACHINE_CODE_ROW], int ICF, int DCF,
                    char * filename) {

    int i, j;

    char k ;

    char subtext[5];

    FILE * fptr;

    fptr = fopen(strcat(filename, ".ob"), "w+");

    subtext[5] = '\0';

    fprintf(fptr, "	        %d   %d\n", ICF - 100 - DCF - 1, DCF + 1);

    for (i = 100; i < ICF; i++) {

        fprintf(fptr, "0%d\t", i);

        for (j = 16, k = 'A'; j > 0; j -= 4, k++) {

            memcpy(subtext, & machine_codes[i][j], 4);

            fprintf(fptr, "%c%c-", k, dictFind(subtext));

            memset(subtext, 0, 4);

        }

        memcpy(subtext, & machine_codes[i][j], 4);

        fprintf(fptr, "%c%c\n", k, dictFind(subtext));

    }

    fclose(fptr);

}

void build_output_files(struct symbol *symbols, int symbols_index, char mc[MACHINE_CODES][MACHINE_CODE_ROW], int ICF, int DCF, char * filename) {

    bin_to_hexfunc(mc, ICF, DCF, strtok(filename, "."));

    build_external(symbols, symbols_index, mc, ICF, DCF, strtok(filename, "."));

    build_entry(symbols, symbols_index, mc, ICF, DCF, strtok(filename, "."));

}
