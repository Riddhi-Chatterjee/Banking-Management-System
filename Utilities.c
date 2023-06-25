#include "Headers/Utilities.h"

/*char str[50] = "Dummy string!!";
MYStringCopy(str, sizeof(str), "This is");
MYStringConcat(str, sizeof(str), " a demo of my C string functions ");
printf("%s\n", str);
printf("Length = %d\n", MYStringLength(str));*/

int MYStringLength(char *str)
{
    int len = 0;
    while(str[len] != '\0')
    {
        len++;
    }
    return len;
}

void MYStringCopy(char dest[], int n, char *source)
{
    int i = 0;
    while(i<n)
    {
        dest[i] = source[i];
        if(source[i] == '\0')
        {
            break;
        }
        i++;
    }
    if(i == n)
    {
        dest[n-1] = '\0';
    }
}

bool MYStringCompare(char *str1, char *str2)
{
    if(MYStringLength(str1) != MYStringLength(str2))
    {
        return false;
    }
    int i;
    while(str1[i] != '\0' && str2[i] != '\0')
    {
        if(str1[i] != str2[i])
        {
            return false;
        }
        i++;
    }
    if(str1[i] != str2[i])
    {
        return false;
    }
    return true;
}

void MYStringConcat(char dest[], int n, char *source)
{
    int i = 0, j = 0;
    while(dest[i] != '\0')
    {
        i++;
    }
    while(i<n)
    {
        dest[i] = source[j];
        if(source[j] == '\0')
        {
            break;
        }
        i++;
        j++;
    }
    if(i == n)
    {
        dest[n-1] = '\0';
    }
}