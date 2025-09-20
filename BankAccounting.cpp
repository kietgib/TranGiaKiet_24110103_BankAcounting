#include <bits/stdc++.h>
using namespace std;

//khai báo enum để phân loại giao dịch: gửi tiền, rút tiền, chuyển vào, chuyển ra
class Transaction {
public:
    enum Type { DEPOSIT, WITHDRAWAL, TRANSFER_IN, TRANSFER_OUT };

    double amount; //số tiền giao dịch
    Type type; //loại giao dịch
    string date; // ngày giờ
    string note; //ghi chú

    Transaction(double amount = 0.0, Type type = DEPOSIT, const string &note = "")
        : amount(amount), type(type), note(note) {
        // set date as current local date-time string
        time_t now = time(nullptr);
        char buf[64];
        //tự động lấy ngày giờ hiện tại 
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        date = string(buf);
    }

    //trả về mô tả loại giao dịch
    string typeStr() const {
        switch (type) {
            case DEPOSIT: return "Deposit";
            case WITHDRAWAL: return "Withdrawal";
            case TRANSFER_IN: return "Transfer In";
            case TRANSFER_OUT: return "Transfer Out";
            default: return "Unknown";
        }
    }
};
//------------------------------------------------------------------------------
// Base Account class
// Tải khoản cơ bản base class...
class Account {
protected:
    string accountNumber;
    double balance;
    string ownerName;
    vector<Transaction> history;

public:
    //constructor
    //gán chuỗi rỗng cho accountnumber, gán giá trị 0.0 cho balance, 
    Account() : accountNumber(""), balance(0.0), ownerName("") {}

    Account(const string &accNo, const string &owner, double initial = 0.0)
        : accountNumber(accNo), balance(initial), ownerName(owner) {}
    //giải phóng bộ nhớ
    //cho phép xóa đối tượng qua con trỏ Account* mà vẫn đảm bảo cho destructor của class
    //con (SavingsAccount) được gọi 
    //Destructor
    //Nếu một class được dùng làm base class, destructor nên được khai báo virtual.
    //Để khi xóa object qua con trỏ base, destructor của class derived vẫn được gọi.


    virtual ~Account() {}

    // Encapsulation: getters
    string getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    string getOwnerName() const { return ownerName; }

    // Deposit (non-virtual)
    virtual bool deposit(double amount) {
        if (amount <= 0) {
            cout << "[Error] Deposit amount must be positive.\n";
            return false;
        }
        balance += amount;
        //nó lưu lịch sử giao dịch
        //Mỗi lần nạp tiền, rút tiền, chuyển khoản... ta sẽ tạo thêm một Transaction và lưu vào history.
        //emplace_back là hàm của std::vector.

        //Nó tạo trực tiếp một phần tử mới ở cuối vector bằng cách gọi constructor của Transaction.

        //Khác với push_back, emplace_back tránh tạo bản tạm (temporary object), nên hiệu quả hơn.
        history.emplace_back(amount, Transaction::DEPOSIT, "Deposit via deposit()");
        return true;
    }

    // Withdraw virtual so derived classes can override
    virtual bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "[Error] Withdrawal amount must be positive.\n";
            return false;
        }
        if (amount > balance) {
            cout << "[Error] Insufficient funds. Attempted to withdraw " << amount
                 << " but balance is " << balance << ".\n";
            return false;
        }
        balance -= amount;
        history.emplace_back(amount, Transaction::WITHDRAWAL, "Withdrawal via withdraw()");
        return true;
    }

    // Apply a transaction to the account using operator+=

    Account &operator+=(const Transaction &t) {
        switch (t.type) {
            case Transaction::DEPOSIT:
                balance += t.amount;
                break;
            case Transaction::WITHDRAWAL:
                // Prevent overdraft via operator+= as well
                if (t.amount > balance) {
                    cout << "[Error] operator+=: insufficient funds for withdrawal of " << t.amount << "\n";
                    // do not modify history in this case
                    return *this;
                }
                balance -= t.amount;
                break;
            case Transaction::TRANSFER_IN:
                balance += t.amount;
                break;
            case Transaction::TRANSFER_OUT:
                if (t.amount > balance) {
                    cout << "[Error] operator+=: insufficient funds for transfer out of " << t.amount << "\n";
                    return *this;
                }
                balance -= t.amount;
                break;
            default:
                break;
        }
        history.push_back(t);// thêm giao dịch vào lịch sử giao dịch
        return *this; //Trả về tham chiếu đến tài khoản hiện tại (this) để có thể chaining.
    }

    // Compare accounts by balance (could be by account number instead)
    bool operator==(const Account &other) const {
        return this->balance == other.balance;
    }

    // Display account details
    virtual void displayInfo() const {
        cout << "Account Number: " << accountNumber << " | Owner: " << ownerName
             << " | Balance: " << fixed << setprecision(2) << balance << "\n";
    }

    // Show transaction history
    void printHistory() const {
        cout << "Transaction history for account " << accountNumber << ":\n";
        if (history.empty()) { cout << "  (no transactions)\n"; return; }
        for (const auto &t : history) {
            cout << "  [" << t.date << "] " << setw(12) << left << t.typeStr()
                 << "  " << right << setw(8) << fixed << setprecision(2) << t.amount
                 << "  -- " << t.note << "\n";
        }
    }
};

