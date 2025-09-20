# Bank Account Management System

## Overview
This project is a simple **Bank Account Management System** implemented in C++ using Object-Oriented Programming (OOP) principles.  
It demonstrates:
- Classes & Objects
- Inheritance (`SavingsAccount` extends `Account`)
- Operator Overloading (`+=`, `==`)
- Encapsulation & Polymorphism

---

## Features
- Create customers and accounts
- Deposit, withdraw, transfer money
- Apply interest for savings accounts
- Prevent overdrafts
- Track transaction history
- Operator overloading for intuitive operations

---

## File Structure
```
YourLastName_OOP_Bank_Assignment/
│── src/
│   ├── Account.h / Account.cpp
│   ├── SavingsAccount.h / SavingsAccount.cpp
│   ├── Customer.h / Customer.cpp
│   ├── Transaction.h / Transaction.cpp
│   └── main.cpp
│
│── UML/
│   ├── ClassDiagram.png
│   └── SequenceDiagram.png
│
│── Report.docx (or Report.pdf)
│── README.md
```

---

## How to Compile & Run
```bash
g++ -std=c++11 main.cpp Account.cpp SavingsAccount.cpp Customer.cpp Transaction.cpp -o bank
./bank
```

---

## Sample Output
```
Customer: John Doe (ID: C001)
Account ACC1001 | Balance: 500
Deposit of 100 successful.
Withdrawal of 50 successful.
Savings Account | Interest applied: 2.5%
```

---

## Documentation
See **Report.docx** (or converted PDF) for:
- OOA analysis
- UML diagrams
- Explanation of design choices
- Test results
- LLM usage

---

## LLM Usage
This project used ChatGPT for:
- Brainstorming operator overloading options
- Refining explanations for documentation
- Debugging conceptual issues

All code and design were implemented by me.
