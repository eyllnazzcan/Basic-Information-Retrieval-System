#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <memory>
#include <unordered_set>
#include <functional>

#include "BTree.h"
#include "InvertedIndex.h"
#include "SearchApp.h"
#include "SpellingCorrector.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc > 1 && string(argv[1]) == "--cli") {
        // CLI Mode
        BTree<string> bTree(3);
        InvertedIndex invertedIndex;

        // Load dataset
        vector<string> documents;
        ifstream file("AllCombined.txt");
        if (!file.is_open()) {
            cerr << "Error: Unable to open dataset file." << endl;
            return 1;
        }
        string line;
        while (getline(file, line)) {
            documents.push_back(line);
        }
        file.close();

        // Build index
        for (size_t i = 0; i < documents.size(); i++) {
            invertedIndex.indexDocument(documents[i], i);
            istringstream stream(documents[i]);
            string word;
            while (stream >> word) {
                bTree.insert(word);
            }
        }

        // CLI Query Processing
        while (true) {
            string query;
            cout << "Enter a query (or 'exit' to quit): ";
            getline(cin, query);
            if (query == "exit") break;

        }
    } else {
        // GUI Mode
        SearchApp app;
        app.show();
        return Fl::run();
    }
}