#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct Tuple
{
  char *key;
  char *value;
};

struct Section
{
  char *sectionName;
  struct Tuple *arrayOfTuples;
  int numTuples;
};

int validateString(char* str)
{
  int len = strlen(str);
  for(int i = 0; i < len; i++){
    if(isalnum(str[i]) || str[i] == '-'){
      continue;
    }else{
      return 0;
    }
  }
  return 1;
}

int isnumber(char* str){
  int len = strlen(str);
  for(int i = 0; i < len; i++){
    if(isdigit(str[i])){
      continue;
    }else{
      return 0;
    }
  }
  return 1;
}

struct Section *parseFile(char *filename, int* dataLength)
{
  int sectionIndex = 0;
  FILE *file = fopen(filename, "r");
  char *line = NULL;
  size_t len = 0;
  char *currentKey;
  char *currentValue;
  char *currentSectionName;
  struct Section *sections = malloc(sizeof(struct Section));

  if (file == NULL)
  {
    printf("File opening error.\n");
    exit(EXIT_FAILURE);
  }

  while (getline(&line, &len, file) != -1)
  {
    currentKey = malloc(sizeof(char) * len);
    currentValue = malloc(sizeof(char) * len);
    currentSectionName = malloc(sizeof(char) * len);
    // skip comment line
    if (line[0] == ';')
      ;
    // getting sections
    else if (sscanf(line, "[%[^]]", currentSectionName) == 1)
    {
      if(!validateString(currentSectionName)){
        printf("Invalid section name: %s", currentSectionName);
        exit(EXIT_FAILURE);
      }
      sections = realloc(sections, (sectionIndex + 1) * sizeof(struct Section));

      if (sections == NULL)
      {
        fclose(file);
        return NULL;
      }
      sections[sectionIndex].sectionName = strdup(currentSectionName);
      sections[sectionIndex].arrayOfTuples = NULL;
      sections[sectionIndex].numTuples = 0;
      sectionIndex++;
    }
    else if (sectionIndex > 0 && (sscanf(line, "%[^ =] = %[^\n]", currentKey, currentValue) == 2))
    {
      if(!validateString(currentKey)){
        printf("Invalid key name: %s", currentKey);
        exit(EXIT_FAILURE);
      }
      sections[sectionIndex - 1].arrayOfTuples = realloc(sections[sectionIndex - 1].arrayOfTuples,
                                                         (sections[sectionIndex - 1].numTuples + 1) * sizeof(struct Tuple));
      int currentTuple = sections[sectionIndex - 1].numTuples;
      sections[sectionIndex - 1].arrayOfTuples[currentTuple].key = strdup(currentKey);
      sections[sectionIndex - 1].arrayOfTuples[currentTuple].value = strdup(currentValue);
      sections[sectionIndex - 1].numTuples++;
    }
    free(currentKey);
    free(currentValue);
    free(currentSectionName);
    *dataLength = sectionIndex;
  }

  fclose(file);
  if (line)
    free(line);
  return sections;
}

char *getValue(struct Section *data, int dataLength, char *sectionKey)
{
  char *section = strtok(sectionKey, ".");
  char *key = strtok(NULL, ".");
  printf("Looking for: %s.%s \n", section, key);
  for (int i = 0; i < dataLength; i++)
  {
    if (strcmp(data[i].sectionName, section) == 0)
    {
      for (int j = 0; j < data[i].numTuples; j++)
      {
        if (strcmp(data[i].arrayOfTuples[j].key, key) == 0)
        {
          return data[i].arrayOfTuples[j].value;
        }
      }
      return "Key not found.";
    }
  }
  return "Section not found.";
}


char *evaluate(struct Section* data, int dataLength, char* expression){
  char* key1 = malloc(sizeof(char) * strlen(expression));
  char* key2 = malloc(sizeof(char) * strlen(expression));;
  char operator;
  
  if(sscanf(expression, "%s %c %s", key1, &operator, key2) == 3){
    char* value1 = strdup(getValue(data, dataLength, key1));
    char* value2 = strdup(getValue(data, dataLength, key2));

    if(isnumber(value1) && isnumber(value2)){
      char* stopStr;
      long long num1 = strtoll(value1, &stopStr, 10);
      long long num2 = strtoll(value2, &stopStr, 10);
      char* output = malloc(sizeof(char) * 256);
      switch (operator)
      {
      case '+':
        sprintf(output, "%lld", num1 + num2);
        break;
      case '-':
        sprintf(output, "%lld", num1 - num2);
        break;
      case '*':
        sprintf(output, "%lld", num1 * num2);
        break;
      case '/':
        sprintf(output, "%lld", num1 / num2);
        break; 
      default:
        strcpy(output, "Invalid operator for integers.\n");
        break;
      }
      free(value1);
      free(value2);
      free(key1);
      free(key2);
      return output;
    }
    else if(!isnumber(value1) && !isnumber(value2)){
      int bufferSize = (strlen(value1) + strlen(value2)) + 1 > 32 ? (strlen(value1) + strlen(value2)) : 32;
      char* output = malloc(sizeof(char) * bufferSize);
      if(operator == '+'){
        output = strcat(value1, value2);
      }
      else{
        strcpy(output, "Invalid operator for strings.\n");
      }
      free(value1);
      free(value2);
      free(key1);
      free(key2);
      return output;
    }
    else{
      free(value1);
      free(value2);
      free(key1);
      free(key2);
      return "Both values must be either strings or integers.\n";
    }
  }
  free(key1);
  free(key2);
}

void freeData(struct Section* data, int dataLength){
  for(int i = 0; i < dataLength; i++){
    free(data[i].sectionName);
    for(int j = 0; j < data[i].numTuples; j++){
      free(data[i].arrayOfTuples[j].value);
      free(data[i].arrayOfTuples[j].key);
    }
    free(data[i].arrayOfTuples);
  }
  free(data);
}

int main(int argc, char *argv[])
{
  int *dataLength = malloc(sizeof(int));
  struct Section *data = parseFile(argv[1], dataLength);

  int expressionFlag = 0;
  for(int i = 2; i < argc; i++){
    if(strcmp(argv[i], "expression") == 0){
      expressionFlag = 1;
      continue;
    }
    if(expressionFlag == 0){
      printf("Value: %s\n", getValue(data, *dataLength, argv[i]));
    }
    else if(expressionFlag == 1){
      printf("Evaluating: %s\n", argv[i]);
      printf("Value of expression: %s\n", evaluate(data, *dataLength, argv[i]));
      expressionFlag = 0;
    }
  }
  freeData(data, *dataLength);
  free(dataLength);
  return 0;
}