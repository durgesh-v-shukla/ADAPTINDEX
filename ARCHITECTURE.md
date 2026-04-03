# AdaptIndex: System Architecture & Code Map

## 1. Physical File Structure

The project is organized into exactly **7 source files** across **3 folders** plus `main.cpp`:

```
/DS-II Project
├── main.cpp                      ← Program entry point
│
├── /indexes
│   ├── DataStructures.h          ← Declarations: Student, Index, AVLTree, BPlusTree, Trie, SkipList
│   └── DataStructures.cpp        ← Implementations of all 4 data structures
│
├── /core
│   ├── AdaptEngine.h             ← Declarations: Profiler, Benchmark, MigrationEngine, ScenarioRunner
│   └── AdaptEngine.cpp           ← Implementations of all engine logic
│
├── /utils
│   ├── Helpers.h                 ← Declarations: ConsoleUI, CSVGenerator, CSVLoader, QueryGenerator
│   └── Helpers.cpp               ← Implementations of all utility tools
│
└── /data
    ├── students.csv
    ├── queries_scenario1.txt
    ├── queries_scenario2.txt
    └── queries_scenario3.txt
```

**Compile command:**
```powershell
g++ -std=c++17 -O2 main.cpp indexes/DataStructures.cpp core/AdaptEngine.cpp utils/Helpers.cpp -o adaptindex
```

**Include dependency tree:**
```
main.cpp
  ├── #include "indexes/DataStructures.h"   (Student, Index, AVL, BPlus, Trie, SkipList)
  ├── #include "core/AdaptEngine.h"          (Profiler, Benchmark, MigrationEngine, ScenarioRunner)
  └── #include "utils/Helpers.h"             (ConsoleUI, CSVGenerator, CSVLoader, QueryGenerator)

core/AdaptEngine.h
  └── #include "../indexes/DataStructures.h"

utils/Helpers.h
  └── #include "../indexes/DataStructures.h"
```

---

## 2. What Each File Contains

### `indexes/DataStructures.h` and `DataStructures.cpp`

This is the pure data structure layer. It has no display logic, no file I/O — only the algorithms.

**`DataStructures.h` declares:**

| Component | Type | Description |
|---|---|---|
| `Student` | struct | The payload: `roll_no`, `name`, `marks`, `department`. Has a `print()` method. |
| `Index` | abstract class | The polymorphic base class. All 4 structures inherit from it. |
| `AVLTree` | class : Index | Self-balancing BST keyed by `roll_no`. |
| `BPlusTree` | class : Index | N-ary tree (ORDER=4). Keys = marks. Leaf-linked for ranges. |
| `Trie` | class : Index | 26-child character tree (a-z). Indexed by lowercase name. |
| `SkipList` | class : Index | Max 4 levels. PROB = 0.5. Keyed by `roll_no`. |

**The `Index` base class interface:**
```cpp
class Index {
public:
    virtual void insert(Student s) = 0;
    virtual vector<Student> search(int roll_no) = 0;
    virtual vector<Student> rangeQuery(int low_marks, int high_marks) = 0;
    virtual vector<Student> prefixSearch(string prefix) = 0;
    virtual vector<Student> getAllRecords() = 0;   // used by MigrationEngine
    virtual string name() = 0;
    virtual int size() = 0;
    virtual ~Index() {}
};
```

Every pointer in the engine is `Index*` — this is how one line of code can talk to all four different data structures interchangeably.

---

### `core/AdaptEngine.h` and `AdaptEngine.cpp`

This layer contains the intelligence and control flow. It knows nothing about how the data structures work internally — it only calls them through the `Index*` interface.

**`AdaptEngine.h` declares:**

