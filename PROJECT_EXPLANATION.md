# AdaptIndex: Project Explanation & Technical Defense

## 1. What Is This Project?

AdaptIndex is a self-optimizing database index engine written in C++17. It maintains a collection of student records and automatically switches between four data structures — **AVL Tree**, **B+ Tree**, **Trie**, and **Skip List** — depending on what types of queries are being run.

The key innovation: instead of locking one data structure for the lifetime of the program, AdaptIndex monitors query patterns every 100 operations and migrates all data into a different structure if a better one is available for the current workload.

---

## 2. The Core Problem With Static Data Structures

Most textbook database systems pick one data structure and use it forever. This creates a fundamental mismatch when query patterns change:

### Problem 1: AVL Tree vs Range Queries
An AVL Tree stores nodes scattered across random memory addresses on the heap. To answer `RANGE 70 90` (find all students with marks between 70 and 90), the tree must do an `inorder traversal` — going left, visiting a node, then right — recursively. Each node hop is a random memory jump that causes CPU **cache misses**.

If there are 500 records and 300 fall in the range, the AVL Tree visits many nodes back and forth across memory just to find sequential values.

### Problem 2: Non-Trie Structures vs Prefix Queries
To find all students whose name starts with "AB", an AVL Tree or B+ Tree must:
1. Visit every node
2. Extract the name string from each record
3. Call `substr(0, 2)` to compare the prefix

This is `O(N × M)` complexity where N is the number of records and M is the prefix length. With 500 records and 2-character prefix, that's 1000 string comparisons.

### The Solution: Real-Time Migration
AdaptIndex measures the query composition every 100 queries. When a threshold is crossed, it transfers all data to the optimal structure for the detected workload pattern.

---

## 3. The Four Data Structures (Exactly As Implemented)

### 3.1 AVL Tree (`AVLTree` class in `indexes/DataStructures.cpp`)

**What it is:** A self-balancing Binary Search Tree. Keys are stored by `roll_no`.

**Node structure:**
```cpp
struct AVLNode {
    Student data;
    AVLNode* left;
    AVLNode* right;
    int height;
};
```

**How balancing works:**
The `balanceFactor()` function computes `height(left) - height(right)`. If this value exceeds ±1 after an insert, the tree is rebalanced using one of four rotations:
- `rotateRight()` — fixes left-heavy imbalance (LL case)
- `rotateLeft()` — fixes right-heavy imbalance (RR case)
- `rotateLeft()` then `rotateRight()` — fixes LR case
- `rotateRight()` then `rotateLeft()` — fixes RL case

**Complexity:**
- Insert: O(log N)
- Search by roll_no: O(log N)
- Range query (marks): O(N) — must scan entire tree via inorder
- Prefix search: O(N × M) — must compare every node's name string

**When AdaptIndex uses it:** When INSERT queries ≥ 65% of the last 100 queries, or as the default starting structure at system boot.

---

### 3.2 B+ Tree (`BPlusTree` class in `indexes/DataStructures.cpp`)

**What it is:** An N-ary tree where ORDER = 4 (each node holds up to 3 keys and 4 children pointers). Only leaf nodes store actual `Student` records. Internal nodes store routing keys only.

**Node structure:**
```cpp
const int ORDER = 4;

struct BPlusNode {
    bool isLeaf;
    vector<int> keys;            // marks values used as routing keys
    vector<Student> records;     // ONLY in leaf nodes
    vector<BPlusNode*> children; // ONLY in internal nodes
    BPlusNode* next;             // Linked list pointer between leaf nodes
};
```

**The critical feature — leaf linking:**
All leaf nodes are connected via `next` pointers forming a linked list at the bottom of the tree. Once a range query finds the starting point (e.g., marks ≥ 70), it simply walks the leaf chain horizontally to collect all records up to the end point (e.g., marks ≤ 90) without going back up the tree.

**Complexity:**
- Insert: O(log N) with potential node split
- Search: O(log N)
- Range query: O(log N + K) where K is the number of results — far better than AVL's O(N) for ranges
- Prefix search: O(N) — must still scan all leaf nodes for name matching

**When AdaptIndex uses it:** When RANGE queries ≥ 60% of the last 100 queries.

---

### 3.3 Trie (`Trie` class in `indexes/DataStructures.cpp`)

**What it is:** A tree where each node represents one character of a student's name. Records are indexed by **lowercase name** (a–z only).

**Node structure:**
```cpp
struct TrieNode {
    TrieNode* children[26];   // One slot per letter a-z
    bool isEnd;
    Student data;
};
```

**How prefix search works:**
To find all students whose name starts with "dur":
1. Start at root
2. Follow `children['d'-'a']`  → `children['u'-'a']` → `children['r'-'a']`
3. From that node, do a DFS to collect all records in the subtree

This is `O(L + K)` where L = prefix length and K = number of matching results. The total number of records N is completely irrelevant.

**Important implementation note:** The Trie uses `children[26]`, meaning it only handles lowercase English letters (a–z). Names are converted to lowercase before insertion and search using the internal `toLowerAlpha()` method.

**Complexity:**
- Insert: O(M) where M = length of name
- Prefix search: O(L + K) — extremely fast for autocomplete
- Search by roll_no: O(N) — must scan entire structure
- Range query: O(N) — not designed for numeric ranges

**When AdaptIndex uses it:** When PREFIX queries ≥ 60% of the last 100 queries.

---

### 3.4 Skip List (`SkipList` class in `indexes/DataStructures.cpp`)

