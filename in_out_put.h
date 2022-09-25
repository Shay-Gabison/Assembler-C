/* this function receive a string , separate the words and counts them. */

void scan_row(char data[MAX], char mission[MAX][MAX], int * words);

/* this function removes all the white chars before the first non-white chars */

void clear_path(char data[MAX], int * i) {

    while (data[ * i] == ' ' || data[ * i] == '\t') {
        /* cleaning all spaces and tabs before first latter*/

        * i = * i + 1;

    }

}

/*this function checks if the word is a special word*/

int check_special_word(char mission[MAX][MAX], int * words) {

    if (!strcmp(mission[ * words], ".data") || !strcmp(mission[ * words], ".string") ||

        !strcmp(mission[ * words], "not") || !strcmp(mission[ * words], "inc") ||

        !strcmp(mission[ * words], "mov") || !strcmp(mission[ * words], "cmp") ||

        !strcmp(mission[ * words], "add") || !strcmp(mission[ * words], "sub") ||

        !strcmp(mission[ * words], "lea") || !strcmp(mission[ * words], "clr") ||

        !strcmp(mission[ * words], "dec") || !strcmp(mission[ * words], "jmp") ||

        !strcmp(mission[ * words], "bne") || !strcmp(mission[ * words], "jsr") ||

        !strcmp(mission[ * words], "red") || !strcmp(mission[ * words], "prn") ||

        !strcmp(mission[ * words], "rts") || !strcmp(mission[ * words], "stop") ||

        !strcmp(mission[ * words], ".extern") || !strcmp(mission[ * words], ".entry")) {

        return 1;

    }

    return 0;

}

/*this function checks if there is a label in the data and build the first words to mission */

int check_label_and_functions(char data[MAX], char mission[MAX][MAX], int * i, int * j, int * words) {

    int temp = 0, flag = 0;

    while (data[ * i] != '\n' && data[ * i] != EOF && data[ * i] != '\0') {

        if (data[ * i] != ' ' && data[ * i] != '\t') {

            mission[ * words][ * j] = data[ * i];

            * j += 1;

            * i += 1;

            continue;

        } else {

            if (data[ * i - 1] == ':') {

                mission[ * words][ * j] = '\0';

                * j = 0;

                * i += 1;

                * words = * words + 1;

                clear_path(data, i);

                temp++;

                continue;

            }

            if (mission[ * words][strlen(mission[ * words]) - 1] == ',') {

                flag = 1;

                mission[ * words][strlen(mission[ * words]) - 1] = '\0';

            }

            if (check_special_word(mission, words)) {

                temp++;

                mission[ * words][ * j] = '\0';

                * j += 1;

                if (flag) {

                    printf("comma after command\n");

                    return 1;

                }

                * i += 1;

                clear_path(data, i);

            }

            if (!temp) {

                printf("the first word isn't allowed");

                return 1;

            }

            if (mission[ * words][ * j - 1] == ',') {

                * j -= 1;

            }

            mission[ * words][ * j] = '\0';

            * j = 0;

            * words = * words + 1;

            clear_path(data, i);

            break;

        }

    }

    return 0;

}

/* rebuild the data without spaces and tabs, this will allow to build the arguments more easy */

int rebuild_data_without_spaces(char data[MAX], int * i, int * j) {

    int in_word = 0;

    while (data[ * i] != '\0' && data[ * i] != '\n' && data[ * i] != EOF) {
        /*removing spaces and tabs in data input*/

        if (data[ * i] != ' ' && data[ * i] != '\t') {

            if (data[ * i] == ',') {

                in_word = 0;

            } else {

                if (in_word == 2) {

                    printf("Missing comma\n");

                    return 1;

                }

                in_word = 1;

            }

            data[ * j] = data[ * i];

            * j += 1;

        } else if (in_word == 1) {

            in_word = 2;

        }

        * i += 1;

    }

    return 0;

}

/* build the arguments to mission */

int building_args(char data[MAX], char mission[MAX][MAX], int * i, int * j, int * words) {

    while (data[ * i] != '\0' && data[ * i] != EOF) {
        /*building args*/

        if (data[ * i] == ',') {

            mission[ * words][ * j] = '\0';

            * words = * words + 1;

            if ( * j == 0 || (data[ * i] == ',' && data[ * i + 1] == '\0')) {

                printf("Multiple consecutive commas\n");

                return 1;

            }

            * j = 0;

            * i += 1;

        } else {

            mission[ * words][ * j] = data[ * i];

            * j += 1;

            * i += 1;

        }

    }

    return 0;

}

void scan_row(char data[MAX], char mission[MAX][MAX], int * words) {

    int i = 0, j = 0;

    * words = 0;

    clear_path(data, & i);

    if (check_label_and_functions(data, mission, & i, & j, words)) {

        return;

    }

    j = 0;

    memset(data, '\0', i - 1);

    if (rebuild_data_without_spaces(data, & i, & j)) {

        return;

    }

    data[j] = '\0';

    i = 0;

    j = 0;

    if (building_args(data, mission, & i, & j, words)) {

        return;

    }

    if ( * words == 1 && mission[1][0] == '\0') {

        * words = * words - 1;

    }

    * words = * words + 1;

    memset(mission[ * words + 1], '\0', MAX);

    if (mission[ * words - 1][strlen(mission[ * words - 1]) - 1] == 13) {

        mission[ * words - 1][strlen(mission[ * words - 1]) - 1] = '\0';

    }

    if (mission[ * words - 1][0] == '\0') {

        * words = * words - 1;

    }

}
