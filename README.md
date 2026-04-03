# AdaptIndex: Self-Optimizing Data Structure Engine

**Course:** CS2308 Data Structures-II Lab | **Semester:** 4 | **VIT Pune**

AdaptIndex is a C++ program that automatically switches between four different data structures — AVL Tree, B+ Tree, Trie, and Skip List — based on the type of database queries being run. Instead of using one fixed structure for everything, it watches query patterns in real time and migrates to the most efficient structure when needed.

---

## What Problem Does It Solve?

A standard database uses one data structure for all operations. This is inefficient:
- An **AVL Tree** is great for individual lookups but slow for scanning ranges of values
- A **B+ Tree** is fast for ranges but wastes overhead for single inserts
- A **Trie** is ideal for prefix/autocomplete searches but useless for numeric range queries
- A **Skip List** handles random mixed workloads without expensive rebalancing

AdaptIndex solves this by profiling every query and automatically switching to the right structure at runtime — no manual intervention needed.

---

## How It Decides When to Switch

The `Profiler` tracks the last **100 queries** (or last **5 queries** in manual console mode) and calculates the percentage of each type:

| Condition | Action |
|---|---|
| RANGE queries ≥ 60% | Switch to **B+ Tree** |
| PREFIX queries ≥ 60% | Switch to **Trie** |
| INSERT queries ≥ 65% | Switch to **AVL Tree** |
| None of the above | Switch to **Skip List** |

If the best structure is already active, no migration happens.

---

## Project Structure

```
/DS-II Project
├── main.cpp                     ← Entry point and main menu
├── Makefile                     ← Compilation settings
├── README.md
├── PROJECT_EXPLANATION.md       ← Deep theory and algorithmic defense
├── ARCHITECTURE.md              ← Code flow and file map
│
├── /core
│   ├── AdaptEngine.h            ← Headers: Profiler, Benchmark, MigrationEngine, ScenarioRunner
│   └── AdaptEngine.cpp          ← Implementations of all engine logic
│
├── /indexes
│   ├── DataStructures.h         ← Headers: Student struct, Index base class, AVL, B+Tree, Trie, SkipList
│   └── DataStructures.cpp       ← Implementations of all 4 data structures
│
├── /utils
│   ├── Helpers.h                ← Headers: ConsoleUI, CSVGenerator, CSVLoader, QueryGenerator
│   └── Helpers.cpp              ← Implementations of all utility tools
│
└── /data
    ├── students.csv             ← Dataset: 500 student records
    ├── queries_scenario1.txt    ← Range-heavy query workload
    ├── queries_scenario2.txt    ← Prefix-heavy query workload
    └── queries_scenario3.txt    ← Mixed/shifting query workload
```

---

## Compilation

Requires `g++` with C++17 support (MinGW on Windows):

```powershell
g++ -std=c++17 -O2 main.cpp indexes/DataStructures.cpp core/AdaptEngine.cpp utils/Helpers.cpp -o adaptindex
```

---

## Running

```powershell
.\adaptindex
```

On startup the system loads `data/students.csv` into memory and presents this menu:

```
==================================================================  (cyan)
  MAIN MENU
  [1] Scenario A  -  Range Queries     (triggers B+ Tree)
  [2] Scenario B  -  Prefix Queries    (triggers Trie)
  [3] Scenario C  -  Mixed Workload    (triggers full hot-swap)
  [4] Manual Console  -  Type your own queries
  [5] Exit
==================================================================  (cyan)
```

---

## Demonstration Modes

### Scenario A — Range Queries
Fires 300 range queries (e.g., "find all students with marks between 70-90"). After 100 queries the Profiler detects >60% RANGE load and migrates from AVL Tree → B+ Tree.

### Scenario B — Prefix Queries
Fires 300 prefix/autocomplete queries (e.g., "find all students whose name starts with 'An'"). After 100 queries, migrates to Trie.

### Scenario C — Mixed Workload
Fires a shifting sequence: starts with inserts, shifts to ranges, then prefixes. The system migrates through multiple structures in one run.

### Manual Console (Option 4)
Type your own queries live. The system checks for a pattern every **5 queries** and migrates if needed. Supported commands:

```
INSERT <roll> <name> <marks> <dept>      Example: INSERT 901 Durgesh 98 CS
SEARCH <roll>                             Example: SEARCH 901
RANGE <min_marks> <max_marks>            Example: RANGE 80 100
PREFIX <name_prefix>                     Example: PREFIX Dur
EXIT
```

---

## Deep Dive Reference

- `PROJECT_EXPLANATION.md` — Full theory: why each data structure works better for specific query types, Big-O analysis, and the migration lifecycle
- `ARCHITECTURE.md` — How code files connect, execution trace of a query, and class relationships
