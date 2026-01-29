#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>

using namespace std;

// ------------------------
// Product Structure
// -----------------------
struct Product {
    int id;
    string name;
    int quantity;
    float price;
    Product(int i = 0, string n = "", int q = 0, float p = 0.0f)
        : id(i), name(n), quantity(q), price(p) {}
};

// ------------------------
// Linked List Node (for Low Stock)
// ------------------------
struct LowListNode {
    Product* product;
    LowListNode* next;
    LowListNode(Product* p) : product(p), next(nullptr) {}
};

// ------------------------
// Cart Node (linked list per customer session)
// ------------------------
struct CartNode {
    int productId;
    int qty;
    CartNode* next;
    CartNode(int id, int q) : productId(id), qty(q), next(nullptr) {}
};

// ------------------------
// BST Node (for sorted inventory by name)
// ------------------------
struct BSTNode {
    Product* product;
    BSTNode* left;
    BSTNode* right;
    BSTNode(Product* p) : product(p), left(nullptr), right(nullptr) {}
};

// ------------------------
// Global Containers
// ------------------------
unordered_map<int, Product*> productMap; // id -> Product*
BSTNode* bstRoot = nullptr;              // sorted products by name
LowListNode* lowStockHead = nullptr;     // linked list for low stock
unordered_map<string, string> users;     // username -> password (simple, memory-only)

// ------------------------
// Utility: safe input helpers
// ------------------------
bool readInt(int &out) {
    if (!(cin >> out)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return false;
    }
    return true;
}
bool readFloat(float &out) {
    if (!(cin >> out)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return false;
    }
    return true;
}
bool readString(string &out) {
    if (!(cin >> out)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return false;
    }
    return true;
}

// ------------------------
// BST Helpers
// ------------------------
BSTNode* insertBST(BSTNode* root, Product* p) {
    if (!root) return new BSTNode(p);
    if (p->name < root->product->name) root->left = insertBST(root->left, p);
    else root->right = insertBST(root->right, p);
    return root;
}

// find minimum node in subtree
BSTNode* findMinNode(BSTNode* node) {
    BSTNode* cur = node;
    while (cur && cur->left) cur = cur->left;
    return cur;
}

// delete node by product name (does NOT delete Product* pointer)
// returns new root
BSTNode* deleteBSTByName(BSTNode* root, const string &name) {
    if (!root) return root;
    if (name < root->product->name) {
        root->left = deleteBSTByName(root->left, name);
    } else if (name > root->product->name) {
        root->right = deleteBSTByName(root->right, name);
    } else {
        // found node to delete
        if (!root->left && !root->right) {
            delete root;
            return nullptr;
        } else if (!root->left) {
            BSTNode* tmp = root->right;
            delete root;
            return tmp;
        } else if (!root->right) {
            BSTNode* tmp = root->left;
            delete root;
            return tmp;
        } else {
            BSTNode* tmp = findMinNode(root->right);
            // move product pointer (we don't delete Product* here)
            root->product = tmp->product;
            // delete the inorder successor node (by name)
            root->right = deleteBSTByName(root->right, tmp->product->name);
        }
    }
    return root;
}

// inorder traversal printing active products (productMap is the authority)
void inorderBST(BSTNode* root) {
    if (!root) return;
    inorderBST(root->left);
    Product* p = root->product;
    // Only print if product exists in productMap and matches same name/id (active)
    if (p && productMap.find(p->id) != productMap.end() && productMap[p->id]->name == p->name) {
        cout << "ID: " << p->id
             << " | Name: " << p->name
             << " | Qty: " << p->quantity
             << " | Price: " << p->price;
        if (p->quantity <= 0) cout << "  [OUT OF STOCK]";
        cout << endl;
    }
    inorderBST(root->right);
}

void freeBST(BSTNode* root) {
    if (!root) return;
    freeBST(root->left);
    freeBST(root->right);
    delete root;
}

// ------------------------
// Low stock helper
// ------------------------
void addToLowStock(Product* p) {
    // allow duplicates in lowStock list; display validates at runtime
    LowListNode* node = new LowListNode(p);
    node->next = lowStockHead;
    lowStockHead = node;
}

// ------------------------
// Product operations
// ------------------------
bool nameExists(const string &name) {
    for (auto &pr : productMap) {
        Product* p = pr.second;
        if (p && p->name == name) return true;
    }
    return false;
}

