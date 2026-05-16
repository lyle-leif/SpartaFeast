/*
This is a console-based version of the SpartaFeast cafeteria ordering system.
Users can browse shops, place orders, and staff can manage menus - all through 
command-line interactions.
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <map>

using namespace std;

// ============================================================
//  DATA STRUCTURES
// ============================================================

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
    int    quantity;
    double unitPrice;
};

struct Order {
    int               orderId;
    string            username;
    string            shopName;
    vector<OrderItem> items;
    double            total;
    string            timestamp;
    string            status;   // "pending" | "completed" | "cancelled"
};

struct User {
    string username;
    string password;
    string role;   // "customer" | "staff"
};

// ============================================================
//  GLOBALS
// ============================================================

vector<Shop>  shops;
vector<Order> orders;
vector<User>  users;

string currentUser = "";
string currentRole = "";

// ============================================================
//  UTILITY
// ============================================================

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHeader(const string& title) {
    cout << "\n" << string(62, '=') << "\n";
    int pad = (62 - (int)title.size()) / 2;
    cout << string(pad, ' ') << title << "\n";
    cout << string(62, '=') << "\n\n";
}

void pressEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(1000, '\n');
}

string currentTimestamp() {
    time_t now = time(nullptr);
    tm*    t   = localtime(&now);
    ostringstream oss;
    oss << (1900 + t->tm_year) << "-"
        << setw(2) << setfill('0') << (1 + t->tm_mon) << "-"
        << setw(2) << setfill('0') << t->tm_mday      << " "
        << setw(2) << setfill('0') << t->tm_hour       << ":"
        << setw(2) << setfill('0') << t->tm_min        << ":"
        << setw(2) << setfill('0') << t->tm_sec;
    return oss.str();
}

string rep(char c, int n) { return string(n, c); }

int findShop(const string& name) {
    for (int i = 0; i < (int)shops.size(); i++)
        if (shops[i].name == name) return i;
    return -1;
}

int findUser(const string& name) {
    for (int i = 0; i < (int)users.size(); i++)
        if (users[i].username == name) return i;
    return -1;
}

string toUpper(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

// ============================================================
//  FILE I/O
// ============================================================

void saveData() {
    ofstream sf("shops.txt");
    for (auto& s : shops) {
        sf << s.name << "\n" << s.menu.size() << "\n";
        for (auto& m : s.menu)
            sf << m.name << "\n" << m.price << "\n";
    }

    ofstream of("orders.txt");
    for (auto& o : orders) {
        of << o.orderId   << "\n"
           << o.username  << "\n"
           << o.shopName  << "\n"
           << o.items.size() << "\n";
        for (auto& i : o.items)
            of << i.itemName << "\n" << i.quantity << "\n" << i.unitPrice << "\n";
        of << o.total     << "\n"
           << o.timestamp << "\n"
           << o.status    << "\n";
    }
}

void saveUsers() {
    ofstream uf("users.txt");
    for (auto& u : users)
        uf << u.username << "\n" << u.password << "\n" << u.role << "\n";
}

void loadData() {
    ifstream sf("shops.txt");
    if (sf.is_open()) {
        string shopName;
        while (getline(sf, shopName)) {
            if (shopName.empty()) continue;
            Shop s; s.name = shopName;
            int cnt; sf >> cnt; sf.ignore();
            for (int i = 0; i < cnt; i++) {
                MenuItem m;
                getline(sf, m.name);
                sf >> m.price; sf.ignore();
                s.menu.push_back(m);
            }
            shops.push_back(s);
        }
    }

    ifstream of("orders.txt");
    if (of.is_open()) {
        Order o;
        while (of >> o.orderId) {
            of.ignore();
            getline(of, o.username);
            getline(of, o.shopName);
            int cnt; of >> cnt; of.ignore();
            o.items.clear();
            for (int i = 0; i < cnt; i++) {
                OrderItem oi;
                getline(of, oi.itemName);
                of >> oi.quantity >> oi.unitPrice; of.ignore();
                o.items.push_back(oi);
            }
            of >> o.total; of.ignore();
            getline(of, o.timestamp);
            getline(of, o.status);
            orders.push_back(o);
        }
    }

    ifstream uf("users.txt");
    if (uf.is_open()) {
        User u;
        while (getline(uf, u.username)) {
            if (u.username.empty()) continue;
            getline(uf, u.password);
            getline(uf, u.role);
            if (findUser(u.username) == -1)
                users.push_back(u);
        }
    }
}

// ============================================================
//  AUTH
// ============================================================

bool doRegister();

bool doLogin() {
    clearScreen();
    printHeader("SpartaFeast  |  Login");
    cout << "  1. Login\n  2. Create Account\n\n";
    cout << "Select option (1-2): ";
    string ch; getline(cin, ch);

    if (ch == "1") {
        cout << "Username: "; string user; getline(cin, user);
        cout << "Password: "; string pass; getline(cin, pass);

        int idx = findUser(user);
        if (idx != -1 && users[idx].password == pass) {
            currentUser = user;
            currentRole = users[idx].role;
            cout << "\nWelcome, " << user << "!  Role: " << toUpper(currentRole) << "\n";
            pressEnter();
            return true;
        }
        cout << "\nInvalid credentials!\n";
        pressEnter();
        return false;

    } else if (ch == "2") {
        return doRegister();
    }

    cout << "\nInvalid option!\n";
    pressEnter();
    return false;
}

bool doRegister() {
    clearScreen();
    printHeader("Create New Account");
    cout << "  1. Student / Customer\n  2. Staff\n\n";
    cout << "Select role (1-2): ";
    string rc; getline(cin, rc);

    string role;
    if      (rc == "1") role = "customer";
    else if (rc == "2") role = "staff";
    else { cout << "\nInvalid role!\n"; pressEnter(); return false; }

    string username;
    while (true) {
        cout << "Username: "; getline(cin, username);
        if (username.empty())         { cout << "Username cannot be empty!\n";     continue; }
        if (username.size() < 3)      { cout << "At least 3 characters required!\n"; continue; }
        if (findUser(username) != -1) { cout << "Username already taken!\n";       continue; }
        break;
    }

    string password;
    while (true) {
        cout << "Password: "; getline(cin, password);
        if (password.size() < 4) { cout << "At least 4 characters required!\n"; continue; }
        cout << "Confirm  : "; string c2; getline(cin, c2);
        if (password != c2) { cout << "Passwords don't match!\n"; continue; }
        break;
    }

    users.push_back({username, password, role});
    saveUsers();
    cout << "\nAccount created!  Username: " << username
         << "  |  Role: " << toUpper(role) << "\n";

    cout << "\nLogin now? (yes/no): "; string ln; getline(cin, ln);
    if (ln == "yes") {
        currentUser = username;
        currentRole = role;
        cout << "Welcome, " << username << "!\n";
        pressEnter();
        return true;
    }
    pressEnter();
    return false;
}

// ============================================================
//  SHARED
// ============================================================

void viewShops() {
    clearScreen();
    printHeader("Cafeteria Shops");

    if (shops.empty()) { cout << "No shops available.\n"; pressEnter(); return; }

    cout << left << setw(28) << "Shop Name" << setw(8) << "Items" << "Sample Menu\n";
    cout << rep('-', 62) << "\n";

    for (auto& s : shops) {
        string sample;
        for (int i = 0; i < (int)min((int)s.menu.size(), 2); i++) {
            if (i) sample += ", ";
            sample += s.menu[i].name;
        }
        if ((int)s.menu.size() > 2) sample += "...";
        cout << left << setw(28) << s.name << setw(8) << s.menu.size() << sample << "\n";
    }
    pressEnter();
}

// ============================================================
//  CUSTOMER FUNCTIONS
// ============================================================

void placeOrder() {
    clearScreen();
    printHeader("Place Order");

    if (shops.empty()) { cout << "No shops available.\n"; pressEnter(); return; }

    cout << "Available Shops:\n";
    for (int i = 0; i < (int)shops.size(); i++)
        cout << "  " << (i+1) << ". " << shops[i].name << "\n";

    cout << "\nSelect shop number: "; string inp; getline(cin, inp);
    int sc;
    try { sc = stoi(inp); } catch (...) { cout << "Invalid input!\n"; pressEnter(); return; }
    if (sc < 1 || sc > (int)shops.size()) { cout << "Invalid selection!\n"; pressEnter(); return; }

    Shop& shop = shops[sc - 1];

    cout << "\n  Menu  >>  " << shop.name << "\n" << rep('-', 44) << "\n";
    for (int i = 0; i < (int)shop.menu.size(); i++)
        cout << "  " << (i+1) << ". " << left << setw(24) << shop.menu[i].name
             << "P" << fixed << setprecision(2) << shop.menu[i].price << "\n";

    cout << "\nEnter quantities (Enter = skip):\n";
    vector<OrderItem> chosen;
    double total = 0;

    for (auto& m : shop.menu) {
        cout << "  " << left << setw(24) << m.name << ": ";
        string q; getline(cin, q);
        if (q.empty()) continue;
        try {
            int qty = stoi(q);
            if (qty > 0) {
                chosen.push_back({m.name, qty, m.price});
                total += m.price * qty;
            }
        } catch (...) {
            cout << "    (skipped - invalid)\n";
        }
    }

    if (chosen.empty()) { cout << "\nNo items selected!\n"; pressEnter(); return; }

    cout << "\n" << rep('=', 50) << "\n";
    cout << "  ORDER SUMMARY\n";
    cout << rep('=', 50) << "\n";
    cout << "  Shop : " << shop.name << "\n";
    cout << rep('-', 50) << "\n";
    cout << left << setw(24) << "  Item" << setw(6) << "Qty" << setw(10) << "Unit" << "Subtotal\n";
    cout << rep('-', 50) << "\n";
    for (auto& oi : chosen) {
        cout << "  " << left << setw(22) << oi.itemName
             << setw(6)  << oi.quantity
             << "P" << setw(9) << fixed << setprecision(2) << oi.unitPrice
             << "P" << fixed << setprecision(2) << (oi.unitPrice * oi.quantity) << "\n";
    }
    cout << rep('-', 50) << "\n";
    cout << right << setw(40) << "TOTAL:  P"
         << fixed << setprecision(2) << total << "\n";
    cout << rep('=', 50) << "\n";

    cout << "\nConfirm order? (yes/no): "; string cf; getline(cin, cf);
    if (cf != "yes") { cout << "Order cancelled.\n"; pressEnter(); return; }

    Order o;
    o.orderId   = (int)orders.size() + 1;
    o.username  = currentUser;
    o.shopName  = shop.name;
    o.items     = chosen;
    o.total     = total;
    o.timestamp = currentTimestamp();
    o.status    = "pending";
    orders.push_back(o);
    saveData();

    cout << "\n  Order #" << o.orderId << " placed successfully!\n";
    cout << "  Total: P" << fixed << setprecision(2) << total << "\n";
    pressEnter();
}

void myOrders() {
    clearScreen();
    printHeader("My Orders");

    vector<Order*> mine;
    for (auto& o : orders)
        if (o.username == currentUser) mine.push_back(&o);

    if (mine.empty()) { cout << "No orders yet.\n"; pressEnter(); return; }

    int pendCnt = 0, doneCnt = 0, cancCnt = 0;
    double totalSpent = 0;
    for (auto* o : mine) {
        if (o->status == "pending")   pendCnt++;
        if (o->status == "completed") { doneCnt++; totalSpent += o->total; }
        if (o->status == "cancelled") cancCnt++;
    }

    cout << rep('=', 66) << "\n";
    cout << left << setw(5) << "ID" << setw(20) << "Shop"
         << setw(10) << "Total" << setw(13) << "Status" << "Date\n";
    cout << rep('-', 66) << "\n";

    for (auto* o : mine) {
        string stat = "[" + toUpper(o->status) + "]";
        cout << left << setw(5) << o->orderId
             << setw(20) << o->shopName
             << "P" << setw(9) << fixed << setprecision(2) << o->total
             << setw(13) << stat
             << o->timestamp << "\n";
    }

    cout << rep('=', 66) << "\n\n";
    cout << "  +----- ORDER SUMMARY --------------------------------+\n";
    cout << "  |  Total orders    : " << left << setw(32) << mine.size()    << "|\n";
    cout << "  |  Pending         : " << setw(32) << pendCnt                << "|\n";
    cout << "  |  Completed       : " << setw(32) << doneCnt                << "|\n";
    cout << "  |  Cancelled       : " << setw(32) << cancCnt                << "|\n";

    ostringstream amtStr;
    amtStr << "P" << fixed << setprecision(2) << totalSpent;
    cout << "  |  Amount spent    : " << left << setw(32) << amtStr.str()   << "|\n";
    cout << "  +----------------------------------------------------+\n";

    cout << "\nView details for order ID (0 = skip): ";
    string inp; getline(cin, inp);
    try {
        int id = stoi(inp);
        if (id > 0) {
            for (auto* o : mine) {
                if (o->orderId != id) continue;
                cout << "\n" << rep('=', 50) << "\n";
                cout << "  ORDER #" << id << " DETAILS\n";
                cout << rep('=', 50) << "\n";
                cout << "  Shop   : " << o->shopName         << "\n";
                cout << "  Status : " << toUpper(o->status)  << "\n";
                cout << "  Date   : " << o->timestamp        << "\n";
                cout << rep('-', 50) << "\n";
                cout << left << setw(24) << "  Item" << setw(6) << "Qty"
                     << setw(10) << "Unit" << "Subtotal\n";
                cout << rep('-', 50) << "\n";
                for (auto& oi : o->items) {
                    cout << "  " << left << setw(22) << oi.itemName
                         << setw(6) << oi.quantity
                         << "P" << setw(9) << fixed << setprecision(2) << oi.unitPrice
                         << "P" << fixed << setprecision(2)
                         << (oi.unitPrice * oi.quantity) << "\n";
                }
                cout << rep('-', 50) << "\n";
                cout << right << setw(40) << "TOTAL:  P"
                     << fixed << setprecision(2) << o->total << "\n";
                cout << rep('=', 50) << "\n";
                break;
            }
        }
    } catch (...) {}

    pressEnter();
}

void cancelOrder() {
    clearScreen();
    printHeader("Cancel an Order");

    vector<Order*> pending;
    for (auto& o : orders)
        if (o.username == currentUser && o.status == "pending")
            pending.push_back(&o);

    if (pending.empty()) {
        cout << "You have no pending orders to cancel.\n";
        pressEnter();
        return;
    }

    cout << "Your Pending Orders:\n\n";
    cout << rep('=', 58) << "\n";
    cout << left << setw(5) << "ID" << setw(20) << "Shop"
         << setw(10) << "Total" << "Date\n";
    cout << rep('-', 58) << "\n";
    for (auto* o : pending) {
        cout << left << setw(5) << o->orderId
             << setw(20) << o->shopName
             << "P" << setw(9) << fixed << setprecision(2) << o->total
             << o->timestamp << "\n";
    }
    cout << rep('=', 58) << "\n";

    cout << "\nEnter Order ID to cancel (0 = back): ";
    string inp; getline(cin, inp);
    int id;
    try { id = stoi(inp); } catch (...) { cout << "Invalid!\n"; pressEnter(); return; }
    if (id == 0) return;

    for (auto* o : pending) {
        if (o->orderId != id) continue;

        cout << "\n" << rep('-', 50) << "\n";
        cout << "  Cancelling Order #" << id << "  |  " << o->shopName << "\n";
        cout << rep('-', 50) << "\n";
        for (auto& oi : o->items)
            cout << "    - " << oi.itemName << " x" << oi.quantity
                 << "  =  P" << fixed << setprecision(2)
                 << (oi.unitPrice * oi.quantity) << "\n";
        cout << "  Total : P" << fixed << setprecision(2) << o->total << "\n";
        cout << rep('-', 50) << "\n";

        cout << "\nAre you sure? (yes/no): ";
        string cf; getline(cin, cf);
        if (cf == "yes") {
            o->status = "cancelled";
            saveData();
            cout << "\nOrder #" << id << " has been cancelled.\n";
        } else {
            cout << "Cancellation aborted.\n";
        }
        pressEnter();
        return;
    }

    cout << "Order ID not found in your pending orders.\n";
    pressEnter();
}

void customerSummary() {
    clearScreen();
    printHeader("My Order Summary");

    vector<Order*> mine;
    for (auto& o : orders)
        if (o.username == currentUser) mine.push_back(&o);

    if (mine.empty()) { cout << "No orders yet.\n"; pressEnter(); return; }

    double spent = 0;
    int pendCnt = 0, doneCnt = 0, cancCnt = 0;
    map<string, double> perShop;
    map<string, int>    itemCount;

    for (auto* o : mine) {
        if (o->status == "pending")   pendCnt++;
        if (o->status == "cancelled") cancCnt++;
        if (o->status == "completed") {
            doneCnt++;
            spent += o->total;
            perShop[o->shopName] += o->total;
            for (auto& i : o->items)
                itemCount[i.itemName] += i.quantity;
        }
    }

    cout << rep('=', 54) << "\n";
    cout << "  ACCOUNT SUMMARY  |  " << currentUser << "\n";
    cout << rep('=', 54) << "\n";
    cout << "  Total orders placed  : " << mine.size()  << "\n";
    cout << "  Pending              : " << pendCnt      << "\n";
    cout << "  Completed            : " << doneCnt      << "\n";
    cout << "  Cancelled            : " << cancCnt      << "\n";
    cout << "  Total amount spent   : P"
         << fixed << setprecision(2) << spent << "\n";

    if (!perShop.empty()) {
        cout << "\n" << rep('-', 54) << "\n";
        cout << "  SPENDING BY SHOP\n";
        cout << rep('-', 54) << "\n";
        for (auto& e : perShop)
            cout << "  " << left << setw(30) << e.first
                 << "P" << fixed << setprecision(2) << e.second << "\n";
    }

    if (!itemCount.empty()) {
        auto best = max_element(itemCount.begin(), itemCount.end(),
            [](auto& a, auto& b){ return a.second < b.second; });
        cout << "\n" << rep('-', 54) << "\n";
        cout << "  Favourite item  :  " << best->first
             << "  (ordered x" << best->second << ")\n";
    }

    cout << rep('=', 54) << "\n";
    pressEnter();
}

// ============================================================
//  STAFF FUNCTIONS
// ============================================================

void addShop() {
    clearScreen();
    printHeader("Add New Shop");
    cout << "Shop name: "; string name; getline(cin, name);
    if (name.empty())         { cout << "Name cannot be empty!\n"; pressEnter(); return; }
    if (findShop(name) != -1) { cout << "'" << name << "' already exists!\n"; pressEnter(); return; }
    shops.push_back({name, {}});
    saveData();
    cout << "\nShop '" << name << "' created!\n";
    pressEnter();
}

void addMenuItem() {
    clearScreen();
    printHeader("Add Menu Item");
    if (shops.empty()) { cout << "No shops yet!\n"; pressEnter(); return; }

    for (int i = 0; i < (int)shops.size(); i++)
        cout << "  " << (i+1) << ". " << shops[i].name << "\n";
    cout << "\nSelect shop: "; string inp; getline(cin, inp);

    int sc;
    try { sc = stoi(inp); } catch (...) { cout << "Invalid!\n"; pressEnter(); return; }
    if (sc < 1 || sc > (int)shops.size()) { cout << "Invalid selection!\n"; pressEnter(); return; }

    Shop& s = shops[sc - 1];
    cout << "Item name : "; string nm; getline(cin, nm);
    if (nm.empty()) { cout << "Name cannot be empty!\n"; pressEnter(); return; }
    cout << "Price (P) : "; string pr; getline(cin, pr);
    double price;
    try { price = stod(pr); } catch (...) { cout << "Invalid price!\n"; pressEnter(); return; }
    if (price < 0) { cout << "Price cannot be negative!\n"; pressEnter(); return; }

    s.menu.push_back({nm, price});
    saveData();
    cout << "\n'" << nm << "' (P" << fixed << setprecision(2) << price
         << ") added to " << s.name << "!\n";
    pressEnter();
}

void removeMenuItem() {
    clearScreen();
    printHeader("Remove Menu Item");
    if (shops.empty()) { cout << "No shops yet!\n"; pressEnter(); return; }

    for (int i = 0; i < (int)shops.size(); i++)
        cout << "  " << (i+1) << ". " << shops[i].name << "\n";
    cout << "\nSelect shop: "; string inp; getline(cin, inp);
    int sc;
    try { sc = stoi(inp); if (sc < 1 || sc > (int)shops.size()) throw 0; }
    catch (...) { cout << "Invalid!\n"; pressEnter(); return; }

    Shop& s = shops[sc - 1];
    if (s.menu.empty()) { cout << "No items in this shop.\n"; pressEnter(); return; }

    for (int i = 0; i < (int)s.menu.size(); i++)
        cout << "  " << (i+1) << ". " << left << setw(24) << s.menu[i].name
             << "P" << fixed << setprecision(2) << s.menu[i].price << "\n";

    cout << "\nSelect item to remove: "; string ic; getline(cin, ic);
    int idx;
    try { idx = stoi(ic); if (idx < 1 || idx > (int)s.menu.size()) throw 0; }
    catch (...) { cout << "Invalid!\n"; pressEnter(); return; }

    string nm = s.menu[idx - 1].name;
    cout << "Remove '" << nm << "'? (yes/no): "; string cf; getline(cin, cf);
    if (cf == "yes") { s.menu.erase(s.menu.begin() + idx - 1); saveData(); cout << "Removed.\n"; }
    else cout << "Cancelled.\n";
    pressEnter();
}

void removeShop() {
    clearScreen();
    printHeader("Remove Shop");
    if (shops.empty()) { cout << "No shops yet!\n"; pressEnter(); return; }

    for (int i = 0; i < (int)shops.size(); i++)
        cout << "  " << (i+1) << ". " << shops[i].name << "\n";
    cout << "\nSelect shop to remove: "; string inp; getline(cin, inp);
    int sc;
    try { sc = stoi(inp); if (sc < 1 || sc > (int)shops.size()) throw 0; }
    catch (...) { cout << "Invalid!\n"; pressEnter(); return; }

    string nm = shops[sc - 1].name;
    cout << "Remove '" << nm << "' and all items? (yes/no): "; string cf; getline(cin, cf);
    if (cf == "yes") { shops.erase(shops.begin() + sc - 1); saveData(); cout << "Removed.\n"; }
    else cout << "Cancelled.\n";
    pressEnter();
}

void viewAllOrders() {
    clearScreen();
    printHeader("All Orders");
    if (orders.empty()) { cout << "No orders yet.\n"; pressEnter(); return; }

    cout << rep('=', 72) << "\n";
    cout << left
         << setw(5)  << "ID"
         << setw(14) << "Customer"
         << setw(20) << "Shop"
         << setw(10) << "Total"
         << setw(13) << "Status"
         << "Date\n";
    cout << rep('-', 72) << "\n";

    for (auto& o : orders) {
        string stat = "[" + toUpper(o.status) + "]";
        cout << left
             << setw(5)  << o.orderId
             << setw(14) << o.username
             << setw(20) << o.shopName
             << "P" << setw(9) << fixed << setprecision(2) << o.total
             << setw(13) << stat
             << o.timestamp << "\n";
    }
    cout << rep('=', 72) << "\n";
    pressEnter();
}

void markOrderDone() {
    clearScreen();
    printHeader("Mark Order as Done");

    vector<Order*> pending;
    for (auto& o : orders)
        if (o.status == "pending") pending.push_back(&o);

    if (pending.empty()) {
        cout << "No pending orders right now.\n";
        pressEnter();
        return;
    }

    cout << "Pending Orders:\n\n";
    cout << rep('=', 64) << "\n";
    cout << left
         << setw(5)  << "ID"
         << setw(14) << "Customer"
         << setw(20) << "Shop"
         << setw(10) << "Total"
         << "Date\n";
    cout << rep('-', 64) << "\n";
    for (auto* o : pending) {
        cout << left
             << setw(5)  << o->orderId
             << setw(14) << o->username
             << setw(20) << o->shopName
             << "P" << setw(9) << fixed << setprecision(2) << o->total
             << o->timestamp << "\n";
    }
    cout << rep('=', 64) << "\n";

    cout << "\nEnter Order ID to mark as DONE (0 = back): ";
    string inp; getline(cin, inp);
    int id;
    try { id = stoi(inp); } catch (...) { cout << "Invalid!\n"; pressEnter(); return; }
    if (id == 0) return;

    for (auto* o : pending) {
        if (o->orderId != id) continue;

        cout << "\n" << rep('-', 50) << "\n";
        cout << "  Order #" << id << "  |  " << o->shopName << "\n";
        cout << "  Customer : " << o->username << "\n";
        cout << rep('-', 50) << "\n";
        for (auto& oi : o->items)
            cout << "    - " << oi.itemName << " x" << oi.quantity
                 << "  =  P" << fixed << setprecision(2)
                 << (oi.unitPrice * oi.quantity) << "\n";
        cout << "  Total    : P" << fixed << setprecision(2) << o->total << "\n";
        cout << rep('-', 50) << "\n";

        cout << "\nMark this order as DONE? (yes/no): ";
        string cf; getline(cin, cf);
        if (cf == "yes") {
            o->status = "completed";
            saveData();
            cout << "\nOrder #" << id << " marked as COMPLETED.\n";
        } else {
            cout << "No changes made.\n";
        }
        pressEnter();
        return;
    }

    cout << "Order ID not found in pending list.\n";
    pressEnter();
}

void revenueReport() {
    clearScreen();
    printHeader("Revenue Report");
    if (orders.empty()) { cout << "No orders yet.\n"; pressEnter(); return; }

    map<string, double> shopRev;
    map<string, int>    shopCnt;
    map<string, int>    itemSold;
    int totPend = 0, totDone = 0, totCanc = 0;
    double grandTotal = 0;

    for (auto& o : orders) {
        if (o.status == "pending")   totPend++;
        if (o.status == "cancelled") totCanc++;
        if (o.status == "completed") {
            totDone++;
            grandTotal += o.total;
            shopRev[o.shopName] += o.total;
            shopCnt[o.shopName]++;
            for (auto& i : o.items)
                itemSold[i.itemName] += i.quantity;
        }
    }

    // overall
    cout << rep('=', 54) << "\n";
    cout << "  OVERALL SUMMARY\n";
    cout << rep('=', 54) << "\n";
    cout << "  Total orders   : " << orders.size() << "\n";
    cout << "  Completed      : " << totDone       << "\n";
    cout << "  Pending        : " << totPend       << "\n";
    cout << "  Cancelled      : " << totCanc       << "\n";
    cout << "  Total Revenue  : P"
         << fixed << setprecision(2) << grandTotal << "\n";
    cout << rep('=', 54) << "\n\n";

    // per-shop table
    if (!shopRev.empty()) {
        cout << rep('-', 54) << "\n";
        cout << left << setw(26) << "  Shop" << setw(10) << "Orders" << "Revenue\n";
        cout << rep('-', 54) << "\n";
        for (auto& e : shopRev)
            cout << "  " << left << setw(24) << e.first
                 << setw(10) << shopCnt[e.first]
                 << "P" << fixed << setprecision(2) << e.second << "\n";
        cout << rep('-', 54) << "\n";
        cout << "  " << left << setw(24) << "TOTAL"
             << setw(10) << totDone
             << "P" << fixed << setprecision(2) << grandTotal << "\n";
        cout << rep('=', 54) << "\n";
    }

    // top 5 items
    if (!itemSold.empty()) {
        vector<pair<string,int>> sorted(itemSold.begin(), itemSold.end());
        sort(sorted.begin(), sorted.end(),
             [](auto& a, auto& b){ return a.second > b.second; });

        cout << "\n" << rep('-', 54) << "\n";
        cout << "  TOP SELLING ITEMS\n";
        cout << rep('-', 54) << "\n";
        cout << left << setw(6) << "  Rank" << setw(28) << "Item" << "Qty Sold\n";
        cout << rep('-', 54) << "\n";
        for (int i = 0; i < (int)min((int)sorted.size(), 5); i++)
            cout << "  #" << (i+1) << "  " << left << setw(28) << sorted[i].first
                 << sorted[i].second << "\n";
        cout << rep('=', 54) << "\n";
    }

    // detailed breakdown
    if (!shopRev.empty()) {
        cout << "\n" << rep('=', 54) << "\n";
        cout << "  DETAILED BREAKDOWN BY SHOP\n";
        cout << rep('=', 54) << "\n";
        for (auto& e : shopRev) {
            cout << "\n  [ " << e.first << " ]\n";
            cout << rep('-', 54) << "\n";
            for (auto& o : orders) {
                if (o.shopName != e.first || o.status != "completed") continue;
                cout << "  Order #" << o.orderId << "  " << o.username
                     << "  |  P" << fixed << setprecision(2) << o.total << "\n";
                for (auto& oi : o.items)
                    cout << "    - " << oi.itemName << " x" << oi.quantity
                         << "  =  P" << fixed << setprecision(2)
                         << (oi.unitPrice * oi.quantity) << "\n";
            }
        }
        cout << rep('=', 54) << "\n";
    }

    pressEnter();
}

void staffDashboard() {
    clearScreen();
    printHeader("Staff Dashboard");

    int totalShops = shops.size();
    int totalItems = 0;
    for (auto& s : shops) totalItems += s.menu.size();

    int pend = 0, done = 0, canc = 0;
    double rev = 0;
    for (auto& o : orders) {
        if (o.status == "pending")   pend++;
        if (o.status == "completed") { done++; rev += o.total; }
        if (o.status == "cancelled") canc++;
    }

    cout << rep('=', 54) << "\n";
    cout << "  CAFETERIA AT A GLANCE\n";
    cout << rep('=', 54) << "\n";
    cout << "  Shops available      : " << totalShops << "\n";
    cout << "  Total menu items     : " << totalItems << "\n";
    cout << rep('-', 54) << "\n";
    cout << "  Orders\n";
    cout << "    Pending            : " << pend << "\n";
    cout << "    Completed          : " << done << "\n";
    cout << "    Cancelled          : " << canc << "\n";
    cout << "    Total revenue      : P"
         << fixed << setprecision(2) << rev << "\n";
    cout << rep('=', 54) << "\n";

    pressEnter();
}

// ============================================================
//  MENUS
// ============================================================

void customerMenu() {
    while (true) {
        clearScreen();
        cout << "  SpartaFeast  |  " << currentUser << "  (CUSTOMER)\n";
        cout << rep('=', 62) << "\n\n";
        cout << "  1. View Shops\n";
        cout << "  2. Place Order\n";
        cout << "  3. My Orders\n";
        cout << "  4. Cancel an Order\n";
        cout << "  5. My Summary\n";
        cout << "  6. Logout\n\n";
        cout << "Select option (1-6): ";
        string ch; getline(cin, ch);

        if      (ch == "1") viewShops();
        else if (ch == "2") placeOrder();
        else if (ch == "3") myOrders();
        else if (ch == "4") cancelOrder();
        else if (ch == "5") customerSummary();
        else if (ch == "6") {
            cout << "\nLogging out...\n";
            currentUser = ""; currentRole = "";
            break;
        } else {
            cout << "Invalid option!\n"; pressEnter();
        }
    }
}

void staffMenu() {
    while (true) {
        clearScreen();
        cout << "  SpartaFeast  |  " << currentUser << "  (STAFF)\n";
        cout << rep('=', 62) << "\n\n";
        cout << "  1.  View Shops\n";
        cout << "  2.  Add New Shop\n";
        cout << "  3.  Add Menu Item\n";
        cout << "  4.  Remove Menu Item\n";
        cout << "  5.  Remove Shop\n";
        cout << "  6.  View All Orders\n";
        cout << "  7.  Mark Order as Done\n";
        cout << "  8.  Revenue Report\n";
        cout << "  9.  Dashboard Summary\n";
        cout << "  10. Logout\n\n";
        cout << "Select option (1-10): ";
        string ch; getline(cin, ch);

        if      (ch == "1")  viewShops();
        else if (ch == "2")  addShop();
        else if (ch == "3")  addMenuItem();
        else if (ch == "4")  removeMenuItem();
        else if (ch == "5")  removeShop();
        else if (ch == "6")  viewAllOrders();
        else if (ch == "7")  markOrderDone();
        else if (ch == "8")  revenueReport();
        else if (ch == "9")  staffDashboard();
        else if (ch == "10") {
            cout << "\nLogging out...\n";
            currentUser = ""; currentRole = "";
            break;
        } else {
            cout << "Invalid option!\n"; pressEnter();
        }
    }
}

// ============================================================
//  MAIN
// ============================================================

int main() {
    // default accounts
    users.push_back({"student1", "pass123",  "customer"});
    users.push_back({"student2", "pass456",  "customer"});
    users.push_back({"staff1",   "staff123", "staff"});
    users.push_back({"staff2",   "staff456", "staff"});

    loadData();  // merges saved data on top of defaults

    while (true) {
        if      (currentUser.empty())        doLogin();
        else if (currentRole == "customer")  customerMenu();
        else if (currentRole == "staff")     staffMenu();
    }

    return 0;
}