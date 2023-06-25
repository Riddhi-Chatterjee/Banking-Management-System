#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    open("Database/Accounts.dat", O_CREAT|O_RDWR|O_TRUNC, 0744);
    open("Database/Administrator.dat", O_CREAT|O_RDWR|O_TRUNC, 0744);
    open("Database/Customers.dat", O_CREAT|O_RDWR|O_TRUNC, 0744);
}