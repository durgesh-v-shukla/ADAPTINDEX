#pragma once
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// --- Content from core/Student.h ---
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <string>
#include <iostream>
#include <iomanip>
using namespace std;

// The fundamental data record stored and retrieved by every index.
struct Student {
    int roll_no;
    string name;
    int marks;
    string department;

    // Prints one formatted row to terminal
    void print() const {
        cout << "| Roll: " << setfill('0') << setw(3) << roll_no << setfill(' ')
             << " | Name: " << left << setw(20) << name
             << " | Marks: " << right << setw(3) << marks
             << " | Dept: " << left << setw(6) << department << " |\n";
    }
};


// --- Content from core/Index.h ---
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <vector>
#include <string>
using namespace std;

// The abstract polymorphic base class.
// Every data structure inherits this. This is what allows the MigrationEngine
// to swap one DS for another without the rest of the system noticing.
class Index {
public:
    virtual void insert(Student s) = 0;
    virtual vector<Student> search(int roll_no) = 0;
    virtual vector<Student> rangeQuery(int low_marks, int high_marks) = 0;
    virtual vector<Student> prefixSearch(string prefix) = 0;

    // Returns ALL records - used by MigrationEngine to dump data before switching
    virtual vector<Student> getAllRecords() = 0;

    virtual string name() = 0;   // e.g. "AVL Tree", "B+ Tree"
    virtual int size() = 0;      // number of records currently stored
    virtual ~Index() {}
};


// --- Content from indexes/AVLTree.h ---
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Self-balancing BST
struct AVLNode {
    Student data;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(Student s) : data(s), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree : public Index {
public:
    AVLTree();
    ~AVLTree();

    void insert(Student s) override;
    vector<Student> search(int roll_no) override;
    vector<Student> rangeQuery(int low_marks, int high_marks) override;
    vector<Student> prefixSearch(string prefix) override;
    vector<Student> getAllRecords() override;

    string name() override;
    int size() override;

private:
    AVLNode* root;
    int count;

    int height(AVLNode* n);
    int balanceFactor(AVLNode* n);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* rebalance(AVLNode* n);
    AVLNode* insertNode(AVLNode* node, Student s);
    void inorderCollect(AVLNode* n, vector<Student>& res);
    
    // extra helpers manually matching criteria
    void searchHelp(AVLNode* n, int roll, vector<Student>& res);
    void rangeHelp(AVLNode* n, int low, int high, vector<Student>& res);
    string toLower(string s);
    void prefixHelp(AVLNode* n, string prefix, vector<Student>& res);
    void deleteTree(AVLNode* n);
};


// --- Content from indexes/BPlusTree.h ---
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Order 4: maximum 3 keys per node, 4 children per internal node
const int ORDER = 4;

struct BPlusNode {
    bool isLeaf;
    vector<int> keys;            // marks values used as routing keys
    vector<Student> records;     // ONLY in leaf nodes: actual student data
    vector<BPlusNode*> children; // ONLY in internal nodes: child pointers
    BPlusNode* next;             // ONLY in leaf nodes: pointer to next leaf

    BPlusNode(bool leaf) : isLeaf(leaf), next(nullptr) {}
};

class BPlusTree : public Index {
public:
    BPlusTree();
    ~BPlusTree();

    void insert(Student s) override;
    vector<Student> search(int roll_no) override;
    vector<Student> rangeQuery(int low_marks, int high_marks) override;
    vector<Student> prefixSearch(string prefix) override;
    vector<Student> getAllRecords() override;

    string name() override;
    int size() override;

private:
    BPlusNode* root;
    int count;

    BPlusNode* findLeaf(int key);
    void insertIntoLeaf(BPlusNode* leaf, int key, Student s);
    void splitLeaf(BPlusNode* parent, int childIndex, BPlusNode* leaf);
    void insertIntoInternal(BPlusNode* parent, int childIndex, int key, BPlusNode* newChild);
    void splitInternal(BPlusNode* parent, int childIndex, BPlusNode* child);
    BPlusNode* getFirstLeaf();
    void deleteTree(BPlusNode* node);
};


// --- Content from indexes/Trie.h ---
#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct TrieNode {
    TrieNode* children[26];
    bool isEnd;
    Student data;
    TrieNode() : isEnd(false) {
        fill(children, children + 26, nullptr);
    }
};

class Trie : public Index {
public:
    Trie();
    ~Trie();

    void insert(Student s) override;
    vector<Student> search(int roll_no) override;
    vector<Student> rangeQuery(int low_marks, int high_marks) override;
    vector<Student> prefixSearch(string prefix) override;
    vector<Student> getAllRecords() override;

    string name() override;
    int size() override;

private:
    TrieNode* root;
    int count;

    void dfs(TrieNode* node, vector<Student>& result);
    string toLowerAlpha(string s);
    void deleteNode(TrieNode* node);
};


// --- Content from indexes/SkipList.h ---
#include <iostream>
#include <vector>
#include <string>
using namespace std;

const int MAX_LEVEL = 4;
const float PROB = 0.5f;

struct SkipNode {
    Student data;
    vector<SkipNode*> forward;
    SkipNode(Student s, int lvl) : data(s), forward(lvl + 1, nullptr) {}
    SkipNode(int lvl) : forward(lvl + 1, nullptr) { data.roll_no = -1; }
};

class SkipList : public Index {
public:
    SkipList();
    ~SkipList();

    void insert(Student s) override;
    vector<Student> search(int roll_no) override;
    vector<Student> rangeQuery(int low_marks, int high_marks) override;
    vector<Student> prefixSearch(string prefix) override;
    vector<Student> getAllRecords() override;

    string name() override;
    int size() override;

private:
    SkipNode* header;
    int currentHighestLevel;
    int count;

    int randomLevel();
};

