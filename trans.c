#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_BALANCE 500

// Account structure
struct clientData
{
    unsigned int acctNum;
    char firstName[15];
    char lastName[15];
    double balance;
};

// Function prototypes
int login(void);
unsigned int menu(void);

void addAccount(FILE *fp);
void updateAccount(FILE *fp);
void deleteAccount(FILE *fp);
void viewAccounts(FILE *fp);
void searchAccount(FILE *fp);

void deposit(FILE *fp);
void withdraw(FILE *fp);
void transfer(FILE *fp);

void exportFile(FILE *fp);

// LOGIN SYSTEM
int login(void)
{
    char pass[20];
    printf("Enter Admin Password: ");
    scanf("%s", pass);

    if (strcmp(pass, "admin123") == 0)
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
    FILE *fp;
    unsigned int choice;

    if (!login())
        return 0;

    fp = fopen("credit.dat", "rb+");

    if (fp == NULL)
    {
        fp = fopen("credit.dat", "wb+");
        if (fp == NULL)
        {
            printf("File error!\n");
            return 1;
        }
    }

    while ((choice = menu()) != 10)
    {
        switch (choice)
        {
        case 1: addAccount(fp); break;
        case 2: updateAccount(fp); break;
        case 3: deleteAccount(fp); break;
        case 4: viewAccounts(fp); break;
        case 5: searchAccount(fp); break;
        case 6: deposit(fp); break;
        case 7: withdraw(fp); break;
        case 8: transfer(fp); break;
        case 9: exportFile(fp); break;
        default: printf("Invalid choice!\n");
        }
    }

    fclose(fp);
    return 0;
}

// MENU
unsigned int menu(void)
{
    unsigned int c;

    printf("\n========== BANK MENU ==========\n");
    printf("1. Add Account\n");
    printf("2. Update Account\n");
    printf("3. Delete Account\n");
    printf("4. View Accounts\n");
    printf("5. Search Account\n");
    printf("6. Deposit\n");
    printf("7. Withdraw\n");
    printf("8. Transfer Money\n");
    printf("9. Export to Text File\n");
    printf("10. Exit\n");
    printf("Enter choice: ");

    scanf("%u", &c);
    return c;
}

// ADD ACCOUNT
void addAccount(FILE *fp)
{
    struct clientData c = {0, "", "", 0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fp, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c, sizeof(struct clientData), 1, fp);

    if (c.acctNum != 0)
    {
        printf("Account already exists!\n");
        return;
    }

    printf("Enter FirstName LastName Balance: ");
    scanf("%s %s %lf", c.firstName, c.lastName, &c.balance);

    c.acctNum = acc;

    fseek(fp, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&c, sizeof(struct clientData), 1, fp);

    printf("Account created successfully!\n");
}

// UPDATE (GENERAL EDIT)
void updateAccount(FILE *fp)
{
    struct clientData c;
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fp, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c, sizeof(struct clientData), 1, fp);

    if (c.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    printf("Enter new FirstName LastName Balance: ");
    scanf("%s %s %lf", c.firstName, c.lastName, &c.balance);

    fseek(fp, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&c, sizeof(struct clientData), 1, fp);

    printf("Account updated!\n");
}

// DELETE ACCOUNT
void deleteAccount(FILE *fp)
{
    struct clientData c;
    struct clientData blank = {0, "", "", 0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fp, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c, sizeof(struct clientData), 1, fp);

    if (c.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    fseek(fp, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fp);

    printf("Account deleted!\n");
}

// VIEW ALL
void viewAccounts(FILE *fp)
{
    struct clientData c;

    rewind(fp);

    printf("\nAcc  FirstName  LastName  Balance\n");

    while (fread(&c, sizeof(struct clientData), 1, fp))
    {
        if (c.acctNum != 0)
        {
            printf("%-4u %-10s %-10s %.2lf\n",
                   c.acctNum, c.firstName, c.lastName, c.balance);
        }
    }
}

// SEARCH
void searchAccount(FILE *fp)
{
    struct clientData c;
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fp, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c, sizeof(struct clientData), 1, fp);

    if (c.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    printf("\nAccount Details:\n");
    printf("Name: %s %s\n", c.firstName, c.lastName);
    printf("Balance: %.2lf\n", c.balance);
}

// DEPOSIT
void deposit(FILE *fp)
{
    struct clientData c;
    unsigned int acc;
    double amt;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fp, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c, sizeof(struct clientData), 1, fp);

    if (c.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    printf("Enter deposit amount: ");
    scanf("%lf", &amt);

    c.balance += amt;

    fseek(fp, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&c, sizeof(struct clientData), 1, fp);

    printf("Deposit successful! Balance = %.2lf\n", c.balance);
}

// WITHDRAW
void withdraw(FILE *fp)
{
    struct clientData c;
    unsigned int acc;
    double amt;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fp, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c, sizeof(struct clientData), 1, fp);

    if (c.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    printf("Enter withdraw amount: ");
    scanf("%lf", &amt);

    if (c.balance - amt < MIN_BALANCE)
    {
        printf("Cannot withdraw! Minimum balance is %d\n", MIN_BALANCE);
        return;
    }

    c.balance -= amt;

    fseek(fp, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&c, sizeof(struct clientData), 1, fp);

    printf("Withdraw successful! Balance = %.2lf\n", c.balance);
}

// TRANSFER
void transfer(FILE *fp)
{
    struct clientData from, to;
    unsigned int a1, a2;
    double amt;

    printf("From account: ");
    scanf("%u", &a1);

    printf("To account: ");
    scanf("%u", &a2);

    printf("Amount: ");
    scanf("%lf", &amt);

    fseek(fp, (a1 - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&from, sizeof(struct clientData), 1, fp);

    fseek(fp, (a2 - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&to, sizeof(struct clientData), 1, fp);

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

    fseek(fp, (a1 - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&from, sizeof(struct clientData), 1, fp);

    fseek(fp, (a2 - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&to, sizeof(struct clientData), 1, fp);

    printf("Transfer successful!\n");
}

// EXPORT FILE
void exportFile(FILE *fp)
{
    FILE *out = fopen("accounts.txt", "w");
    struct clientData c;

    rewind(fp);

    fprintf(out, "Acc First Last Balance\n");

    while (fread(&c, sizeof(struct clientData), 1, fp))
    {
        if (c.acctNum != 0)
        {
            fprintf(out, "%u %s %s %.2lf\n",
                    c.acctNum, c.firstName, c.lastName, c.balance);
        }
    }

    fclose(out);
    printf("Exported to accounts.txt\n");
}
