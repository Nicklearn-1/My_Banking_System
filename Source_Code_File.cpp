// bANK.cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// ====================================================================
//  MULTI-BRANCH BANKING MANAGEMENT SYSTEM
//  Standard Bank Console Prototype v2.0
//  PROGRAMMING 621 Assignment
//  Single-file version — compile with:
//    g++ -std=c++14 -o banking banking_system.cpp
// ====================================================================
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <cctype>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>


using namespace std;

// ====================================================================
//  CONSTANTS
// ====================================================================
const int MAX_LOGIN_ATTEMPTS = 3;
const double MIN_SAVINGS_DEPOSIT = 500.0;
const double MIN_CHEQUE_DEPOSIT = 1000.0;
const double MIN_FIXED_DEPOSIT = 5000.0;
const double MIN_STUDENT_DEPOSIT = 100.0;

const double SAVINGS_INTEREST_RATE = 0.035;  // 3.5% per annum
const double CHEQUE_INTEREST_RATE = 0.01;   // 1.0% per annum
const double FIXED_INTEREST_RATE = 0.07;   // 7.0% per annum
const double STUDENT_INTEREST_RATE = 0.025;  // 2.5% per annum

// ====================================================================
//  SIMPLE XOR ENCRYPTION
// ====================================================================
inline string encryptString(const string& input, char key = 0x5A) {
    string result = input;
    for (char& c : result) c ^= key;
    return result;
}
inline string decryptString(const string& input, char key = 0x5A) {
    return encryptString(input, key); // XOR is its own inverse
}

// ====================================================================
//  STRUCT: Branch
// ====================================================================
struct Branch {
    char branchCode[10];
    char branchName[60];
    char city[40];
    char address[100];
    char phone[15];
    int  tellerCount;
    int  accountCount;
    double totalDeposits;

    void display() const;
    void displaySummary() const;
};

// ====================================================================
//  CLASS: Teller
// ====================================================================
class Teller {
private:
    char tellerID[10];
    char fullName[60];
    char encryptedPassword[60];
    char branchCode[10];
    bool isActive;
public:
    void setData(const string& id, const string& name,
        const string& pass, const string& branch);
    bool authenticate(const string& id, const string& pass) const;
    string getTellerID()   const { return string(tellerID); }
    string getFullName()   const { return string(fullName); }
    string getBranchCode() const { return string(branchCode); }
    bool   getActive()     const { return isActive; }
    void   display()       const;
};

// ====================================================================
//  CLASS: Transaction
// ====================================================================
class Transaction {
public:
    char   transactionID[20];
    char   accountNumber[25];
    char   type[20];
    double amount;
    double balanceAfter;
    char   date[20];
    char   tellerID[10];
    char   note[80];

    void setData(const string& acno, const string& txType,
        double amt, double balAfter,
        const string& tID = "", const string& noteStr = "");
    void display() const;
    string getAccountNumber() const { return string(accountNumber); }
};

// ====================================================================
//  BASE CLASS: Account
// ====================================================================
class Account {
protected:
    char   accountNumber[25];
    char   firstName[40];
    char   surname[40];
    char   idNumber[14];
    char   contactNumber[12];
    char   email[60];
    char   address[100];
    char   dateOfBirth[12];
    char   accountType[20];
    double balance;
    char   branchCode[10];
    char   encryptedPIN[10];
    bool   locked;
    int    loginAttempts;
    char   openDate[20];
    bool   active;

public:
    Account();
    virtual ~Account() {}

    // --- Setters ---
    void setAccountNumber(const string& s) { strncpy(accountNumber, s.c_str(), 24); }
    void setFirstName(const string& s) { strncpy(firstName, s.c_str(), 39); }
    void setSurname(const string& s) { strncpy(surname, s.c_str(), 39); }
    void setIDNumber(const string& s) { strncpy(idNumber, s.c_str(), 13); }
    void setContact(const string& s) { strncpy(contactNumber, s.c_str(), 11); }
    void setEmail(const string& s) { strncpy(email, s.c_str(), 59); }
    void setAddress(const string& s) { strncpy(address, s.c_str(), 99); }
    void setDOB(const string& s) { strncpy(dateOfBirth, s.c_str(), 11); }
    void setAccountType(const string& s) { strncpy(accountType, s.c_str(), 19); }
    void setBalance(double b) { balance = b; }
    void setBranchCode(const string& s) { strncpy(branchCode, s.c_str(), 9); }
    void setPIN(const string& pin) { strncpy(encryptedPIN, encryptString(pin).c_str(), 9); }
    void setOpenDate(const string& s) { strncpy(openDate, s.c_str(), 19); }
    void setActive(bool a) { active = a; }

    // --- Getters ---
    string getAccountNumber() const { return string(accountNumber); }
    string getFirstName()     const { return string(firstName); }
    string getSurname()       const { return string(surname); }
    string getIDNumber()      const { return string(idNumber); }
    string getContact()       const { return string(contactNumber); }
    string getEmail()         const { return string(email); }
    string getAddress()       const { return string(address); }
    string getDOB()           const { return string(dateOfBirth); }
    string getAccountType()   const { return string(accountType); }
    double getBalance()       const { return balance; }
    string getBranchCode()    const { return string(branchCode); }
    bool   isLocked()         const { return locked; }
    bool   isActive()         const { return active; }
    string getOpenDate()      const { return string(openDate); }

    // --- PIN helpers ---
    bool   verifyPIN(const string& pin) const;
    void   changePIN(const string& newPin);
    void   incrementLoginAttempts();
    void   resetLoginAttempts();
    void   lockAccount() { locked = true; }
    void   unlockAccount() { locked = false; loginAttempts = 0; }
    string getRawEncryptedPIN() const { return string(encryptedPIN, 9); }
    void   setRawEncryptedPIN(const char* raw) { memcpy(encryptedPIN, raw, 9); }

    // --- Financial ---
    virtual void   deposit(double amount);
    virtual bool   withdraw(double amount);
    virtual double calculateInterest() const = 0;
    virtual double getMinimumBalance()  const = 0;
    virtual void   applyInterest();

    // --- Display ---
    virtual void display()        const;
    virtual void displaySummary() const;

    // --- Static validators ---
    static bool validateIDNumber(const string& id);
    static bool validateEmail(const string& email);
    static bool validateContact(const string& contact);
    static bool validateDOB(const string& dob);
};

// ====================================================================
//  DERIVED: SavingsAccount
// ====================================================================
class SavingsAccount : public Account {
public:
    SavingsAccount() { setAccountType("SAVINGS"); }
    double calculateInterest() const override { return balance * SAVINGS_INTEREST_RATE / 12.0; }
    double getMinimumBalance()  const override { return MIN_SAVINGS_DEPOSIT; }
};

// ====================================================================
//  DERIVED: ChequeAccount
// ====================================================================
class ChequeAccount : public Account {
private:
    double overdraftLimit;
public:
    ChequeAccount() : overdraftLimit(500.0) { setAccountType("CHEQUE"); }
    bool   withdraw(double amount) override;
    double calculateInterest() const override { return (balance > 0) ? balance * CHEQUE_INTEREST_RATE / 12.0 : 0.0; }
    double getMinimumBalance()  const override { return MIN_CHEQUE_DEPOSIT; }
};

// ====================================================================
//  DERIVED: FixedDepositAccount
// ====================================================================
class FixedDepositAccount : public Account {
private:
    int termMonths;
public:
    FixedDepositAccount() : termMonths(12) { setAccountType("FIXED"); }
    bool   withdraw(double amount) override;
    double calculateInterest() const override { return balance * FIXED_INTEREST_RATE / 12.0; }
    double getMinimumBalance()  const override { return MIN_FIXED_DEPOSIT; }
};

