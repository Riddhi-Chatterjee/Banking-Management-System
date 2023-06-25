//Account types:
#define SINGLE_ACCOUNT 1
#define JOINT_ACCOUNT 2


//Requests:
#define NORMAL_SIGNUP_REQUEST 101 //For normal user
#define JOINT_SIGNUP_REQUEST 102 //For joint account user
#define ADMIN_SIGNUP_REQUEST 103 //For the administrator
#define NORMAL_LOGIN_REQUEST 104 //For normal user
#define JOINT_LOGIN_REQUEST 105 //For joint account user
#define ADMIN_LOGIN_REQUEST 106 //For the administrator
#define DEPOSIT_REQUEST 107
#define WITHDRAW_REQUEST 108
#define BALANCE_ENQUIRY_REQUEST 109
#define PASSWORD_CHANGE_REQUEST 110
#define ACCOUNT_DETAILS_REQUEST 111
#define PROFILE_REQUEST 112
#define CREATE_ACCOUNT_REQUEST 113
#define EXIT_REQUEST 114
#define ADMIN_FILE_EMPTY_CHECK_REQUEST 115

#define ADD_ACCOUNT_REQUEST 201
#define DELETE_ACCOUNT_REQUEST 202
#define ADMIN_ACCOUNT_DETAILS_REQUEST 203
#define ADMIN_MODIFY_ACCOUNT_DETAILS_REQUEST 204
#define ADMIN_PASSWORD_CHANGE_REQUEST 205
#define ADMIN_PROFILE_REQUEST 206


//Extra macros:
#define PORT 6000
#define SERVER_ACK 1
#define SERVER_NACK -2
#define INVALID_ACCOUNT_ID -1
#define ILLEGAL_ACCOUNT_TYPE -3


struct Customer
{
    int ID;
    char Name[100];
    char Password[30];
    int Account_ID;
    int Account_Type;
};

struct Account
{
    int ID;
    float Balance;  
};

struct Admin
{
    char Name[100];
    char Password[30];
};