#include "Helpers.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <windows.h>

// --- Content from utils/ConsoleUI.cpp ---
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <windows.h> 

using namespace std;

namespace ConsoleUI {

    void typeText(const string& text, int delayMs) {
        for (char c : text) {
            cout << c << flush;
            Sleep(delayMs);
        }
        cout << endl;
    }

    void typeColor(const string& text, const string& colorCode, int delayMs) {
        cout << colorCode;
        for (char c : text) {
            cout << c << flush;
            Sleep(delayMs);
        }
        cout << RESET << endl;
    }

    void showLoadingBar(const string& taskName, int durationMs) {
        cout << CYAN << BOLD << taskName << " [";
        int barWidth = 30;
        int stepTime = durationMs / barWidth;
        
        for (int i = 0; i < barWidth; ++i) {
            cout << "#" << flush;
            Sleep(stepTime);
        }
        cout << "] DONE!" << RESET << endl;
    }

}


// --- Content from utils/CSVGenerator.cpp ---
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>

void CSVGenerator::generate(string filepath) {
    system("if not exist data mkdir data");

    vector<string> names = {
        "Aarav Sharma", "Priya Mehta", "Rahul Patil", "Sneha Joshi",
        "Arjun Kulkarni", "Neha Desai", "Vikram Borse", "Kavya Wagh",
        "Rohit Chavan", "Ananya Pawar", "Siddharth Kadam", "Pooja Naik",
        "Aditya Shinde", "Divya More", "Karan Verma", "Riya Jadhav",
        "Amit Thakur", "Shreya Gaikwad", "Nikhil Sawant", "Tanvi Salve",
        "Gaurav Mane", "Ankita Rane", "Yash Deshpande", "Meera Kale",
        "Harsh Bhosale", "Swati Jain", "Pranav Pisal", "Ishaan Nimkar",
        "Varun Phad", "Mrunal Thorat", "Tejas Ghosh", "Aarohi Gupta",
        "Samarth Kulthe", "Sanjana Sutar", "Akash Ingale", "Vaishnavi Gore",
        "Kunal Dhavale", "Madhuri Kolhe", "Omkar Patole", "Ritika Shah"
    };

    vector<string> depts = {"CS", "IT", "ENTC", "MECH", "CIVIL"};

    ofstream file(filepath);
    if (!file.is_open()) {
        cerr << "Failed to open " << filepath << " for writing.\n";
        return;
    }

    file << "roll_no,name,marks,department\n";

    for (int i = 1; i <= 500; ++i) {
        string name = names[(i - 1) % names.size()];
        int marks = ((i * 37 + 13) % 101);
        string dept = depts[(i - 1) % depts.size()];
        file << i << "," << name << "," << marks << "," << dept << "\n";
    }
    
    file.close();
    cout << "[FILE]   " << filepath << " created - 500 records\n";
}


// --- Content from utils/CSVLoader.cpp ---
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <iostream>

vector<Student> CSVLoader::load(string filepath) {
    vector<Student> dataset;
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filepath << "\n";
        return dataset;
    }

    string line;
    // Skip header
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string rStr, name, mStr, dept;
        getline(ss, rStr, ',');
        getline(ss, name, ',');
        getline(ss, mStr, ',');
        getline(ss, dept, ',');

        Student s;
        s.roll_no = stoi(rStr);
        s.name = name;
        s.marks = stoi(mStr);
        s.department = dept;
        dataset.push_back(s);
    }

    return dataset;
}


// --- Content from utils/QueryGenerator.cpp ---
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <fstream>
#include <iostream>