// ====================================================================
//  DERIVED: StudentAccount
// ====================================================================
class StudentAccount : public Account {
public:
    StudentAccount() { setAccountType("STUDENT"); }
    double calculateInterest() const override { return balance * STUDENT_INTEREST_RATE / 12.0; }
    double getMinimumBalance()  const override { return MIN_STUDENT_DEPOSIT; }
};

// ====================================================================
//  UTILITY FUNCTIONS (forward declarations)
// ====================================================================
namespace Utils {
    string currentDateTime();
    string currentDate();
    string generateAccountNumber(const string& branchCode);
    string generatePIN();
    string generateTransactionID();
    void   clearScreen();
    void   printHeader(const string& title);
    void   printSeparator(char c = '=', int len = 70);
    void   pause();
    string toUpper(const string& s);
    bool   confirmAction(const string& prompt);
}

// ====================================================================
//  FILE HANDLER (flat AccountRecord for binary I/O)
// ====================================================================
namespace FileHandler {
    struct AccountRecord {
        char   accountNumber[25];
        char   firstName[40];
        char   surname[40];
        char   idNumber[14];
        char   contactNumber[12];
        char   email[60];
        char   address[100];
        char   dateOfBirth[12];
        char   accountType[20];
        double balance;
        char   branchCode[10];
        char   encryptedPIN[10];
        bool   locked;
        int    loginAttempts;
        char   openDate[20];
        bool   active;
    };

    static void fillRecord(const Account& acc, AccountRecord& rec) {
        memset(&rec, 0, sizeof(rec));
        strncpy(rec.accountNumber, acc.getAccountNumber().c_str(), 24);
        strncpy(rec.firstName, acc.getFirstName().c_str(), 39);
        strncpy(rec.surname, acc.getSurname().c_str(), 39);
        strncpy(rec.idNumber, acc.getIDNumber().c_str(), 13);
        strncpy(rec.contactNumber, acc.getContact().c_str(), 11);
        strncpy(rec.email, acc.getEmail().c_str(), 59);
        strncpy(rec.address, acc.getAddress().c_str(), 99);
        strncpy(rec.dateOfBirth, acc.getDOB().c_str(), 11);
        strncpy(rec.accountType, acc.getAccountType().c_str(), 19);
        rec.balance = acc.getBalance();
        strncpy(rec.branchCode, acc.getBranchCode().c_str(), 9);
        strncpy(rec.openDate, acc.getOpenDate().c_str(), 19);
        rec.active = acc.isActive();
        rec.locked = acc.isLocked();
        string rawPin = acc.getRawEncryptedPIN();
        memcpy(rec.encryptedPIN, rawPin.c_str(), 9);
    }

    static Account* buildAccount(const AccountRecord& rec) {
        Account* acc = nullptr;
        string type = string(rec.accountType);
        if (type == "SAVINGS") acc = new SavingsAccount();
        else if (type == "CHEQUE")  acc = new ChequeAccount();
        else if (type == "FIXED")   acc = new FixedDepositAccount();
        else if (type == "STUDENT") acc = new StudentAccount();
        else                        acc = new SavingsAccount();
        acc->setAccountNumber(rec.accountNumber);
        acc->setFirstName(rec.firstName);
        acc->setSurname(rec.surname);
        acc->setIDNumber(rec.idNumber);
        acc->setContact(rec.contactNumber);
        acc->setEmail(rec.email);
        acc->setAddress(rec.address);
        acc->setDOB(rec.dateOfBirth);
        acc->setAccountType(rec.accountType);
        acc->setBalance(rec.balance);
        acc->setBranchCode(rec.branchCode);
        acc->setOpenDate(rec.openDate);
        acc->setActive(rec.active);
        if (rec.locked) acc->lockAccount();
        acc->setRawEncryptedPIN(rec.encryptedPIN);
        return acc;
    }

    bool saveTeller(const Teller& t) {
        ofstream f("tellers.dat", ios::binary | ios::app);
        if (!f) return false;
        f.write((const char*)&t, sizeof(Teller));
        return true;
    }

    bool loadTellers(vector<Teller>& tellers) {
        tellers.clear();
        ifstream f("tellers.dat", ios::binary);
        if (!f) return false;
        Teller t;
        while (f.read((char*)&t, sizeof(Teller)))
            tellers.push_back(t);
        return true;
    }

    bool saveAccount(const Account& acc) {
        ofstream f("customers.dat", ios::binary | ios::app);
        if (!f) return false;
        AccountRecord rec;
        fillRecord(acc, rec);
        f.write((const char*)&rec, sizeof(AccountRecord));
        return true;
    }

    bool loadAccounts(vector<Account*>& accounts) {
        accounts.clear();
        ifstream f("customers.dat", ios::binary);
        if (!f) return false;
        AccountRecord rec;
        while (f.read((char*)&rec, sizeof(AccountRecord)))
            accounts.push_back(buildAccount(rec));
        return true;
    }

    bool updateAccount(const Account& acc) {
        vector<Account*> all;
        loadAccounts(all);
        bool found = false;
        for (auto& a : all) {
            if (a->getAccountNumber() == acc.getAccountNumber()) {
                delete a; a = nullptr; found = true;
            }
        }
        if (!found) { for (auto a : all) delete a; return false; }
        ofstream f("customers.dat", ios::binary | ios::trunc);
        if (!f) { for (auto a : all) delete a; return false; }
        AccountRecord rec;
        for (auto a : all) {
            if (!a) continue;
            fillRecord(*a, rec);
            f.write((const char*)&rec, sizeof(AccountRecord));
            delete a;
        }
        f.close();
        return saveAccount(acc);
    }

    Account* loadAccountByNumber(const string& accNo) {
        vector<Account*> all;
        loadAccounts(all);
        Account* found = nullptr;
        for (auto a : all) {
            if (a->getAccountNumber() == accNo) {
                if (found) delete found;
                found = a;
            }
            else {
                delete a;
            }
        }
        return found;
    }

    bool saveTransaction(const Transaction& tx) {
        ofstream f("transactions.dat", ios::binary | ios::app);
        if (!f) return false;
        f.write((const char*)&tx, sizeof(Transaction));
        return true;
    }

    bool loadTransactions(vector<Transaction>& txs) {
        txs.clear();
        ifstream f("transactions.dat", ios::binary);
        if (!f) return false;
        Transaction tx;
        while (f.read((char*)&tx, sizeof(Transaction)))
            txs.push_back(tx);
        return true;
    }

    bool loadTransactionsByAccount(const string& accNo, vector<Transaction>& txs) {
        txs.clear();
        vector<Transaction> all;
        loadTransactions(all);
        for (auto& tx : all)
            if (string(tx.accountNumber) == accNo)
                txs.push_back(tx);
        return true;
    }

    bool saveBranch(const Branch& b) {
        ofstream f("branches.dat", ios::binary | ios::app);
        if (!f) return false;
        f.write((const char*)&b, sizeof(Branch));
        return true;
    }

    bool loadBranches(vector<Branch>& branches) {
        branches.clear();
        ifstream f("branches.dat", ios::binary);
        if (!f) return false;
        Branch b;
        while (f.read((char*)&b, sizeof(Branch)))
            branches.push_back(b);
        return true;
    }

    bool updateBranch(const Branch& updated) {
        vector<Branch> all;
        loadBranches(all);
        ofstream f("branches.dat", ios::binary | ios::trunc);
        if (!f) return false;
        bool found = false;
        for (auto& b : all) {
            if (string(b.branchCode) == string(updated.branchCode)) {
                f.write((const char*)&updated, sizeof(Branch)); found = true;
            }
            else {
                f.write((const char*)&b, sizeof(Branch));
            }
        }
        if (!found) f.write((const char*)&updated, sizeof(Branch));
        return true;
    }

