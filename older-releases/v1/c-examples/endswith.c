
#include <assert.h>
#include <stdio.h>

int endswith(char *str, int str_len, char *substr, int substr_len){
    if(str_len < substr_len){
        return 0;
    }

    char *substr_start = substr;

    str += str_len;
    substr += substr_len;

    while(1){

        str -= 1;
        substr -= 1;

        if(substr < substr_start){
            return 1;
        }

        if(*str != *substr){
            return 0;
        }
    }
}

int main(void){

    assert(endswith("asd", 3, "fghg", 4) == 0);
    assert(endswith("asd", 3, "d", 1) == 1);
    assert(endswith("asd", 3, "ds", 2) == 0);
    assert(endswith("asd", 3, "asd", 3) == 1);
    assert(endswith("asd", 3, "fasd", 4) == 0);

    return 0;
}