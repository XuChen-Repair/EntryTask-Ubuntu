#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

void split(char *str, const char *delim, char *token[3]) {
  char *temp;

  temp = strtok(str, delim);
  int i = 0;

  while (temp != NULL) {
    token[i++] = temp;
    temp = strtok(NULL, delim);
  }
}

string char_to_str(char *buf, int length) {
  char temp[length + 1];

  int i = 0;
  while(length != i) {
    temp[i] = *buf;
    
    *buf++;
    i++;
  }
  temp[i] = '\0';

  return temp;
}

int main()
{
   char str[80] = "matching&username=xuchen&password=7360300xc";
   string xc = char_to_str(str, 43);

   cout << xc << endl;

   return(0);
}