| Component | Type | Description |
|---|---|---|
| `Profiler` | class | Tracks query type counts. Recommends next DS after 100 queries. |
| `MigrationEngine` | class | Has one method: `migrate(Index*, string)`. Handles data transfer. |
| `Benchmark` | class | Uses `<chrono>` for microsecond query timing. Calculates % improvement. |
| `ScenarioRunner` | class | Runs scenarios from file and hosts the Manual Console mode. |

**`Profiler` internals:**
```cpp
private:
    int insertCount, searchCount, rangeCount, prefixCount;
    int total;
    static const int WINDOW_SIZE = 100;
```

Decision thresholds in `recommend()`:
```cpp
if (pRange  >= 60.0)  → "BPLUS"
if (pPrefix >= 60.0)  → "TRIE"
if (pInsert >= 65.0)  → "AVL"
else                  → "SKIPLIST"
```

Returns `"NONE"` if the best choice is already the active structure.

---

### `utils/Helpers.h` and `Helpers.cpp`

Pure utility — display, file reading, and data generation.

**`Helpers.h` declares:**

| Component | Type | Description |
|---|---|---|
| `ConsoleUI` namespace | namespace | ANSI color codes + `typeText()`, `typeColor()`, `showLoadingBar()` |
| `CSVGenerator` | class | `generate(filepath)` — writes 500 random student records to CSV |
| `CSVLoader` | class | `load(filepath)` — reads CSV, returns `vector<Student>` |
| `QueryGenerator` | class | `generateAll(dir)` — writes the 3 scenario query files |

**ConsoleUI color constants used:**
```cpp
const string RESET   = "\033[0m";
const string GREEN   = "\033[32m";   // Migration success messages
const string YELLOW  = "\033[33m";   // Scenario separator lines + summary box
const string CYAN    = "\033[36m";   // Menu border lines
```

---

## 3. How `main.cpp` Connects Everything

```cpp
#include "indexes/DataStructures.h"
#include "core/AdaptEngine.h"
#include "utils/Helpers.h"
```

**Startup sequence:**
1. `printBanner()` — prints the ASCII header
2. `ConsoleUI::showLoadingBar()` — displays loading animation
3. `CSVGenerator::generate("data/students.csv")` — creates/overwrites the dataset
4. `QueryGenerator::generateAll("data/")` — creates/overwrites 3 scenario files
5. `CSVLoader::load("data/students.csv")` — reads dataset into `vector<Student> dataset`
6. Enter `while(true)` loop — show menu, read choice, call `ScenarioRunner`

**Menu loop:**
```
Yellow separator line
→ ScenarioRunner::run() or interactiveDeveloperMode()
Yellow separator line + Scenario Summary box (yellow)
→ Show cyan-bordered menu again
```

---

## 4. Full Execution Trace: What Happens When You Run Scenario 1

**Step 1 — `main.cpp`:**
User selects `1`. Prints yellow separator. Calls:
```cpp
ScenarioRunner runner;
runner.run("data/queries_scenario1.txt", dataset, "SCENARIO 1: Range Heavy Workload");
```

**Step 2 — `ScenarioRunner::run()` in `AdaptEngine.cpp`:**
- Creates `Index* current = new AVLTree()`
- Inserts all 500 records from `dataset` into the AVL Tree
- Creates `Profiler profiler`, `Benchmark benchmark`, `MigrationEngine migrationEngine`
- Opens `queries_scenario1.txt` and reads line by line

**Step 3 — Per-query loop:**
For each line (e.g., `RANGE,70,90`):
```cpp
auto t1 = chrono::high_resolution_clock::now();
current->rangeQuery(70, 90);        // polymorphic call → AVLTree::rangeQuery()
auto t2 = chrono::high_resolution_clock::now();
long long latency = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
benchmark.recordQuery(latency);
profiler.recordQuery("RANGE");
queryNumber++;
```

**Step 4 — After every 100 queries:**
```cpp
string rec = profiler.recommend(current->name());
// rec = "BPLUS" because rangeCount/100 >= 60%
```

