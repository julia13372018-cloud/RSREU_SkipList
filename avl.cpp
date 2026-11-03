#include <iostream>
#include <algorithm>

template <typename T>
class AVLTree {
private:
    struct Node {
        T key;
        Node* left;
        Node* right;
        int height;
        
        Node(T k) : key(k), left(nullptr), right(nullptr), height(1) {}
    };
    
    Node* root;
    size_t node_count;  // Для подсчёта памяти
    
    // ===== Вспомогательные методы ====
    
    int height(Node* n) {
        return n ? n->height : 0;
    }
    
    int getBalance(Node* n) {
        return n ? height(n->left) - height(n->right) : 0;
    }
    
    void updateHeight(Node* n) {
        n->height = 1 + std::max(height(n->left), height(n->right));
    }
    
    // Малый правый поворот
    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    // Малый левый поворот
    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    
    // Минимальный узел в поддереве (для удаления)
    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }
    
    // ===== Рекурсивные методы =====
    
    Node* insertNode(Node* node, T key, bool& inserted) {
        // 1. Стандартная вставка в BST
        if (node == nullptr) {
            inserted = true;
            node_count++;
            return new Node(key);
        }
        
        if (key < node->key) {
            node->left = insertNode(node->left, key, inserted);
        } else if (key > node->key) {
            node->right = insertNode(node->right, key, inserted);
        } else {
            // Дубликаты игнорируем (как в Skip List)
            inserted = false;
            return node;
        }
        
        // 2. Обновляем высоту
        updateHeight(node);
        
        // 3. Проверяем баланс и делаем повороты
        int balance = getBalance(node);
        
        // LL-случай
        if (balance > 1 && key < node->left->key)
            return rightRotate(node);
        
        // RR-случай
        if (balance < -1 && key > node->right->key)
            return leftRotate(node);
        
        // LR-случай
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        
        // RL-случай
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        
        return node;
    }
    
    Node* removeNode(Node* root, T key, bool& removed) {
        if (root == nullptr) return nullptr;
        
        if (key < root->key) {
            root->left = removeNode(root->left, key, removed);
        } else if (key > root->key) {
            root->right = removeNode(root->right, key, removed);
        } else {
            // Узел найден
            if (root->left == nullptr || root->right == nullptr) {
                Node* temp = root->left ? root->left : root->right;
                if (temp == nullptr) {
                    temp = root;
                    root = nullptr;
                } else {
                    *root = *temp;
                }
                delete temp;
                node_count--;
                removed = true;
            } else {
                Node* temp = minValueNode(root->right);
                root->key = temp->key;
                root->right = removeNode(root->right, temp->key, removed);
            }
        }
        
        if (root == nullptr) return nullptr;
        
        updateHeight(root);
        
        int balance = getBalance(root);
        
        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);
        
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
        
        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);
        
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }
        
        return root;
    }
    
    bool searchNode(Node* root, T key) const {
        if (root == nullptr) return false;
        if (key < root->key) return searchNode(root->left, key);
        if (key > root->key) return searchNode(root->right, key);
        return true;
    }
    
    void destroyTree(Node* node) {
        if (node == nullptr) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    AVLTree() : root(nullptr), node_count(0) {}
    
    ~AVLTree() {
        destroyTree(root);
    }
    
    // ===== ПУБЛИЧНЫЙ ИНТЕРФЕЙС (одинаковый со Skip List) =====
    
    void insert(T key) {
        bool inserted = false;
        root = insertNode(root, key, inserted);
        if (!inserted) {
            std::cout << "Ключ " << key << " уже существует" << std::endl;
        }
    }
    
    bool search(T key) const {
        bool found = searchNode(root, key);
        std::cout << "Ключ " << key << (found ? " найден" : " не найден") << std::endl;
        return found;
    }
    
    void remove(T key) {
        bool removed = false;
        root = removeNode(root, key, removed);
        if (removed) {
            std::cout << "Ключ " << key << " удалён" << std::endl;
        } else {
            std::cout << "Ключ " << key << " не найден для удаления" << std::endl;
        }
    }
    
    // Подсчёт памяти (важно для бенчмарка!)
    size_t estimate_memory() const {
        // sizeof(Node) = sizeof(T) + 2 указателя + int height + выравнивание
        // Для int: 4 + 8 + 8 + 4 = 24 байта, с выравниванием обычно 32 байта
        return node_count * sizeof(Node);
    }
    
    size_t size() const {
        return node_count;
    }
    
    int get_height() const {
        return height(root);
    }
};