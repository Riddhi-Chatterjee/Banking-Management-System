#include "Headers/Client.h"

int main()
{
    struct sockaddr_in serv;
	int sd;
	sd=socket(AF_INET, SOCK_STREAM, 0) ;
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = INADDR_ANY;
	serv.sin_port = htons(PORT);
	connect(sd, (void *) (&serv) , sizeof (serv));

    printf("Welcome to the online banking management system!\n\n");
    printf("How would you like to use the system:\n");
    printf("1. As a Customer\n");
    printf("2. As an Administrator\n");
    printf("Enter your choice: ");

    int ch1;
    scanf("%d", &ch1);
    printf("\n");
    switch(ch1)
    {
        case 1:
        {
            int myCustID = -1;
            bool signUpSuccess = false;
            bool loginSuccess = false;
            printf("Sign up or Login:\n");
            printf("1. Sign up\n");
            printf("2. Login\n");
            printf("Enter your choice: ");

            int ch2;
            scanf("%d", &ch2);
            printf("\n");

            if(ch2 == 1) //Sign up
            {
                printf("How would you like to sign up to the system:\n");
                printf("1. As a normal user\n");
                printf("2. As a joint account user\n");
                printf("Enter your choice: ");
                int ch3;
                scanf("%d", &ch3);
                printf("\n");
                if(ch3 == 1) //Creating a new account for this customer
                {
                    int request = NORMAL_SIGNUP_REQUEST;
	                write(sd, &request, sizeof(request));

                    struct Customer c;
                    printf("Enter your name:\n");
                    scanf("%s", c.Name);

                    printf("Enter your password:\n");
                    scanf("%s", c.Password);

                    c.Account_ID = -1;
                    c.Account_Type = -1;

                    write(sd, &c, sizeof(c));
                }
                else //Joining this customer to some existing account
                {
                    printf("Enter the ID of the account you want to share:\n");
                    int accountID;
                    scanf("%d", &accountID);

                    int request = JOINT_SIGNUP_REQUEST;
                    write(sd, &request, sizeof(request));

                    struct Customer c;
                    printf("Enter your name:\n");
                    scanf("%s", c.Name);

                    printf("Enter your password:\n");
                    scanf("%s", c.Password);

                    c.Account_ID = accountID;
                    c.Account_Type = JOINT_ACCOUNT;

                    write(sd, &c, sizeof(c));
                }

                int reply;
                read(sd, &reply, sizeof(reply));
                if(reply != SERVER_NACK)
                {
                    printf("Sign up was successfull!\n");
                    signUpSuccess = true;
                    myCustID = reply;
                }
            }
            else //Login
            {
                printf("How would you like to login to the system:\n");
                printf("1. As a normal user\n");
                printf("2. As a joint account user\n");
                printf("Enter your choice: ");

                int ch3;
                scanf("%d", &ch3);

                int account_type = -1;
                if(ch3 == 1)
                {
                    account_type = SINGLE_ACCOUNT;
                }
                else
                {
                    account_type = JOINT_ACCOUNT;
                }

                int attempts = 0;
                bool logFlag = true;
                while(logFlag)
                {
                    printf("\n");
                    char Name[100];
                    char Password[30];
                    printf("Enter your name:\n");
                    scanf("%s", Name);

                    printf("Enter your password:\n");
                    scanf("%s", Password);
                    
                    attempts++;
                    int reply = validate_Customer(sd, account_type, Name, Password);
                    if(reply != SERVER_NACK)
                    {
                        if(reply == ILLEGAL_ACCOUNT_TYPE)
                        {
                            if(account_type == SINGLE_ACCOUNT)
                            {
                                printf("Sorry... You are not a Single Account holder\n");
                            }
                            else
                            {
                                printf("Sorry... You are not a Joint Account holder\n");
                            }
                            logFlag = false;
                        }
                        else
                        {
                            printf("Login was successfull!\n");
                            loginSuccess = true;
                            myCustID = reply;
                            logFlag = false;
                        }
                    }
                    else if(attempts < 3)
                    {
                        printf("Invalid Name or Password! Try again!\n");
                    }
                    else
                    {
                        printf("Too many unsuccessfull attempts!\n");
                        break;
                    }
                }
            }
            if(loginSuccess || signUpSuccess)
            {
                bool flag = true;
                while(flag)
                {
                    printf("\nDo you want to:\n");
                    printf("1. Deposit\n");
                    printf("2. Withdraw\n");
                    printf("3. Balance enquiry\n");
                    printf("4. Change password\n");
                    printf("5. View account details\n");
                    printf("6. View profile\n");
                    printf("7. Create account\n");
                    printf("8. Exit\n");
                    printf("Enter your choice: ");

                    int ch4;
                    scanf("%d", &ch4);
                    printf("\n");
                    int request;
                    switch(ch4)
                    {
                        case 1: //Deposit
                        {
                            request = DEPOSIT_REQUEST;
                            write(sd, &request, sizeof(request));
                            write(sd, &myCustID, sizeof(myCustID));

                            printf("Enter the amount to be deposited:\n");
                            float amount;
                            scanf("%f", &amount);
                            write(sd, &amount, sizeof(amount));

                            int reply;
                            read(sd, &reply, sizeof(reply));
                            if(reply == SERVER_ACK)
                            {
                                printf("The required amount was deposited successfully!\n");
                            }
                            else
                            {
                                printf("Something went wrong... Please try again...\n");
                            }
                            break;
                        }

                        case 2: //Withdraw
                        {
                            request = WITHDRAW_REQUEST;
                            write(sd, &request, sizeof(request));
                            write(sd, &myCustID, sizeof(myCustID));

                            printf("Enter the amount to be withdrawn:\n");
                            float amount;
                            scanf("%f", &amount);
                            write(sd, &amount, sizeof(amount));

                            int reply;
                            read(sd, &reply, sizeof(reply));
                            if(reply == SERVER_ACK)
                            {
                                printf("The required amount was withdrawn successfully!\n");
                            }
                            else
                            {
                                printf("Something went wrong... Please try again...\n");
                            }
                            break;
                        }

                        case 3: //Balance enquiry
                        {
                            request = BALANCE_ENQUIRY_REQUEST;
                            write(sd, &request, sizeof(request));
                            write(sd, &myCustID, sizeof(myCustID));
                            
                            float balance;
                            read(sd, &balance, sizeof(balance));

                            if((int)balance != SERVER_NACK)
                            {
                                printf("The balance of your account is:  Rs %f\n", balance);
                            }
                            else
                            {
                                printf("Something went wrong... Please try again...\n");
                            }
                            break;
                        }

                        case 4: //Change password
                        {
                            request = PASSWORD_CHANGE_REQUEST;
                            write(sd, &request, sizeof(request));
                            write(sd, &myCustID, sizeof(myCustID));

                            printf("Enter your new password:\n");
                            char newPass[30];
                            scanf("%s", newPass);
                            write(sd, newPass, sizeof(newPass));

                            int reply;
                            read(sd, &reply, sizeof(reply));
                            if(reply == SERVER_ACK)
                            {
                                printf("Your password was changed successfully!\n");
                            }
                            else
                            {
                                printf("Something went wrong... Please try again...\n");
                            }
                            break;
                        }

                        case 5: //View account details
                        {
                            request = ACCOUNT_DETAILS_REQUEST;
                            write(sd, &request, sizeof(request));
                            write(sd, &myCustID, sizeof(myCustID));

                            struct Account a;
                            read(sd, &a, sizeof(a));
                            if(a.ID == INVALID_ACCOUNT_ID)
                            {
                                printf("Sorry... You don't have an account yet...\n");
                            }
                            else
                            {
                                printf("The requested account details are:\n");
                                printf("Account ID: %d\n", a.ID);
                                printf("Account balance: %f\n", a.Balance);
                            }
                            break;
                        }

                        case 6: //View profile
                        {
                            request = PROFILE_REQUEST;
                            write(sd, &request, sizeof(request));
                            write(sd, &myCustID, sizeof(myCustID));

                            struct Customer c;
                            read(sd, &c, sizeof(c));
                            printf("Your profile details are:\n");
                            printf("ID: %d\n", c.ID);
                            printf("Name: %s\n", c.Name);
                            printf("Password: %s\n", c.Password);
                            printf("AccountID: %d\n", c.Account_ID);
                            if(c.Account_Type == SINGLE_ACCOUNT)
                            {
                                printf("Account type: Single account\n");
                            }
                            else
                            {
                                printf("Account type: Joint account\n");
                            }
                            break;
                        }

                        case 7: //Create account
                        { //Only needed when the User's account gets deleted (maybe deleted by the admin) and the User wants to create his/her account again.
                          //In this case a new account is created with the same account ID and account balance as those of the deleted account.
                            request = CREATE_ACCOUNT_REQUEST;
                            write(sd, &request, sizeof(request));
                            write(sd, &myCustID, sizeof(myCustID));

                            int reply;
                            read(sd, &reply, sizeof(reply));
                            if(reply == SERVER_ACK)
                            {
                                printf("Your account was created successfully!\n");
                            }
                            else
                            {
                                printf("Sorry! This account was not created as you already have an account.\n");
                            }
                            break;
                        }

                        default: //Exit
                        {
                            flag = false;
                            break;
                        }
                    }
                }
            }
            else
            {
                printf("Sorry... Please try again...\n");
            }
            break;
        }

        case 2:
        {
            bool signUpSuccess = false;
            bool loginSuccess = false;
            if(isAdminFileEmpty(sd)) //Signing up the administrator
            {
                struct Admin a;
                printf("Enter your name:\n");
                scanf("%s", a.Name);
                printf("Enter your password:\n");
                scanf("%s", a.Password);

                int request;
                request = ADMIN_SIGNUP_REQUEST;
                write(sd, &request, sizeof(request));
                write(sd, &a, sizeof(a));
                signUpSuccess = true;
            }
            else //Log in (for the administrator)
            {
                int attempts = 0;
                while(true)
                {
                    char Name[100];
                    char Password[30];
                    printf("Enter your name:\n");
                    scanf("%s", Name);

                    printf("Enter your password:\n");
                    scanf("%s", Password);
                    
                    attempts++;
                    if(validate_Admin(sd, Name, Password))
                    {
                        printf("Login was successfull!\n");
                        loginSuccess = true;
                        break;
                    }
                    else if(attempts < 3)
                    {
                        printf("Invalid Name or Password! Try again!\n\n");
                    }
                    else
                    {
                        printf("Too many unsuccessfull attempts!\n");
                        break;
                    }
                }
            }
            if(signUpSuccess || loginSuccess)
            {
                bool flag = true;
                while(flag)
                {
                    printf("\nDo you want to:\n");
                    printf("1. Add account\n");
                    printf("2. Delete account\n");
                    printf("3. View account details\n");
                    printf("4. Modify account details\n");
                    printf("5. Change password\n");
                    printf("6. View profile\n");
                    printf("7. Exit\n");
                    printf("Enter your choice: ");

                    int ch4;
                    scanf("%d", &ch4);
                    printf("\n");
                    int request;
                    switch(ch4)
                    {
                        case 1: //Add account
                        {
                            request = ADD_ACCOUNT_REQUEST;
                            write(sd, &request, sizeof(request));
                            
                            printf("Enter the Account Balance:\n");
                            float account_balance;
                            scanf("%f", &account_balance);
                            write(sd, &account_balance, sizeof(account_balance));

                            int accountID;
                            read(sd, &accountID, sizeof(accountID));
                            printf("An account with Account ID = %d was added successfully!\n", accountID);
                            break;
                        }

                        case 2: //Delete account
                        {
                            request = DELETE_ACCOUNT_REQUEST;
                            write(sd, &request, sizeof(request));

                            printf("Enter the ID of the account to be deleted:\n");
                            int accountID;
                            scanf("%d", &accountID);
                            write(sd, &accountID, sizeof(accountID));

                            int reply;
                            read(sd, &reply, sizeof(reply));
                            if(reply == SERVER_ACK)
                            {
                                printf("The account was successfully deleted!\n");
                            }
                            else
                            {
                                printf("Something went wrong... Please try again...\n");
                            }

                            break;
                        }

                        case 3: //View account details
                        {
                            request = ADMIN_ACCOUNT_DETAILS_REQUEST;
                            write(sd, &request, sizeof(request));

                            printf("Enter the ID of the account whose details is to be viewed:\n");
                            int accountID;
                            scanf("%d", &accountID);
                            write(sd, &accountID, sizeof(accountID));

                            struct Account a;
                            read(sd, &a, sizeof(a));

                            if(a.ID == INVALID_ACCOUNT_ID)
                            {
                                printf("The requested account does not exist!\n");
                            }
                            else
                            {
                                printf("The requested account details are:\n");
                                printf("Account ID: %d\n", a.ID);
                                printf("Account balance: %f\n", a.Balance);
                            }

                            break;
                        }

                        case 4: //Modify account details
                        {
                            request = ADMIN_MODIFY_ACCOUNT_DETAILS_REQUEST;
                            write(sd, &request, sizeof(request));

                            struct Account a;
                            printf("Enter the ID of the account whose details are to be modified:\n");
                            scanf("%d", &a.ID);
                            printf("Enter the modified balance of this account:\n");
                            scanf("%f", &a.Balance);

                            write(sd, &a, sizeof(a));

                            int reply;
                            read(sd, &reply, sizeof(reply));
                            if(reply == SERVER_ACK)
                            {
                                printf("The account details were modified successfully!\n");
                            }
                            else
                            {
                                printf("Something went wrong... Please try again...\n");
                            }

                            break;
                        }

                        case 5: //Change password
                        {
                            request = ADMIN_PASSWORD_CHANGE_REQUEST;
                            write(sd, &request, sizeof(request));

                            char password[30];
                            printf("Enter your new password:\n");
                            scanf("%s", password);

                            write(sd, password, sizeof(password));
                            printf("Your password was changed successfully!\n");

                            break;
                        }

                        case 6: //View profile
                        {
                            request = ADMIN_PROFILE_REQUEST;
                            write(sd, &request, sizeof(request));

                            struct Admin a;
                            read(sd, &a, sizeof(a));

                            printf("Your profile details are:\n");
                            printf("Name: %s\n", a.Name);
                            printf("Password: %s\n", a.Password);

                            break;
                        }

                        default: //Exit
                        {
                            flag = false;
                            break;
                        }
                    }
                }
            }
            else
            {
                printf("Sorry... Please try again...\n");
            }
            break;
        }

        default:
        {
            printf("Invalid choice!\n");
        }
    }

    int masterRequest = EXIT_REQUEST;
    write(sd, &masterRequest, sizeof(masterRequest));
}

