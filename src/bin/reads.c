#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>
#define MAX_STRING_LEN 1000
#include <openssl/sha.h>
#include <time.h>
#include <math.h>


bool readFloatFromInput(float *value) {
    char input[MAX_STRING_LEN]; 

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false; 
    }

    char *endptr;
    *value = strtof(input, &endptr);

    if (endptr == input || *endptr != '\n') {
        return false; 
    }

    return true; 
}

bool readIntFromInput(int *value) {
    char input[MAX_STRING_LEN]; 

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false; 
    }

    char *endptr;
    *value = strtof(input, &endptr);

        if (endptr == input || *endptr != '\n') {
        return false; 
    }

    return true; 
}

void readNStrings(int len, char ** strings) {
    char input[MAX_STRING_LEN];

    int i;
    for (i = 0; i < len; i++) {
            if (fgets(input, sizeof(input), stdin) != NULL) {
        // Remove the newline character from the input
        input[strcspn(input, "\n")] = '\0';

        strings[i] = calloc(MAX_STRING_LEN, sizeof(char));

        strcpy(strings[i], input);
        // printf("ACK %s \n", strings[i]);
    } else {
        fprintf(stderr, "Failed to read input\n");
        break; 
    }
        
    }
}



typedef struct {
    int a;
    int b;
} HashFunction;

HashFunction* generate_hash_functions(int num_hash_functions) {
    // Выделение памяти для массива структур HashFunction
    HashFunction* hash_functions = malloc(num_hash_functions * sizeof(HashFunction));
    if (hash_functions == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Инициализация генератора случайных чисел
    srand(time(NULL));

    // Генерация хеш-функций
    for (int i = 0; i < num_hash_functions; i++) {
        hash_functions[i].a = rand() % (1ULL << 32);  // Генерация a в диапазоне [0, 2^32 - 1]
        hash_functions[i].b = rand() % (1ULL << 32);  // Генерация b в диапазоне [0, 2^32 - 1]
    }

    return hash_functions;
}

void sha1_to_long(const unsigned char hash[10], unsigned long *result) {
    *result = 0;
    for (int i = 0; i < sizeof(unsigned long); i++) {
        *result = (*result << 8) | hash[i];
    }
}



void getSignatures(char **strings, int len, HashFunction* hash_functions, 
                    int  num_hash_functions, int ** signatures) {
    int i;
    int j;
    int p;

    int k = 2;

    // формирование сигнатур
    // итерация по строкам
    for(i = 0; i < len; i++) {
        signatures[i] = calloc(num_hash_functions, sizeof(int));

        // итерация по хеш функциям
        int h;      
        for (h = 0; h < num_hash_functions; h++) {    
                        // итерация по символам (шинглам)
            for(j = 0; j < MAX_STRING_LEN - 1; j++) { 
                if(!strings[i][j]) {
                    break;
                }

                // запись шингла
                char * shingle = calloc(k, sizeof(char));
    
                int sh;
                for(sh = 0; sh < k; sh++) {  
                    shingle[sh] = strings[i][j+sh];
                }

                // вычисление хеша для шингла
                unsigned char hash[21];
                unsigned long result;
    
                SHA1(shingle, k, hash);
                sha1_to_long(hash, &result);

                
                int sign = (hash_functions[h].a * result 
                                    + hash_functions[h].b) % (int)(pow(2, 32) - 1);
                
                // printf("%s %i \n", shingle, sign);
                // поиск минимального хэша
                if(j == 0 || sign < signatures[i][h]) {
                    signatures[i][h] = sign;
                }

                free(shingle);
            }
        }
    }

}




#define INITIAL_CAPACITY 1000
#define INITIAL_ARRAY_CAPACITY 500

typedef struct {
    int key1;
    int key2;
} Key;

typedef struct {
    int* array;
    int size;
    int capacity;
} DynamicArray;

typedef struct {
    Key key;
    DynamicArray value;
} KeyValuePair;

typedef struct {
    KeyValuePair* pairs;
    int size;
    int capacity;
} Dictionary;

DynamicArray createDynamicArray() {
    DynamicArray arr;
    arr.array = malloc(sizeof(int) * INITIAL_ARRAY_CAPACITY);
    arr.size = 0;
    arr.capacity = INITIAL_ARRAY_CAPACITY;
    return arr;
}

void addToArray(DynamicArray* arr, int value) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->array = realloc(arr->array, sizeof(int) * arr->capacity);
    }
    arr->array[arr->size++] = value;
}

Dictionary* createDictionary() {
    Dictionary* dict = malloc(sizeof(Dictionary));
    dict->pairs = malloc(sizeof(KeyValuePair) * INITIAL_CAPACITY);
    dict->size = 0;
    dict->capacity = INITIAL_CAPACITY;
    return dict;
}

KeyValuePair* findPair(Dictionary* dict, int key1, int key2) {
    for (int i = 0; i < dict->size; i++) {
        if (dict->pairs[i].key.key1 == key1 && dict->pairs[i].key.key2 == key2) {
            return &dict->pairs[i];
        }
    }
    return NULL;
}

void insert(Dictionary* dict, int key1, int key2, int value) {
    KeyValuePair* pair = findPair(dict, key1, key2);
    if (pair) {
        addToArray(&pair->value, value);
    } else {
        if (dict->size == dict->capacity) {
            dict->capacity *= 2;
            dict->pairs = realloc(dict->pairs, sizeof(KeyValuePair) * dict->capacity);
        }
        dict->pairs[dict->size].key.key1 = key1;
        dict->pairs[dict->size].key.key2 = key2;
        dict->pairs[dict->size].value = createDynamicArray();
        addToArray(&dict->pairs[dict->size].value, value);
        dict->size++;
    }
}

DynamicArray* get(Dictionary* dict, int key1, int key2) {
    KeyValuePair* pair = findPair(dict, key1, key2);
    return pair ? &pair->value : NULL;
}

void freeDictionary(Dictionary* dict) {
    for (int i = 0; i < dict->size; i++) {
        free(dict->pairs[i].value.array);
    }
    free(dict->pairs);
    free(dict);
}

void printArray(DynamicArray* arr) {
    printf("[");
    for (int i = 0; i < arr->size; i++) {
        printf("%d", arr->array[i]);
        if (i < arr->size - 1) printf(", ");
    }
    printf("]\n");
}


void printAllValues(Dictionary* dict) {
    for (int i = 0; i < dict->size; i++) {
        // printf("(%d,%d)", dict->pairs[i].key.key1, dict->pairs[i].key.key2);
        if (dict->pairs[i].value.size > 1){
                    printArray(&dict->pairs[i].value);
        }

    }
}