// SavingsAccount derived from Account
//tài khoản tiết kiệm
class SavingsAccount : public Account {
private:
    double interestRate; // e.g., 0.03 for 3%, lãi suất
    int withdrawalLimit; // number of free withdrawals per month - simplified, số lần rút miễn phí
    int withdrawalsThisPeriod;//số lần đã rút
    double withdrawalFee; // fee when crossing free limit, phí khi qua vượt qua số lần 

public:
    SavingsAccount() : Account(), interestRate(0.0), withdrawalLimit(3), withdrawalsThisPeriod(0), withdrawalFee(2.0) {}

    SavingsAccount(const string &accNo, const string &owner, double initial, double rate)
        : Account(accNo, owner, initial), interestRate(rate), withdrawalLimit(3), withdrawalsThisPeriod(0), withdrawalFee(2.0) {}

    virtual ~SavingsAccount() {}

    void setInterestRate(double r) { interestRate = r; }
    double getInterestRate() const { return interestRate; }

    // Apply interest to balance
    // cộng lãi suất vào số dư 
    void applyInterest() {
        if (interestRate <= 0) return;
        double interest = balance * interestRate;
        balance += interest;
        history.emplace_back(interest, Transaction::DEPOSIT, "Interest applied");
        cout << "[Info] Interest of " << interest << " applied to " << accountNumber << "\n";
    }

    // Override withdraw to enforce limits and fees
    //nếu rút ra nhiều hơn thì tính thêm phí
    virtual bool withdraw(double amount) override {
        if (amount <= 0) {
            cout << "[Error] Withdrawal amount must be positive.\n";
            return false;
        }
        double fee = 0.0;
        if (withdrawalsThisPeriod >= withdrawalLimit) fee = withdrawalFee;
        double total = amount + fee;
        if (total > balance) {
            cout << "[Error] Insufficient funds (including fee). Need " << total << ", have " << balance << "\n";
            return false;
        }
        balance -= total;
        withdrawalsThisPeriod++;
        history.emplace_back(amount, Transaction::WITHDRAWAL, string("Withdrawal (fee: ") + to_string(fee) + ")");
        if (fee > 0) history.emplace_back(fee, Transaction::WITHDRAWAL, "Withdrawal fee");
        return true;
    }

    // Reset period counters (simplified: call manually in tests)
    void resetPeriod() { withdrawalsThisPeriod = 0; }

    void displayInfo() const override {
        cout << "Savings ";
        Account::displayInfo();
        cout << "   Interest rate: " << interestRate << " | Withdrawals this period: " << withdrawalsThisPeriod << "\n";
    }
};

// Customer class - manages multiple accounts
class Customer {
private:
    string name;
    string id;
    vector<shared_ptr<Account>> accounts; // store polymorphically

public:
    Customer() {}
    Customer(const string &name, const string &id) : name(name), id(id) {}

    string getName() const { return name; }
    string getId() const { return id; }

    // Open an account (polymorphic behavior supported by shared_ptr)
    void addAccount(shared_ptr<Account> acc) {
        accounts.push_back(acc);
    }

    // Find account by account number
    shared_ptr<Account> findAccount(const string &accNo) const {
        for (auto &a : accounts) if (a->getAccountNumber() == accNo) return a;
        return nullptr;
    }