void addProduct(int id, const string &name, int qty, float price) {
    if (productMap.find(id) != productMap.end()) {
        cout << "Product ID already exists!\n";
        return;
    }
    if (nameExists(name)) {
        cout << "Product name already exists!\n";
        return;
    }
    Product* p = new Product(id, name, qty, price);
    productMap[id] = p;
    bstRoot = insertBST(bstRoot, p);
    if (qty > 0 && qty < 5) addToLowStock(p);
    cout << "Product added successfully!\n";
}

void deleteProductById(int id) {
    auto it = productMap.find(id);
    if (it == productMap.end()) {
        cout << "Product not found!\n";
        return;
    }
    Product* p = it->second;
    // remove from BST by name (BST nodes won't manage Product* lifetime)
    bstRoot = deleteBSTByName(bstRoot, p->name);
    // remove from productMap and free memory
    delete p;
    productMap.erase(it);
    cout << "Product deleted successfully (physically removed).\n";
}

void viewSortedProducts() {
    if (productMap.empty()) {
        cout << "No products available!\n";
        return;
    }
    cout << "===== Sorted Inventory =====" << endl;
    inorderBST(bstRoot);
}

void displayLowStock() {
    bool any = false;
    cout << "===== Low Stock Products (Qty < 5) =====" << endl;
    LowListNode* temp = lowStockHead;
    while (temp) {
        Product* p = temp->product;
        // validate current product still exists and qty < 5 and >0
        if (p && productMap.find(p->id) != productMap.end()) {
            Product* cur = productMap[p->id];
            if (cur->quantity > 0 && cur->quantity < 5) {
                cout << "ID: " << cur->id << " | Name: " << cur->name
                     << " | Qty: " << cur->quantity << endl;
                any = true;
            }
        }
        temp = temp->next;
    }
    if (!any) cout << "No low stock products!\n";
}

void searchProduct(int id) {
    auto it = productMap.find(id);
    if (it != productMap.end()) {
        Product* p = it->second;
        cout << "Found Product -> "
             << "ID: " << p->id
             << " | Name: " << p->name
             << " | Qty: " << p->quantity
             << " | Price: " << p->price << endl;
    } else {
        cout << "Product not found!" << endl;
    }
}

void updateStock(int id, int newQty) {
    auto it = productMap.find(id);
    if (it == productMap.end()) {
        cout << "Product not found!\n";
        return;
    }
    Product* p = it->second;
    p->quantity = newQty;
    if (newQty > 0 && newQty < 5) addToLowStock(p);
    cout << "Stock updated successfully!\n";
}

// When buying: reduce product qty by amountBought (usually 1 or cart qty).
// If product becomes zero, remove product entirely (so customers won't see it).
void reduceStockOnBuy(int id, int amountBought) {
    auto it = productMap.find(id);
    if (it == productMap.end()) {
        cout << "Product not found!\n";
        return;
    }
    Product* p = it->second;
    if (p->quantity <= 0) {
        cout << "Out of stock!\n";
        return;
    }
    if (amountBought <= 0) amountBought = 1;
    if (amountBought >= p->quantity) {
        // buy all available -> delete product
        cout << "Bought " << p->name << " x" << p->quantity << " (all remaining). Product removed.\n";
        deleteProductById(p->id);
    } else {
        p->quantity -= amountBought;
        cout << "Bought " << p->name << " x" << amountBought << ". Remaining: " << p->quantity << "\n";
        if (p->quantity < 5 && p->quantity > 0) addToLowStock(p);
    }
}

// ------------------------
// File Handling (save/load only active products)
// ------------------------
void saveToFile(const string &filename) {
    ofstream fout(filename);
    if (!fout.is_open()) {
        cout << "Error saving to file!\n";
        return;
    }
    for (auto &pr : productMap) {
        Product* p = pr.second;
        if (p) fout << p->id << " " << p->name << " " << p->quantity << " " << p->price << "\n";
    }
    fout.close();
    cout << "Data saved to " << filename << endl;
}

void loadFromFile(const string &filename) {
    ifstream fin(filename);
    if (!fin.is_open()) return;
    int id, qty;
    float price;
    string name;
    while (fin >> id >> name >> qty >> price) {
        // avoid duplicate IDs or names during load
        if (productMap.find(id) == productMap.end() && !nameExists(name)) {
            Product* p = new Product(id, name, qty, price);
            productMap[id] = p;
            bstRoot = insertBST(bstRoot, p);
            if (qty > 0 && qty < 5) addToLowStock(p);
        }
    }
    fin.close();
}

