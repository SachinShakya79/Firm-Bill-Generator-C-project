#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <conio.h>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <thread> //Added for sleep
#include <chrono> //Added for time duration

using namespace std;

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[36m"
#define UP_ARROW 72
#define DOWN_ARROW 80
#define ENTER_KEY 13
#define ESC_KEY 27
#define MAX_PRODUCTS 100
#define MAX_BILLS 100
#define MAX_ITEMS_PER_BILL 50

//Forward Declaration
class BillingSystem;

class Product
{
private:
    string id;
    string name;
    double price;
    int stock;

public:
    Product() : id("0"), name(""), price(0.0), stock(0) {}

    void setId(string i) { id = i; }
    void setName(string n) { name = n; }
    void setPrice(double p) { price = p; }
    void setStock(int s) { stock = s; }
    void adjustStock(int s) { stock += s; }

    string getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    bool inputDetails()
    {
        cout << "Enter product name: ";
        getline(cin, name);
        if (name.empty())
        {
            return false;
        }

        cout << "Enter price: ";
        while (!(cin >> price) || price <= 0)
        {
            cout << RED << "Invalid input. Enter a valid price: " << RESET;
            cin.clear();
            cin.ignore();
        }

        cout << "Enter stock: ";
        while (!(cin >> stock) || stock <= 0)
        {
            cout << RED << "Invalid input. Enter a valid stock quantity: " << RESET;
            cin.clear();
            cin.ignore();
        }
        cout << GREEN << "Product added successfully!\n"
             << RESET;
             cin.ignore();
        return true;
      
    }

    void viewDetails(int sn) const
    {
        cout << left
     << setw(8) << sn
     << setw(11) << id
     << setw(25) << name
     << setw(11) << fixed << setprecision(2) << price
     << setw(11) 
        << (stock == 0 ? string(RED) + "Out of Stock" + RESET : string(GREEN) + to_string(stock) + RESET)
     << endl;
    }

    void saveToFile(ofstream &fout) const
    {
        fout << id << endl;
        fout << name << endl;
        fout << price << endl;
        fout << stock << endl;
    }

    void loadFromFile(ifstream &fin)
    {
        getline(fin, id);
        getline(fin, name);
        fin >> price;
        fin >> stock;
        fin.ignore();
    }
};

struct BillItem
{
    string productName;
    double price;
    int quantity;
};

class Bill
{
private:
    string id;
    string customerName;
    string customerPhone;
    long long dateVal;
    BillItem items[MAX_ITEMS_PER_BILL];
    int itemCount;
    double taxPercent;
    double discountPercent;
    bool paid;

public:
    Bill() : itemCount(0), dateVal(0), taxPercent(0), discountPercent(0), paid(false)
    {
        id = "0";
    }

    void setCustomerInfo(string name, string phone)
    {
        customerName = name;
        customerPhone = phone;
    }

    void setDate(long long t) { dateVal = t; }
    void setDiscount(double d) { discountPercent = d; }
    void setTax(double t) { taxPercent = t; }
    void setPaid(bool status) { paid = status; }
    void setId(string i) { id = i; }

    string getId() const { return id; }
    string getCustomerName() const { return customerName; }
    string getCustomerPhone() const { return customerPhone; }
    bool isPaid() const { return paid; }

    //adds item or updates quantity if already exists
    bool addItem(string pName, double pPrice, int qty)
    {
        for (int i = 0; i < itemCount; i++)
        {
            if (items[i].productName == pName)
            {
                items[i].quantity += qty;
                return true;
            }
        }
        
        if (itemCount >= MAX_ITEMS_PER_BILL)
            return false;

        items[itemCount].productName = pName;
        items[itemCount].price = pPrice;
        items[itemCount].quantity = qty;
        itemCount++;
        return true;
    }

    bool removeItem(int index) {
        if (index < 0 || index >= itemCount) return false;
        for(int i = index; i < itemCount - 1; i++) {
            items[i] = items[i+1];
        }
        itemCount--;
        return true;
    }

    string getItemName(int index) const {
        if (index >= 0 && index < itemCount) return items[index].productName;
        return "";
    }

    int getItemQty(int index) const {
         if (index >= 0 && index < itemCount) return items[index].quantity;
         return 0;
    }

    double subtotal() const
    {
        double sum = 0;
        for (int i = 0; i < itemCount; i++)
        {
            sum += items[i].price * items[i].quantity;
        }
        return sum;
    }

    double getDiscountAmount() const
    {
        return subtotal() * (discountPercent / 100.0);
    }

    double getTaxableAmount() const
    {
        return subtotal() - getDiscountAmount();
    }

    double getTaxAmount() const
    {
        return getTaxableAmount() * (taxPercent / 100.0);
    }

    double total() const
    {
        return getTaxableAmount() + getTaxAmount();
    }

    void generateId()
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char buffer[80];
        if (ltm)
        {
            strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", ltm);
            id = string(buffer);
        }
        else
        {
            id = "0";
        }
    }

    void printBill(const string& shopName) const
    {
        cout << BLUE << "\n======================================================\n"
             << RESET;
        int padding = (54 - shopName.length()) / 2;
        if (padding < 0) padding = 0;
        cout << BLUE << string(padding, ' ') << shopName << "\n" << RESET;
        cout << BLUE << "                     INVOICE\n"
             << RESET;
        cout << BLUE << "======================================================\n"
             << RESET;
        cout << "Bill ID: " << id << "\n";
        cout << "Customer: " << customerName << "\n";
        cout << "Phone: " << customerPhone << "\n";
        time_t rawTime = (time_t)dateVal;
        const char *dt = ctime(&rawTime);
        if (dt)
            cout << "Date: " << dt;
        else
            cout << "Date: Unknown\n";
        cout << BLUE << "------------------------------------------------------\n"
             << RESET;
        cout << left
             << setw(4) << "S.No"
             << setw(20) << "Item Particulars"
             << setw(11) << "Qty"
             << setw(11) << "Price"
             << setw(11) << "Subtotal" << endl;
        cout << BLUE << "------------------------------------------------------\n"
             << RESET;
        for (int i = 0; i < itemCount; i++)
        {
            cout << left
                 << setw(4) << i + 1
                 << setw(20) << items[i].productName
                 << setw(11) << items[i].quantity
                 << setw(11) << fixed << setprecision(2) << items[i].price
                 << setw(11) << fixed << setprecision(2) << items[i].price * items[i].quantity << endl;
        }
        cout << BLUE << "------------------------------------------------------\n"
             << RESET;
        cout << "Subtotal:        $" << fixed << setprecision(2) << subtotal() << endl;
        cout << "Discount (" << (int)discountPercent << "%): -$" << fixed << setprecision(2) << getDiscountAmount() << endl;
        cout << "Taxable Amount:  $" << fixed << setprecision(2) << getTaxableAmount() << endl;
        cout << "VAT/Tax (" << (int)taxPercent << "%):  +$" << fixed << setprecision(2) << getTaxAmount() << endl;
        cout << "Grand Total:     $" << fixed << setprecision(2) << total() << endl;
        cout << BLUE << "======================================================\n"
             << RESET;
        cout << "Status: " << (paid ? GREEN "Paid" : RED "Not Paid") << RESET << endl;
        cout << BLUE << "======================================================\n"
             << RESET;
    }

    void printCart() const
    {
        cout << BLUE << "Current Cart:\n"
             << RESET;
        if (itemCount == 0)
        {
            cout << YELLOW << "Cart is empty.\n"
                 << RESET;
        }
        else
        {
            cout << left
                 << setw(8) << "No."
                 << setw(20) << "Item"
                 << setw(11) << "Qty"
                 << setw(11) << "Price"
                 << setw(11) << "Subtotal" << endl;
            cout << "--------------------------------------------------\n";
            for (int i = 0; i < itemCount; i++)
            {
                cout << left << setw(8) << i + 1
                     << setw(20) << items[i].productName
                     << setw(11) << items[i].quantity
                     << setw(11) << fixed << setprecision(2) << items[i].price
                     << setw(11) << fixed << setprecision(2) << items[i].price * items[i].quantity << endl;
            }
        }
    }

    bool hasItems() const { return itemCount > 0; }
    int getItemCount() const { return itemCount; }
    
    string toLower(string s) const {
        string lower = s;
        for(size_t i=0; i<lower.length(); i++) lower[i] = tolower(lower[i]);
        return lower;
    }

    void searchItem(string query) const {
         cout << left << setw(8) << "Ref" << setw(20) << "Item" << setw(11) << "Qty" << endl;
         bool found = false;
         string qLower = toLower(query);
         for (int i = 0; i < itemCount; i++) {
             string nLower = toLower(items[i].productName);
             if (nLower.find(qLower) != string::npos) {
                 cout << left << setw(8) << i + 1
                      << setw(20) << items[i].productName
                      << setw(11) << items[i].quantity << endl;
                 found = true;
             }
         }
         if(!found) cout << RED << "No items found in cart matching '" << query << "'\n" << RESET;
    }

    void saveToFile(ofstream &fout) const
    {
        fout << id << endl;
        fout << customerName << endl;
        fout << customerPhone << endl;
        fout << dateVal << endl;
        fout << taxPercent << endl;
        fout << discountPercent << endl;
        fout << paid << endl;
        fout << itemCount << endl;
        for (int i = 0; i < itemCount; i++)
        {
            fout << items[i].productName << endl;
            fout << items[i].price << endl;
            fout << items[i].quantity << endl;
        }
    }

    void loadFromFile(ifstream &fin)
    {
        getline(fin, id);
        getline(fin, customerName);
        getline(fin, customerPhone);
        fin >> dateVal;
        fin >> taxPercent;
        fin >> discountPercent;
        fin >> paid;
        fin.ignore();
        int count;
        fin >> count;
        fin.ignore();
        itemCount = 0;
        for (int i = 0; i < count && i < MAX_ITEMS_PER_BILL; i++)
        {
            getline(fin, items[i].productName);
            fin >> items[i].price;
            fin >> items[i].quantity;
            fin.ignore();
            itemCount++;
        }
    }
};

