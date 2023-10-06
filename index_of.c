
#include <stdio.h>
#include <assert.h>

int startswith(char *str, int str_len, char *substr, int substr_len){
    if(str_len - substr_len < 0){
        return 0;
    }

    while(substr_len > 0){
        char c1 = *str++;
        char c2 = *substr++;

        if(c1 != c2){
            return 0;
        }

        substr_len -= 1;
    }

    return 1;
}

int index_of(char *str, int str_len, char *substr, int substr_len){

    int c2 = substr[0];

    for(int cur_idx=0;;){
        if(str_len < substr_len){
            return -1;
        }

        int c1 = *str++;

        str_len -= 1;

        if(c1 == c2){
            if(startswith(str, str_len, substr+1, substr_len-1)){
                return cur_idx;
            }
        }

        cur_idx += 1;
    }
}

int main(void){

    assert(startswith("asd", 3, "s", 1) == 0);
    assert(startswith("asd", 3, "a", 1) == 1);
    assert(startswith("asd", 3, "as", 2) == 1);
    assert(startswith("asd", 3, "asd", 3) == 1);
    assert(startswith("asd", 3, "asdf", 4) == 0);
    assert(startswith("asd", 3, "sd", 2) == 0);

    assert(index_of("asd", 3, "sd", 2) == 1);
    assert(index_of("asd", 3, "d", 1) == 2);
    assert(index_of("asd", 3, "asd", 3) == 0);
    assert(index_of("asd", 3, "asdf", 4) == -1);

    return 0;
}