#pragma once
#include "../indexes/DataStructures.h"
#include <string>
#include <vector>
using namespace std;

// --- Content from utils/ConsoleUI.h ---
#include <string>
#include <vector>
using namespace std;

#include <string>

using namespace std;

namespace ConsoleUI {
    // ANSI Escape Codes for formatting
    const string RESET   = "\033[0m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string CYAN    = "\033[36m";
    const string MAGENTA = "\033[35m";
    const string BOLD    = "\033[1m";

    // Smooth print
    void typeText(const string& text, int delayMs = 15);
    
    // Smooth print with color
    void typeColor(const string& text, const string& colorCode, int delayMs = 15);
    
    // Simulate processing
    void showLoadingBar(const string& taskName, int durationMs);
}



// --- Content from utils/CSVGenerator.h ---
#include <string>
#include <vector>
using namespace std;
#include <string>
using namespace std;

class CSVGenerator {
public:
    void generate(string filepath);
};


// --- Content from utils/CSVLoader.h ---
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <string>
#include <vector>
using namespace std;

class CSVLoader {
public:
    // Reads filepath, skips header, returns vector of Student structs
    vector<Student> load(string filepath);
};


// --- Content from utils/QueryGenerator.h ---
#include <string>
#include <vector>
using namespace std;
#include <string>
#include <vector>
using namespace std;

class QueryGenerator {
public:
    void generateAll(string outputDir);
};

