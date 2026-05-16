/*
    SpartaFeast Cafeteria Ordering System
    A simple console-based ordering system for students and staff.

*/

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <sstream>

using namespace std;

// ===================== DATA STRUCTURES =====================

struct MenuItem {
    string name;
    double price;
};

struct Shop {
    string name;
    vector<MenuItem> menu;
};

struct OrderItem {
    string itemName;
    int quantity;
    double price;
};

struct Order {
    int orderId;
    string username;
    string shopName;
    vector<OrderItem> items;
    double total;
    string timestamp;
    string status;
};

struct User {
    string username;
    string password;
    string role; // "customer" or "staff"
};

// ===================== GLOBAL VARIABLES =====================

vector<Shop> shops;
vector<Order> orders;
vector<User> users;

string currentUser = "";
string currentRole = "";

// ===================== HELPER FUNCTIONS =====================

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader(string title) {
    cout << "\n" << string(60, '=') << endl;
    // center the title
    int spaces = (60 - title.length()) / 2;
    cout << string(spaces, ' ') << title << endl;
    cout << string(60, '=') << "\n" << endl;
}

void pressEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

string getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    // format: YYYY-MM-DD HH:MM:SS
    ostringstream oss;
    oss << (1900 + ltm->tm_year) << "-"
        << setw(2) << setfill('0') << (1 + ltm->tm_mon) << "-"
        << setw(2) << setfill('0') << ltm->tm_mday << " "
        << setw(2) << setfill('0') << ltm->tm_hour << ":"
        << setw(2) << setfill('0') << ltm->tm_min << ":"
        << setw(2) << setfill('0') << ltm->tm_sec;
    return oss.str();
}

// find shop index by name, returns -1 if not found
int findShop(string name) {
    for (int i = 0; i < shops.size(); i++) {
        if (shops[i].name == name) return i;
    }
    return -1;
}

// find user index by username
int findUser(string username) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i].username == username) return i;
    }
    return -1;
}

// ===================== SAVE / LOAD =====================

void saveData() {
    // save shops
    ofstream shopFile("shops.txt");
    for (auto& shop : shops) {
        shopFile << shop.name << "\n";
        shopFile << shop.menu.size() << "\n";
        for (auto& item : shop.menu) {
            shopFile << item.name << "\n" << item.price << "\n";
        }
    }
    shopFile.close();

    // save orders
    ofstream orderFile("orders.txt");
    for (auto& o : orders) {
        orderFile << o.orderId << "\n"
                  << o.username << "\n"
                  << o.shopName << "\n"
                  << o.items.size() << "\n";
        for (auto& item : o.items) {
            orderFile << item.itemName << "\n"
                      << item.quantity << "\n"
                      << item.price << "\n";
        }
        orderFile << o.total << "\n"
                  << o.timestamp << "\n"
                  << o.status << "\n";
    }
    orderFile.close();
}

void loadData() {
    // load shops
    ifstream shopFile("shops.txt");
    if (shopFile.is_open()) {
        string shopName;
        while (getline(shopFile, shopName)) {
            if (shopName.empty()) continue;
            Shop s;
            s.name = shopName;
            int count;
            shopFile >> count;
            shopFile.ignore();
            for (int i = 0; i < count; i++) {
                MenuItem m;
                getline(shopFile, m.name);
                shopFile >> m.price;
                shopFile.ignore();
                s.menu.push_back(m);
            }
            shops.push_back(s);
        }
        shopFile.close();
    }

    // load orders
    ifstream orderFile("orders.txt");
    if (orderFile.is_open()) {
        Order o;
        while (orderFile >> o.orderId) {
            orderFile.ignore();
            getline(orderFile, o.username);
            getline(orderFile, o.shopName);
            int itemCount;
            orderFile >> itemCount;
            orderFile.ignore();
            o.items.clear();
            for (int i = 0; i < itemCount; i++) {
                OrderItem oi;
                getline(orderFile, oi.itemName);
                orderFile >> oi.quantity >> oi.price;
                orderFile.ignore();
                o.items.push_back(oi);
            }
            orderFile >> o.total;
            orderFile.ignore();
            getline(orderFile, o.timestamp);
            getline(orderFile, o.status);
            orders.push_back(o);
        }
        orderFile.close();
    }

    // load users from file (if it exists)
    ifstream userFile("users.txt");
    if (userFile.is_open()) {
        User u;
        while (getline(userFile, u.username)) {
            if (u.username.empty()) continue;
            getline(userFile, u.password);
            getline(userFile, u.role);
            // only add if not already in users list
            if (findUser(u.username) == -1) {
                users.push_back(u);
            }
        }
        userFile.close();
    }
}

