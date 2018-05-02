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