int validate_Customer(int sd, int account_type, char Name[], char Password[])
{
    int request;
    if(account_type == SINGLE_ACCOUNT)
    {
        request = NORMAL_LOGIN_REQUEST;
    }
    else
    {
        request = JOINT_LOGIN_REQUEST;
    }
    write(sd, &request, sizeof(request));

    struct Customer c;
    MYStringCopy(c.Name, sizeof(c.Name), Name);
    MYStringCopy(c.Password, sizeof(c.Password), Password);

    write(sd, &c, sizeof(c));

    int reply;
    read(sd, &reply, sizeof(reply));
    return reply;
}

bool validate_Admin(int sd, char Name[], char Password[])
{
    int request = ADMIN_LOGIN_REQUEST;
    write(sd, &request, sizeof(request));

    struct Admin a;
    MYStringCopy(a.Name, sizeof(a.Name), Name);
    MYStringCopy(a.Password, sizeof(a.Password), Password);

    write(sd, &a, sizeof(a));

    int reply;
    read(sd, &reply, sizeof(reply));
    if(reply == SERVER_ACK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isAdminFileEmpty(int sd)
{
    int request = ADMIN_FILE_EMPTY_CHECK_REQUEST;
    write(sd, &request, sizeof(request));

    int reply;
    read(sd, &reply, sizeof(reply));
    if(reply == SERVER_ACK)
    {
        return true;
    }
    else
    {
        return false;
    }
}