void saveUsers() {
    ofstream userFile("users.txt");
    for (auto& u : users) {
        userFile << u.username << "\n"
                 << u.password << "\n"
                 << u.role << "\n";
    }
    userFile.close();
}

// ===================== AUTH FUNCTIONS =====================

bool doRegister();

bool doLogin() {
    clearScreen();
    printHeader("SpartaFeast Login");
    cout << "1. Login" << endl;
    cout << "2. Create Account\n" << endl;
    cout << "Select option (1-2): ";
    string choice;
    getline(cin, choice);

    if (choice == "1") {
        cout << "Username: ";
        string username;
        getline(cin, username);

        cout << "Password: ";
        string password;
        getline(cin, password);

        int idx = findUser(username);
        if (idx != -1 && users[idx].password == password) {
            currentUser = username;
            currentRole = users[idx].role;
            cout << "\nWelcome, " << username << "! Role: ";
            // uppercase the role
            string roleUpper = currentRole;
            transform(roleUpper.begin(), roleUpper.end(), roleUpper.begin(), ::toupper);
            cout << roleUpper << endl;
            pressEnter();
            return true;
        } else {
            cout << "\nInvalid credentials!" << endl;
            pressEnter();
            return false;
        }

    } else if (choice == "2") {
        return doRegister();
    } else {
        cout << "\nInvalid option!" << endl;
        pressEnter();
        return false;
    }
}

bool doRegister() {
    clearScreen();
    printHeader("Create New Account");

    cout << "1. Student/Customer" << endl;
    cout << "2. Staff\n" << endl;
    cout << "Select role (1-2): ";
    string roleChoice;
    getline(cin, roleChoice);

    string role;
    if (roleChoice == "1") {
        role = "customer";
    } else if (roleChoice == "2") {
        role = "staff";
    } else {
        cout << "\nInvalid role!" << endl;
        pressEnter();
        return false;
    }

    // get username
    string username;
    while (true) {
        cout << "Username: ";
        getline(cin, username);
        if (username.empty()) {
            cout << "Username cannot be empty!" << endl;
        } else if (username.length() < 3) {
            cout << "Username must be at least 3 characters!" << endl;
        } else if (findUser(username) != -1) {
            cout << "Username already taken!" << endl;
        } else {
            break;
        }
    }

    // get password
    string password;
    while (true) {
        cout << "Password: ";
        getline(cin, password);
        if (password.length() < 4) {
            cout << "Password must be at least 4 characters!" << endl;
            continue;
        }
        cout << "Confirm password: ";
        string confirm;
        getline(cin, confirm);
        if (password != confirm) {
            cout << "Passwords don't match!" << endl;
        } else {
            break;
        }
    }

    User newUser;
    newUser.username = username;
    newUser.password = password;
    newUser.role = role;
    users.push_back(newUser);
    saveUsers();

    string roleUpper = role;
    transform(roleUpper.begin(), roleUpper.end(), roleUpper.begin(), ::toupper);
    cout << "\nAccount created! Username: " << username << " | Role: " << roleUpper << endl;

    cout << "\nLogin now? (yes/no): ";
    string loginNow;
    getline(cin, loginNow);
    if (loginNow == "yes") {
        currentUser = username;
        currentRole = role;
        cout << "Welcome, " << username << "!" << endl;
        pressEnter();
        return true;
    } else {
        pressEnter();
        return false;
    }
}

// ===================== CUSTOMER FUNCTIONS =====================

