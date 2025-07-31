#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include "SpellingCorrector.h"

#ifndef SearchApp_H 
#define SearchApp_H

using namespace std;

class SearchApp {
    vector<string> documents;
    BTree<string> bTree;
    InvertedIndex invertedIndex;
    SpellingCorrector spellingCorrector;

    Fl_Window* window;
    Fl_Input* input;
    Fl_Button* searchButton;
    Fl_Text_Display* resultDisplay;
    Fl_Text_Buffer* textBuffer;

public:
    SearchApp()
        : bTree(3), spellingCorrector(3) {
        loadDocuments("AllCombined.txt");
        buildIndex();
        buildSpellingIndex();

        window = new Fl_Window(900, 700, "📚 Information Retrieval System");
        window->color(FL_DARK3);

        input = new Fl_Input(100, 20, 600, 38, "🔍 Query:");
        input->textsize(16);
        input->labelfont(FL_BOLD);
        input->labelcolor(FL_WHITE);
        input->textsize(16);
        input->color(FL_WHITE);
        input->textcolor(FL_BLACK);

        searchButton = new Fl_Button(720, 20, 80, 38, "Search");
        searchButton->labelsize(16);
        searchButton->color(FL_GREEN);
        searchButton->labelcolor(FL_WHITE);
        searchButton->labelfont(FL_BOLD);
        searchButton->tooltip("Click to search your query in documents");

        resultDisplay = new Fl_Text_Display(20, 70, 860, 600);
        resultDisplay->textfont(FL_COURIER);
        resultDisplay->textsize(14);
        resultDisplay->color(FL_WHITE);
        resultDisplay->textcolor(FL_BLACK);

        textBuffer = new Fl_Text_Buffer();
        resultDisplay->buffer(textBuffer);

        searchButton->callback(SearchApp::onSearch, this);

        window->end();
    }

    void show() { window->show(); }

private:
    void loadDocuments(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Unable to open dataset file." << endl;
            exit(1);
        }
        string line;
        while (getline(file, line)) {
            documents.push_back(line);
        }
        file.close();
    }

    void buildIndex() {
        ifstream check("index.txt");
        if (check.good()) {
            invertedIndex.loadFromFile("index.txt");
            cout << "Index loaded from file.\n";
        } else {
            for (size_t i = 0; i < documents.size(); i++) {
                invertedIndex.indexDocument(documents[i], i);
                istringstream stream(documents[i]);
                string word;
                while (stream >> word) {
                    bTree.insert(word); 
                }
            }
            invertedIndex.saveToFile("index.txt");
            cout << "Index built and saved to file.\n";
        }
    }


    void buildSpellingIndex() {
        for (const string& doc : documents) {
            istringstream stream(doc);
            string word;
            while (stream >> word) {
                spellingCorrector.addWord(word);
            }
        }
    }

    static void onSearch(Fl_Widget* widget, void* data) {
        auto* app = static_cast<SearchApp*>(data);
        string query = app->input->value();
        transform(query.begin(), query.end(), query.begin(), ::tolower);

        //Check for spelling correction
        string correction = app->spellingCorrector.suggestCorrection(query);
        ostringstream results;
        if (!correction.empty() && correction != query) {
            results << "Did you mean: " << correction << "?\n";
            query = correction;  // Use the corrected query
        }
        results << "Searching for: " << query << "\n";

        vector<string> terms;
        istringstream queryStream(query);
        string term;
        while (queryStream >> term) {
            term = stemWord(term);  // Apply stemming before searching
            terms.push_back(term);
        }
    
        set<int> resultsSet;
        if (query.find(" OR ") != string::npos) {
            resultsSet = app->invertedIndex.disjunctiveSearch(terms);
            results << "Documents containing ANY term: " << resultsSet.size() << "\n";
        } else if (terms.size() > 1) {
            resultsSet = app->invertedIndex.optimizedConjunctiveSearch(terms);
            results << "Documents containing ALL terms: " << resultsSet.size() << "\n";
        } else if (query.find('*') != string::npos) {
            vector<string> wildcardResults = app->bTree.wildcardSearch(query);
            set<int> docIds;
            for (const auto& word : wildcardResults) {
                set<int> wordDocs = app->invertedIndex.search(word);
                docIds.insert(wordDocs.begin(), wordDocs.end());
            }
            results << "Documents containing any of the wildcard term: " << docIds.size() << "\n";
            for (int docId : docIds) {
                results << "Document " << docId << ": " << app->documents[docId] << "\n";
            }
        } else {
            resultsSet = app->invertedIndex.search(query);
            results << "Documents containing the term: " << resultsSet.size() << "\n";
        }
    
        for (int docId : resultsSet) {
            results << "Document " << docId << ": " << app->documents[docId] << "\n";
        }
    
        app->textBuffer->text(results.str().c_str());
    }

};

#endif