# 🍽️ SpartaFeast Cafeteria Ordering System

A console-based cafeteria ordering system built in **C++** as part of my CS101 course project. This program lets students browse cafeteria shops and place food orders, while staff can manage shops, menus, and view revenue reports.

---

## 📋 Features

**For Customers (Students):**
- Create an account or log in
- View all available cafeteria shops and their menus
- Place orders from any shop
- View personal order history with total amount spent

**For Staff:**
- Add and remove cafeteria shops
- Add and remove menu items with prices
- View all orders placed by all customers
- Generate revenue reports per shop

---

## 🛠️ How to Compile and Run

Make sure you have a C++ compiler installed (like g++ or MinGW on Windows).

**On Windows (using g++):**
```bash
g++ main.cpp -o spartafeast
spartafeast.exe
```

**On Linux/Mac:**
```bash
g++ main.cpp -o spartafeast
./spartafeast
```

---

## 🔑 Default Accounts

You can use these test accounts to try the program:

| Username  | Password  | Role     |
|-----------|-----------|----------|
| student1  | pass123   | Customer |
| student2  | pass456   | Customer |
| staff1    | staff123  | Staff    |
| staff2    | staff456  | Staff    |

You can also register your own account when you run the program.

---

## 💾 Data Storage

The program saves data to simple `.txt` files in the same folder:
- `shops.txt` — stores all shop and menu data
- `orders.txt` — stores all placed orders
- `users.txt` — stores registered user accounts

These files are created automatically when you run the program.

---

## 📁 Project Structure

```
SpartaFeast/
│
├── main.cpp        ← main source code (everything is here)
├── shops.txt       ← auto-generated when shops are added
├── orders.txt      ← auto-generated when orders are placed
├── users.txt       ← auto-generated when accounts are created
└── README.md       ← you are here
```

---

## 🧠 What I Learned

This project helped me practice:
- C++ structs and vectors
- File input/output (reading and writing `.txt` files)
- Functions and modular programming
- Basic user authentication logic
- Console-based menu systems

---

## 📌 Notes

- This is my first major C++ project so the code might not be perfect!
- The revenue report only shows orders with `completed` status (a future improvement would be adding an order status update feature for staff).
- Passwords are stored as plain text — I know this isn't secure, but it's just a school project for now 😅

---

## 👤 Author

- **Name:** Lyle Daniel M. Esguerra
- **Course:** CS1203 - Data Structures and Algorithm
- **School:** Batangas State University
- **Year:** 2026
