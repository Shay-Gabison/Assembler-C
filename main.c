/*
 * this project was writen by Itay Cohen and Shay Gabison.
 * */

#include "main.h"


/* run: will run on the given files from the user. */
void run(int argc, char **argv) {
    FILE *fp;
    char files[MAX][MAX];
    int i;

    macro_handler(argc, argv, files); /*opens the Macros*/

    for (i = 0; i < argc - 1 ; i++) {
        fp = fopen(files[i], "r");
        file_assembler(fp, files[i]);
        fclose(fp);
    }
}

int main(int argc, char **argv) {
    run(argc, argv);
    return 0;
}

