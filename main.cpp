#include <iostream>
#include <unordered_map>
#include <string>

#include <fstream>

using namespace std;

// ------------------------
// Product Structure
// ------------------------
struct Product {
    int id;
    string name;
    int quantity;
    float price;
};

// ------------------------
// Linked List Node (for Low Stock / Cart)
// ------------------------
struct ListNode {
    Product* product;
    ListNode* next;
    ListNode(Product* p) : product(p), next(nullptr) {}
};

// ------------------------
// BST Node (for sorted inventory)
// ------------------------
struct BSTNode {
    Product* product;
    BSTNode* left;
    BSTNode* right;
    BSTNode(Product* p) : product(p), left(nullptr), right(nullptr) {}
};

// ------------------------
// Inventory Containers
// ------------------------
unordered_map<int, Product*> productMap; // fast lookup by ID
BSTNode* bstRoot = nullptr;              // sorted products
ListNode* lowStockHead = nullptr;        // linked list for low stock








// ---------------------
// ------------------------
// Linked List Helper
// ------------------------
void addToLowStock(Product* p) {
    ListNode* newNode = new ListNode(p);
    newNode->next = lowStockHead;
    lowStockHead = newNode;
}

// ------------------------
// BST Helper: Insert
// ------------------------
BSTNode* insertBST(BSTNode* root, Product* p) {
    if (!root) return new BSTNode(p);
    if (p->name < root->product->name) root->left = insertBST(root->left, p);
    else root->right = insertBST(root->right, p);
    return root;
}

// ------------------------
// Add Product
// ------------------------
void addProduct(int id, string name, int qty, float price) {
    if (productMap.find(id) != productMap.end()) {
        cout << "Product ID already exists!" << endl;
        return;
    }

    Product* p = new Product{id, name, qty, price};
    productMap[id] = p;           // fast lookup
    bstRoot = insertBST(bstRoot, p); // sorted BST
    if (qty < 5) addToLowStock(p);  // low stock linked list

    cout << "Product added successfully!" << endl;
} 

// ------------------------
// In-order BST Traversal
// ------------------------
void inorderBST(BSTNode* root) {
    if (!root) return;
    inorderBST(root->left);
    cout << "ID: " << root->product->id
         << " | Name: " << root->product->name
         << " | Qty: " << root->product->quantity
         << " | Price: " << root->product->price << endl;
    inorderBST(root->right);
}

// Helper function to call
void viewSortedProducts() {
    if (!bstRoot) {
        cout << "No products available!" << endl;
        return;
    }
    cout << "===== Sorted Inventory =====" << endl;
    inorderBST(bstRoot);
}

void displayLowStock() {
    if (!lowStockHead) {
        cout << "No low stock products!" << endl;
        return;
    }
    cout << "===== Low Stock Products =====" << endl;
    ListNode* temp = lowStockHead;
    while (temp) {
        cout << "ID: " << temp->product->id
             << " | Name: " << temp->product->name
             << " | Qty: " << temp->product->quantity << endl;
        temp = temp->next;
    }
}

// ------------------------
// Search by ID
// ------------------------
void searchProduct(int id) {
    if (productMap.find(id) != productMap.end()) {
        Product* p = productMap[id];
        cout << "Found Product -> "
             << "ID: " << p->id
             << " | Name: " << p->name
             << " | Qty: " << p->quantity
             << " | Price: " << p->price << endl;
    } else cout << "Product not found!" << endl;
}
// ------------------------
// Update Stock
// ------------------------
void updateStock(int id, int newQty) {
    if (productMap.find(id) == productMap.end()) {
        cout << "Product not found!" << endl;
        return;
    }

    Product* p = productMap[id];
    p->quantity = newQty;

    // Update low stock linked list
    if (newQty < 5) addToLowStock(p);

    cout << "Stock updated successfully!" << endl;
}

// ------------------------
// File Handling
// ------------------------
void saveToFile(string filename) {
    ofstream file(filename);
    for (auto &pair : productMap) {
        Product* p = pair.second;
        file << p->id << " " << p->name << " " << p->quantity << " " << p->price << endl;
    }
    file.close();
    cout << "Data saved to " << filename << endl;
}

void loadFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    
 int id, qty;
    float price;
    string name;
    while (file >> id >> name >> qty >> price) {
        addProduct(id, name, qty, price);
    }
    file.close();
}
// ------------------------
// Menu UI
// ------------------------
void menu() {
    int choice;
    do {
        cout << "\n===== INVENTORY SYSTEM =====\n";
        cout << "1. Add Product\n2. View Sorted Inventory\n3. View Low Stock\n";
        cout << "4. Search Product by ID\n5. Update Stock\n6. Save & Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        int id, qty;
        string name;
        float price;

        switch(choice) {
            case 1:
    cout << "Enter ID Name Quantity Price (space separated): ";
    cin >> id >> name >> qty >> price;
    if(cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Use spaces only.\n";
        break;
    }
    addProduct(id, name, qty, price);
    break;

            case 2: viewSortedProducts(); break;
            case 3: displayLowStock(); break;
            case 4:
                cout << "Enter Product ID to search: "; cin >> id;
                searchProduct(id);
                break;
            case 5:
                cout << "Enter Product ID and new Quantity: "; cin >> id >> qty;
                updateStock(id, qty);
                break;
            case 6:
                saveToFile("inventory.txt");
                cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 6);
}

   int main() {
    loadFromFile("inventory.txt"); // load saved products first

    // Optional: add test products if file is empty
    if (productMap.empty()) {
        addProduct(101, "Laptop", 10, 120000);
        addProduct(102, "Mouse", 3, 1500);
        addProduct(103, "Keyboard", 2, 3000);
    }

    cout << "Total products in inventory: " << productMap.size() << endl;

    menu();
    return 0;
}


