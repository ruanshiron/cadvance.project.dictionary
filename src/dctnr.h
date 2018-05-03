#include <stdio.h>
#include "bt-5.0.0/inc/btree.h"

#define WORD_MAX 255
#define MEAN_MAX 10000

int covert_text_to_bt(char *);
int find_meaning_word(BTA * data, char * word, char * mean);
int delete_meaning_word(BTA * data, char * word);
int existed_word(BTA * data, char * word);
int find_next_word(BTA * data, char *word);
int add_a_word(BTA * data, char * word, char * mean);
int update_a_word(BTA * data, char * word, char * mean);
int isBlank(char * text);