    void exportAccountsToCSV(const string& filename) {
        vector<Account*> accounts;
        loadAccounts(accounts);
        ofstream f(filename);
        f << "AccountNumber,FirstName,Surname,IDNumber,Contact,Email,AccountType,Branch,Balance,OpenDate,Status\n";
        for (auto a : accounts) {
            f << a->getAccountNumber() << "," << a->getFirstName() << ","
                << a->getSurname() << "," << a->getIDNumber() << ","
                << a->getContact() << "," << a->getEmail() << ","
                << a->getAccountType() << "," << a->getBranchCode() << ","
                << fixed << setprecision(2) << a->getBalance() << ","
                << a->getOpenDate() << "," << (a->isActive() ? "Active" : "Closed") << "\n";
            delete a;
        }
        cout << "\n  Accounts exported to " << filename << "\n";
    }

    void exportTransactionsToCSV(const string& filename) {
        vector<Transaction> txs;
        loadTransactions(txs);
        ofstream f(filename);
        f << "TransactionID,AccountNumber,Type,Amount,BalanceAfter,Date,TellerID,Note\n";
        for (auto& tx : txs)
            f << tx.transactionID << "," << tx.accountNumber << ","
            << tx.type << "," << tx.amount << "," << tx.balanceAfter << ","
            << tx.date << "," << tx.tellerID << "," << tx.note << "\n";
        cout << "\n  Transactions exported to " << filename << "\n";
    }

    void backupData() {
        auto copyFile = [](const string& src, const string& dst) {
            ifstream in(src, ios::binary);
            ofstream out(dst, ios::binary);
            if (in && out) out << in.rdbuf();
            };
        string stamp = Utils::currentDate();
        copyFile("customers.dat", "backup_customers_" + stamp + ".dat");
        copyFile("transactions.dat", "backup_transactions_" + stamp + ".dat");
        copyFile("tellers.dat", "backup_tellers_" + stamp + ".dat");
        copyFile("branches.dat", "backup_branches_" + stamp + ".dat");
        cout << "\n  Backup completed (date: " << stamp << ")\n";
    }
}

// ====================================================================
//  UTILS IMPLEMENTATION
// ====================================================================
namespace Utils {
    string currentDateTime() {
        time_t now = time(nullptr);
        char buf[20];
        tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        return string(buf);   // ✅ FIX
    }

    string currentDate() {
        time_t now = time(nullptr);
        char buf[12];
        tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(buf, sizeof(buf), "%Y-%m-%d", &timeinfo);
        return string(buf);   // ✅ FIX
    }
    static int accountCounter = 1000;
    static int txCounter = 1;
    string generateAccountNumber(const string& branchCode) {
        ostringstream oss;
        oss << "ACC-" << branchCode << "-" << setw(5) << setfill('0') << (accountCounter++);
        return oss.str();
    }
    string generatePIN() {
        srand((unsigned)time(nullptr) + rand());
        ostringstream oss;
        oss << setw(5) << setfill('0') << (rand() % 100000);
        return oss.str();
    }
    string generateTransactionID() {
        ostringstream oss;
        oss << "TX" << setw(8) << setfill('0') << (txCounter++);
        return oss.str();
    }
    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    void printHeader(const string& title) {
        clearScreen();
        printSeparator('=');
        int pad = (70 - (int)title.size()) / 2;
        cout << string(pad > 0 ? pad : 0, ' ') << title << "\n";
        printSeparator('=');
        cout << "\n";
    }
    void printSeparator(char c, int len) {
        cout << string(len, c) << "\n";
    }
    void pause() {
        cout << "\n  Press Enter to continue...";
        cin.ignore(1000, '\n');
        cin.get();
    }
    string toUpper(const string& s) {
        string r = s;
        for (char& c : r) c = toupper(c);
        return r;
    }
    bool confirmAction(const string& prompt) {
        cout << "\n  " << prompt << " [Y/N]: ";
        char c; cin >> c; cin.ignore(1000, '\n');
        return toupper(c) == 'Y';
    }
}

// ====================================================================
//  Branch implementations
// ====================================================================
void Branch::display() const {
    Utils::printSeparator('-');
    cout << "  Branch Code   : " << branchCode << "\n";
    cout << "  Branch Name   : " << branchName << "\n";
    cout << "  City          : " << city << "\n";
    cout << "  Address       : " << address << "\n";
    cout << "  Phone         : " << phone << "\n";
    cout << "  Tellers       : " << tellerCount << "\n";
    cout << "  Accounts      : " << accountCount << "\n";
    cout << "  Total Deposits: R" << fixed << setprecision(2) << totalDeposits << "\n";
    Utils::printSeparator('-');
}
void Branch::displaySummary() const {
    cout << left << setw(10) << branchCode
        << setw(30) << branchName
        << setw(15) << city
        << "Acc: " << setw(5) << accountCount
        << "  Dep: R" << fixed << setprecision(2) << totalDeposits << "\n";
}

// ====================================================================
//  Teller implementations
// ====================================================================
void Teller::setData(const string& id, const string& name,
    const string& pass, const string& branch) {
    strncpy(tellerID, id.c_str(), 9);
    strncpy(fullName, name.c_str(), 59);
    strncpy(branchCode, branch.c_str(), 9);
    string enc = encryptString(pass);
    strncpy(encryptedPassword, enc.c_str(), 59);
    isActive = true;
}
bool Teller::authenticate(const string& id, const string& pass) const {
    if (!isActive) return false;
    if (string(tellerID) != id) return false;
    return decryptString(string(encryptedPassword)) == pass;
}
void Teller::display() const {
    cout << "  Teller ID : " << tellerID << "\n";
    cout << "  Name      : " << fullName << "\n";
    cout << "  Branch    : " << branchCode << "\n";
    cout << "  Status    : " << (isActive ? "Active" : "Inactive") << "\n";
}

// ====================================================================
//  Transaction implementations
// ====================================================================
void Transaction::setData(const string& acno, const string& txType,
    double amt, double balAfter,
    const string& tID, const string& noteStr) {
    strncpy(transactionID, Utils::generateTransactionID().c_str(), 19);
    strncpy(accountNumber, acno.c_str(), 24);
    strncpy(type, txType.c_str(), 19);
    strncpy(date, Utils::currentDateTime().c_str(), 19);
    strncpy(tellerID, tID.c_str(), 9);
    strncpy(note, noteStr.c_str(), 79);
    amount = amt;
    balanceAfter = balAfter;
}
void Transaction::display() const {
    cout << left << setw(20) << transactionID
        << setw(18) << date
        << setw(14) << type
        << right << setw(12) << fixed << setprecision(2) << amount
        << setw(14) << balanceAfter << "\n";
}

