# 🍽️ SpartaFeast Cafeteria Ordering System

A console-based cafeteria ordering system written in **C++** for my CS101 course project.
Students can browse shops and place food orders, while cafeteria staff can manage menus, mark orders as done, and view revenue reports — all through a simple text menu.

---

## ✨ Features

### 👤 Customer Menu
| # | Feature | Description |
|---|---------|-------------|
| 1 | View Shops | See all available cafeteria shops and sample items |
| 2 | Place Order | Browse a shop's menu, pick quantities, and confirm |
| 3 | My Orders | View full order history with status labels |
| 4 | **Cancel an Order** | Cancel any of your **pending** orders |
| 5 | **My Summary** | See total spent, spending per shop, and favourite item |
| 6 | Logout | Return to the login screen |

### 🛠️ Staff Menu
| # | Feature | Description |
|---|---------|-------------|
| 1 | View Shops | Overview of all shops and item counts |
| 2 | Add New Shop | Create a new cafeteria shop |
| 3 | Add Menu Item | Add a food item and price to a shop |
| 4 | Remove Menu Item | Delete an item from a shop's menu |
| 5 | Remove Shop | Remove a shop and all its items |
| 6 | View All Orders | See every order placed by all customers |
| 7 | **Mark Order as Done** | Change a pending order's status to **completed** |
| 8 | **Revenue Report** | Overall summary, per-shop revenue table, top-selling items, and detailed breakdown |
| 9 | **Dashboard Summary** | Quick glance at shops, items, and order stats |
| 10 | Logout | Return to the login screen |

---

## 🗂️ Order Statuses

| Status | Meaning |
|--------|---------|
| `PENDING` | Order placed, waiting to be prepared |
| `COMPLETED` | Staff marked the order as done |
| `CANCELLED` | Customer cancelled the order |

---

## 🛠️ How to Compile and Run

Make sure you have **g++** installed.

**Windows (MinGW / g++):**
```bash
g++ main.cpp -o spartafeast
spartafeast.exe
```

**Linux / Mac:**
```bash
g++ main.cpp -o spartafeast
./spartafeast
```

No external libraries needed — just standard C++.

---

## 🔑 Default Accounts

| Username | Password | Role |
|----------|----------|------|
| student1 | pass123 | Customer |
| student2 | pass456 | Customer |
| staff1 | staff123 | Staff |
| staff2 | staff456 | Staff |

You can also register a new account when the program starts.

---

## 💾 How Data is Saved

The program saves everything to plain `.txt` files in the same folder:

| File | Contents |
|------|----------|
| `shops.txt` | All shop names and menu items |
| `orders.txt` | All orders with status and items |
| `users.txt` | Registered user accounts |

These files are created automatically — you don't need to make them yourself.

---

## 📁 Project Structure

```
SpartaFeast/
│
├── main.cpp        ← all source code is here (single file)
├── shops.txt       ← auto-created when shops are added
├── orders.txt      ← auto-created when orders are placed
├── users.txt       ← auto-created when accounts are registered
└── README.md       ← this file
```

---

## 📸 Sample Screens

```
  SpartaFeast  |  student1  (CUSTOMER)
==============================================================

  1. View Shops
  2. Place Order
  3. My Orders
  4. Cancel an Order
  5. My Summary
  6. Logout
```

```
==================================================
  ORDER SUMMARY
==================================================
  Shop : Manang's Kitchen
--------------------------------------------------
  Item                    Qty   Unit      Subtotal
--------------------------------------------------
  Adobo Rice              2     P55.00    P110.00
  Sago't Gulaman          1     P30.00    P30.00
--------------------------------------------------
                                TOTAL:   P140.00
==================================================
```

---

## 🧠 What I Practiced

- C++ structs and vectors
- File I/O with `ifstream` and `ofstream`
- Functions and modular code organization
- Console formatting with `iomanip`
- Basic authentication logic
- Order status tracking (pending → completed / cancelled)

---

## 📌 Known Limitations

- Passwords are stored as plain text (this is a school project, not a real app)
- No admin account to manage users
- Data files can get corrupted if the program crashes mid-save

---

## 👤 Author

- **Name:** Lyle Daniel M. Esguerra
- **Course:** CS1203 - Data Structures and Algorithm
- **School:** Batangas State University
- **Year:** 2026