**Step 5 — `MigrationEngine::migrate()` in `AdaptEngine.cpp`:**
```cpp
vector<Student> all = current->getAllRecords();   // dump AVL Tree contents
delete current;                                    // free AVL Tree heap memory
Index* newIndex = new BPlusTree();                 // allocate B+ Tree
for (auto& s : all) newIndex->insert(s);          // populate B+ Tree
assert(newIndex->size() == savedCount);            // verify no data loss
return newIndex;                                   // ScenarioRunner uses this pointer
```

**Step 6 — Back in `ScenarioRunner`:**
```cpp
current = migrationEngine.migrate(current, rec);
// current now points to a BPlusTree with all 500 records
```

Subsequent `RANGE` queries now call `BPlusTree::rangeQuery()` via the same `current->rangeQuery()` call — the `ScenarioRunner` code did not change at all.

**Step 7 — End of file:**
Prints yellow-bordered SCENARIO SUMMARY:
```
Total Queries Run    : 300
Data Structure Swaps : 1
Final Data Structure : B+ Tree
```

---

## 5. Full Execution Trace: Manual Console Mode (Option 4)

**Key difference from scenario mode:** Migration check happens every **5 queries** instead of 100, so the reviewer can trigger a switch quickly by typing queries manually.

```cpp
if (profiler.getTotalQueries() > 0 && profiler.getTotalQueries() % 5 == 0) {
    string rec = profiler.recommend(currentStructure);
    if (rec != "NONE") {
        // migrate and display green switch message
    }
}
```

**To trigger a Trie migration in a live demo:**
Type `PREFIX Dur` five times in a row. After the fifth, the profiler sees 100% PREFIX queries → recommends TRIE → migration executes live.

---

## 6. Polymorphism: How One Pointer Talks to Four Structures

This is the architectural heart of the project. The C++ compiler builds a **vtable** for every class that declares `virtual` methods.

When `ScenarioRunner` calls `current->rangeQuery(70, 90)`:
1. CPU looks up the vtable pointer stored inside `current`
2. The vtable for `BPlusTree` points to `BPlusTree::rangeQuery`
3. `BPlusTree::rangeQuery` is executed

Replace the object that `current` points to with a `Trie`, and `current->rangeQuery()` automatically calls `Trie::rangeQuery` — zero changes needed in `ScenarioRunner`.

This is why the `MigrationEngine` can swap structures dynamically at runtime. The entire engine only interacts with `Index*` pointers, never with `AVLTree*` or `BPlusTree*` directly.

---

## 7. Memory Management

**No memory leaks by design:**

| Event | Memory Action |
|---|---|
| Structure created | `new AVLTree()` / `new BPlusTree()` etc. |
| Migration triggered | `delete current` before creating new structure |
| Scenario ends | `delete current` at end of `ScenarioRunner::run()` |
| Manual console exits | `delete activeIndex` when user types `EXIT` |
| Each structure's destructor | Recursively frees all internal nodes |

The `assert(newIndex->size() == savedCount)` check after each migration guarantees no records are lost during transfers.

---

## 8. The Data Flow Summary

```
students.csv
    │
    ▼
CSVLoader::load()
    │
    ▼ vector<Student> dataset (500 records in RAM)
    │
    ▼
ScenarioRunner receives dataset reference
    │
    ├── Insert all into Index* current (starts as AVLTree)
    │
    ├── Read queries from file / user input
    │
    ├── Execute query on current via virtual dispatch
    │
    ├── Profiler::recordQuery() after each
    │
    └── Every 100 queries:
            │
            ├── Profiler::recommend() → "BPLUS" / "TRIE" / "SKIPLIST" / "NONE"
            │
            └── if not "NONE":
                    MigrationEngine::migrate(current, rec)
                        ├── getAllRecords() → vector<Student>
                        ├── delete current
                        ├── new TargetStructure()
                        ├── insert all records
                        └── return new Index*
```
