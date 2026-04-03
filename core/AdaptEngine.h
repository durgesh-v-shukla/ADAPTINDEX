#pragma once
#include "../indexes/DataStructures.h"
#include <string>
#include <vector>
using namespace std;

// --- Content from core/Profiler.h ---
#include <string>
#include <vector>
using namespace std;
#include <string>
using namespace std;

// The brain of AdaptIndex.
// Watches every query and decides when to migrate based on empirical observation of last 100 queries.
class Profiler {
public:
    Profiler();
    void recordQuery(string type);          // type: "INSERT","SEARCH","RANGE","PREFIX"
    string recommend(string currentDSName); // returns "AVL","BPLUS","TRIE","SKIPLIST","NONE"
    void reset();                           // clears all counts, resets window
    void printDistribution();               // prints % breakdown of current window
    int getCount(string type);
    int getTotalQueries();
    bool isWindowFull();                    // true when 100 queries have been recorded

private:
    int insertCount, searchCount, rangeCount, prefixCount;
    int total;
    static const int WINDOW_SIZE = 100;
};


// --- Content from core/MigrationEngine.h ---
#include <string>
#include <vector>
using namespace std;
#include <string>
using namespace std;

// Handles the actual live data structure switch.
class MigrationEngine {
public:
    // Migrates from current DS to targetDS.
    // targetDS: "AVL", "BPLUS", "TRIE", "SKIPLIST"
    // Returns pointer to newly constructed and populated index.
    Index* migrate(Index* current, string targetDS);
};


// --- Content from core/Benchmark.h ---
#include <string>
#include <vector>
using namespace std;
#include <string>
#include <chrono>
using namespace std;

// Tracks real timing per 100-query phase.
class Benchmark {
public:
    Benchmark();
    void startPhase();
    void recordQuery(long long microseconds);     // log one query's latency

    double getAvgLatency();                       // avg microseconds per query this phase
    double getTotalTime();                        // total ms for this phase
    double getImprovement(double prevAvgLatency); // % faster than prev phase
    void printPhaseReport(string phaseName);
    void reset();

private:
    long long totalMicroseconds;
    int queryCount;
};


// --- Content from core/ScenarioRunner.h ---
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <string>
#include <vector>
using namespace std;

// Drives the execution pipeline reading queries and switching trees
class ScenarioRunner {
public:
    void run(string queryFile, vector<Student>& dataset, string scenarioName);
    void interactiveDeveloperMode(vector<Student>& dataset);
    void printFinalReport();

private:
    struct ScenarioResult {
        string name;
        string finalDS;
        int migrations;
        double speedup;
        int totalQueries;
    };
    vector<ScenarioResult> results;
};