// ====================================================================
//  Account base class implementations
// ====================================================================
Account::Account() {
    memset(accountNumber, 0, sizeof(accountNumber));
    memset(firstName, 0, sizeof(firstName));
    memset(surname, 0, sizeof(surname));
    memset(idNumber, 0, sizeof(idNumber));
    memset(contactNumber, 0, sizeof(contactNumber));
    memset(email, 0, sizeof(email));
    memset(address, 0, sizeof(address));
    memset(dateOfBirth, 0, sizeof(dateOfBirth));
    memset(accountType, 0, sizeof(accountType));
    memset(branchCode, 0, sizeof(branchCode));
    memset(encryptedPIN, 0, sizeof(encryptedPIN));
    memset(openDate, 0, sizeof(openDate));
    balance = 0.0;
    locked = false;
    loginAttempts = 0;
    active = true;
}
bool Account::verifyPIN(const string& pin) const {
    return decryptString(string(encryptedPIN)) == pin;
}
void Account::changePIN(const string& newPin) {
    strncpy(encryptedPIN, encryptString(newPin).c_str(), 9);
}
void Account::incrementLoginAttempts() {
    loginAttempts++;
    if (loginAttempts >= MAX_LOGIN_ATTEMPTS) locked = true;
}
void Account::resetLoginAttempts() { loginAttempts = 0; locked = false; }
void Account::deposit(double amount) {
    if (amount <= 0) throw invalid_argument("Deposit amount must be positive.");
    balance += amount;
}
bool Account::withdraw(double amount) {
    if (amount <= 0) throw invalid_argument("Withdrawal amount must be positive.");
    if (balance - amount < getMinimumBalance()) {
        cout << "\n  [!] Insufficient funds. Minimum balance R"
            << fixed << setprecision(2) << getMinimumBalance() << " required.\n";
        return false;
    }
    balance -= amount;
    return true;
}
void Account::applyInterest() {
    double interest = calculateInterest();
    if (interest > 0) {
        balance += interest;
        cout << "  Interest R" << fixed << setprecision(2) << interest
            << " applied to " << accountNumber << "\n";
    }
}
void Account::display() const {
    Utils::printSeparator('-');
    cout << "  Account No    : " << accountNumber << "\n";
    cout << "  Name          : " << firstName << " " << surname << "\n";
    cout << "  ID Number     : " << idNumber << "\n";
    cout << "  Contact       : " << contactNumber << "\n";
    cout << "  Email         : " << email << "\n";
    cout << "  Address       : " << address << "\n";
    cout << "  Date of Birth : " << dateOfBirth << "\n";
    cout << "  Account Type  : " << accountType << "\n";
    cout << "  Branch        : " << branchCode << "\n";
    cout << "  Balance       : R" << fixed << setprecision(2) << balance << "\n";
    cout << "  Opened        : " << openDate << "\n";
    cout << "  Status        : " << (active ? (locked ? "LOCKED" : "Active") : "Closed") << "\n";
    Utils::printSeparator('-');
}
void Account::displaySummary() const {
    cout << left << setw(22) << accountNumber
        << setw(25) << (string(firstName) + " " + string(surname))
        << setw(10) << accountType
        << setw(8) << branchCode
        << right << "R" << setw(12) << fixed << setprecision(2) << balance
        << (locked ? "  [LOCKED]" : "") << "\n";
}
bool Account::validateIDNumber(const string& id) {
    // Must be exactly 13 characters
    if (id.length() != 13) return false;

    // Must contain only digits
    for (char c : id) {
        if (!isdigit(c)) return false;
    }

    return true;
}

bool Account::validateEmail(const string& e) {
    size_t at = e.find('@');
    if (at == string::npos || at == 0) return false;
    size_t dot = e.find('.', at);
    return (dot != string::npos && dot < e.size() - 1);
}
bool Account::validateContact(const string& c) {
    if (c.size() != 10) return false;
    for (char ch : c) if (!isdigit(ch)) return false;
    return true;
}
bool Account::validateDOB(const string& d) {
    if (d.size() != 10) return false;
    if (d[2] != '/' || d[5] != '/') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(d[i])) return false;
    }
    return true;
}

// ====================================================================
//  Derived account special overrides
// ====================================================================
bool ChequeAccount::withdraw(double amount) {
    if (amount <= 0) throw invalid_argument("Amount must be positive.");
    if (balance + overdraftLimit - amount < 0) {
        cout << "\n  [!] Exceeds overdraft limit (R" << overdraftLimit << ").\n";
        return false;
    }
    balance -= amount;
    return true;
}
bool FixedDepositAccount::withdraw(double /*amount*/) {
    cout << "\n  [!] Fixed Deposit accounts do not allow early withdrawal.\n";
    return false;
}

// ====================================================================
//  FORWARD DECLARATIONS (menus & helpers)
// ====================================================================
bool authenticateTeller(Teller& outTeller);
bool authenticateCustomer(Account*& outAccount);
void tellerMenu(Teller& teller);
void customerMenu(Account* acc);
void registerTeller(const Teller& teller);
void registerCustomer(const Teller& teller);
void viewCustomerDetails(const Teller& teller);
void processTellerTransaction(const Teller& teller);
void generateBranchReport(const Teller& teller);
void viewAllBranches();
void viewBalance(Account* acc);
void customerDeposit(Account* acc, const string& tellerID = "");
void customerWithdraw(Account* acc, const string& tellerID = "");
void customerTransfer(Account* acc, const string& tellerID = "");
void viewStatement(Account* acc);
void changeCustomerPIN(Account* acc);
void applyInterestAll();
void searchCustomer();
void managementReports();
void seedDefaultData();

// ====================================================================
//  TELLER AUTHENTICATION
// ====================================================================
bool authenticateTeller(Teller& outTeller) {
    Utils::printHeader("TELLER LOGIN");
    vector<Teller> tellers;
    FileHandler::loadTellers(tellers);
    if (tellers.empty()) {
        cout << "  [!] No teller accounts found.\n";
        Utils::pause();
        return false;
    }
    int attempts = 0;
    while (attempts < MAX_LOGIN_ATTEMPTS) {
        string id, pass;
        cout << "  Teller ID : "; getline(cin, id);
        cout << "  Password  : "; getline(cin, pass);
        for (auto& t : tellers) {
            if (t.authenticate(id, pass)) {
                outTeller = t;
                cout << "\n  Welcome, " << t.getFullName()
                    << "! (Branch: " << t.getBranchCode() << ")\n";
                Utils::pause();
                return true;
            }
        }
        attempts++;
        cout << "\n  [!] Invalid credentials. Attempt " << attempts
            << "/" << MAX_LOGIN_ATTEMPTS << "\n\n";
    }
    cout << "\n  [!] Too many failed attempts. Access denied.\n";
    Utils::pause();
    return false;
}

