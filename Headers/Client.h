#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "Structures.h"
#include "Utilities.h"

int validate_Customer(int sd, int account_type, char Name[], char Password[]);
bool validate_Admin(int sd, char Name[], char Password[]);
bool isAdminFileEmpty(int sd);