void viewShops() {
    clearScreen();
    printHeader("Cafeteria Shops");

    if (shops.empty()) {
        cout << "No shops available!\n" << endl;
        pressEnter();
        return;
    }

    cout << left << setw(30) << "Shop Name"
         << setw(10) << "Items"
         << "Sample Menu" << endl;
    cout << string(60, '-') << endl;

    for (auto& shop : shops) {
        string samples = "";
        int count = shop.menu.size();
        for (int i = 0; i < min(2, count); i++) {
            if (i > 0) samples += ", ";
            samples += shop.menu[i].name;
        }
        if (count > 2) samples += "...";
        cout << left << setw(30) << shop.name
             << setw(10) << count
             << samples << endl;
    }

    pressEnter();
}

void placeOrder() {
    clearScreen();
    printHeader("Place Order");

    if (shops.empty()) {
        cout << "No shops available!\n" << endl;
        pressEnter();
        return;
    }

    cout << "Available Shops:" << endl;
    for (int i = 0; i < shops.size(); i++) {
        cout << "  " << (i + 1) << ". " << shops[i].name << endl;
    }

    cout << "\nSelect shop number: ";
    string input;
    getline(cin, input);

    int shopChoice;
    try {
        shopChoice = stoi(input);
    } catch (...) {
        cout << "\nInvalid input!" << endl;
        pressEnter();
        return;
    }

    if (shopChoice < 1 || shopChoice > (int)shops.size()) {
        cout << "\nInvalid selection!" << endl;
        pressEnter();
        return;
    }

    Shop& selectedShop = shops[shopChoice - 1];

    cout << "\nMenu for " << selectedShop.name << ":" << endl;
    cout << string(40, '-') << endl;
    for (int i = 0; i < selectedShop.menu.size(); i++) {
        cout << "  " << (i + 1) << ". "
             << selectedShop.menu[i].name
             << " - P" << fixed << setprecision(2)
             << selectedShop.menu[i].price << endl;
    }

    cout << "\nEnter quantities (press Enter to skip):" << endl;

    vector<OrderItem> orderItems;
    double total = 0.0;

    for (auto& menuItem : selectedShop.menu) {
        cout << "  " << menuItem.name << ": ";
        string qtyInput;
        getline(cin, qtyInput);
        if (!qtyInput.empty()) {
            try {
                int qty = stoi(qtyInput);
                if (qty > 0) {
                    OrderItem oi;
                    oi.itemName = menuItem.name;
                    oi.quantity = qty;
                    oi.price = menuItem.price;
                    orderItems.push_back(oi);
                    total += menuItem.price * qty;
                }
            } catch (...) {
                cout << "  Skipping " << menuItem.name << " (invalid input)" << endl;
            }
        }
    }

    if (orderItems.empty()) {
        cout << "\nNo items selected!" << endl;
        pressEnter();
        return;
    }

    cout << "\n" << string(40, '=') << endl;
    cout << "ORDER SUMMARY" << endl;
    cout << string(40, '=') << endl;
    cout << "Shop: " << selectedShop.name << "\nItems:" << endl;
    for (auto& oi : orderItems) {
        cout << "  - " << oi.itemName << " x" << oi.quantity
             << " = P" << fixed << setprecision(2) << (oi.price * oi.quantity) << endl;
    }
    cout << "\nTotal: P" << fixed << setprecision(2) << total << endl;
    cout << string(40, '=') << endl;

    cout << "\nConfirm order? (yes/no): ";
    string confirm;
    getline(cin, confirm);
    if (confirm != "yes") {
        cout << "Order cancelled!" << endl;
        pressEnter();
        return;
    }

    Order newOrder;
    newOrder.orderId = orders.size() + 1;
    newOrder.username = currentUser;
    newOrder.shopName = selectedShop.name;
    newOrder.items = orderItems;
    newOrder.total = total;
    newOrder.timestamp = getCurrentTime();
    newOrder.status = "pending";

    orders.push_back(newOrder);
    saveData();

    cout << "\nOrder #" << newOrder.orderId << " placed! Total: P"
         << fixed << setprecision(2) << total << endl;
    pressEnter();
}