// ====================================================================
//  CUSTOMER AUTHENTICATION
// ====================================================================
bool authenticateCustomer(Account*& outAccount) {
    Utils::printHeader("CUSTOMER LOGIN");
    string accNo;
    cout << "  Account Number (e.g. ACC-JHB-01000): ";
    getline(cin, accNo);
    accNo = Utils::toUpper(accNo);
    Account* acc = FileHandler::loadAccountByNumber(accNo);
    if (!acc) {
        cout << "\n  [!] Account not found.\n"; Utils::pause(); return false;
    }
    if (!acc->isActive()) {
        cout << "\n  [!] This account has been closed.\n";
        delete acc; Utils::pause(); return false;
    }
    if (acc->isLocked()) {
        cout << "\n  [!] Account is locked. Visit a branch to unlock.\n";
        delete acc; Utils::pause(); return false;
    }
    int attempts = 0;
    while (attempts < MAX_LOGIN_ATTEMPTS) {
        string pin;
        cout << "  Enter 5-digit PIN: "; getline(cin, pin);
        if (acc->verifyPIN(pin)) {
            acc->resetLoginAttempts();
            FileHandler::updateAccount(*acc);
            outAccount = acc;
            cout << "\n  Welcome, " << acc->getFirstName() << "!\n";
            Utils::pause();
            return true;
        }
        attempts++;
        acc->incrementLoginAttempts();
        FileHandler::updateAccount(*acc);
        cout << "\n  [!] Incorrect PIN. Attempt " << attempts
            << "/" << MAX_LOGIN_ATTEMPTS << "\n\n";
        if (acc->isLocked()) {
            cout << "\n  [!] Account locked. Visit a branch to unlock.\n";
            delete acc; Utils::pause(); return false;
        }
    }
    delete acc; Utils::pause();
    return false;
}
// ====================================================================
//  REGISTER NEW TELLER  (EXISTING Teller only)
// ====================================================================
void registerTeller(const Teller& teller) {

    if (teller.getTellerID() != "T001") {
        cout << "\n  [!] Only admin can create tellers.\n";
        Utils::pause();
        return;
    }

    Utils::printHeader("REGISTER NEW TELLER");

    string id, name, password, branchCode;

    cout << "  Enter Teller ID (e.g. T004): ";
    getline(cin, id);

    cout << "  Enter Full Name: ";
    getline(cin, name);

    cout << "  Enter Password: ";
    getline(cin, password);

    // Show available branches
    vector<Branch> branches;
    FileHandler::loadBranches(branches);

    if (branches.empty()) {
        cout << "\n  [!] No branches available.\n";
        Utils::pause();
        return;
    }

    cout << "\n  Available Branches:\n";
    for (auto& b : branches) {
        cout << "  - " << b.branchCode << " (" << b.branchName << ")\n";
    }

    cout << "\n  Enter Branch Code: ";
    getline(cin, branchCode);
    branchCode = Utils::toUpper(branchCode);

    // Validate branch exists
    bool validBranch = false;
    for (auto& b : branches) {
        if (branchCode == b.branchCode) {
            validBranch = true;
            break;
        }
    }

    if (!validBranch) {
        cout << "\n  [!] Invalid branch code.\n";
        Utils::pause();
        return;
    }

    // Check duplicate Teller ID
    vector<Teller> tellers;
    FileHandler::loadTellers(tellers);

    for (auto& t : tellers) {
        if (t.getTellerID() == id) {
            cout << "\n  [!] Teller ID already exists.\n";
            Utils::pause();
            return;
        }
    }

    Teller t;
    t.setData(id, name, password, branchCode);

    if (FileHandler::saveTeller(t)) {
        cout << "\n  Teller created successfully.\n";

        // OPTIONAL: update branch teller count
        for (auto& b : branches) {
            if (branchCode == b.branchCode) {
                b.tellerCount++;
                FileHandler::updateBranch(b);
                break;
            }
        }
    }
    else {
        cout << "\n  [!] Failed to save teller.\n";
    }

    Utils::pause();
}

// ====================================================================
//  REGISTER NEW CUSTOMER  (Teller only)
// ====================================================================
void registerCustomer(const Teller& teller) {
    Utils::printHeader("REGISTER NEW CUSTOMER");
    cout << "  Account Types:\n"
        << "  [1] SAVINGS  (Min: R500)\n"
        << "  [2] CHEQUE   (Min: R1000)\n"
        << "  [3] FIXED    (Min: R5000)\n"
        << "  [4] STUDENT  (Min: R100)\n"
        << "\n  Select [1-4]: ";
    string choice; getline(cin, choice);

    Account* acc = nullptr;
    double minDeposit = 0;
    if (choice == "1") { acc = new SavingsAccount();      minDeposit = MIN_SAVINGS_DEPOSIT; }
    else if (choice == "2") { acc = new ChequeAccount();       minDeposit = MIN_CHEQUE_DEPOSIT; }
    else if (choice == "3") { acc = new FixedDepositAccount(); minDeposit = MIN_FIXED_DEPOSIT; }
    else if (choice == "4") { acc = new StudentAccount();      minDeposit = MIN_STUDENT_DEPOSIT; }
    else {
        cout << "\n  [!] Invalid selection.\n"; Utils::pause(); return;
    }

    string branchCode = teller.getBranchCode();
    acc->setBranchCode(branchCode);

    string input;
    cout << "\n  First Name : "; getline(cin, input); acc->setFirstName(input);
    cout << "  Surname    : "; getline(cin, input); acc->setSurname(input);

    do {
        cout << "  SA ID Number (13 digits): "; getline(cin, input);
        if (!Account::validateIDNumber(input)) cout << " [!] ID must be exactly 13 digits (numbers only).\n";
    } while (!Account::validateIDNumber(input));
    acc->setIDNumber(input);

    do {
        cout << "  Contact (10 digits): "; getline(cin, input);
        if (!Account::validateContact(input)) cout << "  [!] Must be 10 digits.\n";
    } while (!Account::validateContact(input));
    acc->setContact(input);

    do {
        cout << "  Email Address: "; getline(cin, input);
        if (!Account::validateEmail(input)) cout << "  [!] Invalid email.\n";
    } while (!Account::validateEmail(input));
    acc->setEmail(input);

    cout << "  Physical Address: "; getline(cin, input); acc->setAddress(input);

    do {
        cout << "  Date of Birth (DD/MM/YYYY): "; getline(cin, input);
        if (!Account::validateDOB(input)) cout << "  [!] Use format DD/MM/YYYY.\n";
    } while (!Account::validateDOB(input));
    acc->setDOB(input);

    double initialDeposit = 0;
    do {
        cout << "\n  Initial Deposit (min R" << fixed << setprecision(2) << minDeposit << "): R";
        try {
            getline(cin, input);
            initialDeposit = stod(input);
            if (initialDeposit < minDeposit)
                cout << "  [!] Below minimum. Try again.\n";
        }
        catch (...) {
            cout << "  [!] Invalid amount.\n"; initialDeposit = 0;
        }
    } while (initialDeposit < minDeposit);

    string accNo = Utils::generateAccountNumber(branchCode);
    string pin = Utils::generatePIN();
    acc->setAccountNumber(accNo);
    acc->setPIN(pin);
    acc->setBalance(initialDeposit);
    acc->setOpenDate(Utils::currentDate());
    acc->setActive(true);

    if (FileHandler::saveAccount(*acc)) {
        Transaction tx;
        tx.setData(accNo, "DEPOSIT", initialDeposit, initialDeposit,
            teller.getTellerID(), "Account opening deposit");
        FileHandler::saveTransaction(tx);

        vector<Branch> branches;
        FileHandler::loadBranches(branches);
        for (auto& b : branches) {
            if (string(b.branchCode) == branchCode) {
                b.accountCount++;
                b.totalDeposits += initialDeposit;
                FileHandler::updateBranch(b);
                break;
            }
        }

        Utils::printSeparator('=');
        cout << "\n  ACCOUNT CREATED SUCCESSFULLY\n\n";
        cout << "  Account Number : " << accNo << "\n";
        cout << "  Customer Name  : " << acc->getFirstName() << " " << acc->getSurname() << "\n";
        cout << "  Account Type   : " << acc->getAccountType() << "\n";
        cout << "  Opening Balance: R" << fixed << setprecision(2) << initialDeposit << "\n";
        cout << "\n  *** GENERATED PIN (show once): " << pin << " ***\n";
        cout << "  [!] Advise customer to change PIN on first login.\n";
        Utils::printSeparator('=');
    }
    else {
        cout << "\n  [!] Failed to save account.\n";
    }
    delete acc;
    Utils::pause();
}

// ====================================================================
//  VIEW CUSTOMER DETAILS  (Teller)
// ====================================================================
void viewCustomerDetails(const Teller& teller) {
    Utils::printHeader("VIEW CUSTOMER DETAILS");
    string accNo;
    cout << "  Account Number: "; getline(cin, accNo);
    accNo = Utils::toUpper(accNo);
    Account* acc = FileHandler::loadAccountByNumber(accNo);
    if (!acc) { cout << "\n  [!] Not found.\n"; Utils::pause(); return; }
    if (acc->getBranchCode() != teller.getBranchCode()) {
        cout << "\n  [!] Access denied: different branch.\n";
        delete acc; Utils::pause(); return;
    }
    acc->display();
    delete acc;
    Utils::pause();
}

