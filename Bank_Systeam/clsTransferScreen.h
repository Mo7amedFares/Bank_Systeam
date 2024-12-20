#pragma once
#include <iostream>
#include "clsScreen.h"
#include "clsPerson.h"
#include "clsBankClient.h"
#include "clsInputValidate.h"

class clsTransferScreen :protected clsScreen
{

private:
    static void _PrintClient(clsBankClient Client)
    {
        cout << "\nClient Card:";
        cout << "\n___________________\n";
        cout << "\nFull Name   : " << Client.FullName();
        cout << "\nAcc. Number : " << Client.AccountNumber();
        cout << "\nBalance     : " << Client.AccountBalance;
        cout << "\n___________________\n";

    }

    static string _ReadAccountNumberFrom()
    {
        string AccountNumber;
        cout << "\nPlease Enter Account Number to Transfer From: ";
        AccountNumber = clsInputValidate<string>::ReadString();
        while (!clsBankClient::IsClientExist(AccountNumber))
        {
            cout << "\nAccount number is not found, choose another one: ";
            AccountNumber = clsInputValidate<string>::ReadString();
        }
        return AccountNumber;
    }

    static string _ReadAccountNumberTo(string AccountNumberFrom)
    {
        string AccountNumber;
        cout << "\nPlease Enter Account Number to Transfer To: ";
        AccountNumber = clsInputValidate<string>::ReadString();
        while ((!clsBankClient::IsClientExist(AccountNumber)) || AccountNumberFrom == AccountNumber)
        {
            if (AccountNumber == AccountNumberFrom)
                cout << "\nError, It is not possible to transfer money to the same Account Number, Choose another one: ";
            else
                cout << "\nAccount number is not found, choose another one: ";

            AccountNumber = clsInputValidate<string>::ReadString();

        }
        return AccountNumber;
    }

    static float ReadAmount(clsBankClient SourceClient)
    {
        float Amount;

        cout << "\nEnter Transfer Amount? ";

        Amount = clsInputValidate<float>::ReadTNumber();

        while (Amount > SourceClient.AccountBalance)
        {
            cout << "\nAmount Exceeds the available Balance, Enter another Amount ? ";
            Amount = clsInputValidate<double>::ReadTNumber();
        }
        return Amount;
    }

public:

    static void ShowTransferScreen()
    {

        _DrawScreenHeader("\tTransfer Screen");
        string AccountNumberFrom = _ReadAccountNumberFrom();
        clsBankClient SourceClient = clsBankClient::Find(AccountNumberFrom);

        _PrintClient(SourceClient);

        clsBankClient DestinationClient = clsBankClient::Find(_ReadAccountNumberTo(AccountNumberFrom));

        _PrintClient(DestinationClient);

        float Amount = ReadAmount(SourceClient);


        cout << "\nAre you sure you want to perform this operation? y/n? ";
        char Answer = 'n';
        cin >> Answer;
        if (Answer == 'Y' || Answer == 'y')
        {
            if (SourceClient.Transfer(Amount, DestinationClient, CurrentUser.UserName))
            {
                cout << "\nTransfer done successfully\n";
            }
            else
            {
                cout << "\nTransfer Faild \n";
            }
        }

        _PrintClient(SourceClient);
        _PrintClient(DestinationClient);


    }

};

