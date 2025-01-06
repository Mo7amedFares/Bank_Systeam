#pragma once
#include <iostream>
#include <string>
#include "clsPerson.h"
#include "clsString.h"
#include <vector>
#include <fstream>
#include "clsDate.h"
#include "clsUtil.h"

using namespace std;

class clsUser : public clsPerson {
private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;
    string _UserName;
    string _Password;
    string _EncryptedPassword;
    int _Permissions;
    bool _MarkedForDelete = false;

    string _PrepareLogInRecord(string Seperator = "#//#") {
        string LoginRecord = "";
        LoginRecord += clsDate::GetSystemDateTimeString() + Seperator;
        LoginRecord += GetUserName() + Seperator;
        LoginRecord += clsUtil::EncryptText(GetPassword()) + Seperator;
        LoginRecord += to_string(GetPermissions());
        return LoginRecord;
    }

    static clsUser _ConvertLinetoUserObject(string Line, string Seperator = "#//#") {
        vector<string> vUserData = clsString::Split(Line, Seperator);
        if (vUserData.size() != 7) {
            cout << "Invalid user data format!\n";
            return GetEmptyUserObject();
        }
        return clsUser(enMode::UpdateMode, vUserData[0], vUserData[1], vUserData[2],
            vUserData[3], vUserData[4], clsUtil::DecryptText(vUserData[5]), stoi(vUserData[6]));
    }

    static string _ConverUserObjectToLine(clsUser User, string Seperator = "#//#") {
        string UserRecord = "";
        UserRecord += User.GetFirstName() + Seperator;
        UserRecord += User.GetLastName() + Seperator;
        UserRecord += User.GetEmail() + Seperator;
        UserRecord += User.GetPhone() + Seperator;
        UserRecord += User.GetUserName() + Seperator;
        UserRecord += clsUtil::EncryptText(User.GetPassword()) + Seperator;
        UserRecord += to_string(User.GetPermissions());
        return UserRecord;
    }

    static vector<clsUser> _LoadUsersDataFromFile() {
        vector<clsUser> vUsers;
        fstream MyFile;
        MyFile.open("Users.txt", ios::in);

        if (MyFile.is_open()) {
            string Line;
            while (getline(MyFile, Line)) {
                clsUser User = _ConvertLinetoUserObject(Line);
                vUsers.push_back(User);
            }
            MyFile.close();
        }
        return vUsers;
    }

    static void _SaveUsersDataToFile(vector<clsUser> vUsers) {
        fstream MyFile;
        MyFile.open("Users.txt", ios::out);

        if (MyFile.is_open()) {
            for (clsUser U : vUsers) {
                if (!U.MarkedForDeleted()) {
                    MyFile << _ConverUserObjectToLine(U) << endl;
                }
            }
            MyFile.close();
        }
    }

    void _Update() {
        vector<clsUser> _vUsers = _LoadUsersDataFromFile();
        for (clsUser& U : _vUsers) {
            if (U.GetUserName() == GetUserName()) {
                U = *this;
                break;
            }
        }
        _SaveUsersDataToFile(_vUsers);
    }

    void _AddNew() {
        _AddDataLineToFile(_ConverUserObjectToLine(*this));
    }

    void _AddDataLineToFile(string stDataLine) {
        fstream MyFile;
        MyFile.open("Users.txt", ios::out | ios::app);

        if (MyFile.is_open()) {
            MyFile << stDataLine << endl;
            MyFile.close();
        }
    }


    static string EnctyptedPassword(string Password) {
        return clsUtil::EncryptText(Password);
    }

public:
    enum enPermissions {
        eAll = -1, pListClients = 1, pAddNewClient = 2, pDeleteClient = 4,
        pUpdateClients = 8, pFindClient = 16, pTranactions = 32, pManageUsers = 64,
        pShowLogInRegister = 128
    };

    struct stLoginRegisterRecord {
        string DateTime;
        string UserName;
        string Password;
        int Permissions;
    };

    static stLoginRegisterRecord _ConvertLoginRegisterLineToRecord(string Line, string Seperator = "#//#") {
        stLoginRegisterRecord LoginRegisterRecord;
        vector<string> LoginRegisterDataLine = clsString::Split(Line, Seperator);
        if (LoginRegisterDataLine.size() < 4) {
            cout << "Invalid login register data format!\n";
            return LoginRegisterRecord;
        }
        LoginRegisterRecord.DateTime = LoginRegisterDataLine[0];
        LoginRegisterRecord.UserName = LoginRegisterDataLine[1];
        LoginRegisterRecord.Password = clsUtil::DecryptText(LoginRegisterDataLine[2]);
        LoginRegisterRecord.Permissions = stoi(LoginRegisterDataLine[3]);
        return LoginRegisterRecord;
    }

