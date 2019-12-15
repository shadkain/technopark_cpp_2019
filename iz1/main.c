// Выполнил: Кряжев Святослав, АПО-13
// Преподаватель: Илья Санеев

// Чтобы показать информацию о пришедшем письме, нужно сначала её найти.
// Для этого в файле письма необходимо найти специальные заголовки.
//
// Составить программу построчной фильтрации текста.
// Суть фильтра — отбор строк, начинающихся с одного из следующих выражений: «Date:», «From:»,«To:», «Subject:».
// Текстовые строки подаются на стандартный ввод программы, результат программы должен подаваться на стандартный вывод.
//
// Процедура фильтрации должна быть оформлена в виде отдельной функции, которой подается на вход массив строк,
// выделенных в динамической памяти и его длина.
// На выходе функция возвращает указатель на NULL-терминированный массив с найденными строками
// (последним элементом массива добавлен NULL для обозначения, что данных больше нет).
//
// Программа должна уметь обрабатывать возникающие ошибки (например, ошибки выделения памяти).
// В случае возникновения ошибки нужно вывести в поток стандартного вывода сообщение "[error]"
// и завершить выполнение программы.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ERROR_MSG       "[error]"

#define STD_CAPACITY    16
#define STD_STR_LENGTH  256


// Structure declaration

typedef struct string_array {
    size_t capacity;
    size_t size;
    char** array;
} string_array;


// Functions declarations

/* Just returns 'false' on failure */
bool read_stream(string_array* object_ptr);

/* Just returns 'false' on failure */
bool process_filtering(const string_array* origin, string_array* copy);

/* Returns 'true' if suitable, 'false' otherwise */
bool is_suitable_string(const char* string);

/* Allocates memory for array field, returns 'false' on failure */
bool alloc(string_array* object_ptr, size_t capacity);

/* Frees memory of array field */
void dealloc(string_array* object_ptr);

/* Prints all strings into stdout */
void print_strings(const string_array* object_ptr);


int main() {
    // Creating stream object and allocating array field
    string_array stream = {.capacity = 0, .size = 0, .array = NULL};
    if (!alloc(&stream, STD_CAPACITY)) {
        printf(ERROR_MSG);
        return 0;
    }

    // Reading from std input stream
    if (!read_stream(&stream)) {
        dealloc(&stream);
        printf(ERROR_MSG);
        return 0;
    }

    // Creating filtered stream object and allocating array field
    string_array filtered_stream = {.capacity = 0, .size = 0, .array = NULL};
    if (!alloc(&filtered_stream, stream.size + 1)) {
        dealloc(&stream);
        printf(ERROR_MSG);
        return 0;
    }

    // Filtering stream
    if (!process_filtering(&stream, &filtered_stream)) {
        dealloc(&stream);
        dealloc(&filtered_stream);
        printf(ERROR_MSG);
        return 0;
    }

    // Outputting results
    print_strings(&filtered_stream);

    // Freeing memory reserved for arrays
    dealloc(&stream);
    dealloc(&filtered_stream);

    return 0;
}

// Functions implementations

bool read_stream(string_array* object_ptr) {
    if (!object_ptr)
        return false;

    if (!object_ptr->array)
        return false;

    int c = 0;

    while (c != EOF) {
        // Checking if it's enough memory to store strings and extending if it's not
        if (object_ptr->size >= object_ptr->capacity) {
            char** boofer = (char**) realloc(object_ptr->array, 2 * object_ptr->capacity * sizeof(char*));
            if (!boofer)
                return false;

            object_ptr->array = boofer;
            object_ptr->capacity *= 2;
        }

        // Allocating new string
        object_ptr->array[object_ptr->size] = (char*) malloc(STD_STR_LENGTH * sizeof(char));
        if (!object_ptr->array[object_ptr->size])
            return false;

        object_ptr->size++;

        size_t str_capacity = STD_STR_LENGTH;
        size_t length = 0;

        for (c = getchar(); c != '\n' && c != EOF; c = getchar()) {
            // Checking if it's enough memory to store string and extending if it's not
            if (length >= str_capacity - 1) {
                char* boofer = (char*) realloc(object_ptr->array[object_ptr->size - 1], 2 * str_capacity * sizeof(char));
                if (!boofer)
                    return false;

                object_ptr->array[object_ptr->size - 1] = boofer;
                str_capacity *= 2;
            }

            object_ptr->array[object_ptr->size - 1][length] = (char) c;
            length++;
        }

        object_ptr->array[object_ptr->size - 1][length] = '\0';
    }

    return true;
}

bool process_filtering(const string_array* origin, string_array* copy) {
    if (!origin || !copy)
        return false;

    if (!origin->array || !copy->array)
        return false;

    // There's no need to extend string array, 'cause of this condition (reserving extra place for NULL-string)
    if (copy->capacity < origin->size + 1)
        return false;

    for (size_t i = 0; i < origin->size; ++i) {
        if (!is_suitable_string(origin->array[i]))
            continue;

        // Creating new string and copying string from origin
        size_t required_length = strlen(origin->array[i]) + 1;
        copy->array[copy->size] = malloc(required_length * sizeof(char));
        if (!copy->array[copy->size])
            return false;

        snprintf(copy->array[copy->size], required_length, "%s", origin->array[i]);

        copy->size++;
    }

    return true;
}

bool is_suitable_string(const char* string) {
    if (!string)
        return false;

    char c = string[0];

    char* check_word;

    switch (c) {
        case 'D':
            check_word = "Date:";
            break;

        case 'F':
            check_word = "From:";
            break;

        case 'T':
            check_word = "To:";
            break;

        case 'S':
            check_word = "Subject:";
            break;

        default:
            return false;
    }

    size_t i = 1;
    for (; string[i] != '\0' && i < strlen(check_word); ++i) {
        if (string[i] != check_word[i])
            return false;
    }

    if (i != strlen(check_word))
        return false;

    return true;
}

bool alloc(string_array* object_ptr, size_t capacity) {
    if (!object_ptr)
        return false;

    if (object_ptr->array)
        return false;

    object_ptr->array = (char**) calloc(capacity, sizeof(char*));
    if (!object_ptr->array)
        return false;

    object_ptr->capacity = capacity;

    return true;
}

void dealloc(string_array* object_ptr) {
    if (!object_ptr)
        return;

    if (!object_ptr->array)
        return;

    for (size_t i = 0; i < object_ptr->size; ++i) {
        if (object_ptr->array[i]) {
            free(object_ptr->array[i]);
            object_ptr->array[i] = NULL;
        }
    }

    free(object_ptr->array);
    object_ptr->array = NULL;
}

void print_strings(const string_array* object_ptr) {
    if (!object_ptr)
        return;

    if (!object_ptr->array)
        return;

    for (size_t i = 0; i < object_ptr->size; ++i) {
        if (object_ptr->array[i])
            printf("%s\n", object_ptr->array[i]);
    }
}