// ====================================================================
//  TELLER-ASSISTED TRANSACTION
// ====================================================================
void processTellerTransaction(const Teller& teller) {
    Utils::printHeader("PROCESS TRANSACTION");
    string accNo;
    cout << "  Customer Account Number: "; getline(cin, accNo);
    accNo = Utils::toUpper(accNo);
    Account* acc = FileHandler::loadAccountByNumber(accNo);
    if (!acc) { cout << "\n  [!] Not found.\n"; Utils::pause(); return; }
    if (acc->getBranchCode() != teller.getBranchCode()) {
        cout << "\n  [!] Branch restriction.\n"; delete acc; Utils::pause(); return;
    }
    string pin;
    cout << "\n  Verify customer PIN: "; getline(cin, pin);
    if (!acc->verifyPIN(pin)) {
        cout << "\n  [!] PIN incorrect. Transaction cancelled.\n";
        delete acc; Utils::pause(); return;
    }
    cout << "\n  [1] Deposit  [2] Withdraw  [3] Transfer\n  Select: ";
    string opt; getline(cin, opt);
    if (opt == "1") customerDeposit(acc, teller.getTellerID());
    else if (opt == "2") customerWithdraw(acc, teller.getTellerID());
    else if (opt == "3") customerTransfer(acc, teller.getTellerID());
    else cout << "\n  [!] Invalid.\n";
    delete acc;
    Utils::pause();
}

// ====================================================================
//  BALANCE ENQUIRY
// ====================================================================
void viewBalance(Account* acc) {
    Utils::printHeader("ACCOUNT BALANCE");
    cout << "  Account : " << acc->getAccountNumber() << "\n";
    cout << "  Name    : " << acc->getFirstName() << " " << acc->getSurname() << "\n";
    cout << "  Type    : " << acc->getAccountType() << "\n";
    Utils::printSeparator('-');
    cout << "  BALANCE : R" << fixed << setprecision(2) << acc->getBalance() << "\n";
    Utils::printSeparator('-');
    Utils::pause();
}

// ====================================================================
//  DEPOSIT
// ====================================================================
void customerDeposit(Account* acc, const string& tellerID) {
    Utils::printHeader("CASH DEPOSIT");
    cout << "  Account : " << acc->getAccountNumber() << "\n";
    cout << "  Balance : R" << fixed << setprecision(2) << acc->getBalance() << "\n\n";
    cout << "  Amount  : R";
    string input; getline(cin, input);
    try {
        double amount = stod(input);
        acc->deposit(amount);
        FileHandler::updateAccount(*acc);
        Transaction tx;
        tx.setData(acc->getAccountNumber(), "DEPOSIT", amount, acc->getBalance(),
            tellerID, tellerID.empty() ? "Self-service" : "Teller deposit");
        FileHandler::saveTransaction(tx);
        cout << "\n  Deposit successful. New Balance: R"
            << fixed << setprecision(2) << acc->getBalance() << "\n";
    }
    catch (exception& e) {
        cout << "\n  [!] " << e.what() << "\n";
    }
    Utils::pause();
}

// ====================================================================
//  WITHDRAWAL
// ====================================================================
void customerWithdraw(Account* acc, const string& tellerID) {
    Utils::printHeader("CASH WITHDRAWAL");
    cout << "  Account : " << acc->getAccountNumber() << "\n";
    cout << "  Balance : R" << fixed << setprecision(2) << acc->getBalance() << "\n\n";
    cout << "  Amount  : R";
    string input; getline(cin, input);
    try {
        double amount = stod(input);
        if (acc->withdraw(amount)) {
            FileHandler::updateAccount(*acc);
            Transaction tx;
            tx.setData(acc->getAccountNumber(), "WITHDRAWAL", amount, acc->getBalance(),
                tellerID, tellerID.empty() ? "Self-service" : "Teller withdrawal");
            FileHandler::saveTransaction(tx);
            cout << "\n  Withdrawal successful. New Balance: R"
                << fixed << setprecision(2) << acc->getBalance() << "\n";
        }
    }
    catch (exception& e) {
        cout << "\n  [!] " << e.what() << "\n";
    }
    Utils::pause();
}

// ====================================================================
//  TRANSFER
// ====================================================================
void customerTransfer(Account* acc, const string& tellerID) {
    Utils::printHeader("FUND TRANSFER");
    cout << "  From    : " << acc->getAccountNumber() << "\n";
    cout << "  Balance : R" << fixed << setprecision(2) << acc->getBalance() << "\n\n";
    string destNo;
    cout << "  Destination Account: "; getline(cin, destNo);
    destNo = Utils::toUpper(destNo);
    Account* dest = FileHandler::loadAccountByNumber(destNo);
    if (!dest) { cout << "\n  [!] Destination not found.\n"; Utils::pause(); return; }
    cout << "  Recipient : " << dest->getFirstName() << " " << dest->getSurname() << "\n";
    cout << "  Amount    : R";
    string input; getline(cin, input);
    try {
        double amount = stod(input);
        if (acc->withdraw(amount)) {
            dest->deposit(amount);
            FileHandler::updateAccount(*acc);
            FileHandler::updateAccount(*dest);
            Transaction txOut, txIn;
            txOut.setData(acc->getAccountNumber(), "TRANSFER_OUT", amount, acc->getBalance(),
                tellerID, "Transfer to " + destNo);
            txIn.setData(dest->getAccountNumber(), "TRANSFER_IN", amount, dest->getBalance(),
                tellerID, "Transfer from " + acc->getAccountNumber());
            FileHandler::saveTransaction(txOut);
            FileHandler::saveTransaction(txIn);
            cout << "\n  Transfer successful. New Balance: R"
                << fixed << setprecision(2) << acc->getBalance() << "\n";
        }
    }
    catch (exception& e) {
        cout << "\n  [!] " << e.what() << "\n";
    }
    delete dest;
    Utils::pause();
}

// ====================================================================
//  ACCOUNT STATEMENT
// ====================================================================
void viewStatement(Account* acc) {
    Utils::printHeader("ACCOUNT STATEMENT");
    cout << "  Account : " << acc->getAccountNumber() << "\n";
    cout << "  Name    : " << acc->getFirstName() << " " << acc->getSurname() << "\n";
    cout << "  Balance : R" << fixed << setprecision(2) << acc->getBalance() << "\n\n";
    vector<Transaction> txs;
    FileHandler::loadTransactionsByAccount(acc->getAccountNumber(), txs);
    if (txs.empty()) {
        cout << "  No transactions on record.\n";
    }
    else {
        Utils::printSeparator('-');
        cout << left << setw(20) << "Transaction ID"
            << setw(18) << "Date/Time"
            << setw(14) << "Type"
            << right << setw(12) << "Amount(R)"
            << setw(14) << "Balance(R)" << "\n";
        Utils::printSeparator('-');
        for (auto& tx : txs) tx.display();
        Utils::printSeparator('-');
    }
    Utils::pause();
}

// ====================================================================
//  CHANGE PIN
// ====================================================================
void changeCustomerPIN(Account* acc) {
    Utils::printHeader("CHANGE PIN");
    string current;
    cout << "  Current PIN: "; getline(cin, current);
    if (!acc->verifyPIN(current)) {
        cout << "\n  [!] Incorrect PIN.\n"; Utils::pause(); return;
    }
    string newPIN, confirmPIN;
    do {
        cout << "  New 5-digit PIN   : "; getline(cin, newPIN);
        if (newPIN.size() != 5 || newPIN.find_first_not_of("0123456789") != string::npos) {
            cout << "  [!] Must be exactly 5 digits.\n"; continue;
        }
        cout << "  Confirm new PIN   : "; getline(cin, confirmPIN);
        if (newPIN != confirmPIN) cout << "  [!] PINs do not match.\n";
    } while (newPIN != confirmPIN ||
        newPIN.size() != 5 ||
        newPIN.find_first_not_of("0123456789") != string::npos);
    acc->changePIN(newPIN);
    FileHandler::updateAccount(*acc);
    cout << "\n  PIN changed successfully.\n";
    Utils::pause();
}

