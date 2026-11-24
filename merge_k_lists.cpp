/*
 * CS250: Data Structures and Algorithms - Lab 11
 * Task 2: Merge K Sorted Linked Lists using a Binary (Min) Heap
 *
 * Input  : k, then for each list its size followed by its (ascending) values.
 * Output : the single merged sorted linked list.
 *
 * Idea:
 *   1. Put the head node of every non-empty list into a min-heap
 *      (ordered by node value). The heap never holds more than k nodes.
 *   2. Repeatedly extract the smallest node, append it to the result list,
 *      and push that node's next node (if any) into the heap.
 *   3. Stop when the heap is empty.
 *
 * Time  : O(N log k)   (N = total nodes, each heap operation costs log k)
 * Space : O(k) extra   (the heap); nodes are re-linked, not copied.
 *
 * Compile : g++ -std=c++17 merge_k_lists.cpp -o merge_k_lists
 * Run     : ./merge_k_lists
 */

#include <iostream>
#include <vector>
using namespace std;

// ---------------------------------------------------------------
// Singly linked list node
// ---------------------------------------------------------------
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v) : val(v), next(nullptr) {}
};

// ---------------------------------------------------------------
// Binary min-heap of ListNode pointers (ordered by val)
// ---------------------------------------------------------------
class MinHeap {
private:
    vector<ListNode*> a;

    void heapifyUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (a[i]->val < a[parent]->val) {
                swap(a[i], a[parent]);
                i = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int i) {
        int n = (int)a.size();
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, smallest = i;
            if (l < n && a[l]->val < a[smallest]->val) smallest = l;
            if (r < n && a[r]->val < a[smallest]->val) smallest = r;
            if (smallest == i) break;
            swap(a[i], a[smallest]);
            i = smallest;
        }
    }

public:
    bool empty() const { return a.empty(); }

    void insert(ListNode* node) {
        a.push_back(node);
        heapifyUp((int)a.size() - 1);
    }

    ListNode* extractMin() {
        ListNode* top = a[0];
        a[0] = a.back();
        a.pop_back();
        if (!a.empty()) heapifyDown(0);
        return top;
    }
};

// ---------------------------------------------------------------
// Merge all k lists into one sorted list
// ---------------------------------------------------------------
ListNode* mergeKLists(vector<ListNode*>& lists) {
    MinHeap heap;

    // Step 1: push the head of every non-empty list
    for (ListNode* head : lists)
        if (head != nullptr) heap.insert(head);

    // Dummy node makes appending to the result simple (no special first case)
    ListNode dummy(0);
    ListNode* tail = &dummy;

    // Step 2: extract min, append it, push its successor
    while (!heap.empty()) {
        ListNode* smallest = heap.extractMin();
        tail->next = smallest;
        tail = smallest;
        if (smallest->next != nullptr) heap.insert(smallest->next);
    }
    tail->next = nullptr;
    return dummy.next;
}

// ---------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------
ListNode* buildList(const vector<int>& values) {
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    for (int v : values) {
        ListNode* node = new ListNode(v);
        if (head == nullptr) head = tail = node;
        else { tail->next = node; tail = node; }
    }
    return head;
}

void printList(ListNode* head) {
    if (head == nullptr) { cout << "(empty)\n"; return; }
    for (ListNode* cur = head; cur != nullptr; cur = cur->next) {
        cout << cur->val;
        if (cur->next != nullptr) cout << " -> ";
    }
    cout << " -> NULL\n";
}

void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* nxt = head->next;
        delete head;
        head = nxt;
    }
}

int main() {
    int k;
    cout << "Enter number of linked lists (k): ";
    if (!(cin >> k) || k < 0) {
        cout << "Invalid k.\n";
        return 1;
    }

    vector<ListNode*> lists;
    for (int i = 0; i < k; i++) {
        int size;
        cout << "List " << i + 1 << " - enter size: ";
        if (!(cin >> size) || size < 0) {
            cout << "Invalid size.\n";
            return 1;
        }

        vector<int> values(size);
        if (size > 0) cout << "List " << i + 1 << " - enter " << size
                           << " values in ascending order: ";
        for (int j = 0; j < size; j++) cin >> values[j];

        // Check the "sorted" assumption so wrong input does not silently
        // produce a wrong answer.
        for (int j = 1; j < size; j++) {
            if (values[j] < values[j - 1]) {
                cout << "Error: list " << i + 1 << " is not sorted ascending.\n";
                return 1;
            }
        }
        lists.push_back(buildList(values));
    }

    cout << "\nInput lists:\n";
    for (int i = 0; i < k; i++) {
        cout << "  List " << i + 1 << ": ";
        printList(lists[i]);
    }

    ListNode* merged = mergeKLists(lists);

    cout << "\nMerged sorted list:\n  ";
    printList(merged);

    freeList(merged);  // all nodes now belong to the merged list
    return 0;
}
