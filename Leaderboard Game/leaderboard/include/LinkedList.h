#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "RegisteredPlayer.h"
#include <stdexcept>

// ============================================================
// STRUKTUR DATA MANUAL: Linked List tanpa std::list/vector
// BONUS: implementasi manual untuk nilai tambahan
// Kompleksitas:
//   - append: O(1)
//   - getAt:  O(n)
//   - size:   O(1)  [tracked via counter]
// ============================================================

struct Node {
    RegisteredPlayer* data;
    Node* next;
    Node(RegisteredPlayer* p) : data(p), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
    Node* tail;
    int count;

public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~LinkedList() {
        Node* curr = head;
        while (curr) {
            Node* tmp = curr->next;
            delete curr->data;
            delete curr;
            curr = tmp;
        }
    }

    // Insert di akhir — O(1)
    void append(RegisteredPlayer* player) {
        Node* node = new Node(player);
        if (!tail) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        count++;
    }

    // Akses elemen ke-i — O(n)
    RegisteredPlayer* getAt(int index) const {
        if (index < 0 || index >= count)
            throw std::out_of_range("Index out of range");
        Node* curr = head;
        for (int i = 0; i < index; i++) curr = curr->next;
        return curr->data;
    }

    // Rebuild list dari array sementara (dipakai setelah sorting)
    void rebuildFrom(RegisteredPlayer** arr, int size) {
        Node* curr = head;
        while (curr) {
            Node* tmp = curr->next;
            delete curr;
            curr = tmp;
        }
        head = tail = nullptr;
        count = 0;

        for (int i = 0; i < size; i++) {
            Node* node = new Node(arr[i]);
            if (!tail) {
                head = tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
            count++;
        }
    }

    int size() const { return count; }
    Node* getHead() const { return head; }
};

#endif
