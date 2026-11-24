/*
 * CS250: Data Structures and Algorithms - Lab 11
 * Task 1: Huffman Coding using a Binary (Min) Heap
 *
 * Input  : N, a string S of N distinct characters, and N frequencies.
 * Output : Huffman codes printed in PREORDER traversal of the Huffman tree.
 *
 * Idea:
 *   1. Make one leaf node per character and insert all into a min-heap
 *      (ordered by frequency).
 *   2. Repeatedly extract the two smallest nodes, join them under a new
 *      parent whose frequency is their sum, and insert the parent back.
 *   3. The last node left in the heap is the root of the Huffman tree.
 *   4. Preorder traversal: going left appends '0', going right appends '1'.
 *      A leaf prints its character together with the code built so far.
 *
 * Compile : g++ -std=c++17 huffman.cpp -o huffman
 * Run     : ./huffman
 */

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ---------------------------------------------------------------
// Node of the Huffman tree
// ---------------------------------------------------------------
struct HNode {
    char ch;        // character (only meaningful for leaves)
    int freq;       // frequency (sum of children for internal nodes)
    int order;      // creation order, used to break ties deterministically
    HNode* left;
    HNode* right;

    HNode(char c, int f, int o)
        : ch(c), freq(f), order(o), left(nullptr), right(nullptr) {}
};

// ---------------------------------------------------------------
// Binary min-heap stored in a vector (array representation)
//   parent(i) = (i-1)/2, left(i) = 2i+1, right(i) = 2i+2
// ---------------------------------------------------------------
class MinHeap {
private:
    vector<HNode*> a;

    // x is "smaller" than y if it has lower frequency; ties go to the
    // node that was created first so the output is always the same.
    bool isSmaller(HNode* x, HNode* y) const {
        if (x->freq != y->freq) return x->freq < y->freq;
        return x->order < y->order;
    }

    // Move element at index i UP until the heap property is restored.
    void heapifyUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (isSmaller(a[i], a[parent])) {
                swap(a[i], a[parent]);
                i = parent;
            } else {
                break;
            }
        }
    }

    // Move element at index i DOWN until the heap property is restored.
    void heapifyDown(int i) {
        int n = (int)a.size();
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, smallest = i;
            if (l < n && isSmaller(a[l], a[smallest])) smallest = l;
            if (r < n && isSmaller(a[r], a[smallest])) smallest = r;
            if (smallest == i) break;
            swap(a[i], a[smallest]);
            i = smallest;
        }
    }

public:
    void insert(HNode* node) {
        a.push_back(node);
        heapifyUp((int)a.size() - 1);
    }

    // Remove and return the node with the smallest frequency.
    HNode* extractMin() {
        HNode* top = a[0];
        a[0] = a.back();
        a.pop_back();
        if (!a.empty()) heapifyDown(0);
        return top;
    }

    int size() const { return (int)a.size(); }
};

// ---------------------------------------------------------------
// Build the Huffman tree using the min-heap
// ---------------------------------------------------------------
HNode* buildHuffmanTree(const string& s, const vector<int>& f) {
    MinHeap heap;
    int order = 0;

    for (size_t i = 0; i < s.size(); i++)
        heap.insert(new HNode(s[i], f[i], order++));

    while (heap.size() > 1) {
        HNode* first = heap.extractMin();   // smallest  -> left child
        HNode* second = heap.extractMin();  // 2nd smallest -> right child

        HNode* parent = new HNode('$', first->freq + second->freq, order++);
        parent->left = first;
        parent->right = second;
        heap.insert(parent);
    }
    return heap.extractMin();  // root
}

// ---------------------------------------------------------------
// Preorder traversal: Root -> Left -> Right. Print code at each leaf.
// ---------------------------------------------------------------
void printCodesPreorder(HNode* root, const string& code, vector<string>& out) {
    if (root == nullptr) return;

    if (root->left == nullptr && root->right == nullptr) {
        // Special case: only one character -> give it code "0"
        string c = code.empty() ? "0" : code;
        cout << root->ch << " : " << c << "\n";
        out.push_back(c);
        return;
    }
    printCodesPreorder(root->left, code + "0", out);
    printCodesPreorder(root->right, code + "1", out);
}

// Free all nodes of the tree (postorder)
void freeTree(HNode* root) {
    if (root == nullptr) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

int main() {
    int n;
    cout << "Enter number of characters (N): ";
    if (!(cin >> n) || n <= 0) {
        cout << "Invalid N.\n";
        return 1;
    }

    string s;
    cout << "Enter the " << n << " distinct characters (no spaces, e.g. abcdef): ";
    cin >> s;
    if ((int)s.size() != n) {
        cout << "Error: string length must be exactly " << n << ".\n";
        return 1;
    }

    vector<int> freq(n);
    cout << "Enter the " << n << " frequencies (space separated): ";
    for (int i = 0; i < n; i++) {
        if (!(cin >> freq[i]) || freq[i] <= 0) {
            cout << "Error: frequencies must be positive integers.\n";
            return 1;
        }
    }

    HNode* root = buildHuffmanTree(s, freq);

    cout << "\nHuffman codes (preorder traversal):\n";
    vector<string> codes;
    printCodesPreorder(root, "", codes);

    // Same codes on a single line
    cout << "\nSingle line: ";
    for (const string& c : codes) cout << c << " ";
    cout << "\n";

    freeTree(root);
    return 0;
}