void QueryGenerator::generateAll(string outputDir) {
    vector<string> names = {
        "Aarav Sharma", "Priya Mehta", "Rahul Patil", "Sneha Joshi",
        "Arjun Kulkarni", "Neha Desai", "Vikram Borse", "Kavya Wagh",
        "Rohit Chavan", "Ananya Pawar", "Siddharth Kadam", "Pooja Naik",
        "Aditya Shinde", "Divya More", "Karan Verma", "Riya Jadhav",
        "Amit Thakur", "Shreya Gaikwad", "Nikhil Sawant", "Tanvi Salve",
        "Gaurav Mane", "Ankita Rane", "Yash Deshpande", "Meera Kale",
        "Harsh Bhosale", "Swati Jain", "Pranav Pisal", "Ishaan Nimkar",
        "Varun Phad", "Mrunal Thorat", "Tejas Ghosh", "Aarohi Gupta",
        "Samarth Kulthe", "Sanjana Sutar", "Akash Ingale", "Vaishnavi Gore",
        "Kunal Dhavale", "Madhuri Kolhe", "Omkar Patole", "Ritika Shah"
    };
    vector<string> depts = {"CS", "IT", "ENTC", "MECH", "CIVIL"};
    vector<pair<int, int>> ranges = {{40,60}, {60,80}, {70,90}, {50,75}, {80,100}, {30,55}, {65,85}, {45,70}};
    vector<string> prefixes = {"Aa", "Pr", "Ra", "Sn", "Ar", "Ne", "Vi", "Ka", "Ro", "An", "Si", "Po", "Ad", "Di", "Ri", "Am", "Sh", "Ni", "Ta", "Ga"};

    // Scenario 1
    ofstream f1(outputDir + "queries_scenario1.txt");
    for (int i = 1; i <= 100; i++) {
        if (i % 2 == 1) {
            f1 << "SEARCH," << (i % 499 == 0 ? 1 : i % 499) << "\n";
        } else {
            f1 << "INSERT," << (500 + i) << "," << names[i % names.size()] << "," << (i % 100) << "," << depts[i % depts.size()] << "\n";
        }
    }
    for (int i = 101; i <= 300; i++) {
        if (i % 10 == 0) {
            if (i % 20 == 0) f1 << "SEARCH," << (i % 499) << "\n";
            else f1 << "INSERT," << (500 + i) << "," << names[i % names.size()] << "," << (i % 100) << "," << depts[i % depts.size()] << "\n";
        } else {
            f1 << "RANGE," << ranges[i % ranges.size()].first << "," << ranges[i % ranges.size()].second << "\n";
        }
    }
    f1.close();
    cout << "[FILE]   " << outputDir << "queries_scenario1.txt created - 300 queries\n";

    // Scenario 2
    ofstream f2(outputDir + "queries_scenario2.txt");
    for (int i = 1; i <= 100; i++) {
        if (i % 2 == 1) f2 << "SEARCH," << (i % 499 == 0 ? 1 : i % 499) << "\n";
        else f2 << "INSERT," << (500 + i) << "," << names[i % names.size()] << "," << (i % 100) << "," << depts[i % depts.size()] << "\n";
    }
    for (int i = 101; i <= 300; i++) {
        if (i % 10 == 0) {
            if (i % 20 == 0) f2 << "SEARCH," << (i % 499) << "\n";
            else f2 << "INSERT," << (500 + i) << "," << names[i % names.size()] << "," << (i % 100) << "," << depts[i % depts.size()] << "\n";
        } else {
            f2 << "PREFIX," << prefixes[i % prefixes.size()] << "\n";
        }
    }
    f2.close();
    cout << "[FILE]   " << outputDir << "queries_scenario2.txt created - 300 queries\n";

    // Scenario 3
    ofstream f3(outputDir + "queries_scenario3.txt");
    int rollCounter = 601;
    // Phases 1 and 2: heavily insertions (65%), search(35%)
    for (int i = 1; i <= 200; i++) {
        if (i % 3 == 0) {
            f3 << "SEARCH," << (i % 499 == 0 ? 1 : i % 499) << "\n";
        } else {
            f3 << "INSERT," << rollCounter++ << "," << names[i % names.size()] << "," << (i % 100) << "," << depts[i % depts.size()] << "\n";
        }
    }
    
    // Phases 3 and 4: heavily range queries
    for (int i = 201; i <= 400; i++) {
        if (i % 5 == 0) {
            f3 << "INSERT," << rollCounter++ << "," << names[i % names.size()] << "," << (i % 100) << "," << depts[i % depts.size()] << "\n";
        } else if (i % 7 == 0) {
            f3 << "SEARCH," << (i % 499 == 0 ? 1 : i % 499) << "\n";
        } else {
            f3 << "RANGE," << ranges[i % ranges.size()].first << "," << ranges[i % ranges.size()].second << "\n";
        }
    }
    
    // Phases 5 and 6: heavily prefix queries
    for (int i = 401; i <= 600; i++) {
        if (i % 6 == 0) {
            f3 << "INSERT," << rollCounter++ << "," << names[i % names.size()] << "," << (i % 100) << "," << depts[i % depts.size()] << "\n";
        } else if (i % 7 == 0) {
            f3 << "SEARCH," << (i % 499 == 0 ? 1 : i % 499) << "\n";
        } else {
            f3 << "PREFIX," << prefixes[i % prefixes.size()] << "\n";
        }
    }
    f3.close();
    cout << "[FILE]   " << outputDir << "queries_scenario3.txt created - 600 queries\n";
}