    double getTotalBalance() const {
        double total = 0.0;
        for (auto &a : accounts) total += a->getBalance();
        return total;
    }

    void displayPortfolio() const {
        cout << "Customer: " << name << " (ID: " << id << ")\n";
        cout << "Accounts:\n";
        for (auto &a : accounts) a->displayInfo();
        cout << "Total balance across accounts: " << fixed << setprecision(2) << getTotalBalance() << "\n";
    }
};

// Utility: transfer funds between accounts (basic)
bool transfer(shared_ptr<Account> from, shared_ptr<Account> to, double amount) {
    if (!from || !to) { cout << "[Error] Null account provided to transfer().\n"; return false; }
    if (amount <= 0) { cout << "[Error] Transfer amount must be positive.\n"; return false; }
    // Attempt withdrawal from 'from'
    // We'll use withdraw() then deposit() to ensure derived behavior is enforced
    if (!from->withdraw(amount)) {
        cout << "[Error] Transfer failed: withdrawal from source account failed.\n";
        return false;
    }
    // deposit into 'to'
    to->deposit(amount);
    // add explicit transfer history entries
    Transaction outT(amount, Transaction::TRANSFER_OUT, string("Transfer to ") + to->getAccountNumber());
    Transaction inT(amount, Transaction::TRANSFER_IN, string("Transfer from ") + from->getAccountNumber());
    // push to histories using operator+=
    *from += outT;
    *to += inT;
    cout << "[Info] Transferred " << amount << " from " << from->getAccountNumber() << " to " << to->getAccountNumber() << "\n";
    return true;
}

// ------------------ Tests in main() ------------------
int main() {
    cout << "=== Bank Account Management System Demo ===\n\n";

    // Create a customer
    Customer c1("Nguyen Van A", "CUST001");

    // Create accounts
    auto acc1 = make_shared<Account>("ACC1001", "Nguyen Van A", 500.00);
    auto sav1 = make_shared<SavingsAccount>("SAV2001", "Nguyen Van A", 1000.00, 0.02); // 2% interest

    c1.addAccount(acc1);
    c1.addAccount(sav1);

    // Display initial state
    c1.displayPortfolio();
    cout << "\n";

    // Deposit using deposit()
    cout << "[Test] Deposit 200 into ACC1001\n";
    acc1->deposit(200.0);
    acc1->displayInfo();

    // Withdraw tests
    cout << "[Test] Withdraw 100 from ACC1001\n";
    acc1->withdraw(100.0);
    acc1->displayInfo();

    cout << "[Test] Withdraw 3000 from ACC1001 (should fail)\n";
    acc1->withdraw(3000.0);

    // Use operator+= to add a Transaction (deposit)
    cout << "[Test] Add Transaction (deposit 150) to ACC1001 using operator+=\n";
    Transaction t1(150.0, Transaction::DEPOSIT, "Manual deposit via operator+=");
    *acc1 += t1;
    acc1->displayInfo();

    // Savings account withdraw with limits and fees
    cout << "[Test] Perform 4 withdrawals on savings (3 free, 4th charged)\n";
    sav1->withdraw(50.0);
    sav1->withdraw(50.0);
    sav1->withdraw(50.0);
    sav1->withdraw(50.0); // this one should charge a fee
    sav1->displayInfo();

    // Apply interest to savings
    cout << "[Test] Apply interest to savings account\n";
    sav1->applyInterest();
    sav1->displayInfo();

    // Transfer from savings to checking
    cout << "[Test] Transfer 100 from SAV2001 to ACC1001\n";
    transfer(sav1, acc1, 100.0);

    // Compare accounts by balance using operator==
    cout << "[Test] Compare ACC1001 and SAV2001 balances using operator==\n";
    if (*acc1 == *sav1) cout << "Accounts have equal balances.\n"; else cout << "Balances differ.\n";

    // Print histories
    cout << "\nHistories:\n";
    acc1->printHistory();
    cout << "\n";
    sav1->printHistory();

    // Customer portfolio after transactions
    cout << "\nFinal portfolio:\n";
    c1.displayPortfolio();

    // Extra: show getTotalBalance()
    cout << "\n[Info] Total balance for customer " << c1.getName() << ": " << c1.getTotalBalance() << "\n";

    cout << "\n=== End of Demo ===\n";
    return 0;
}