void myOrders() {
    clearScreen();
    printHeader("My Orders");

    vector<Order*> userOrders;
    for (auto& o : orders) {
        if (o.username == currentUser) {
            userOrders.push_back(&o);
        }
    }

    if (userOrders.empty()) {
        cout << "No orders found!\n" << endl;
        pressEnter();
        return;
    }

    double totalSpent = 0.0;
    cout << left << setw(5) << "ID"
         << setw(20) << "Shop"
         << setw(12) << "Total"
         << "Date" << endl;
    cout << string(60, '-') << endl;

    for (auto* o : userOrders) {
        cout << left << setw(5) << o->orderId
             << setw(20) << o->shopName
             << "P" << setw(11) << fixed << setprecision(2) << o->total
             << o->timestamp << endl;
        totalSpent += o->total;
    }

    cout << string(60, '-') << endl;
    cout << "Total Spent: P" << fixed << setprecision(2) << totalSpent << endl;

    cout << "\nView details for order ID (0 to skip): ";
    string input;
    getline(cin, input);

    try {
        int orderId = stoi(input);
        if (orderId > 0) {
            for (auto* o : userOrders) {
                if (o->orderId == orderId) {
                    cout << "\nOrder #" << orderId << " Details:" << endl;
                    cout << "Shop: " << o->shopName << endl;
                    for (auto& oi : o->items) {
                        cout << "  - " << oi.itemName << " x" << oi.quantity
                             << " = P" << fixed << setprecision(2)
                             << (oi.price * oi.quantity) << endl;
                    }
                    cout << "Total: P" << fixed << setprecision(2) << o->total << endl;
                    cout << "Placed: " << o->timestamp << endl;
                    break;
                }
            }
        }
    } catch (...) {
        // user pressed enter without input, just skip
    }

    pressEnter();
}

// ===================== STAFF FUNCTIONS =====================

void addShop() {
    clearScreen();
    printHeader("Add New Shop");

    cout << "Enter shop name: ";
    string name;
    getline(cin, name);

    if (name.empty()) {
        cout << "Shop name cannot be empty!" << endl;
        pressEnter();
        return;
    }

    if (findShop(name) != -1) {
        cout << "'" << name << "' already exists!" << endl;
        pressEnter();
        return;
    }

    Shop newShop;
    newShop.name = name;
    shops.push_back(newShop);
    saveData();

    cout << "\nShop '" << name << "' created!" << endl;
    pressEnter();
}

void addMenuItem() {
    clearScreen();
    printHeader("Add Menu Item");

    if (shops.empty()) {
        cout << "No shops yet! Create a shop first." << endl;
        pressEnter();
        return;
    }

    for (int i = 0; i < shops.size(); i++) {
        cout << "  " << (i + 1) << ". " << shops[i].name << endl;
    }

    cout << "\nSelect shop: ";
    string input;
    getline(cin, input);

    int choice;
    try {
        choice = stoi(input);
    } catch (...) {
        cout << "Invalid input!" << endl;
        pressEnter();
        return;
    }

    if (choice < 1 || choice > (int)shops.size()) {
        cout << "Invalid selection!" << endl;
        pressEnter();
        return;
    }

    Shop& selectedShop = shops[choice - 1];

    cout << "Item name: ";
    string itemName;
    getline(cin, itemName);

    if (itemName.empty()) {
        cout << "Item name cannot be empty!" << endl;
        pressEnter();
        return;
    }

    cout << "Price (P): ";
    string priceInput;
    getline(cin, priceInput);

    double price;
    try {
        price = stod(priceInput);
    } catch (...) {
        cout << "Invalid price!" << endl;
        pressEnter();
        return;
    }

    if (price < 0) {
        cout << "Price cannot be negative!" << endl;
        pressEnter();
        return;
    }

    MenuItem newItem;
    newItem.name = itemName;
    newItem.price = price;
    selectedShop.menu.push_back(newItem);
    saveData();

    cout << "\n'" << itemName << "' (P" << fixed << setprecision(2) << price
         << ") added to '" << selectedShop.name << "'!" << endl;
    pressEnter();
}

