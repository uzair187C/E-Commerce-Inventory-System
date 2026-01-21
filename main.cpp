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

