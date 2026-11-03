#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <windows.h> 
// Узел списка с пропусками
template <typename T>
class Node {
public:
    T key;                        
    std::vector<Node<T>*> forward;
    
    Node(T key, int level) : key(key), forward(level + 1, nullptr) {}

};

// Skip List — вероятностная структура данных
template <typename T>
class SkipList {
private:
    int max_level;          // Максимально возможный уровень структуры
    int current_level;      // Текущий максимальный уровень (растёт динамически)
    float p;                
    Node<T>* header;
    

    int random_level() {
        int lvl = 0;
        while (static_cast<float>(std::rand()) / RAND_MAX < p && lvl < max_level) {
            lvl++;
        }
        return lvl;
    }

public:

    SkipList(int max_lvl = 16, float probability = 0.5) 
        : max_level(max_lvl), current_level(0), p(probability) {
        header = new Node<T>(INT_MIN, max_level);
        std::srand(std::time(nullptr));  
    }
    
    ~SkipList() {
        Node<T>* current = header;
        while (current != nullptr) {
            Node<T>* next = current->forward[0];
            delete current;
            current = next;
        }
    }
    
    void insert(T key) {
        std::vector<Node<T>*> update(max_level + 1, nullptr);
        Node<T>* current = header;
        

        for (int i = current_level; i >= 0; i--) {
            // Прыгаем вперёд, пока следующий узел меньше вставляемого ключа
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            // Запоминаем последний узел перед позицией вставки на уровне i
            update[i] = current;
        }
        
        // Переходим к узлу на базовом уровне
        current = current->forward[0];
        

        if (current == nullptr || current->key != key) {
            int rlevel = random_level();  
            

            if (rlevel > current_level) {
                for (int i = current_level + 1; i <= rlevel; i++) {
                    update[i] = header;  // На новых уровнях предшественник — header
                }
                current_level = rlevel;
            }
            
            Node<T>* new_node = new Node<T>(key, rlevel);
            
            // Встраиваем узел во все уровни от 0 до rlevel
            for (int i = 0; i <= rlevel; i++) {
                new_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = new_node;
            }
            
            std::cout << "Вставлен ключ " << key << " на уровень " << rlevel << std::endl;
        } else {
            std::cout << "Ключ " << key << " уже существует" << std::endl;
        }
    }
    
    // Поиск ключа
    bool search(T key) {
        Node<T>* current = header;
        
        // Двигаемся сверху вниз
        for (int i = current_level; i >= 0; i--) {
            // Прыгаем вперёд, пока следующий узел меньше искомого
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
        }
        
        // Переходим к следующему узлу на базовом уровне
        current = current->forward[0];
        
        // Проверяем точное совпадение
        if (current != nullptr && current->key == key) {
            std::cout << "Ключ " << key << " найден" << std::endl;
            return true;
        }
        
        std::cout << "Ключ " << key << " не найден" << std::endl;
        return false;
    }
    
    // Удаление ключа
    void remove(T key) {
        std::vector<Node<T>*> update(max_level + 1, nullptr);
        Node<T>* current = header;
        
        // Ищем узел, запоминая предшественников на каждом уровне
        for (int i = current_level; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        
        current = current->forward[0];
        
        // Если узел найден — удаляем
        if (current != nullptr && current->key == key) {
            // "Сшиваем" указатели на всех уровнях, где присутствует удаляемый узел
            for (int i = 0; i <= current_level; i++) {
                // Если update[i] указывает на удаляемый узел — перенаправляем
                if (update[i]->forward[i] != current) {
                    break;  // Узел не присутствует на этом и более высоких уровнях
                }
                update[i]->forward[i] = current->forward[i];
            }
            
            // Освобождаем память
            delete current;
            
            // Уменьшаем current_level, если верхние уровни опустели
            while (current_level > 0 && header->forward[current_level] == nullptr) {
                current_level--;
            }
            
            std::cout << "Ключ " << key << " удалён" << std::endl;
        } else {
            std::cout << "Ключ " << key << " не найден для удаления" << std::endl;
        }
    }
    
    void display() {
        std::cout << "\n=== Структура Skip List ===" << std::endl;
        for (int i = current_level; i >= 0; i--) {
            std::cout << "Уровень " << i << ": ";
            Node<T>* current = header->forward[i];
            while (current != nullptr) {
                std::cout << current->key << " -> ";
                current = current->forward[i];
            }
            std::cout << "NULL" << std::endl;
        }
        std::cout << "===========================\n" << std::endl;
    }
};

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001); 
    SkipList<int> sl(16, 0.5);  // max_level=16, p=0.5
    
    sl.insert(3);
    sl.insert(7);
    sl.insert(12);
    sl.insert(19);
    sl.insert(25);
    sl.insert(30);
    
    sl.display();
    
    sl.search(19);
    sl.search(15);  
    
  
    sl.remove(12);
    sl.display();
    
    sl.search(12);  
    
    return 0;
}