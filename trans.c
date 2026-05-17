#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_BALANCE 500

// Structure
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// Function prototypes
int login(void);
unsigned int enterChoice(void);

void textFile(FILE *fPtr);
void viewRecords(FILE *fPtr);
void searchRecord(FILE *fPtr);

void deposit(FILE *fPtr);
void withdraw(FILE *fPtr);
void transfer(FILE *fPtr);

void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

// LOGIN SYSTEM
int login(void)
{
    char password[20];
    printf("Enter Admin Password: ");
    scanf("%s", password);

    if (strcmp(password, "admin123") == 0)
    {
        printf("Login Successful!\n");
        return 1;
    }
    else
    {
        printf("Wrong Password!\n");
        return 0;
    }
}

// MAIN
int main()
{
    FILE *cfPtr;
    unsigned int choice;

    if (!login())
        return 0;

    cfPtr = fopen("credit.dat", "rb+");

    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");
        if (cfPtr == NULL)
        {
            printf("File error!\n");
            return 1;
        }
    }

    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        case 1: newRecord(cfPtr); break;
        case 2: deposit(cfPtr); break;
        case 3: withdraw(cfPtr); break;
        case 4: transfer(cfPtr); break;
        case 5: viewRecords(cfPtr); break;
        case 6: searchRecord(cfPtr); break;
        case 7: textFile(cfPtr); break;
        case 8: deleteRecord(cfPtr); break;
        default: printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    return 0;
}

// MENU
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n===== BANK SYSTEM =====\n");
    printf("1. Add Account\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Transfer Money\n");
    printf("5. View All Accounts\n");
    printf("6. Search Account\n");
    printf("7. Export to Text File\n");
    printf("8. Delete Account\n");
    printf("9. Exit\n");
    printf("Enter choice: ");

    scanf("%u", &choice);
    return choice;
}

// ADD ACCOUNT
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists!\n");
        return;
    }

    printf("Enter FirstName LastName Balance: ");
    scanf("%s %s %lf", client.firstName, client.lastName, &client.balance);

    client.acctNum = acc;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created successfully!\n");
}

// DEPOSIT
void deposit(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;
    double amt;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    printf("Enter deposit amount: ");
    scanf("%lf", &amt);

    client.balance += amt;

    fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Deposit successful! New Balance = %.2lf\n", client.balance);
}

// WITHDRAW
void withdraw(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;
    double amt;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    printf("Enter withdraw amount: ");
    scanf("%lf", &amt);

    if (client.balance - amt < MIN_BALANCE)
    {
        printf("Cannot withdraw! Minimum balance must be %d\n", MIN_BALANCE);
        return;
    }

    client.balance -= amt;

    fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Withdraw successful! New Balance = %.2lf\n", client.balance);
}

// TRANSFER
void transfer(FILE *fPtr)
{
    struct clientData from, to;
    unsigned int fromAcc, toAcc;
    double amt;

    printf("Enter From Account: ");
    scanf("%u", &fromAcc);

    printf("Enter To Account: ");
    scanf("%u", &toAcc);

    printf("Enter Amount: ");
    scanf("%lf", &amt);

    // from account
    fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&from, sizeof(struct clientData), 1, fPtr);

    // to account
    fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&to, sizeof(struct clientData), 1, fPtr);

    if (from.acctNum == 0 || to.acctNum == 0)
    {
        printf("Invalid account(s)!\n");
        return;
    }

    if (from.balance - amt < MIN_BALANCE)
    {
        printf("Transfer failed! Minimum balance required.\n");
        return;
    }

    from.balance -= amt;
    to.balance += amt;

    fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&from, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&to, sizeof(struct clientData), 1, fPtr);

    printf("Transfer successful!\n");
}

// VIEW
void viewRecords(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\nAcct  FirstName  LastName  Balance\n");

    while (fread(&client, sizeof(struct clientData), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("%-5u %-10s %-10s %.2lf\n",
                   client.acctNum,
                   client.firstName,
                   client.lastName,
                   client.balance);
        }
    }
}

// SEARCH
void searchRecord(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
        printf("Account not found!\n");
    else
        printf("Name: %s %s | Balance: %.2lf\n",
               client.firstName,
               client.lastName,
               client.balance);
}

// EXPORT TEXT FILE
void textFile(FILE *fPtr)
{
    FILE *out = fopen("accounts.txt", "w");
    struct clientData client;

    rewind(fPtr);

    fprintf(out, "Acct First Last Balance\n");

    while (fread(&client, sizeof(struct clientData), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(out, "%u %s %s %.2lf\n",
                    client.acctNum,
                    client.firstName,
                    client.lastName,
                    client.balance);
        }
    }

    fclose(out);
    printf("Exported to accounts.txt\n");
}

// DELETE
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blank = {0, "", "", 0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted!\n");
}