// ------------------------
// Cart linked-list helpers (per customer session)
// ------------------------
void addToCart(CartNode* &cartHead, int productId, int qty) {
    if (productMap.find(productId) == productMap.end()) {
        cout << "Invalid Product ID.\n";
        return;
    }
    if (qty <= 0) {
        cout << "Quantity must be positive.\n";
        return;
    }
    // if already in cart, increase qty
    CartNode* temp = cartHead;
    while (temp) {
        if (temp->productId == productId) {
            temp->qty += qty;
            cout << "Added more to existing cart item.\n";
            return;
        }
        temp = temp->next;
    }
    // else push front
    CartNode* node = new CartNode(productId, qty);
    node->next = cartHead;
    cartHead = node;
    cout << "Product added to cart.\n";
}

void viewCart(CartNode* cartHead) {
    if (!cartHead) {
        cout << "Cart is empty.\n";
        return;
    }
    cout << "===== Your Cart =====\n";
    CartNode* temp = cartHead;
    while (temp) {
        int id = temp->productId;
        int qty = temp->qty;
        if (productMap.find(id) != productMap.end()) {
            Product* p = productMap[id];
            cout << "ID: " << id << " | Name: " << p->name << " | Qty in Cart: " << qty
                 << " | Available: " << p->quantity << " | UnitPrice: " << p->price;
            if (p->quantity <= 0) cout << " [OUT OF STOCK]";
            cout << endl;
        } else {
            cout << "ID: " << id << " | Product removed from store.\n";
        }
        temp = temp->next;
    }
}

