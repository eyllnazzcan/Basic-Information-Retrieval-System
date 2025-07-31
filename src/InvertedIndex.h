#include <iostream>

#ifndef InvertedIndex_H
#define InvertedIndex_H

using namespace std;

// Main indexing and retrieval functions
class InvertedIndex {
    map<string, set<int>> index;
    map<string, set<int>> invertedMap;

public:
    void indexDocument(const string& document, int docId);
    set<int> search(const string& term);
    set<int> optimizedConjunctiveSearch(const vector<string>& terms);
    set<int> disjunctiveSearch(const vector<string>& terms);
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);

};

string stemWord(const string& word) {
    if (word.length() <= 2) return word;  // Ignore short words

    string stem = word;

    // common suffixes
    if (stem.length() > 3) {
        if (stem.substr(stem.length() - 3) == "ing") {
            stem = stem.substr(0, stem.length() - 3);
        } else if (stem.substr(stem.length() - 2) == "ed") {
            stem = stem.substr(0, stem.length() - 2);
        } else if (stem.substr(stem.length() - 1) == "s" && stem.length() > 3) {
            stem = stem.substr(0, stem.length() - 1);
        }
    }

    return stem;
}

set<string> stopWords = {"the", "is", "of", "and", "a", "to", "in", "it", "that", "on", "with"};

void InvertedIndex::indexDocument(const string& document, int docId) {
    istringstream stream(document);
    string word;
    while (stream >> word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        word = stemWord(word);
        if (stopWords.find(word) == stopWords.end()) {
            index[word].insert(docId);
        }
    }
}

void InvertedIndex::saveToFile(const string& filename) {
    ofstream out(filename);
    for (const auto& [term, postings] : invertedMap) {
        out << term;
        for (int docId : postings) {
            out << " " << docId;
        }
        out << "\n";
    }
    out.close();
}

void InvertedIndex::loadFromFile(const string& filename) {
    ifstream in(filename);
    string line;
    while (getline(in, line)) {
        istringstream iss(line);
        string term;
        iss >> term;
        int docId;
        while (iss >> docId) {
            invertedMap[term].insert(docId);
        }
    }
    in.close();
}

set<int> InvertedIndex::search(const string& term) {
    string stemmedTerm = stemWord(term);  // Apply stemming before searching
    auto it = index.find(stemmedTerm);
    return it != index.end() ? it->second : set<int>();
}


set<int> InvertedIndex::optimizedConjunctiveSearch(const vector<string>& terms) {
    if (terms.empty()) return {};

    // Sort terms by increasing frequency for better performance
    vector<pair<int, string>> termFreq;
    for (const string& term : terms) {
        termFreq.push_back({index[term].size(), term});
    }
    sort(termFreq.begin(), termFreq.end());

    set<int> result = search(termFreq[0].second);
    for (size_t i = 1; i < termFreq.size(); ++i) {
        set<int> termResult = search(termFreq[i].second);
        set<int> intersection;
        set_intersection(result.begin(), result.end(),
                         termResult.begin(), termResult.end(),
                         inserter(intersection, intersection.begin()));
        result = intersection;
    }
    return result;
}

set<int> InvertedIndex::disjunctiveSearch(const vector<string>& terms) {
    set<int> result;
    for (const string& term : terms) {
        set<int> termResult = search(term);
        result.insert(termResult.begin(), termResult.end()); // Merge results (OR)
    }
    return result;
}

#endif