#include <iostream>

#ifndef SpellingCorrector_H
#define SpellingCorrector_H

using namespace std;

// Function to compute Levenshtein Distance
int levenshteinDistance(const string& s1, const string& s2) {
    int len1 = s1.size(), len2 = s2.size();
    vector<vector<int>> dp(len1 + 1, vector<int>(len2 + 1));
    
    for (int i = 0; i <= len1; i++) dp[i][0] = i;
    for (int j = 0; j <= len2; j++) dp[0][j] = j;
    
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[len1][len2];
}

// K-Gram Index for Spelling Correction
class KGramIndex {
    map<string, set<string>> kGramIndex;
    int k;

public:
    KGramIndex(int k) : k(k) {}
    
    void addWord(const string& word) {
        string padded = "$" + word + "$";
        for (size_t i = 0; i + k <= padded.size(); i++) {
            string kgram = padded.substr(i, k);
            kGramIndex[kgram].insert(word);
        }
    }
    
    set<string> getCandidates(const string& query) {
        set<string> candidates;
        string padded = "$" + query + "$";
        for (size_t i = 0; i + k <= padded.size(); i++) {
            string kgram = padded.substr(i, k);
            if (kGramIndex.count(kgram)) {
                candidates.insert(kGramIndex[kgram].begin(), kGramIndex[kgram].end());
            }
        }
        return candidates;
    }
};

class SpellingCorrector {
    KGramIndex kGramIndex;
    unordered_set<string> vocabulary;

public:
    SpellingCorrector(int k) : kGramIndex(k) {}
    
    void addWord(const string& word) {
        vocabulary.insert(word);
        kGramIndex.addWord(word);
    }
    
    string suggestCorrection(const string& query) {
        if (vocabulary.count(query)) return query; // Already correct
        
        set<string> candidates = kGramIndex.getCandidates(query);
        string bestMatch;
        int minDistance = INT_MAX;
        
        for (const string& candidate : candidates) {
            int dist = levenshteinDistance(query, candidate);
            if (dist < minDistance) {
                minDistance = dist;
                bestMatch = candidate;
            }
        }
        return minDistance <= 2 ? bestMatch : ""; // Only suggest close matches
    }
};

#endif