void clearCart(CartNode* &cartHead) {
    CartNode* cur = cartHead;
    while (cur) {
        CartNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    cartHead = nullptr;
}

// Checkout: try to buy all items in cart (partial buys allowed). After processing, clears cart.
void checkoutCart(CartNode* &cartHead) {
    if (!cartHead) {
        cout << "Cart empty.\n";
        return;
    }
    CartNode* cur = cartHead;
    while (cur) {
        int id = cur->productId;
        int want = cur->qty;
        if (productMap.find(id) == productMap.end()) {
            cout << "Skipping ID " << id << " — removed from store.\n";
            cur = cur->next;
            continue;
        }
        Product* p = productMap[id];
        if (p->quantity <= 0) {
            cout << "Skipping " << p->name << " — out of stock.\n";
        } else if (p->quantity < want) {
            cout << "Partial buy for " << p->name << ". Buying " << p->quantity << " available.\n";
            // buy all remaining -> delete product
            deleteProductById(id);
        } else {
            // buy exactly want
            reduceStockOnBuy(id, want);
        }
        cur = cur->next;
    }
    clearCart(cartHead);
    cout << "Checkout finished. Cart cleared.\n";
}

// Buy single product immediately
void buySingle(int productId) {
    if (productMap.find(productId) == productMap.end()) {
        cout << "Product not found.\n";
        return;
    }
    reduceStockOnBuy(productId, 1);
}

// ------------------------
// Menus
// ------------------------
void adminMenu() {
    const string ADMIN_PASS = "admin123";
    cout << "Enter admin password: ";
    string pass;
    if (!readString(pass)) { cout << "Invalid input.\n"; return; }
    if (pass != ADMIN_PASS) {
        cout << "Wrong password!\n";
        return;
    }

    while (true) {
        cout << "\n--- ADMIN MENU ---\n";
        cout << "1. Add Product\n2. View Inventory (sorted)\n3. View Low Stock\n4. Delete Product\n5. Update Stock / Restock\n6. Save\n7. Back\n";
        cout << "Enter choice: ";
        int choice;
        if (!readInt(choice)) continue;

        if (choice == 1) {
            cout << "Enter ID Name Quantity Price: ";
            int id, qty; string name; float price;
            if (!readInt(id)) { cout << "Invalid ID.\n"; continue; }
            if (!readString(name)) { cout << "Invalid name.\n"; continue; }
            if (!readInt(qty)) { cout << "Invalid qty.\n"; continue; }
            if (!readFloat(price)) { cout << "Invalid price.\n"; continue; }
            addProduct(id, name, qty, price);
        } else if (choice == 2) {
            viewSortedProducts();
        } else if (choice == 3) {
            displayLowStock();
        } else if (choice == 4) {
            cout << "Enter Product ID to delete: ";
            int id; if (!readInt(id)) { cout << "Invalid id.\n"; continue; }
            deleteProductById(id);
        } else if (choice == 5) {
            cout << "Enter Product ID and new quantity: ";
            int id, q; if (!readInt(id) || !readInt(q)) { cout << "Invalid input.\n"; continue; }
            updateStock(id, q);
        } else if (choice == 6) {
            saveToFile("inventory.txt");
        } else if (choice == 7) {
            cout << "Returning to main menu.\n";
            break;
        } else {
            cout << "Invalid choice!\n";
        }
    }
}

void customerMenu() {
    while (true) {
        cout << "\n--- CUSTOMER AUTH ---\n";
        cout << "1. Register\n2. Login\n3. Back\n";
        cout << "Enter choice: ";
        int ch;
        if (!readInt(ch)) continue;

        if (ch == 1) {
            cout << "Enter username password: ";
            string u, p; if (!readString(u) || !readString(p)) { cout << "Invalid.\n"; continue; }
            if (users.find(u) != users.end()) {
                cout << "Username already exists!\n";
            } else {
                users[u] = p;
                cout << "Registered. Now login.\n";
            }
        } else if (ch == 2) {
            cout << "Enter username password: ";
            string u, p; if (!readString(u) || !readString(p)) { cout << "Invalid.\n"; continue; }
            if (users.find(u) != users.end() && users[u] == p) {
                cout << "Login successful. Welcome " << u << "!\n";
                // customer session
                CartNode* cartHead = nullptr;
                while (true) {
                    cout << "\n--- CUSTOMER MENU ---\n";
                    cout << "1. View Products\n2. Add to Cart\n3. View Cart\n4. Checkout Cart\n5. Buy Single Product Now\n6. Search Product by ID\n7. Logout\n";
                    cout << "Enter choice: ";
                    int c;
                    if (!readInt(c)) continue;

                    if (c == 1) {
                        viewSortedProducts();
                    } else if (c == 2) {
                        cout << "Enter Product ID and quantity to add: ";
                        int id, q; if (!readInt(id) || !readInt(q)) { cout << "Invalid.\n"; continue; }
                        addToCart(cartHead, id, q);
                    } else if (c == 3) {
                        viewCart(cartHead);
                    } else if (c == 4) {
                        checkoutCart(cartHead);
                    } else if (c == 5) {
                        cout << "Enter Product ID to buy (one unit): ";
                        int id; if (!readInt(id)) { cout << "Invalid id.\n"; continue; }
                        buySingle(id);
                    } else if (c == 6) {
                        cout << "Enter Product ID: ";
                        int id; if (!readInt(id)) { cout << "Invalid.\n"; continue; }
                        searchProduct(id);
                    } else if (c == 7) {
                        clearCart(cartHead);
                        cout << "Logged out.\n";
                        break;
                    } else {
                        cout << "Invalid choice!\n";
                    }
                }
            } else {
                cout << "Invalid credentials!\n";
            }
        } else if (ch == 3) {
            return;
        } else {
            cout << "Invalid choice!\n";
        }
    }
}

// ------------------------
// Program start
// ------------------------
void seedDefaultsIfEmpty() {
    if (!productMap.empty()) return;
    addProduct(101, "Laptop", 10, 120000.0f);
    addProduct(102, "Mouse", 3, 1500.0f);
    addProduct(103, "Keyboard", 2, 3000.0f);
}

int main() {
    // Load from file if exists
    loadFromFile("inventory.txt");

    // seed defaults if none loaded
    seedDefaultsIfEmpty();

    // some dummy users for testing
    users["user1"] = "pass1";
    users["user2"] = "pass2";

    cout << "Total products in inventory: " << productMap.size() << endl;

    while (true) {
        cout << "\n===== INVENTORY SYSTEM =====\n";
        cout << "1. Admin\n2. Customer\n3. Save & Exit\n";
        cout << "Enter choice: ";
        int choice;
        if (!readInt(choice)) continue;

        if (choice == 1) adminMenu();
        else if (choice == 2) customerMenu();
        else if (choice == 3) {
            saveToFile("inventory.txt");
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice!\n";
        }
    }

    // cleanup product memory
    for (auto &pr : productMap) delete pr.second;
    productMap.clear();

    // free BST nodes
    freeBST(bstRoot);
    bstRoot = nullptr;

    // free low stock list nodes
    LowListNode* cur = lowStockHead;
    while (cur) {
        LowListNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    lowStockHead = nullptr;

    return 0;
}