// ====================================================================
//  APPLY INTEREST
// ====================================================================
void applyInterestAll() {
    Utils::printHeader("APPLY MONTHLY INTEREST");
    vector<Account*> accounts;
    FileHandler::loadAccounts(accounts);
    int count = 0;
    for (auto acc : accounts) {
        if (acc->isActive() && !acc->isLocked()) {
            double interest = acc->calculateInterest();
            if (interest > 0) {
                acc->applyInterest();
                FileHandler::updateAccount(*acc);
                Transaction tx;
                tx.setData(acc->getAccountNumber(), "INTEREST", interest,
                    acc->getBalance(), "SYSTEM", "Monthly interest");
                FileHandler::saveTransaction(tx);
                count++;
            }
        }
        delete acc;
    }
    cout << "\n  Interest applied to " << count << " account(s).\n";
    Utils::pause();
}

// ====================================================================
//  SEARCH CUSTOMER
// ====================================================================
void searchCustomer() {
    Utils::printHeader("CUSTOMER SEARCH");
    cout << "  [1] Account Number  [2] Surname  [3] ID Number\n  Select: ";
    string choice; getline(cin, choice);
    cout << "  Search term: ";
    string term; getline(cin, term);
    term = Utils::toUpper(term);
    vector<Account*> all;
    FileHandler::loadAccounts(all);
    bool found = false;
    cout << "\n";
    Utils::printSeparator('-');
    cout << left << setw(22) << "Account No"
        << setw(25) << "Name"
        << setw(10) << "Type"
        << setw(8) << "Branch"
        << right << setw(14) << "Balance(R)" << "\n";
    Utils::printSeparator('-');
    for (auto acc : all) {
        bool match = false;
        if (choice == "1") match = Utils::toUpper(acc->getAccountNumber()).find(term) != string::npos;
        else if (choice == "2") match = Utils::toUpper(acc->getSurname()).find(term) != string::npos;
        else if (choice == "3") match = acc->getIDNumber().find(term) != string::npos;
        if (match) { acc->displaySummary(); found = true; }
        delete acc;
    }
    if (!found) cout << "  No matching records.\n";
    Utils::printSeparator('-');
    Utils::pause();
}

// ====================================================================
//  VIEW ALL BRANCHES
// ====================================================================
void viewAllBranches() {
    Utils::printHeader("BRANCH NETWORK");
    vector<Branch> branches;
    FileHandler::loadBranches(branches);
    if (branches.empty()) { cout << "  No branches found.\n"; Utils::pause(); return; }
    for (auto& b : branches) b.display();
    cout << "\n  INTER-BRANCH COMPARISON\n";
    Utils::printSeparator('-');
    cout << left << setw(10) << "Code" << setw(30) << "Name" << setw(15) << "City"
        << "Accounts  Deposits\n";
    Utils::printSeparator('-');
    Branch* top = &branches[0];
    for (auto& b : branches) {
        b.displaySummary();
        if (b.totalDeposits > top->totalDeposits) top = &b;
    }
    Utils::printSeparator('-');
    cout << "\n  Top performing: " << top->branchName
        << " (R" << fixed << setprecision(2) << top->totalDeposits << ")\n";
    Utils::pause();
}

// ====================================================================
//  BRANCH REPORT  (Teller)
// ====================================================================
void generateBranchReport(const Teller& teller) {
    Utils::printHeader("BRANCH REPORT: " + teller.getBranchCode());
    vector<Account*> all;
    FileHandler::loadAccounts(all);
    double total = 0; int count = 0;
    cout << left << setw(22) << "Account No"
        << setw(25) << "Name"
        << setw(10) << "Type"
        << right << setw(14) << "Balance(R)" << "\n";
    Utils::printSeparator('-');
    for (auto acc : all) {
        if (acc->getBranchCode() == teller.getBranchCode() && acc->isActive()) {
            acc->displaySummary();
            total += acc->getBalance();
            count++;
        }
        delete acc;
    }
    Utils::printSeparator('-');
    cout << "\n  Accounts: " << count
        << "   Total: R" << fixed << setprecision(2) << total << "\n";
    Utils::pause();
}

// ====================================================================
//  MANAGEMENT REPORTS
// ====================================================================
void managementReports() {
    Utils::printHeader("MANAGEMENT REPORTS");
    cout << "  [1] Daily Transaction Report\n"
        << "  [2] Customer Account Summary\n"
        << "  [3] Branch Performance\n"
        << "  [4] Export Accounts to CSV\n"
        << "  [5] Export Transactions to CSV\n"
        << "  [6] Backup All Data\n"
        << "  [7] Apply Monthly Interest\n"
        << "  [8] Back\n\n  Select: ";
    string choice; getline(cin, choice);

    if (choice == "1") {
        Utils::printHeader("DAILY TRANSACTION REPORT");
        vector<Transaction> txs;
        FileHandler::loadTransactions(txs);
        string today = Utils::currentDate();
        cout << left << setw(20) << "TX ID" << setw(22) << "Account"
            << setw(14) << "Type"
            << right << setw(12) << "Amount(R)" << setw(14) << "Balance(R)" << "\n";
        Utils::printSeparator('-');
        double dailyTotal = 0; int txCount = 0;
        for (auto& tx : txs) {
            if (string(tx.date).substr(0, 10) == today) {
                tx.display(); dailyTotal += tx.amount; txCount++;
            }
        }
        Utils::printSeparator('-');
        cout << "\n  Transactions: " << txCount
            << "  |  Total: R" << fixed << setprecision(2) << dailyTotal << "\n";
        Utils::pause();
    }
    else if (choice == "2") {
        Utils::printHeader("CUSTOMER ACCOUNT SUMMARY");
        vector<Account*> all;
        FileHandler::loadAccounts(all);
        cout << left << setw(22) << "Account No"
            << setw(25) << "Name" << setw(10) << "Type"
            << setw(8) << "Branch"
            << right << setw(14) << "Balance(R)" << "\n";
        Utils::printSeparator('-');
        double grand = 0;
        for (auto acc : all) { acc->displaySummary(); grand += acc->getBalance(); delete acc; }
        Utils::printSeparator('-');
        cout << "\n  Total held: R" << fixed << setprecision(2) << grand << "\n";
        Utils::pause();
    }
    else if (choice == "3") { viewAllBranches(); return; }
    else if (choice == "4") { FileHandler::exportAccountsToCSV("accounts_export.csv");     Utils::pause(); }
    else if (choice == "5") { FileHandler::exportTransactionsToCSV("transactions_export.csv"); Utils::pause(); }
    else if (choice == "6") { FileHandler::backupData(); Utils::pause(); }
    else if (choice == "7") { applyInterestAll(); return; }
    else if (choice == "8") return;
}