void removeMenuItem() {
    clearScreen();
    printHeader("Remove Menu Item");

    if (shops.empty()) {
        cout << "No shops yet!" << endl;
        pressEnter();
        return;
    }

    for (int i = 0; i < shops.size(); i++) {
        cout << "  " << (i + 1) << ". " << shops[i].name << endl;
    }

    cout << "\nSelect shop: ";
    string input;
    getline(cin, input);

    int choice;
    try {
        choice = stoi(input);
        if (choice < 1 || choice > (int)shops.size()) throw 0;
    } catch (...) {
        cout << "Invalid input!" << endl;
        pressEnter();
        return;
    }

    Shop& selectedShop = shops[choice - 1];

    if (selectedShop.menu.empty()) {
        cout << "This shop has no items!" << endl;
        pressEnter();
        return;
    }

    for (int i = 0; i < selectedShop.menu.size(); i++) {
        cout << "  " << (i + 1) << ". " << selectedShop.menu[i].name
             << " (P" << fixed << setprecision(2) << selectedShop.menu[i].price << ")" << endl;
    }

    cout << "\nSelect item to remove: ";
    string itemInput;
    getline(cin, itemInput);

    int itemChoice;
    try {
        itemChoice = stoi(itemInput);
        if (itemChoice < 1 || itemChoice > (int)selectedShop.menu.size()) throw 0;
    } catch (...) {
        cout << "Invalid input!" << endl;
        pressEnter();
        return;
    }

    string itemName = selectedShop.menu[itemChoice - 1].name;

    cout << "Remove '" << itemName << "'? (yes/no): ";
    string confirm;
    getline(cin, confirm);

    if (confirm == "yes") {
        selectedShop.menu.erase(selectedShop.menu.begin() + itemChoice - 1);
        saveData();
        cout << "'" << itemName << "' removed!" << endl;
    } else {
        cout << "Cancelled." << endl;
    }

    pressEnter();
}

void removeShop() {
    clearScreen();
    printHeader("Remove Shop");

    if (shops.empty()) {
        cout << "No shops yet!" << endl;
        pressEnter();
        return;
    }

    for (int i = 0; i < shops.size(); i++) {
        cout << "  " << (i + 1) << ". " << shops[i].name << endl;
    }

    cout << "\nSelect shop to remove: ";
    string input;
    getline(cin, input);

    int choice;
    try {
        choice = stoi(input);
        if (choice < 1 || choice > (int)shops.size()) throw 0;
    } catch (...) {
        cout << "Invalid input!" << endl;
        pressEnter();
        return;
    }

    string shopName = shops[choice - 1].name;

    cout << "Remove '" << shopName << "' and all its items? (yes/no): ";
    string confirm;
    getline(cin, confirm);

    if (confirm == "yes") {
        shops.erase(shops.begin() + choice - 1);
        saveData();
        cout << "'" << shopName << "' removed!" << endl;
    } else {
        cout << "Cancelled." << endl;
    }

    pressEnter();
}

void viewAllOrders() {
    clearScreen();
    printHeader("All Orders");

    if (orders.empty()) {
        cout << "No orders yet!\n" << endl;
        pressEnter();
        return;
    }

    cout << left << setw(5) << "ID"
         << setw(15) << "Customer"
         << setw(20) << "Shop"
         << setw(12) << "Total"
         << "Date" << endl;
    cout << string(75, '-') << endl;

    for (auto& o : orders) {
        cout << left << setw(5) << o.orderId
             << setw(15) << o.username
             << setw(20) << o.shopName
             << "P" << setw(11) << fixed << setprecision(2) << o.total
             << o.timestamp << endl;
    }

    pressEnter();
}

