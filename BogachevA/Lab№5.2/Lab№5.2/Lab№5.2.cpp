#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
enum startingCapital {
    lessThan100, from_100_To500, from_500_To1m, over1m
};
double interestRate[5][4] = {
  {4.5, 5.0, 5.5, 6.0},
  {5.0, 6.0, 6.5, 7.0},
  {6.0, 6.5, 6.6, 7.5},
  {6.5, 7.0, 7.5, 8.0},
  {7.0, 7.5, 8.0, 8.5}
};
double getRate(int balance, int term)
{
    startingCapital range;
    if (balance < 100000)
    {
        range = lessThan100;
    }
    else if (balance < 500000)
    {
        range = from_100_To500;
    }
    else if (balance < 1000000)
    {
        range = from_500_To1m;
    }
    else
    {
        range = over1m;
    }
    int i;
    switch (term)
    {
    case 3:
        i = 0;
        break;
    case 6:
        i = 1;
        break;
    case 12:
        i = 2;
        break;
    case 24:
        i = 3;
        break;
    case 36:
        i = 4;
        break;
    default:
        throw std::runtime_error("Invalid deposit term");
    }
    return interestRate[i][range];
}
class Deposit
{
private:
    int initialCapital;
    double rate;
    int currentIncome;
    int term;
    int monthPassed;
    bool active;
public:
    Deposit(int cap, int term_, double rate_)
        : initialCapital(cap), term(term_), rate(rate_),
        monthPassed(0), currentIncome(0), active(true)
    {
        if (cap <= 0)
        {
            throw std::runtime_error("Deposit amount must be positive");
        }
        if (term_ != 3 && term_ != 6 && term_ != 12 && term_ != 24 && term_ != 36)
        {
            throw std::runtime_error("Invalid deposit term in constructor");
        }
        if (rate_ <= 0)
        {
            throw std::runtime_error("Interest rate must be positive");
        }
    }
    void Passed()
    {
        if (!active)
        {
            throw std::runtime_error("Deposit is closed");
        }
        if (monthPassed == term)
        {
            throw std::runtime_error("Deposit has already expired");
        }
        double earnings = initialCapital * rate / 12.0 / 100.0;
        ++monthPassed;
        currentIncome += static_cast<int>(earnings);
    }
    int getIncome() const
    {
        return currentIncome;
    }
    int getTerm() const
    {
        return term;
    }
    int getPassed() const
    {
        return monthPassed;
    }
    int getInitialCapital() const
    {
        return initialCapital;
    }
    int withdraw()
    {
        if (!active)
        {
            throw std::runtime_error("Deposit is closed");
        }
        int temp = currentIncome;
        currentIncome = 0;
        return temp;
    }
    int close()
    {
        if (!active)
        {
            throw std::runtime_error("Deposit is already closed");
        }
        if (monthPassed < term)
        {
            throw std::runtime_error("Cannot close deposit: term not yet expired");
        }
        int currentTotal = currentIncome + initialCapital;
        currentIncome = 0;
        initialCapital = 0;
        active = false;
        return currentTotal;
    }
    bool isActive() const
    {
        return active;
    }
};
struct Client
{
    string number;
    string fullname;
    string password;
    int salaryBalance;
    Deposit* deposit;
    Client(const string& num, const string& name, const string& pwd, int balance, Deposit* dep)
        : number(num), fullname(name), password(pwd), salaryBalance(balance), deposit(dep)
    {
        if (num.length() != 4 || num < "0001" || num > "9999")
        {
            throw std::runtime_error("Invalid account number");
        }
        if (pwd.size() <= 3)
        {
            throw std::runtime_error("Password must be longer than 3 characters");
        }
        if (balance < 0)
        {
            throw std::runtime_error("Salary balance cannot be negative");
        }
    }
};
class ProcessingCenter
{
private:
    vector<Client> clients;
    Client* findClient(const string& acc)
    {
        for (Client& client : clients)
        {
            if (client.number == acc)
            {
                return &client;
            }
        }
        return nullptr;
    }
public:
    void addClient(const string& number, const string& name, const string& password, int balance)
    {
        try
        {
            Client client(number, name, password, balance, nullptr);
            if (findClient(number) != nullptr)
            {
                throw std::runtime_error("Client with such account already exists");
            }
            clients.push_back(client);
        }
        catch (const std::runtime_error& e)
        {
            cout << "Error adding client: " << e.what() << endl;
        }
    }
    Client* authorize(const string& number_, const string& password_)
    {
        Client* client = findClient(number_);
        if (!client || client->password != password_)
        {
            throw std::runtime_error("Authorization failed: incorrect account number or password");
        }
        return client;
    }
    bool hasActiveDeposit(Client& client)
    {
        return client.deposit != nullptr && client.deposit->isActive();
    }
    void showAvailableDeposits(Client& client)
    {
        cout << "Available deposits for " << client.salaryBalance << " rubles" << endl;
        int terms[] = { 3, 6, 12, 24, 36 };
        for (int i = 0; i < 5; ++i)
        {
            double rate = getRate(client.salaryBalance, terms[i]);
            cout << "  " << terms[i] << " months " << rate << "% per year" << endl;
        }
    }
    void openDeposit(Client& client)
    {
        if (hasActiveDeposit(client))
        {
            throw std::runtime_error("Your deposit is already opened");
        }
        int amount;
        cout << "Enter the amount of the transfer to the deposit up to " << client.salaryBalance << " rubles: ";
        if (!(cin >> amount))
        {
            throw std::runtime_error("Invalid input for amount");
        }
        if (amount <= 0 || amount > client.salaryBalance)
        {
            throw std::runtime_error("Insufficient funds or incorrect amount");
        }
        int term;
        cout << "Enter the term (3, 6, 12, 24, 36 months): ";
        if (!(cin >> term))
        {
            throw std::runtime_error("Invalid input for term");
        }
        double rate = getRate(amount, term);
        client.salaryBalance -= amount;
        client.deposit = new Deposit(amount, term, rate);
        cout << "Deposit has been successfully opened" << endl;
    }
    void showDepositState(Client& client)
    {
        if (!hasActiveDeposit(client))
        {
            throw std::runtime_error("No active deposit");
        }
        Deposit* d = client.deposit;
        cout << "Deposit state:" << endl;
        cout << "  Initial capital: " << d->getInitialCapital() << " rubles" << endl;
        cout << "  Accrued interest: " << d->getIncome() << " rubles" << endl;
        cout << "  Months passed: " << d->getPassed() << " of " << d->getTerm() << endl;
        cout << "  Matured: " << (d->getPassed() >= d->getTerm() ? "yes" : "no") << endl;
    }
    void addMonthToDeposit(Client& client)
    {
        if (!hasActiveDeposit(client))
        {
            throw std::runtime_error("No active deposit");
        }
        client.deposit->Passed();
        cout << "One month added, interest accrued." << endl;
    }
    void withdrawInterest(Client& client)
    {
        if (!hasActiveDeposit(client))
        {
            throw std::runtime_error("No active deposit");
        }
        int interest = client.deposit->withdraw();
        if (interest > 0)
        {
            client.salaryBalance += interest;
            cout << "Withdrawn " << interest << " rubles of interest." << endl;
        }
        else
        {
            cout << "No interest available to withdraw." << endl;
        }
    }
    void closeDeposit(Client& client)
    {
        if (!hasActiveDeposit(client))
        {
            throw std::runtime_error("No active deposit");
        }
        int total = client.deposit->close();
        client.salaryBalance += total;
        delete client.deposit;
        client.deposit = nullptr;
        cout << "Deposit closed. " << total << " rubles transferred to salary account." << endl;
    }
    ~ProcessingCenter()
    {
        for (Client& client : clients)
        {
            delete client.deposit;
        }
    }
};
int main()
{
    ProcessingCenter pc;
    pc.addClient("1111", "Bespalov Roman", "qwerty", 750000);
    pc.addClient("2222", "Birukov Sergey", "zxc322", 45000);
    pc.addClient("3333", "Bogachev Artem", "1234", 1200000);
    pc.addClient("4444", "Garevskaya Yaroslava", "Sdnemdnya", 1500000);

    string acc, pwd;
    cout << "Enter account number 4 digits from 0001 to 9999: ";
    cin >> acc;
    cout << "Enter your password: ";
    cin >> pwd;
    Client* user = nullptr;
    try
    {
        user = pc.authorize(acc, pwd);
        cout << "Welcome, " << user->fullname << "!" << endl;
        cout << "Your current salary balance: " << user->salaryBalance << " rubles." << endl;
    }
    catch (const std::runtime_error& e)
    {
        cout << "Authorization failed: " << e.what() << endl;
        return 1;
    }
    int choice;
    do
    {
        cout << "Menu" << endl;
        cout << "1. Show available deposits" << endl;
        cout << "2. Open a new deposit" << endl;
        cout << "3. Show current deposit state" << endl;
        cout << "4. Add one month (accrue interest)" << endl;
        cout << "5. Withdraw accrued interest" << endl;
        cout << "6. Close deposit (if matured)" << endl;
        cout << "0. Exit" << endl;
        cout << "Your choice: ";
        cin >> choice;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }
        try
        {
            switch (choice)
            {
            case 1:
                pc.showAvailableDeposits(*user);
                break;
            case 2:
                pc.openDeposit(*user);
                break;
            case 3:
                pc.showDepositState(*user);
                break;
            case 4:
                pc.addMonthToDeposit(*user);
                break;
            case 5:
                pc.withdrawInterest(*user);
                break;
            case 6:
                pc.closeDeposit(*user);
                break;
            case 0:
                cout << "Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        }
        catch (const std::runtime_error& e)
        {
            cout << "Error: " << e.what() << endl;
        }
    } while (choice != 0);
    return 0;
}