// ====================================================================
//  TELLER MENU
// ====================================================================
void tellerMenu(Teller& teller) {
    string choice;
    do {
        Utils::printHeader("TELLER MENU - " + teller.getFullName() + " [" + teller.getBranchCode() + "]");
        if (teller.getTellerID() == "T001") {
            cout << " [1] Register New Teller\n"
                << " [2] Register New Customer\n"
                << " [3] View Customer Details\n"
                << " [4] Process Transaction\n"
                << " [5] Generate Branch Report\n"
                << " [6] View All Branches\n"
                << " [7] Management Reports\n"
                << " [8] Search Customer\n"
                << " [9] Logout\n\n Select: ";
        }
        else {
            cout << " [1] Register New Customer\n"
                << " [2] View Customer Details\n"
                << " [3] Process Transaction\n"
                << " [4] Generate Branch Report\n"
                << " [5] View All Branches\n"
                << " [6] Management Reports\n"
                << " [7] Search Customer\n"
                << " [8] Logout\n\n Select: ";
        }

        getline(cin, choice);

        if (teller.getTellerID() == "T001") {
            // ===== This logic is for "ADMIN TELLER" - T001=====
            if (choice == "1") registerTeller(teller);
            else if (choice == "2") registerCustomer(teller);
            else if (choice == "3") viewCustomerDetails(teller);
            else if (choice == "4") processTellerTransaction(teller);
            else if (choice == "5") generateBranchReport(teller);
            else if (choice == "6") viewAllBranches();
            else if (choice == "7") managementReports();
            else if (choice == "8") searchCustomer();
            else if (choice == "9") {
                cout << "\n  Goodbye, " << teller.getFullName() << "!\n";
                break;
            }
            else {
                cout << "\n  [!] Invalid option.\n";
            }
        }
        else {
            // ===== This logic is for "NORMAL TELLER" =====
            if (choice == "1") registerCustomer(teller);
            else if (choice == "2") viewCustomerDetails(teller);
            else if (choice == "3") processTellerTransaction(teller);
            else if (choice == "4") generateBranchReport(teller);
            else if (choice == "5") viewAllBranches();
            else if (choice == "6") managementReports();
            else if (choice == "7") searchCustomer();
            else if (choice == "8") {
                cout << "\n  Goodbye, " << teller.getFullName() << "!\n";
                break;
            }
            else {
                cout << "\n  [!] Invalid option.\n";
            }
        }
    } while (choice != "9");
}

// ====================================================================
//  CUSTOMER MENU
// ====================================================================
void customerMenu(Account* acc) {
    string choice;
    do {
        string header = "CUSTOMER MENU - "
            + acc->getFirstName() + " "
            + acc->getSurname();
        Utils::printHeader(header);
        cout << "  Account : " << acc->getAccountNumber()
            << "  |  Balance: R" << fixed << setprecision(2) << acc->getBalance() << "\n\n";
        cout << "  [1] View Balance\n"
            << "  [2] Deposit\n"
            << "  [3] Withdraw\n"
            << "  [4] Transfer Funds\n"
            << "  [5] View Statement\n"
            << "  [6] Change PIN\n"
            << "  [7] Logout\n\n  Select: ";
        getline(cin, choice);
        // Reload latest state before each operation
        Account* fresh = FileHandler::loadAccountByNumber(acc->getAccountNumber());
        if (fresh) { delete acc; acc = fresh; }
        if (choice == "1") viewBalance(acc);
        else if (choice == "2") customerDeposit(acc);
        else if (choice == "3") customerWithdraw(acc);
        else if (choice == "4") customerTransfer(acc);
        else if (choice == "5") viewStatement(acc);
        else if (choice == "6") changeCustomerPIN(acc);
        else if (choice == "7") { cout << "\n  Goodbye, " << acc->getFirstName() << "!\n"; break; }
        else cout << "\n  [!] Invalid option.\n";
    } while (choice != "7");
    delete acc;
}

// ====================================================================
//  SEED DEFAULT DATA (first run only)
// ====================================================================
void seedDefaultData() {
    ifstream check("branches.dat", ios::binary);
    if (check.good() && check.peek() != EOF) { check.close(); return; }
    check.close();
    cout << "\n  [SETUP] Initialising default system data...\n";

    Branch branches[3];
    memset(branches, 0, sizeof(branches));

    strncpy(branches[0].branchCode, "JHB", 9);
    strncpy(branches[0].branchName, "Johannesburg Main", 59);
    strncpy(branches[0].city, "Johannesburg", 39);
    strncpy(branches[0].address, "1 Commissioner St, JHB CBD", 99);
    strncpy(branches[0].phone, "0112345678", 14);
    branches[0].tellerCount = 1; branches[0].accountCount = 0; branches[0].totalDeposits = 0;

    strncpy(branches[1].branchCode, "CPT", 9);
    strncpy(branches[1].branchName, "Cape Town Branch", 59);
    strncpy(branches[1].city, "Cape Town", 39);
    strncpy(branches[1].address, "10 Adderley St, Cape Town", 99);
    strncpy(branches[1].phone, "0219876543", 14);
    branches[1].tellerCount = 1; branches[1].accountCount = 0; branches[1].totalDeposits = 0;

    strncpy(branches[2].branchCode, "DUR", 9);
    strncpy(branches[2].branchName, "Durban Branch", 59);
    strncpy(branches[2].city, "Durban", 39);
    strncpy(branches[2].address, "22 Smith St, Durban CBD", 99);
    strncpy(branches[2].phone, "0315556789", 14);
    branches[2].tellerCount = 1; branches[2].accountCount = 0; branches[2].totalDeposits = 0;

    for (auto& b : branches) FileHandler::saveBranch(b);

    Teller t1, t2, t3;
    t1.setData("T001", "Nickson Furidzirai", "Pass@1234", "JHB");
    t2.setData("T002", "Dyllan Mugochi", "Pass@5678", "CPT");
    t3.setData("T003", "Alison Alisson", "Pass@9012", "DUR");
    FileHandler::saveTeller(t1);
    FileHandler::saveTeller(t2);
    FileHandler::saveTeller(t3);

    ofstream cfg("system_config.txt");
    cfg << "SYSTEM_NAME=Standard Bank Console Banking System\n";
    cfg << "VERSION=2.0\n";
    cfg << "MAX_LOGIN_ATTEMPTS=3\n";
    cfg << "CURRENCY=ZAR\n";
    cfg << "SETUP_DATE=" << Utils::currentDate() << "\n";
    cfg.close();

    cout << "  [SETUP] Default data created.\n\n";
    cout << "  DEFAULT TELLER CREDENTIALS:\n";
    cout << "  T001 / Pass@1234  (Johannesburg)\n";
    cout << "  T002 / Pass@5678  (Cape Town)\n";
    cout << "  T003 / Pass@9012  (Durban)\n\n";
}

// ====================================================================
//  MAIN
// ====================================================================
int main() {
    seedDefaultData();

    int choice;
    do {
        Utils::clearScreen();
        Utils::printSeparator('=');
        Utils::printSeparator('-');
        cout << "    MULTI-BRANCH BANKING MANAGEMENT SYSTEM\n";
        cout << "    Standard Bank - Console Prototype v2.1\n";
        Utils::printSeparator('-');
        Utils::printSeparator('=');
        cout << "\n";
        cout << "  [1] Teller Login\n"
            << "  [2] Customer Login\n"
            << "  [3] View Branch Network\n"
            << "  [4] Exit\n\n"
            << "  Select: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
        case 1: {
            Teller teller;
            if (authenticateTeller(teller))
                tellerMenu(teller);
            break;
        }
        case 2: {
            Account* acc = nullptr;
            if (authenticateCustomer(acc))
                customerMenu(acc);
            break;
        }
        case 3: {
            viewAllBranches();
            break;
        }
        case 4: {
            Utils::clearScreen();
            cout << "\n  Thank you for choosing Standard Bank.\n"
                << "  Goodbye!\n\n";
            break;
        }
        default: {
            cout << "\n  [!] Invalid option.\n";
            Utils::pause();
            break;
        }
        }

    } while (choice != 4);

    return 0;
}