void revenueReport() {
    clearScreen();
    printHeader("Revenue Report");

    if (orders.empty()) {
        cout << "No orders yet!\n" << endl;
        pressEnter();
        return;
    }

    map<string, double> shopRevenue;
    map<string, int> shopOrderCount;

    for (auto& o : orders) {
        if (o.status == "completed") {
            shopRevenue[o.shopName] += o.total;
            shopOrderCount[o.shopName]++;
        }
    }

    cout << left << setw(25) << "Shop"
         << setw(10) << "Orders"
         << "Revenue" << endl;
    cout << string(50, '-') << endl;

    double totalRevenue = 0;
    int totalOrderCount = 0;

    for (auto& entry : shopRevenue) {
        cout << left << setw(25) << entry.first
             << setw(10) << shopOrderCount[entry.first]
             << "P" << fixed << setprecision(2) << entry.second << endl;
        totalRevenue += entry.second;
        totalOrderCount += shopOrderCount[entry.first];
    }

    cout << string(50, '-') << endl;
    cout << left << setw(25) << "TOTAL"
         << setw(10) << totalOrderCount
         << "P" << fixed << setprecision(2) << totalRevenue << endl;

    cout << "\n" << string(50, '=') << endl;
    cout << "DETAILED BREAKDOWN" << endl;
    cout << string(50, '=') << endl;

    for (auto& entry : shopRevenue) {
        cout << "\n" << entry.first << endl;
        for (auto& o : orders) {
            if (o.shopName == entry.first && o.status == "completed") {
                cout << "  Order #" << o.orderId << " - " << o.username
                     << ": P" << fixed << setprecision(2) << o.total << " | ";
                for (int i = 0; i < o.items.size(); i++) {
                    if (i > 0) cout << ", ";
                    cout << o.items[i].itemName << "(x" << o.items[i].quantity << ")";
                }
                cout << endl;
            }
        }
    }

    pressEnter();
}

// ===================== MENUS =====================

void customerMenu() {
    while (true) {
        clearScreen();
        cout << "SpartaFeast | " << currentUser << " (CUSTOMER)" << endl;
        cout << string(60, '=') << "\n" << endl;
        cout << "1. View Shops" << endl;
        cout << "2. Place Order" << endl;
        cout << "3. My Orders" << endl;
        cout << "4. Logout\n" << endl;

        cout << "Select option (1-4): ";
        string choice;
        getline(cin, choice);

        if (choice == "1") viewShops();
        else if (choice == "2") placeOrder();
        else if (choice == "3") myOrders();
        else if (choice == "4") {
            cout << "\nLogging out..." << endl;
            currentUser = "";
            currentRole = "";
            break;
        } else {
            cout << "Invalid option!" << endl;
            pressEnter();
        }
    }
}

void staffMenu() {
    while (true) {
        clearScreen();
        cout << "SpartaFeast | " << currentUser << " (STAFF)" << endl;
        cout << string(60, '=') << "\n" << endl;
        cout << "1. View Shops" << endl;
        cout << "2. Add New Shop" << endl;
        cout << "3. Add Menu Item" << endl;
        cout << "4. Remove Menu Item" << endl;
        cout << "5. Remove Shop" << endl;
        cout << "6. View All Orders" << endl;
        cout << "7. Revenue Report" << endl;
        cout << "8. Logout\n" << endl;

        cout << "Select option (1-8): ";
        string choice;
        getline(cin, choice);

        if (choice == "1") viewShops();
        else if (choice == "2") addShop();
        else if (choice == "3") addMenuItem();
        else if (choice == "4") removeMenuItem();
        else if (choice == "5") removeShop();
        else if (choice == "6") viewAllOrders();
        else if (choice == "7") revenueReport();
        else if (choice == "8") {
            cout << "\nLogging out..." << endl;
            currentUser = "";
            currentRole = "";
            break;
        } else {
            cout << "Invalid option!" << endl;
            pressEnter();
        }
    }
}

// ===================== MAIN =====================

int main() {
    // default users
    users.push_back({"student1", "pass123", "customer"});
    users.push_back({"student2", "pass456", "customer"});
    users.push_back({"staff1",   "staff123", "staff"});
    users.push_back({"staff2",   "staff456", "staff"});

    loadData(); // load saved data (will merge with defaults)

    while (true) {
        if (currentUser.empty()) {
            doLogin();
        } else if (currentRole == "customer") {
            customerMenu();
        } else if (currentRole == "staff") {
            staffMenu();
        }
    }

    return 0;
}