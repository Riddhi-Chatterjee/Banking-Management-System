#include "Headers/Server.h"

int main()
{
    struct sockaddr_in serv, cli;
    int sd,nsd;

    sd = socket(AF_INET, SOCK_STREAM, 0);   

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY; 
    serv.sin_port = htons(PORT); 

    bind(sd,(void *)&serv, sizeof(serv));
    
    listen(sd, 5); 
    
    while(1) //Creating a concurrent server using pthread_create
    {
        int cli_size = sizeof(cli);
        if((nsd = accept(sd, (void *)&cli, &cli_size)) > 0)
        {
            pthread_t tid;
            pthread_create(&tid, NULL, (void *)serve_client, (void*)(&nsd));
        }
    }
}

void serve_client(void* var)
{
    char buf[80];
    int nsd = *(int*)var;

    bool flag = true;
    while(flag)
    {
        int request;
        read(nsd, &request, sizeof(request));
        switch(request)
        {
            case NORMAL_SIGNUP_REQUEST:
            {
                struct Customer c;
                read(nsd, &c, sizeof(c));

                struct flock lock;
	            int fd;
	            fd = open("Database/Customers.dat", O_RDWR);
	            lock.l_type = F_WRLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = 0;
	            lock.l_len = 0;
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                struct Customer temp;
                int CustomerID = 0;
                while(read(fd, &temp, sizeof(temp)) != 0)
                {
                    if(temp.ID > CustomerID)
                    {
                        CustomerID = temp.ID;
                    }
                }
                CustomerID++;
                c.ID = CustomerID;
                c.Account_Type = SINGLE_ACCOUNT;

                struct flock lock1;
	            int fd1;
	            fd1 = open("Database/Accounts.dat", O_RDWR);
	            lock1.l_type = F_WRLCK;
	            lock1.l_whence = SEEK_SET;
	            lock1.l_start = 0;
	            lock1.l_len = 0;
	            lock1.l_pid = getpid();

                fcntl(fd1, F_SETLKW, &lock1);
                //Inside the critical section now....

                lseek(fd1, 0, SEEK_SET);
                struct Account a;
                struct Account temp1;
                int AccountID = 0;
                while(read(fd1, &temp1, sizeof(temp1)) != 0)
                {
                    if(temp1.ID > AccountID)
                    {
                        AccountID = temp1.ID;
                    }
                }
                AccountID++;
                a.ID = AccountID;
                a.Balance = 0;
                c.Account_ID = AccountID;
                
                lseek(fd1, 0, SEEK_END);
                write(fd1, &a, sizeof(a));

                lock1.l_type = F_UNLCK;
                fcntl(fd1, F_SETLK, &lock1);
                //Outside the critical section now...
                close(fd1);

                lseek(fd, 0, SEEK_END);
                write(fd, &c, sizeof(c));

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                int reply = c.ID;
                write(nsd, &reply, sizeof(reply));

                break;
            }
            case JOINT_SIGNUP_REQUEST:
            {
                struct Customer c;
                read(nsd, &c, sizeof(c));

                struct flock lock;
	            int fd;
	            fd = open("Database/Customers.dat", O_RDWR);
	            lock.l_type = F_WRLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = 0;
	            lock.l_len = 0;
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                struct Customer temp;
                int CustomerID = 0;
                bool isAccountValid = false;
                while(read(fd, &temp, sizeof(temp)) != 0)
                {
                    if(temp.ID > CustomerID)
                    {
                        CustomerID = temp.ID;
                    }
                    if(temp.Account_ID == c.Account_ID)
                    {
                        isAccountValid = true;
                        lseek(fd, -(sizeof(temp)), SEEK_CUR);
                        temp.Account_Type = JOINT_ACCOUNT;
                        write(fd, &temp, sizeof(temp));
                    }
                }
                CustomerID++;
                c.ID = CustomerID;

                int reply = SERVER_NACK;
                if(isAccountValid)
                {
                    lseek(fd, 0, SEEK_END);
                    write(fd, &c, sizeof(c));
                    reply = c.ID;
                }
    
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                write(nsd, &reply, sizeof(reply));

                break;
            }
            case ADMIN_SIGNUP_REQUEST:
            {
                struct Admin a;
                read(nsd, &a, sizeof(a));

                struct flock lock;
	            int fd;
	            fd = open("Database/Administrator.dat", O_WRONLY);
	            lock.l_type = F_WRLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = SEEK_SET;
	            lock.l_len = sizeof(struct Admin);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                write(fd, &a, sizeof(a));

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);
                
                break;
            }
            case NORMAL_LOGIN_REQUEST:
            {
                struct Customer c;
                read(nsd, &c, sizeof(c));

                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = 0;
	            lock.l_len = 0;
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                struct Customer tempC;
                bool isValidCustomer = false;
                while (read(fd, &tempC, sizeof(tempC)) != 0)
                {
                    if(MYStringCompare(c.Name, tempC.Name) && MYStringCompare(c.Password, tempC.Password))
                    {
                        c = tempC;
                        isValidCustomer = true;
                        break;
                    }
                }

                int reply = SERVER_NACK;
                if(isValidCustomer)
                {
                    reply = c.ID;
                    if(c.Account_Type != SINGLE_ACCOUNT)
                    {
                        reply = ILLEGAL_ACCOUNT_TYPE;
                    }
                }

                write(nsd, &reply, sizeof(reply));

                break;
            }
            case JOINT_LOGIN_REQUEST:
            {
                struct Customer c;
                read(nsd, &c, sizeof(c));

                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = 0;
	            lock.l_len = 0;
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                struct Customer tempC;
                bool isValidCustomer = false;
                while (read(fd, &tempC, sizeof(tempC)) != 0)
                {
                    if(MYStringCompare(c.Name, tempC.Name) && MYStringCompare(c.Password, tempC.Password))
                    {
                        c = tempC;
                        isValidCustomer = true;
                        break;
                    }
                }

                int reply = SERVER_NACK;
                if(isValidCustomer)
                {
                    reply = c.ID;
                    if(c.Account_Type != JOINT_ACCOUNT)
                    {
                        reply = ILLEGAL_ACCOUNT_TYPE;
                    }
                }
                
                write(nsd, &reply, sizeof(reply));

                break;
            }
            case ADMIN_LOGIN_REQUEST:
            {
                struct Admin a;
                read(nsd, &a, sizeof(a));

                struct flock lock;
	            int fd;
                fd = open("Database/Administrator.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = SEEK_SET;
	            lock.l_len = sizeof(struct Admin);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                struct Admin tempA;
                bool isValidAdmin = false;
                read(fd, &tempA, sizeof(tempA));
                
                if(MYStringCompare(a.Name, tempA.Name) && MYStringCompare(a.Password, tempA.Password))
                {
                    isValidAdmin = true;
                }

                int reply = SERVER_NACK;
                if(isValidAdmin)
                {
                    reply = SERVER_ACK;
                }
                
                write(nsd, &reply, sizeof(reply));

                break;
            }
            case DEPOSIT_REQUEST:
            {
                int CustomerID;
                read(nsd, &CustomerID, sizeof(CustomerID));

                float amount;
                read(nsd, &amount, sizeof(amount));

                struct Customer c;
                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (CustomerID - 1)*sizeof(struct Customer);
	            lock.l_len = sizeof(struct Customer);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (CustomerID - 1)*sizeof(struct Customer), SEEK_SET);
                read(fd, &c, sizeof(c));
                
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                struct Account a;
                fd = open("Database/Accounts.dat", O_RDWR);
	            lock.l_type = F_WRLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (c.Account_ID - 1)*sizeof(struct Account);
	            lock.l_len = sizeof(struct Account);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (c.Account_ID - 1)*sizeof(struct Account), SEEK_SET);
                bool isAccountValid = false;
                read(fd, &a, sizeof(a));
                if(a.ID == c.Account_ID)
                {
                    lseek(fd, -(sizeof(a)), SEEK_CUR);
                    a.Balance = a.Balance + amount;
                    write(fd, &a, sizeof(a));
                    isAccountValid = true;
                }

                int reply = SERVER_NACK;
                if(isAccountValid)
                {
                    reply = SERVER_ACK;
                }

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                write(nsd, &reply, sizeof(reply));
                
                break;
            }
            case WITHDRAW_REQUEST:
            {
                int CustomerID;
                read(nsd, &CustomerID, sizeof(CustomerID));

                float amount;
                read(nsd, &amount, sizeof(amount));

                struct Customer c;
                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (CustomerID - 1)*sizeof(struct Customer);
	            lock.l_len = sizeof(struct Customer);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (CustomerID - 1)*sizeof(struct Customer), SEEK_SET);
                read(fd, &c, sizeof(c));
                
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                struct Account a;
                fd = open("Database/Accounts.dat", O_RDWR);
	            lock.l_type = F_WRLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (c.Account_ID - 1)*sizeof(struct Account);
	            lock.l_len = sizeof(struct Account);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (c.Account_ID - 1)*sizeof(struct Account), SEEK_SET);
                bool isAccountValid = false;
                read(fd, &a, sizeof(a));
                if(a.ID == c.Account_ID)
                {
                    lseek(fd, -(sizeof(a)), SEEK_CUR);
                    a.Balance = a.Balance - amount;
                    write(fd, &a, sizeof(a));
                    isAccountValid = true;
                }

                int reply = SERVER_NACK;
                if(isAccountValid)
                {
                    reply = SERVER_ACK;
                }

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                write(nsd, &reply, sizeof(reply));
                
                break;
            }
            case BALANCE_ENQUIRY_REQUEST:
            {
                int CustomerID;
                read(nsd, &CustomerID, sizeof(CustomerID));

                struct Customer c;
                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (CustomerID - 1)*sizeof(struct Customer);
	            lock.l_len = sizeof(struct Customer);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (CustomerID - 1)*sizeof(struct Customer), SEEK_SET);
                read(fd, &c, sizeof(c));
                
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                struct Account a;
                fd = open("Database/Accounts.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (c.Account_ID - 1)*sizeof(struct Account);
	            lock.l_len = sizeof(struct Account);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                bool isAccountValid = false;
                lseek(fd, (c.Account_ID - 1)*sizeof(struct Account), SEEK_SET);
                read(fd, &a, sizeof(a));
                if(a.ID == c.Account_ID)
                {
                    isAccountValid = true;
                }

                float reply = SERVER_NACK;
                if(isAccountValid)
                {
                    reply = a.Balance;
                }

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                write(nsd, &reply, sizeof(reply));

                break;
            }
            case PASSWORD_CHANGE_REQUEST:
            {
                int CustomerID;
                read(nsd, &CustomerID, sizeof(CustomerID));

                char newPass[30];
                read(nsd, newPass, sizeof(newPass));

                struct Customer c;
                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDWR);
	            lock.l_type = F_WRLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (CustomerID - 1)*sizeof(struct Customer);
	            lock.l_len = sizeof(struct Customer);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (CustomerID - 1)*sizeof(struct Customer), SEEK_SET);
                read(fd, &c, sizeof(c));
                lseek(fd, -(sizeof(c)), SEEK_CUR);
                MYStringCopy(c.Password, sizeof(c.Password), newPass);
                write(fd, &c, sizeof(c));

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                int reply = SERVER_ACK;
                write(nsd, &reply, sizeof(reply));

                break;
            }
            case ACCOUNT_DETAILS_REQUEST:
            {
                int CustomerID;
                read(nsd, &CustomerID, sizeof(CustomerID));

                struct Customer c;
                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (CustomerID - 1)*sizeof(struct Customer);
	            lock.l_len = sizeof(struct Customer);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (CustomerID - 1)*sizeof(struct Customer), SEEK_SET);
                read(fd, &c, sizeof(c));
                
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);


                struct Account a;
                fd = open("Database/Accounts.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (c.Account_ID - 1)*sizeof(struct Account);
	            lock.l_len = sizeof(struct Account);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (c.Account_ID - 1)*sizeof(struct Account), SEEK_SET);
                read(fd, &a, sizeof(a));

                if(a.ID != c.Account_ID)
                {
                    a.ID = INVALID_ACCOUNT_ID;
                }

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                write(nsd, &a, sizeof(a));
                
                break;
            }
            case PROFILE_REQUEST:
            {
                int CustomerID;
                read(nsd, &CustomerID, sizeof(CustomerID));

                struct Customer c;
                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (CustomerID - 1)*sizeof(struct Customer);
	            lock.l_len = sizeof(struct Customer);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (CustomerID - 1)*sizeof(struct Customer), SEEK_SET);
                read(fd, &c, sizeof(c));
                
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                write(nsd, &c, sizeof(c));

                break;
            }
            case CREATE_ACCOUNT_REQUEST:
            {
                int CustomerID;
                read(nsd, &CustomerID, sizeof(CustomerID));

                struct Customer c;
                struct flock lock;
	            int fd;
                fd = open("Database/Customers.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (CustomerID - 1)*sizeof(struct Customer);
	            lock.l_len = sizeof(struct Customer);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (CustomerID - 1)*sizeof(struct Customer), SEEK_SET);
                read(fd, &c, sizeof(c));
                
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                struct Account a;
                fd = open("Database/Accounts.dat", O_RDWR);
	            lock.l_type = F_WRLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = (c.Account_ID - 1)*sizeof(struct Account);
	            lock.l_len = sizeof(struct Account);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, (c.Account_ID - 1)*sizeof(struct Account), SEEK_SET);
                bool isAccountDeleted = false;
                read(fd, &a, sizeof(a));
                if(a.ID == INVALID_ACCOUNT_ID)
                {
                    lseek(fd, -(sizeof(struct Account)), SEEK_CUR);
                    a.ID = c.Account_ID;
                    write(fd, &a, sizeof(a));
                    isAccountDeleted = true;
                }

                int reply = SERVER_NACK;
                if(isAccountDeleted)
                {
                    reply = SERVER_ACK;
                }

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                write(nsd, &reply, sizeof(reply));
                
                break;
            }
            case EXIT_REQUEST:
            {
                flag = false;
                break;
            }
            case ADMIN_FILE_EMPTY_CHECK_REQUEST:
            {
                struct flock lock;
	            int fd;
	            fd = open("Database/Administrator.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = SEEK_SET;
	            lock.l_len = sizeof(struct Admin);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                struct Admin a;
                int reply;
                if(read(fd, &a, sizeof(a)) == 0)
                {
                    reply = SERVER_ACK;
                }
                else
                {
                    reply = SERVER_NACK;
                }

                write(nsd, &reply, sizeof(reply));

                break;
            }
            case ADD_ACCOUNT_REQUEST:
            {
                float account_balance;
                read(nsd, &account_balance, sizeof(account_balance));

                struct flock lock1;
	            int fd1;
	            fd1 = open("Database/Accounts.dat", O_RDWR);
	            lock1.l_type = F_WRLCK;
	            lock1.l_whence = SEEK_SET;
	            lock1.l_start = 0;
	            lock1.l_len = 0;
	            lock1.l_pid = getpid();

                fcntl(fd1, F_SETLKW, &lock1);
                //Inside the critical section now....

                lseek(fd1, 0, SEEK_SET);
                struct Account a;
                struct Account temp1;
                int AccountID = 0;
                while(read(fd1, &temp1, sizeof(temp1)) != 0)
                {
                    if(temp1.ID > AccountID)
                    {
                        AccountID = temp1.ID;
                    }
                }
                AccountID++;
                a.ID = AccountID;
                a.Balance = account_balance;
                
                lseek(fd1, 0, SEEK_END);
                write(fd1, &a, sizeof(a));

                lock1.l_type = F_UNLCK;
                fcntl(fd1, F_SETLK, &lock1);
                //Outside the critical section now...
                close(fd1);

                write(nsd, &AccountID, sizeof(AccountID));

                break;
            }
            case DELETE_ACCOUNT_REQUEST:
            {
                int accountID;
                read(nsd, &accountID, sizeof(accountID));

                int reply = SERVER_NACK;
                if(accountID <= 0)
                {
                    reply = SERVER_NACK;
                }
                else
                {
                    int fd;
                    struct flock lock;
                    struct Account a;
                    fd = open("Database/Accounts.dat", O_RDWR);
	                lock.l_type = F_WRLCK;
	                lock.l_whence = SEEK_SET;
	                lock.l_start = (accountID - 1)*sizeof(struct Account);
	                lock.l_len = sizeof(struct Account);
	                lock.l_pid = getpid();

                    fcntl(fd, F_SETLKW, &lock);
                    //Inside the critical section now....

                    bool isAccountValid = false;
                    lseek(fd, (accountID - 1)*sizeof(struct Account), SEEK_SET);
                    read(fd, &a, sizeof(a));
                    if(a.ID == accountID)
                    {
                        lseek(fd, -(sizeof(struct Account)), SEEK_CUR);
                        a.ID = INVALID_ACCOUNT_ID;
                        write(fd, &a, sizeof(a));
                        isAccountValid = true;
                    }

                    if(isAccountValid)
                    {
                        reply = SERVER_ACK;
                    }

                    lock.l_type = F_UNLCK;
                    fcntl(fd, F_SETLK, &lock);
                    //Outside the critical section now...
                    close(fd);
                }

                write(nsd, &reply, sizeof(reply));

                break;
            }
            case ADMIN_ACCOUNT_DETAILS_REQUEST:
            {
                int accountID;
                read(nsd, &accountID, sizeof(accountID));

                struct Account a;
                a.ID = INVALID_ACCOUNT_ID;
                if(accountID <= 0)
                {
                    a.ID = INVALID_ACCOUNT_ID;
                }
                else
                {
                    struct flock lock;
                    int fd;
                    fd = open("Database/Accounts.dat", O_RDONLY);
	                lock.l_type = F_RDLCK;
	                lock.l_whence = SEEK_SET;
	                lock.l_start = (accountID - 1)*sizeof(struct Account);
	                lock.l_len = sizeof(struct Account);
	                lock.l_pid = getpid();

                    fcntl(fd, F_SETLKW, &lock);
                    //Inside the critical section now....

                    lseek(fd, (accountID - 1)*sizeof(struct Account), SEEK_SET);
                    read(fd, &a, sizeof(a));

                    if(a.ID != accountID)
                    {
                        a.ID = INVALID_ACCOUNT_ID;
                    }

                    lock.l_type = F_UNLCK;
                    fcntl(fd, F_SETLK, &lock);
                    //Outside the critical section now...
                    close(fd);
                }

                write(nsd, &a, sizeof(a));

                break;
            }
            case ADMIN_MODIFY_ACCOUNT_DETAILS_REQUEST:
            {
                struct Account a;
                read(nsd, &a, sizeof(a));

                int reply = SERVER_NACK;
                if(a.ID <= 0)
                {
                    reply = SERVER_NACK;
                }
                else
                {
                    struct flock lock;
                    int fd;
                    fd = open("Database/Accounts.dat", O_RDWR);
	                lock.l_type = F_WRLCK;
	                lock.l_whence = SEEK_SET;
	                lock.l_start = (a.ID - 1)*sizeof(struct Account);
	                lock.l_len = sizeof(struct Account);
	                lock.l_pid = getpid();

                    fcntl(fd, F_SETLKW, &lock);
                    //Inside the critical section now....

                    lseek(fd, (a.ID - 1)*sizeof(struct Account), SEEK_SET);
                    struct Account tempA;
                    read(fd, &tempA, sizeof(tempA));

                    if(tempA.ID == a.ID)
                    {
                        lseek(fd, -(sizeof(struct Account)), SEEK_CUR);
                        write(fd, &a, sizeof(a));
                        reply = SERVER_ACK;
                    }
                }

                write(nsd, &reply, sizeof(reply));

                break;
            }
            case ADMIN_PASSWORD_CHANGE_REQUEST:
            {
                char password[30];
                read(nsd, password, sizeof(password));

                struct flock lock;
	            int fd;
	            fd = open("Database/Administrator.dat", O_RDWR);
	            lock.l_type = F_WRLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = SEEK_SET;
	            lock.l_len = sizeof(struct Admin);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                struct Admin a;
                read(fd, &a, sizeof(a));
                MYStringCopy(a.Password, sizeof(a.Password), password);
                lseek(fd, -(sizeof(struct Admin)), SEEK_CUR);
                write(fd, &a, sizeof(a));

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                break;
            }
            case ADMIN_PROFILE_REQUEST:
            {
                struct flock lock;
	            int fd;
	            fd = open("Database/Administrator.dat", O_RDONLY);
	            lock.l_type = F_RDLCK;
	            lock.l_whence = SEEK_SET;
	            lock.l_start = SEEK_SET;
	            lock.l_len = sizeof(struct Admin);
	            lock.l_pid = getpid();

                fcntl(fd, F_SETLKW, &lock);
                //Inside the critical section now....

                lseek(fd, 0, SEEK_SET);
                struct Admin a;
                read(fd, &a, sizeof(a));

                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                //Outside the critical section now...
                close(fd);

                write(nsd, &a, sizeof(a));

                break;
            }
            default:
            {
                break;
            }
        }
    }
}
