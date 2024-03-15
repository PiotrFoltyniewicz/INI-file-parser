#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
    Create a C program that reads INI files using only the C standard library
    You are allowed to work in pairs
    Share the source code through eKursy
    Tips:
        Compile with -Wall -Wextra flags and resolve all warnings
        Use Valgrind to check for any memory errors or leaks.

Requirements for 3.0

    The program needs to receive two command line parameters in this format:

    $ ./program PATH-TO-INI-FILE.ini section.key

    For example:

    $ ./program test.ini data.velocity

    The program should display the value of the key in the specified section.

    For this example, the program needs to show the value associated with the velocity key in the [data] section.

    The program can work with the following constraints:
        Number of sections is less than 30.
        Length of section name is less than 30.
        Length of the key is less than 30.
        Length of the value is less than 30.

Requirements for 3.5

    The program should be able to identify invalid identifiers in an INI file, both in section names and keys (but not values).
        Invalid identifiers are those that contain any characters other than letters, digits, and dashes (-).
        If an invalid identifier is found, the program should show a message and halt.
    The program needs to identify when a section is missing and display a specific message like Failed to find section [data].
    The program must identify when a key is missing in a section that exists, displaying a message such as Failed to find key "velocity" in section [data].

Requirements for 4.0

///   Define C structures for storing key-value pairs and sections.
    Use these structures to read the entire INI file and search for the section.key specified by the user.

Requirements for 4.5

    The limitations specified in the 3.0 requirements are no longer relevant.
    As a result, the program needs to be able to handle arbitrary INI files containing section names, keys, or values of any length without being restricted by compile-time limits.
    The program should parse the INI file just once and adjust dynamically to accommodate data of different sizes.

Requirements for 5.0

    The program needs to differentiate between strings and numbers for values.

    The program should be able to process simple mathematical expressions provided through the command line, for example:

    $ ./program PATH-TO-INI-FILE.ini expression "distance.velocity * travel.time"

    The program has the following rules for evaluating expressions:
        For numbers, it should support addition (+), subtraction (-), multiplication (*), and division (/).
        For strings, it should support concatenation (+).
        An expression containing operands of different types is considered invalid.
        The use of operators -, *, and / with string operands is also invalid.
        In such cases, an error message should be displayed.
*/

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

enum Flag
{
  NONE,
  SECTIONNAME,
  KEY,
  VALUE
};

void flushString(char *buffer)
{
  int length = strlen(buffer);
  for (int i = 0; i < length; i++)
  {
    buffer[i] = NULL;
  }
}

int charValidity(char c)
{
  if (isalnum(c) || c == '-')
  {
    return 1;
  }
  return 0;
}

void resizeString(char *str)
{
  // TODO
}

struct Section *parseFile(char *filename)
{
  int sectionIndex = 0;
  FILE *file = fopen(filename, "r");
  char *line = NULL;
  size_t len = 0;
  // TODO change to realocate memory
  char *currentKey = malloc(sizeof(char) * 300);
  char *currentValue = malloc(sizeof(char) * 300);
  char *currentSectionName = malloc(sizeof(char) * 300);
  struct Section *sections = malloc(sizeof(struct Section));

  if (file == NULL)
    exit(EXIT_FAILURE);

  while (getline(&line, &len, file) != -1)
  {
    // skip comment line
    if (line[0] == ';')
      ;
    // getting sections
    else if (sscanf(line, "[%[^]]", currentSectionName) == 1)
    {
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
      sections[sectionIndex - 1].arrayOfTuples = realloc(sections[sectionIndex - 1].arrayOfTuples,
                                                         (sections[sectionIndex - 1].numTuples + 1) * sizeof(struct Tuple));
      int currentTuple = sections[sectionIndex - 1].numTuples;
      sections[sectionIndex - 1].arrayOfTuples[currentTuple].key = strdup(currentKey);
      sections[sectionIndex - 1].arrayOfTuples[currentTuple].value = strdup(currentValue);
      sections[sectionIndex - 1].numTuples++;
    }
  }

  fclose(file);
  if (line)
    free(line);
  return sections;
}

char *getValue(struct Section *data, char *sectionKey)
{
  char *section = strtok(sectionKey, ".");
  char *key = strtok(NULL, ".");
  printf("Looking for: %s %s \n", section, key);
  for (int i = 0; i < 14; i++)
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
    }
  };
  return "key not found";
}

int main(int argc, char *argv[])
{
  struct Section *data = parseFile(argv[1]);
  for (int i = 0; i < 14; i++)
  {
    printf("section:%d %s \n", i, data[i].sectionName);
    for (int j = 0; j < data[i].numTuples; j++)
    {
      printf("key :%s value: %s \n", data[i].arrayOfTuples[j].key, data[i].arrayOfTuples[j].value);
    }
  };
  if (argv[2])
  {
    printf("\n-----------\n");
    printf("value: %s", getValue(data, argv[2]));
  };

  return 0;
}