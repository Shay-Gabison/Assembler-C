#include "lines.h"

#include "handler.h"


int file_assembler(FILE * fptr, char * file_name);

int stage_two(FILE * fptr, int * ICF, int * DCF, struct symbol *symbols, int * symbol_index,

              char mc[MACHINE_CODES][MACHINE_CODE_ROW],  int keeper[MAX_keeper]);

/*running on the file we got for the first time updating the ICF to the number of rows we need and filling the data we

 * can add without the second run (AKA stage two)*/

int file_assembler(FILE * fptr, char * file_name) {

    int ICF = START_OF_ICF, DCF = 0, symbols_index = 0;

    int size = MAX_SYMBOLS;

    char machine_codes[MACHINE_CODES][MACHINE_CODE_ROW];

    int keeper[MAX_keeper];

    struct symbol * symbols;

    char mission[MAX][MAX];

    int temp_address[2];

    char data[MAX];

    int cur, words, i, L, temp, temp_len, label, flag = 0, counter = 0;

    memset(keeper, 0, MAX_keeper * sizeof(int));

    symbols = (struct symbol*) malloc(size * sizeof(struct symbol));

    machine_code_reset(machine_codes);

    while (fgets(data, MAX, fptr) != NULL) {

        if(symbols_index < size && symbols_index > (size - 10)){

            symbols = symbols_increase(symbols, &size);

        }

        for (i = 0; i < MAX; i++) {

            memset(mission[i], '\0', MAX);

        }

        counter += 1;


        if (data[0] == '\0' || data[0] == '\n' || data[0] == 13) {

            continue;

        }

        i = 0;

        cur = 0;

        words = 0;

        L = 0;

        temp_len = 0;

        label = 0;

        clear_path(data, & i);

        if (data[i] == ';') {

            continue;

        }

        scan_row(data, mission, & words);

        temp_len = strlen(mission[cur]);

        /*lvl 3*/

        if (temp_len > 0 && mission[cur][temp_len - 1] == ':') {

            label = 1;

            /*to handle ICF for label? */

            cur = cur + 1;

            if (cur == words) {

                /*what to do? end of file to soon*/

            }

        }

        /*lvl 5*/

        if (!strcmp(mission[cur], ".data") || !strcmp(mission[cur], ".string")) {

            if (label) {

                get_base_and_offset(ICF, temp_address);

                temp = edit_or_create_symbol(symbols, & symbols_index, mission[0], ICF,

                                             temp_address[1], temp_address[0], 0, 1, 0, 0, 0);

                if (!temp) {

                    flag = 1;

                }

            }

            L = words - (cur + 1);

            data_handler(mission, & words, mission[cur], & ICF, & DCF, & flag, L, machine_codes);

            continue;

        }

        /*lvl 8*/

        if (!strcmp(mission[cur], ".entry")) {

            continue;

        }

        if (!strcmp(mission[cur], ".extern")) {

            cur = cur + 1;

            if (cur == words) {

                /*what to do? end of file to soon*/

            }

            temp = edit_or_create_symbol(symbols, & symbols_index, mission[cur], 0, 0, 0, 0,

                                         0, 0, 1, 0); /*if exist without ext  error*/

            if (!temp) {
                /*if exist without ext  error*/

                flag = 1;

            }

            continue;

        }

        /*lvl 11 this is an instruction row (: */

        if (label) {

            get_base_and_offset(ICF, temp_address);

            temp = edit_or_create_symbol(symbols, & symbols_index, mission[0], ICF, temp_address[1], temp_address[0],

                                         1, 0, 0, 0, 0);

            if (!temp) {

                flag = 1;

            }

        }

        if (words > 0) {

            L = job_secure(mission[cur]); /*handling the job creating 2 or 1 machines code rows and work with ICF*/

            if (L == -1) {

                flag = 1;

                continue;

            }

            if (words - label - 1 != L) {

                flag = 1;

                printf("the number of arguments: %d, are not the right. the job: %s needs: %d arguments",

                       (words - label - 1), mission[cur], L);

                continue;

            }

            handle_machine_code_stage_one(mission, mission[cur], & words, & ICF, & DCF,

                                          machine_codes, keeper, counter); /*calculate number of rows needed for this command line*/

            continue;

        }

    }

    /*lvl 17*/

    if (flag) {

        printf("\n the program is closing \n");

        return flag;

    }

    stage_two(fptr, & ICF, & DCF, symbols, &symbols_index, machine_codes, keeper);

    build_output_files(symbols, symbols_index, machine_codes, ICF, DCF, strtok(file_name, "."));
    free(symbols);
    return flag;

}

/*this function running on the file we got for the second time updating the missing rows

 * in the machine code using the symbols table*/

int stage_two(FILE * fptr, int * ICF, int * DCF, struct symbol *symbols, int * symbol_index,

              char mc[MACHINE_CODES][MACHINE_CODE_ROW], int keeper[MAX_keeper]) {

    char mission[MAX][MAX];

    char data[MAX];

    int cur, words, i, temp, temp_len, label, flag = 0, counter = 0;

    fseek(fptr, 0, SEEK_SET);

    while (fgets(data, MAX, fptr) != NULL) {
        /* runs on the file per row*/

        counter += 1;

        if (data[0] == '\0' || data[0] == '\n') {

            continue;

        }

        for (i = 0; i < MAX; i++) {

            memset(mission[i], 0, MAX);

        }

        i = 0;

        cur = 0;

        words = 0;

        temp = 0;

        temp_len = 0;

        label = 0;

        clear_path(data, & i);

        if (data[i] == ';') {

            continue;

        }

        scan_row(data, mission, & words); /*scan the data inside the current row*/

        temp_len = strlen(mission[cur]);

        if (temp_len > 0 && mission[cur][temp_len - 1] == ':') {
            /*checks if this is a label*/

            label = 1;

            cur = cur + 1;

            if (cur == words) {

                printf("to edit: mission to short");

            }

        }

        if (!strcmp(mission[cur], ".data") || !strcmp(mission[cur], ".string") || !strcmp(mission[cur], ".extern")) {

            continue;

        }

        if (!strcmp(mission[cur], ".entry")) {

            temp = edit_or_create_symbol(symbols, symbol_index, mission[cur + 1], * ICF, 0,

                                         0, 0, 0, 0, 0, 1);

            if (!temp) {

                flag = 1;

            }

            continue;

        }

        /*lvl 6*/

        if (words > 0) {
            /* stage 6 in the given algorithm */

            handle_machine_code_stage_two(mission, &words, ICF, DCF, mc, symbols,

                                          keeper, counter, * symbol_index, label, & flag);

            continue;

        }

    }

    return flag;

}
