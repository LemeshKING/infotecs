#include "lib.h"
#include <algorithm>
#include <iostream>


void sortAndChange(std::string& str)
{
   std::sort(str.begin(), str.end());
   for (size_t i = 0; i < str.size(); i++)
      if (isdigit(str[i]) && (str[i] - '0') % 2 == 0)
      {
         str.replace(i, 1, "KB");
         i++;
      }
}

int calculateSum(const std::string& str)
{
   int sum = 0;
   for(const char& i : str)
      if(isdigit(i))
         sum += i - '0';
   return sum;
}

bool analyzeString(const std::string& str)
{
   return str.size() > 2 && str.size() % 32 == 0;
}