class BillingSystem
{
private:
    Product products[MAX_PRODUCTS];
    int productCount;
    Bill bills[MAX_BILLS];
    int billCount;
    string shopName; 
    string adminUser;
    string adminPass;

    string takePassword()
    {
        string pass = "";
        char ch;
        while (true)
        {
            ch = getch();
            if (ch == 13)
                break;
            if (ch == 8)
            {
                if (pass.length() > 0)
                {
                    cout << "\b \b";
                    pass.pop_back();
                }
            }
            else if (ch >= 32 && ch <= 126)
            {
                pass.push_back(ch);
                cout << "*";
            }
        }
        cout << endl;
        return pass;
    }

    string toLower(string s) const {
       std::transform(s.begin(), s.end(), s.begin(), ::toupper);
       return s;       
    }
    
  string toUpper(string s) const {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

    // construct path: "ShopName/filename.txt"
    string getFilePath(string filename) {
        if (shopName.empty()) return filename;
        return shopName + "/" + filename;
    }

    bool saveConfig()
    {
        ofstream fout(getFilePath("config.txt"));
        if (!fout) return false;
        fout << shopName << endl;
        fout << adminUser << endl;
        fout << adminPass << endl;
        fout.close();
        return true;
    }

    bool loadConfig()
    {
        ifstream fin(getFilePath("config.txt"));
        if (!fin) {
             return false; //shop config not found
        }
        string tempName;
        getline(fin, tempName); 
        getline(fin, adminUser);
        getline(fin, adminPass);
        fin.close();
        if(adminUser.empty()) adminUser = "admin";
        if(adminPass.empty()) adminPass = "admin";
        return true;
    }

    void saveProducts()
    {
        ofstream fout(getFilePath("products.txt"));
        if (!fout)
            return;
        fout << productCount << endl;
        for (int i = 0; i < productCount; i++)
        {
            products[i].saveToFile(fout);
        }
        fout.close();
    }

    void loadProducts()
    {
        ifstream fin(getFilePath("products.txt"));
        if (!fin)
        {
            productCount = 0;
            return;
        }
        int count;
        if (!(fin >> count))
        {
            productCount = 0;
            return;
        }
        fin.ignore();
        productCount = 0;
        for (int i = 0; i < count && i < MAX_PRODUCTS; i++)
        {
            products[i].loadFromFile(fin);
            productCount++;
        }
        fin.close();
    }

    void saveBills()
    {
        ofstream fout(getFilePath("bills.txt"));
        if (!fout)
            return;
        fout << billCount << endl;
        for (int i = 0; i < billCount; i++)
        {
            bills[i].saveToFile(fout);
        }
        fout.close();
    }

    void loadBills()
    {
        ifstream fin(getFilePath("bills.txt"));
        if (!fin)
        {
            billCount = 0;
            return;
        }
        int count;
        if (!(fin >> count))
        {
            billCount = 0;
            return;
        }
        fin.ignore();
        billCount = 0;
        for (int i = 0; i < count && i < MAX_BILLS; i++)
        {
            bills[i].loadFromFile(fin);
            billCount++;
        }
        fin.close();
    }

    void showProductList()
    {
        cout << "Current Products in Stock:\n\n";
        if (productCount == 0)
        {
            cout << "No products added yet!\n";
        }
        else
        {
            cout << left
                 << setw(8) << "S.No"
                 << setw(11) << "ID"
                 << setw(25) << "Name"
                 << setw(11) << "Price"
                 << setw(11) << "Stock" << endl;
            cout << "--------------------------------------------------------------------\n";
            for (int i = 0; i < productCount; i++)
            {
                products[i].viewDetails(i + 1);
            }
        }
    }

    void restoreStock(string pName, int qty) {
        for(int i=0; i<productCount; i++) {
            if(products[i].getName() == pName) {
                products[i].adjustStock(qty);
                break;
            }
        }
    }

    string generateProductID() {
        while(true) {
            int num = 1000 + rand() % 9000;
            string idCandidate = to_string(num);
            bool exists = false;
            for(int i=0; i<productCount; i++) {
                if(products[i].getId() == idCandidate) {
                    exists = true; break;
                }
            }
            if(!exists) return idCandidate;
        }
    }

    void changeCredentials() {
        system("cls");
        cout << BLUE << "--- Change Admin Credentials ---\n" << RESET;
        cout << "Enter Current Password: ";
        string oldPass = takePassword();
        
        if (oldPass != adminPass) {
            cout << RED << "Incorrect Password!\n" << RESET;
            getch();
            return;
        }

        cout << "Enter New Username: ";
        string newUser;
        cin >> newUser;
        cout << "Enter New Password: ";
        string newPass = takePassword();

        if (newUser.length() > 0 && newPass.length() > 0) {
            adminUser = newUser;
            adminPass = newPass;
            saveConfig();
            cout << GREEN << "Credentials updated successfully!\n" << RESET;
        } else {
            cout << RED << "Invalid Input.\n" << RESET;
        }
        getch();
    }

    void setupShop() {
        system("cls");
        cout << BLUE << "=== CREATE NEW SHOP ===\n" << RESET;
        cin.ignore();
        
        cout << "1. Enter Business/Shop Name (Use for Login): ";
        string tempName;
        getline(cin, tempName);
        
        if(tempName.empty()) {
            cout << RED << "Shop name cannot be empty.\n" << RESET;
            getch();
            return;
        }
        string originalShopName = shopName;
        shopName = tempName;
        if (loadConfig()) {
             cout << RED << "Shop '" << tempName << "' already exists! Please login instead.\n" << RESET;
             shopName = originalShopName;
             getch();
             return;
        }

        //Create Directory using system command
        string cmd = "mkdir \"" + tempName + "\"";
        system(cmd.c_str());

        // Set current shop name
        shopName = tempName;

        cout << "2. Set Admin Username (Default: admin): ";
        string u;
        getline(cin, u);
        if(!u.empty()) adminUser = u;
        else adminUser = "admin";

        cout << "3. Set Admin Password (Default: admin): ";
        string p;
        getline(cin, p);
        if(!p.empty()) adminPass = p;
        else adminPass = "admin";
        
        if(!saveConfig()) {
            cout << RED << "\nError: Could not create shop data. Invalid shop name (illegal characters) or permission denied.\n" << RESET;
            shopName = ""; //reset
            getch();
            return;
        }
        
        //initialize empty files
        productCount = 0;
        billCount = 0;
        saveProducts();
        saveBills();

        cout << GREEN << "\nShop Registered! Folder '" << shopName << "' created.\n" << RESET;
        cout << "Press any key to return to main menu...";
        getch();
    }

public:
    BillingSystem()
    {
        productCount = 0;
        billCount = 0;
        shopName = "";
        adminUser = "admin";
        adminPass = "admin";
    }

    int adminLogin()
    {
        string user, pass;
        int chance = 0;
        while (chance < 3)
        {
            system("cls");
            //display shopName in uppercase
            cout << BLUE << "=== " << (shopName.empty() ? "ADMIN LOGIN" : toUpper(shopName)) << " ===\n" << RESET;
            cout << "Username: ";
            getline(cin, user); 
            
            cout << "Password: ";
            pass = takePassword();
            if (user == adminUser && pass == adminPass)
            {
                cout << "\nLogin Successful!" << endl;
                getch();
                return 1; // success
            }
            cout << "\nIncorrect Username or Password!" << endl;
            cout << "Press any key to try again..." << endl;
            getch();
            chance++;
        }
        cout << "\nToo many failed attempts!" << endl;
        getch();
        return 0; //failed
    }

    void runAddProduct()
    {
        system("cls");
        showProductList();
        cout << BLUE << "\n--- Add New Product ---\n"
             << RESET;
        cout << BLUE << "-----------------------\n"
             << RESET;
        if (productCount < MAX_PRODUCTS)
        {
            Product newProduct;
            
            //Generate ID
            string newId = generateProductID();
            newProduct.setId(newId);
//            cout << "Generated Product ID: " << YELLOW << newId << RESET << endl;

            if (newProduct.inputDetails())
            {
                products[productCount++] = newProduct;
                saveProducts();
                cout << GREEN << "Product saved to file successfully!\n"
                     << RESET;
            }
            getch();
        }
        else
        {
            cout << RED << "Cannot add more products. Maximum reached!\n"
                 << RESET;
            getch();
        }
    }

    void updateProduct()
    {
        if (productCount == 0)
        {
            cout << RED << "\nNo products to update!\n"
                 << RESET;
            getch();
            return;
        }
        system("cls");
        cout << "--- Update Product ---\n\n";
        cout << "1. Search by Name/ID\n";
        cout << "2. View All and Select by S.No\n";
        cout << "0. Cancel\n";
        cout << "\nChoice: ";
        
        int method;
        if (!(cin >> method)) { cin.clear(); cin.ignore(); method = 0; }

        int choice = 0;

        if (method == 0) return;

        if (method == 1) {
            cin.ignore();
            string query;
            cout << "Enter Product Name (or partial name): ";
            getline(cin, query);
            string qLower = toLower(query);

            int foundIndices[MAX_PRODUCTS];
            int foundCount = 0;
            
            cout << "\nSearch Results:\n";
            cout << left << setw(8) << "Ref" << setw(11) << "ID" << setw(25) << "Name" << endl; 
            for(int i=0; i<productCount; i++) {
                string nLower = toLower(products[i].getName());
                string idLower = toLower(products[i].getId());
                if (nLower.find(qLower) != string::npos || idLower == qLower) {
                    foundIndices[foundCount] = i;
                    cout << left << setw(8) << foundCount + 1 
                         << setw(11) << products[i].getId() 
                         << setw(25) << products[i].getName() << endl;
                    foundCount++;
                }
            }

            if(foundCount == 0) {
                cout << RED << "No products found matching '" << query << "'.\n" << RESET;
                getch();
                return;
            }

            cout << "\nEnter Ref number to update (0 to cancel): ";
            int ref;
            cin >> ref;
            if (ref <= 0 || ref > foundCount) return;
            choice = foundIndices[ref-1] + 1;
        }
        else if (method == 2) {
             showProductList();
             cout << "\nEnter S.No to Update: ";
             cin >> choice;
        }
        else {
            return;
        }

        if (choice < 1 || choice > productCount)
        {
            cout << RED << "\nInvalid selection!\n"
                 << RESET;
            getch();
            return;
        }

        Product &p = products[choice - 1];
        system("cls");
        cout << BLUE << "Updating Product: " << p.getName() << RESET << endl;
        cout << "1. Update Stock (Current: " << p.getStock() << ")" << endl;
        cout << "2. Update Price (Current: $" << fixed << setprecision(2) << p.getPrice() << ")" << endl;
        cout << "3. Update Name (Current: " << p.getName() << ")" << endl;
        cout << "\nChoice: ";
        int update_choice;
        cin >> update_choice;
        if (update_choice == 1)
        {
            int add;
            cout << "Enter quantity to add/subtract (e.g., -5 for removal): ";
            while (!(cin >> add))
            {
                cout << RED << "Invalid input. Enter number: " << RESET;
                cin.clear();
                cin.ignore();
            }
            if (p.getStock() + add < 0)
            {
                cout << RED << "Error: Cannot subtract more than available stock.\n"
                     << RESET;
            }
            else
            {
                p.adjustStock(add);
                cout << GREEN << "Stock updated. New stock: " << p.getStock() << RESET << endl;
            }
        }
        else if (update_choice == 2)
        {
            double new_price;
            cout << "Enter new price: ";
            while (!(cin >> new_price) || new_price < 0)
            {
                cout << RED << "Invalid input. Enter valid price: " << RESET;
                cin.clear();
                cin.ignore();
            }
            p.setPrice(new_price);
            cout << GREEN << "Price updated.\n"
                 << RESET;
        }
        else if (update_choice == 3) {
            cin.ignore();
            cout << "Enter new name: ";
            string nn;
            getline(cin, nn);
            if(!nn.empty()) {
                p.setName(nn);
                cout << GREEN << "Name updated.\n" << RESET;
            }
        }
        saveProducts();
        getch();
    }

    void removeProduct()
    {
        if (productCount == 0)
        {
            cout << RED << "\nNo products to remove!\n"
                 << RESET;
            getch();
            return;
        }
        system("cls");
        cout << "--- Remove Product ---\n\n";
        cout << "1. Search by Name/ID\n";
        cout << "2. Select by S.No\n";
        cout << "0. Cancel\n";
        cout << "\nChoice: ";
        
        int method;
        if (!(cin >> method)) { cin.clear(); cin.ignore(); method = 0; }
        
        int choice = 0;
        if (method == 0) return;
        
        if (method == 1) { 
            cin.ignore();
            string query;
            cout << "Enter Product Name (or partial name): ";
            getline(cin, query);
            string qLower = toLower(query);

            int foundIndices[MAX_PRODUCTS];
            int foundCount = 0;
            
            cout << "\nSearch Results:\n";
            cout << left << setw(8) << "Ref" << setw(11) << "ID" << setw(25) << "Name" << endl; 
            for(int i=0; i<productCount; i++) {
                string nLower = toLower(products[i].getName());
                string idLower = toLower(products[i].getId());
                if (nLower.find(qLower) != string::npos || idLower == qLower) {
                    foundIndices[foundCount] = i;
                    cout << left << setw(8) << foundCount + 1 
                         << setw(11) << products[i].getId() 
                         << setw(25) << products[i].getName() << endl;
                    foundCount++;
                }
            }
            if(foundCount == 0) {
                cout << RED << "No products found matching '" << query << "'.\n" << RESET;
                getch();
                return;
            }
            cout << "\nEnter Ref number to remove (0 to cancel): ";
            int ref;
            cin >> ref;
            if (ref <= 0 || ref > foundCount) return;
            choice = foundIndices[ref-1] + 1;
        } 
        else if (method == 2) { 
            showProductList();
            cout << "\nEnter S.No: ";
            cin >> choice;
        } else {
            return;
        }

        if (choice < 1 || choice > productCount)
        {
            cout << RED << "Invalid selection!\n"
                 << RESET;
            getch();
            return;
        }
        cout << YELLOW << "Are you sure you want to remove " << products[choice - 1].getName() << "? (y/n): " << RESET;
        char confirm = getch();
        if (confirm == 'y' || confirm == 'Y')
        {
            for (int i = choice - 1; i < productCount - 1; i++)
            {
                products[i] = products[i + 1];
            }
            productCount--;
            saveProducts();
            cout << GREEN << "\nProduct removed successfully!\n"
                 << RESET;
        }
        else
        {
            cout << GREEN << "\nRemoval cancelled.\n"
                 << RESET;
        }
        getch();
    }

    void generateBill()
    {
        if (billCount >= MAX_BILLS)
        {
            cout << RED << "Bill memory full! Cannot generate new bill.\n"
                 << RESET;
            getch();
            return;
        }
        Bill bill;
        cin.ignore();
        string cName, cPhone;
        cout << "\n\nEnter customer name: ";
        getline(cin, cName);
        
        while (true) {
            cout << "Enter customer phone number (10 digits): ";
            getline(cin, cPhone);
            bool isDigit = true;
            for(char c : cPhone) {
                if(!isdigit(c)) isDigit = false;
            }
            
            //Must be 10 digits AND start with '9'
            if (cPhone.length() == 10 && isDigit && cPhone[0] == '9') {
                break;
            }
            cout << RED << "Invalid phone number! Must be 10 digits and start with 9.\n" << RESET;
        }

        bill.setCustomerInfo(cName, cPhone);
        bill.generateId();
        bill.setDate(time(0));

        while (true)
        {
            system("cls");
            bill.printCart();
            cout << "\n\n" << BLUE << "--- Bill Menu ---\n" << RESET;
            cout << "1. Add Item to Cart\n";
            cout << "2. Remove Item from Cart\n";
            cout << "0. Finish/Checkout\n";
            cout << "\nChoice: ";
            
            int action;
            if (!(cin >> action)) {
                cin.clear(); cin.ignore(); continue;
            }

            if (action == 0) break;

            if (action == 1) //ADD ITEM
            {
                system("cls");
                cout << "--- Add Item ---\n1. Search by Name/ID\n2. Select by S.No\nChoice: ";
                int sm; 
                cin >> sm;
                int choice = 0;
                
                if (sm == 1) {
                    cin.ignore();
                    string query;
                    cout << "Enter Name to Search: ";
                    getline(cin, query);
                    string qLower = toLower(query);
                    
                    int foundIndices[MAX_PRODUCTS];
                    int foundCount = 0;
                    cout << left << setw(8) << "Ref" << setw(11) << "ID" << setw(25) << "Name" << setw(11) << "Stock" << endl;
                    for(int i=0; i<productCount; i++) {
                         string nLower = toLower(products[i].getName());
                         string idLower = toLower(products[i].getId());
                         if (nLower.find(qLower) != string::npos || idLower == qLower) {
                             foundIndices[foundCount] = i;
                             cout << left << setw(8) << foundCount + 1 
                                  << setw(11) << products[i].getId() 
                                  << setw(25) << products[i].getName() 
                                  << setw(11) << products[i].getStock() << endl;
                             foundCount++;
                         }
                    }
                    if(foundCount == 0) {
                        cout << RED << "No items found.\n" << RESET; getch(); continue;
                    }
                    cout << "Enter Ref to add (0 to cancel): ";
                    int ref; cin >> ref;
                    if(ref <=0 || ref > foundCount) continue;
                    choice = foundIndices[ref-1] + 1;
                } else {
                    showProductList();
                    cout << "\nEnter S.No of product to add (0 to cancel): ";
                    cin >> choice;
                }

                if (choice == 0) continue;
                if (choice < 1) {
                    cout << RED << "Out Of Stock\n" << RESET; getch(); continue;
                }
                if(choice > productCount){
                	 cout << RED << "Invalid Choice!\n" << RESET; getch(); continue;
				}

                Product &p = products[choice - 1];
                cout << "Selected: " << p.getName() << "\nQuantity: ";
                int qty;
                while (!(cin >> qty)) {
                    cout << RED << "Invalid input.\n" << RESET; 
					cin.clear(); 
					cin.ignore();
                }

                if (qty <= 0 || qty > p.getStock()) {
                    cout << RED << "Invalid quantity or not enough stock!\n" << RESET;
                    getch();
                    continue;
                }

                if (bill.addItem(p.getName(), p.getPrice(), qty)) {
                    p.adjustStock(-qty);
                    cout << GREEN << "Item added to bill!\n" << RESET;
                } else {
                    cout << RED << "Cart full!\n" << RESET;
                }
                getch();
            }
            else if (action == 2) //REMOVE ITEM
            {
                if (!bill.hasItems()) {
                    cout << RED << "Cart is empty!\n" << RESET; getch(); continue;
                }
                
                cout << "--- Remove Item ---\n1. Search in Cart by Name\n2. Select by S.No (Cart index)\nChoice: ";
                int sm; cin >> sm;
                int idx = -1;

                if (sm == 1) {
                    cin.ignore();
                    string query;
                    cout << "Enter Name to Search in Cart: ";
                    getline(cin, query);
                    string qLower = toLower(query);

                    bill.searchItem(query); 
                    cout << "Enter Ref (S.No from search) to remove (0 to cancel): ";
                    int rChoice; cin >> rChoice;
                    
                    if (rChoice > 0 && rChoice <= bill.getItemCount()) {
                        string iLower = toLower(bill.getItemName(rChoice-1));
                        if (iLower.find(qLower) != string::npos) {
                             idx = rChoice - 1;
                        } else {
                            cout << RED << "Selected item didn't match search query, but removing anyway.\n" << RESET;
                            idx = rChoice - 1;
                        }
                    } else { idx = -1; }
                } else {
                    cout << "\nEnter S.No of item in CART to remove (1-" << bill.getItemCount() << "): ";
                    int rChoice; cin >> rChoice;
                    if(rChoice > 0 && rChoice <= bill.getItemCount()) idx = rChoice - 1;
                }

                if(idx == -1) {
                    cout << RED << "Invalid selection.\n" << RESET; getch(); continue;
                }
                
                string pName = bill.getItemName(idx);
                int pQty = bill.getItemQty(idx);
                
                if (bill.removeItem(idx)) {
                    restoreStock(pName, pQty);
                    cout << GREEN << "Item removed from cart and stock restored.\n" << RESET;
                }
                getch();
            }
        }

        if (!bill.hasItems())
        {
            cout << RED << "No items added. Cancelling bill!\n"
                 << RESET;
            getch();
            return;
        }

        system("cls");
        cout << BLUE << "--- Bill Finalization ---\n"
             << RESET;
        cout << "Subtotal: $" << fixed << setprecision(2) << bill.subtotal() << endl
             << endl;
        double disc, tax;
        cout << "Enter Discount Percentage (0 for none): ";
        while (!(cin >> disc) || disc < 0)
        {
            cout << RED << "Invalid. Enter valid %: " << RESET;
            cin.clear();
            cin.ignore();
        }
        bill.setDiscount(disc);
        cout << "Enter VAT/Tax Percentage: ";
        while (!(cin >> tax) || tax < 0)
        {
            cout << RED << "Invalid. Enter valid %: " << RESET;
            cin.clear();
            cin.ignore();
        }
        bill.setTax(tax);
        cin.ignore();
        system("cls");
        
        // ANIMATION: Generating Bill...
        cout << "\nGenerating Bill";
        cout.flush();
        for(int i = 0; i < 4; ++i) {
             std::this_thread::sleep_for(std::chrono::milliseconds(300 + rand() % 200));
             cout << ".";
             cout.flush();
        }
        cout << "\n";

        bill.printBill(toUpper(shopName));
        
        cout << YELLOW << "\nIs this bill paid? (y/n): " << RESET;
        char paid_choice = getch();
        if (paid_choice == 'y' || paid_choice == 'Y')
        {
            bill.setPaid(true);
            cout << GREEN << "\nBill marked as paid." << RESET;
        }
        else
        {
            cout << RED << "\nBill remains unpaid." << RESET;
        }
        cout << YELLOW << " Press any key to save..." << RESET;
        getch();
        bills[billCount++] = bill;
        saveBills();
        saveProducts();
        cout << GREEN << "\nBill saved successfully!\n"
             << RESET;
        getch();
    }

    void viewUnpaidBills()
    {
        int unpaidIndices[MAX_BILLS];
        int unpaidCount = 0;
        for (int i = 0; i < billCount; i++)
        {
            if (!bills[i].isPaid())
            {
                unpaidIndices[unpaidCount++] = i;
            }
        }
        if (unpaidCount == 0)
        {
            cout << RED << "\nNo unpaid bills found.\n"
                 << RESET;
            getch();
            return;
        }
        int selected = 0;
        while (true)
        {
            system("cls");
            cout << BLUE << "View Unpaid Bills:\n\n"
                 << RESET;
            cout << left
                 << setw(8) << "S.No"
                 << setw(25) << "Name"
                 << setw(25) << "Bill ID" << endl;
            cout << BLUE << "--------------------------------------------------\n"
                 << RESET;
            for (int i = 0; i < unpaidCount; ++i)
            {
                int originalIndex = unpaidIndices[i];
                cout << (i == selected ? "-> " : "    ") << left
                     << setw(2) << i + 1
                     << setw(25) << bills[originalIndex].getCustomerName()
                     << setw(25) << bills[originalIndex].getId() << endl;
            }
            cout << "\nPress " << YELLOW << "Enter" << RESET << " to view, " << YELLOW << "ESC" << RESET << " to back.\n";
            int key = getch();
            if (key == 0 || key == 224)
            {
                key = getch();
                if (key == DOWN_ARROW)
                    selected = (selected + 1) % unpaidCount;
                else if (key == UP_ARROW)
                    selected = (selected - 1 + unpaidCount) % unpaidCount;
            }
            else if (key == ENTER_KEY)
            {
                system("cls");
                int idx = unpaidIndices[selected];
                
                bills[idx].printBill(toUpper(shopName));
                
                cout << "\nPress 'M' to Mark Paid, 'C' for Contact, other key to back...\n";
                char action = getch();
                if (action == 'm' || action == 'M')
                {
                    bills[idx].setPaid(true);
                    saveBills();
                    cout << GREEN << "Bill marked as paid!\n"
                         << RESET;
                    getch();
                    return;
                }
                else if (action == 'c' || action == 'C')
                {
                    cout << "\nName: " << bills[idx].getCustomerName()
                         << "\nPhone: " << bills[idx].getCustomerPhone() << endl;
                    getch();
                }
            }
            else if (key == ESC_KEY)
            {
                break;
            }
        }
    }

    void transactionHistory()
    {
        int paidIndices[MAX_BILLS];
        int paidCount = 0;
        for (int i = 0; i < billCount; i++)
        {
            if (bills[i].isPaid())
            {
                paidIndices[paidCount++] = i;
            }
        }
        if (paidCount == 0)
        {
            system("cls");
            cout << RED << "No transactions recorded.\n"
                 << RESET;
            getch();
            return;
        }
        int selected = 0;
        while (true)
        {
            system("cls");
            cout << BLUE << "Transaction History (Paid Bills):\n\n"
                 << RESET;
            cout << left
                 << setw(8) << "S.No"
                 << setw(25) << "Name"
                 << setw(25) << "Bill ID" << endl;
            cout << BLUE << "--------------------------------------------------\n"
                 << RESET;
            for (int i = 0; i < paidCount; ++i)
            {
                int idx = paidIndices[i];
                cout << (i == selected ? "-> " : "    ") << left
                     << setw(2) << i + 1
                     << setw(25) << bills[idx].getCustomerName()
                     << setw(25) << bills[idx].getId() << endl;
            }
            cout << "\nPress " << YELLOW << "Enter" << RESET << " to view, " << YELLOW << "ESC" << RESET << " to back.\n";
            int key = getch();
            if (key == 0 || key == 224)
            {
                key = getch();
                if (key == DOWN_ARROW)
                    selected = (selected + 1) % paidCount;
                else if (key == UP_ARROW)
                    selected = (selected - 1 + paidCount) % paidCount;
            }
            else if (key == ENTER_KEY)
            {
                system("cls");
                bills[paidIndices[selected]].printBill(toUpper(shopName));
                cout << "\nPress any key to return...";
                getch();
            }
            else if (key == ESC_KEY)
            {
                break;
            }
        }
    }

    void customerDetails()
    {
        system("cls");
        if (billCount == 0)
        {
            cout << RED << "No customer data found.\n"
                 << RESET;
            getch();
            return;
        }
        struct UniqueCust
        {
            string name;
            string phone;
        };
        UniqueCust uniqueList[MAX_BILLS];
        int uniqueCount = 0;
        for (int i = 0; i < billCount; i++)
        {
            string bName = bills[i].getCustomerName();
            string bPhone = bills[i].getCustomerPhone();
            if (bName.empty() || bPhone.empty())
                continue;
            bool exists = false;
            for (int j = 0; j < uniqueCount; j++)
            {
                if (uniqueList[j].name == bName && uniqueList[j].phone == bPhone)
                {
                    exists = true;
                    break;
                }
            }
            if (!exists)
            {
                uniqueList[uniqueCount].name = bName;
                uniqueList[uniqueCount].phone = bPhone;
                uniqueCount++;
            }
        }
        cout << BLUE << "--- Unique Customer Details from Bills ---\n"
             << RESET;
        cout << BLUE << "------------------------------------------\n"
             << RESET;
        cout << left << setw(30) << "Customer Name" << "Phone Number\n";
        cout << BLUE << "------------------------------------------\n"
             << RESET;
        for (int i = 0; i < uniqueCount; i++)
        {
            cout << left << setw(30) << uniqueList[i].name << uniqueList[i].phone << "\n";
        }
        cout << "\nPress any key to continue...";
        getch();
    }

    // Main Function
    void mainMenu()
    {
        const char *menu[] = {
            "Add Products",
            "View Products",
            "Update Product",
            "Remove Product",
            "Generate Bill",
            "Transaction History",
            "Unpaid Bills",
            "Customer Details",
            "Settings (Change Pass)",
            "Logout"}; 
        int selected = 0;
        int maxOptions = 10;
        while (true)
        {
            time_t now = time(0);
            tm *ltm = localtime(&now);
            char date_buffer[100];
            if (ltm)
                strftime(date_buffer, sizeof(date_buffer), "%A, %d %B %Y", ltm);
            else
                strcpy(date_buffer, "Unknown Date");
            system("cls");
            cout << BLUE << toUpper(shopName) << " BILLING SYSTEM\n"
                 << RESET;
            cout << BLUE << date_buffer << "\n\n"
                 << RESET;
            for (int i = 0; i < maxOptions; ++i)
            {
                if (i == selected)
                    cout << "-> " << i + 1 << ". " << YELLOW << menu[i] << RESET << "\n";
                else
                    cout << "   " << i + 1 << ". " << menu[i] << "\n";
            }
            cout << "\nUse Arrow Keys to Navigate, Enter to Select.";
            int key = getch();
            if (key == 0 || key == 224)
            {
                key = getch();
                if (key == DOWN_ARROW)
                    selected = (selected + 1) % maxOptions;
                else if (key == UP_ARROW)
                    selected = (selected - 1 + maxOptions) % maxOptions;
            }
            else if (key == ENTER_KEY)
            {
                switch (selected)
                {
                case 0:
                    runAddProduct();
                    break;
                case 1:
                    system("cls");
                    showProductList();
                    cout << "\nPress any key...";
                    getch();
                    break;
                case 2:
                    updateProduct();
                    break;
                case 3:
                    removeProduct();
                    break;
                case 4:
                    generateBill();
                    break;
                case 5:
                    transactionHistory();
                    break;
                case 6:
                    viewUnpaidBills();
                    break;
                case 7:
                    customerDetails();
                    break;
                case 8:
                    changeCredentials();
                    break;
                case 9:
                    return; 
                }
            }
            else if (key == ESC_KEY)
            {
                return; 
            }
        }
    }

    void run()
    {
        while (true)
        {
            system("cls");
            cout << BLUE << "========================================\n" << RESET;
            cout << BLUE << "        BILLING SYSTEM STARTUP\n" << RESET;
            cout << BLUE << "========================================\n" << RESET;
            cout << "1. Login (Existing Shop)\n";
            cout << "2. Register New Shop\n";
            cout << "3. Exit Application\n";
            cout << "\nSelect Option: ";
            
            int choice;
            if (!(cin >> choice)) {
                cin.clear(); cin.ignore(); continue;
            }

            if (choice == 1) {
                 cin.ignore();
                 cout << "Enter Shop Name: ";
                 string tempName;
                 getline(cin, tempName);
                 
                 shopName = tempName;
                 if(!loadConfig()) {
                     cout << RED << "Shop not found! Please register first.\n" << RESET;
                     shopName = ""; 
                     getch();
                 } else {
                     if (adminLogin()) {
                         loadProducts();
                         loadBills();
                         mainMenu();
                         shopName = "";
                         productCount = 0;
                         billCount = 0;
                     }
                 }
            }
            else if (choice == 2) {
                setupShop();
            }
            else if (choice == 3) {
                exit(0);
            }
        }
    }
};

int main()
{
    srand(time(0)); //random number generator
    BillingSystem app;
    app.run();
    return 0;
}