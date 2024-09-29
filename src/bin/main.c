#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include "reads.c"
#include <wchar.h>
#include <locale.h>
#include <math.h>
#include <openssl/sha.h>

#define MAX_STRINGS 10    
#define MAX_STRING_LEN 1000


long hash_djb2(unsigned char *str) {
    long hash = 15485863; // Initial value
    int c;

    while ((c = *str++)) {
        // hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

int main() {
    setlocale(LC_ALL, "en_US.UTF-8");

    char input[MAX_STRING_LEN];
    int num_hash_functions;
    int num_bands;
    float threshold;
    int len;

    readIntFromInput(&num_hash_functions);
    HashFunction* hash_functions = generate_hash_functions(num_hash_functions);

    readIntFromInput(&num_bands);
    readFloatFromInput(&threshold);

    readIntFromInput(&len);

    char **strings = calloc(MAX_STRING_LEN + 1, sizeof(char *));
    readNStrings(len, strings);

    int ** signatures = calloc(len, sizeof(int *));
    //signatures [l][n] - номер строки, номер хэш фунцкии

    getSignatures(strings, len, hash_functions, num_hash_functions, signatures);

    int i;
    int width = (int) (num_hash_functions / num_bands);
    
    //запись срезов сигнатур в словарь
    Dictionary* dict = createDictionary();
    
    //сумма среза сигнатур
    int j;
    int band_sign_sum = 0;


    for (i = 0; i < len; i++) {
        for(j = 0; j < num_hash_functions; j++) {
            int num_band = (int) j / width;
            band_sign_sum += signatures[i][j];
            if (j % width == 0 && j != 0) {
                insert(dict, num_band, band_sign_sum, i);
                band_sign_sum = 0;
            } 
        }
    }


    printAllValues(dict);

    freeDictionary(dict);

    for (i = 0; i < len; i++) {
        free(signatures[i]);
    }

    for (i = 0; i < len; i++) {
        free(strings[i]);
    }

    free(signatures);
    free(strings);
    free(hash_functions);
    return 0;
}