**What it is:** A layered linked list where nodes are probabilistically promoted to higher "express lanes" to enable faster traversal.

**Constants:**
```cpp
const int MAX_LEVEL = 4;  // Maximum number of express levels
const float PROB = 0.5f;  // 50% chance of promotion to next level
```

**Node structure:**
```cpp
struct SkipNode {
    Student data;
    vector<SkipNode*> forward;  // forward[0] = base level, forward[1] = level 1 express, etc.
};
```

**How insertion works:**
Each new node is assigned a random level using `randomLevel()`, which flips a coin (probability 0.5) up to MAX_LEVEL times. A node at level 3 appears in the base list AND in express lanes 1, 2, and 3, giving it reach across long spans.

**How search works:**
Start at the highest level header. Jump forward until the next node's key exceeds the target, then drop down one level and repeat. This skips large sections of the list without examining them.

**Complexity:** Expected O(log N) for all operations, with no mandatory rebalancing overhead.

**When AdaptIndex uses it:** When no specific query type dominates — the default fallback for unpredictable or evenly mixed workloads.

---

## 4. The Profiler — How Decisions Are Made

The `Profiler` class (inside `core/AdaptEngine.cpp`) is the decision-making brain.

**Internal tracking variables:**
```cpp
int insertCount, searchCount, rangeCount, prefixCount;
int total;
static const int WINDOW_SIZE = 100;
```

**Every time a query runs**, `recordQuery(string type)` is called and increments the matching counter.

**After every 100 queries** (or every 5 queries in Manual Console mode), `recommend()` is called:

```cpp
double pInsert = (insertCount * 100.0) / total;
double pSearch = (searchCount * 100.0) / total;
double pRange  = (rangeCount  * 100.0) / total;
double pPrefix = (prefixCount * 100.0) / total;

if (pRange  >= 60.0) recommended = "BPLUS";
else if (pPrefix >= 60.0) recommended = "TRIE";
else if (pInsert >= 65.0) recommended = "AVL";
else                       recommended = "SKIPLIST";
```

If the recommended structure is the same as what's currently active, `recommend()` returns `"NONE"` — no unnecessary migration.

---

## 5. The Migration Engine — How Data Is Transferred

When `recommend()` returns a new structure name, `MigrationEngine::migrate()` executes:

**Step 1 — Extract all data:**
```cpp
vector<Student> all = current->getAllRecords();
int savedCount = all.size();
```
The virtual `getAllRecords()` method works for any structure because every data structure class inherits from the abstract `Index` base class.

**Step 2 — Delete old structure:**
```cpp
delete current;  // Frees all heap memory of old structure
```

**Step 3 — Create new structure and populate:**
```cpp
Index* newIndex = new BPlusTree();  // (or AVLTree, Trie, SkipList)
for (const auto& s : all) {
    newIndex->insert(s);
}
```

**Step 4 — Data integrity check:**
```cpp
assert(newIndex->size() == savedCount);
```
If the record count doesn't match after migration, the program terminates to prevent silent data loss.

**Step 5 — Return new pointer:**
The `ScenarioRunner` replaces its `Index* current` pointer with the new structure. No other part of the program needs to change — they all interact through the `Index` interface.

---

## 6. The Benchmark — How Speed Is Measured

The `Benchmark` class uses the C++ `<chrono>` library for microsecond-precision timing.

For each query:
```cpp
auto t1 = chrono::high_resolution_clock::now();
// ... execute query ...
auto t2 = chrono::high_resolution_clock::now();
long long latency = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
benchmark.recordQuery(latency);
```

After migration, improvement is calculated:
```cpp
double improvement = ((prevAvgLatency - currentAvg) / prevAvgLatency) * 100.0;
```

This is displayed in the terminal so the viewer can see a concrete percentage speedup after each switch.

---

## 7. The Dataset

`data/students.csv` contains **500 generated student records** in this format:
```
roll_no,name,marks,department
1,Alice,87,CS
2,Bob,72,EC
...
```

Fields:
- `roll_no` — integer, 1 to 500
- `name` — random English name
- `marks` — integer, 30 to 100
- `department` — one of: CS, EC, ME, CE, EE

The `CSVLoader` reads this file on startup and passes the records to `ScenarioRunner` as a `vector<Student>`.

---

## 8. Query Files

Three pre-generated workload files are used by the automated scenarios:

| File | Contents | Purpose |
|---|---|---|
| `data/queries_scenario1.txt` | ~70% RANGE, mixed rest | Forces B+ Tree migration |
| `data/queries_scenario2.txt` | ~70% PREFIX, mixed rest | Forces Trie migration |
| `data/queries_scenario3.txt` | Shifting pattern | Tests multi-step migration |

Each line in these files is a comma-separated query:
```
RANGE,70,90
PREFIX,An
SEARCH,45
INSERT,901,TestUser,88,CS
```

The `ScenarioRunner::run()` method reads these files line by line and executes each query against the active data structure.

---

## 9. Why C++17 With Raw Pointers?

All four data structures use raw `new` and `delete` for memory management — no `std::map`, `std::set`, `std::unordered_map`, or any STL container for the actual index storage. This is a deliberate academic requirement to demonstrate understanding of:
- Pointer arithmetic
- Dynamic memory allocation and deallocation
- Memory leak prevention (`delete` in MigrationEngine and destructors)
- Polymorphism via vtables (virtual functions in the `Index` base class)

`std::vector<Student>` is used only as a transfer buffer during migration, not as the primary storage mechanism of any data structure.
