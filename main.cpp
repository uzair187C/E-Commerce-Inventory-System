#include <iostream>
#include <unordered_map>
#include <string>
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
int main() {
    addProduct(101, "Laptop", 10, 120000);
    addProduct(102, "Mouse", 3, 1500); // will appear in low-stock
    addProduct(103, "Keyboard", 2, 3000); // low-stock too

    cout << "Total products in hash map: " << productMap.size() << endl;

    return 0;
}


