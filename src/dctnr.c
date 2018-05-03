#include "dctnr.h"

void separate_mean(char* mean) 
{
    int i = 0, j = 1;
    while (mean[j] != '\0') 
    {
        if (mean[j] == '\\' && mean[j + 1] == 'n') 
        {
            mean[i++] = '\n';
            j += 2;
        }
        else {
            if (i != j)
                mean[i++] = mean[j++];
            else 
            {
                i++; j++;
            }
        }
    }
    mean[i] = '\0';
}

int convert_text_to_bt(char * filename)
{
    FILE * datafile;
    if ((datafile = fopen(filename, "r")) == NULL) 
    {
        printf("Loi mo file\n");
        return 0;;
    };

    BTA *convertfile;
    convertfile = btcrt("resource.dat", 0, 0);

    char word[WORD_MAX], mean[MEAN_MAX];
    int line;

    while (fscanf(datafile, "%[^\t]", word) == 1) 
    {
        fgets(mean, MEAN_MAX, datafile);
        line++;
        separate_mean(mean);
        btins(convertfile, word, mean, strlen(mean) + 1);    
    }

    printf("%d lines.\n",line);
    btcls(convertfile);
    fclose(datafile);
    return 1;
}

int existed_word(BTA * data, char * word)
{
    int rsize;
    if (bfndky(data, word, &rsize)==0) return 1;
    else return 0;
}

int find_meaning_word(BTA * data, char * word, char * mean)
{
    int rsize;
    btsel(data, word, mean, MEAN_MAX, &rsize);
    printf("result for find %s: %d\n", word, rsize);
    return rsize;
}

int delete_meaning_word(BTA * data, char * word)
{
    int r = btdel(data, word);
    return r;
}

int find_next_word(BTA * data, char *word)
{
    int value;
    int r = bnxtky(data, word, &value);
    return !value;
}

int add_a_word(BTA * data, char * word, char * mean)
{
    int r = btins(data, word, mean, strlen(mean) + 1);
    return r;
}

int update_a_word(BTA * data, char * word, char * mean)
{
    int r = btupd(data, word, mean, strlen(mean) + 1);
    return r;
}

int isBlank(char * text)
{
    int i=0, l=strlen(text);
    while (i<l)
    {
        if (!((text[i]==' ') || (text[i]=='\n'))) 
        {  
            return 0;
        }
        else i++;
    }
    return 1;
}