    clsUser(enMode Mode, string FirstName, string LastName,
        string Email, string Phone, string UserName, string Password,
        int Permissions) :
        clsPerson(FirstName, LastName, Email, Phone) {
        _Mode = Mode;
        _UserName = UserName;
        _Password = Password;
        _Permissions = Permissions;
    }

    static clsUser GetEmptyUserObject() {
        return clsUser(enMode::EmptyMode, "", "", "", "", "", "", 0);
    }

    bool IsEmpty() {
        return (_Mode == enMode::EmptyMode);
    }

    bool MarkedForDeleted() {
        return _MarkedForDelete;
    }

    string GetUserName() {
        return _UserName;
    }

    void SetUserName(string UserName) {
        _UserName = UserName;
    }


    void SetPassword(string Password) {
        _Password = Password;
    }

    string GetPassword() {
        return _Password;
    }


    void SetPermissions(int Permissions) {
        _Permissions = Permissions;
    }

    int GetPermissions() {
        return _Permissions;
    }


    static clsUser Find(string UserName) {
        fstream MyFile;
        MyFile.open("Users.txt", ios::in);

        if (MyFile.is_open()) {
            string Line;
            while (getline(MyFile, Line)) {
                clsUser User = _ConvertLinetoUserObject(Line);
                if (User.GetUserName() == UserName) {
                    MyFile.close();
                    return User;
                }
            }
            MyFile.close();
        }
        return GetEmptyUserObject();
    }

    static clsUser Find(string UserName, string Password) {
        fstream MyFile;
        MyFile.open("Users.txt", ios::in);

        if (MyFile.is_open()) {
            string Line;
            while (getline(MyFile, Line)) {
                clsUser User = _ConvertLinetoUserObject(Line);
                if (User.GetUserName() == UserName && User.GetPassword() == Password) {
                    MyFile.close();
                    return User;
                }
            }
            MyFile.close();
        }
        return GetEmptyUserObject();
    }

    enum enSaveResults { svFaildEmptyObject = 0, svSucceeded = 1, svFaildUserExists = 2 };

    enSaveResults Save() {
        switch (_Mode) {
        case enMode::EmptyMode:
            if (IsEmpty()) {
                return enSaveResults::svFaildEmptyObject;
            }
        case enMode::UpdateMode:
            _Update();
            return enSaveResults::svSucceeded;
        case enMode::AddNewMode:
            if (clsUser::IsUserExist(_UserName)) {
                return enSaveResults::svFaildUserExists;
            }
            else {
                _AddNew();
                _Mode = enMode::UpdateMode;
                return enSaveResults::svSucceeded;
            }
        }
    }

    static bool IsUserExist(string UserName) {
        clsUser User = clsUser::Find(UserName);
        return (!User.IsEmpty());
    }

    bool Delete() {
        vector<clsUser> _vUsers = _LoadUsersDataFromFile();
        for (clsUser& U : _vUsers) {
            if (U.GetUserName() == _UserName) {
                U._MarkedForDelete = true;
                break;
            }
        }
        _SaveUsersDataToFile(_vUsers);
        *this = GetEmptyUserObject();
        return true;
    }

    static clsUser GetAddNewUserObject(string UserName) {
        return clsUser(enMode::AddNewMode, "", "", "", "", UserName, "", 0);
    }

    static vector<clsUser> GetUsersList() {
        return _LoadUsersDataFromFile();
    }

    bool CheckAccessPermission(enPermissions Permission) {
        if (this->GetPermissions() == enPermissions::eAll)
            return true;
        if ((Permission & this->GetPermissions()) == Permission)
            return true;
        else
            return false;
    }

    void RegisterLogIn() {
        string stDataLine = _PrepareLogInRecord();
        fstream MyFile;
        MyFile.open("LoginRegister.txt", ios::out | ios::app);

        if (MyFile.is_open()) {
            MyFile << stDataLine << endl;
            MyFile.close();
        }
    }

    static vector<stLoginRegisterRecord> GetLoginRegisterList() {
        vector<stLoginRegisterRecord> vLoginRegisterRecord;
        fstream MyFile;
        MyFile.open("LoginRegister.txt", ios::in);

        if (MyFile.is_open()) {
            string Line;
            stLoginRegisterRecord LoginRegisterRecord;
            while (getline(MyFile, Line)) {
                LoginRegisterRecord = _ConvertLoginRegisterLineToRecord(Line);
                vLoginRegisterRecord.push_back(LoginRegisterRecord);
            }
            MyFile.close();
        }
        return vLoginRegisterRecord;
    }
};
