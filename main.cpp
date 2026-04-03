#include <iostream>
#include <vector>
#include <chrono>
#include "indexes/DataStructures.h"
#include "core/AdaptEngine.h"
#include "utils/Helpers.h"
using namespace std;

void printBanner() {
    cout << "\n";
    cout << "==========================================================================\n";
    cout << "                        A D A P T I N D E X                             \n";
    cout << "                   Self-Optimizing Data Structure Engine                \n";
    cout << "==========================================================================\n\n";
}

int main() {
    printBanner();

    cout << "Loading dataset...\n";
    ConsoleUI::showLoadingBar("Starting up", 600);

    // STEP 1: Generate dataset file
    CSVGenerator csvGen;
    csvGen.generate("data/students.csv");
    QueryGenerator queryGen;
    queryGen.generateAll("data/");

    // STEP 2: Load dataset into memory
    CSVLoader loader;
    vector<Student> dataset = loader.load("data/students.csv");

    cout << "\n" << dataset.size() << " student records loaded.\n";

    while (true) {
        cout << ConsoleUI::CYAN << "\n==================================================================\n" << ConsoleUI::RESET;
        cout << "  MAIN MENU\n";
        cout << "  [1] Scenario A  -  Range Queries     (triggers B+ Tree)\n";
        cout << "  [2] Scenario B  -  Prefix Queries    (triggers Trie)\n";
        cout << "  [3] Scenario C  -  Mixed Workload    (triggers full hot-swap)\n";
        cout << "  [4] Manual Console  -  Type your own queries\n";
        cout << "  [5] Exit\n";
        cout << ConsoleUI::CYAN << "==================================================================\n" << ConsoleUI::RESET;
        cout << "Enter your choice: ";
        
        int choice;
        if (!(cin >> choice)) break;

        // Yellow separator line before scenario begins
        auto printSeparator = [&]() {
            cout << ConsoleUI::YELLOW
                 << "\n=========================================================================="
                 << "\n" << ConsoleUI::RESET;
        };

        ScenarioRunner runner;

        if (choice == 1) {
            printSeparator();
            runner.run("data/queries_scenario1.txt", dataset, "SCENARIO 1: Range Heavy Workload");
            printSeparator();
        } else if (choice == 2) {
            printSeparator();
            runner.run("data/queries_scenario2.txt", dataset, "SCENARIO 2: Prefix Heavy Workload");
            printSeparator();
        } else if (choice == 3) {
            printSeparator();
            runner.run("data/queries_scenario3.txt", dataset, "SCENARIO 3: Shifting Workload");
            printSeparator();
        } else if (choice == 4) {
            printSeparator();
            runner.interactiveDeveloperMode(dataset);
            printSeparator();
        } else if (choice == 5) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << ConsoleUI::RED << "Invalid choice. Try again." << ConsoleUI::RESET << "\n";
        }
    }

    return 0;
}
