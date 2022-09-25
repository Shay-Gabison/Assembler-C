#include "lines.h"

struct symbol{
    char _name[SYMBOL_LENGTH];
    int _value;
    int _base;
    int _offset;
    int _isCode;
    int _isdata;
    int _isIns;
    int _isExt;
    int _isEnt;
};

/* check if there is a symbol in the line */
int check_if_symbol(char mission[MAX], struct symbol *symbols, int words, int symbol_index, int label);

/* this function checks if there a sy*/
int edit_or_create_symbol( struct symbol *symbols, int *symbols_index, char name[], int value, int base,
                          int offset, int code, int data, int ins, int ext, int ent);

int check_if_symbol(char mission[MAX], struct symbol *symbols, int words, int symbol_index, int label) {
    int i;
    for (i = 0; i < symbol_index; i++){
        if(strstr(mission,symbols[i]._name)){
            return i;
        }
    }
    printf("\n there isn't any symbol named: %s. in the symbol table \n", mission);
    return -1;
}

int edit_or_create_symbol(struct symbol *symbols, int *symbols_index, char name[], int value, int base,
                          int offset, int code, int data, int ins, int ext, int ent){
    /* what to do if there isn't any symbol or there is */
    int i;
    char sName[MAX];
    if(name[strlen(name) -1] == ':'){
        strcpy(sName, name);
        sName[strlen(sName) -1] = '\0';
    } else{
        strcpy(sName, name);
    }
    for(i = 0;i < *symbols_index;i++){/* checking if the symbol exist*/
        if(!strcmp(symbols[i]._name,sName)){
            if(symbols[i]._isEnt){
                return 1;
            }
            if(!(symbols[i]._isEnt) && ent){
                symbols[i]._isEnt=ent;
                return 1;
            }
            else{
                if(ext && !symbols[i]._isExt){
                    printf("\nthere is a symbol with this name without the external value\n");
                    return 0;
                }
                printf("\nthere is a symbol with this name\n");
                return 0;
            }
        }
    }
    strcpy(symbols[*symbols_index]._name, sName);
    symbols[*symbols_index]._value=value;
    symbols[*symbols_index]._base=base;
    symbols[*symbols_index]._offset=offset;
    symbols[*symbols_index]._isCode=code;
    symbols[*symbols_index]._isdata=data;
    symbols[*symbols_index]._isIns=ins;
    symbols[*symbols_index]._isExt=ext;
    symbols[*symbols_index]._isEnt=ent;
    *symbols_index = *symbols_index + 1;
    return 1;
}


void get_base_and_offset(int value,int temp_address[2]){
    temp_address[0]=value%16;/*base address*/
    temp_address[1]=value-value%16;/*offset*/

}
