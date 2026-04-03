CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Icore -Iindexes -Iutils
TARGET   = adaptindex
# Support windows binary extension if compiled on windows environments directly via MSYS or MinGW
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
endif

SOURCES  = main.cpp \
           indexes/DataStructures.cpp \
           core/AdaptEngine.cpp \
           utils/Helpers.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) data/*.csv data/*.txt

run: all
	./$(TARGET)

.PHONY: all clean run

# Manual: g++ -std=c++17 -O2 -Icore -Iindexes -Iutils main.cpp core/Profiler.cpp core/MigrationEngine.cpp core/Benchmark.cpp core/ScenarioRunner.cpp indexes/AVLTree.cpp indexes/BPlusTree.cpp indexes/Trie.cpp indexes/SkipList.cpp utils/CSVGenerator.cpp utils/QueryGenerator.cpp utils/CSVLoader.cpp utils/ConsoleUI.cpp -o adaptindex
