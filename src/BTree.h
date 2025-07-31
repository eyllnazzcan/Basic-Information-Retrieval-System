#include <iostream>

#ifndef BTree_H
#define BTree_H

using namespace std;

// Node structure for B-Tree
template<typename T>
struct BTreeNode {
    vector<T> keys;
    vector<shared_ptr<BTreeNode>> children;
    bool isLeaf;
    BTreeNode(bool isLeaf) : isLeaf(isLeaf) {}
};

// B-Tree class template
template<typename T>
class BTree {
    shared_ptr<BTreeNode<T>> root;
    size_t degree;

    void splitChild(shared_ptr<BTreeNode<T>> node, int i, shared_ptr<BTreeNode<T>> child);
    void insertNonFull(shared_ptr<BTreeNode<T>> node, const T& key);

    void wildcardTraverse(shared_ptr<BTreeNode<T>> node, const string& pattern, vector<string>& matches) const;
    bool matchesPattern(const string& word, const string& pattern) const;

public:
    BTree(size_t degree) : root(make_shared<BTreeNode<T>>(true)), degree(degree) {}

    void insert(const T& key);
    bool search(const T& key, shared_ptr<BTreeNode<T>> node = nullptr);
    vector<string> wildcardSearch(const string& pattern) const;
};

// Splits a full child node
template<typename T>
void BTree<T>::splitChild(shared_ptr<BTreeNode<T>> node, int i, shared_ptr<BTreeNode<T>> child) {
    auto newChild = make_shared<BTreeNode<T>>(child->isLeaf);
    newChild->keys.assign(child->keys.begin() + degree, child->keys.end());

    if (!child->isLeaf) {
        newChild->children.assign(child->children.begin() + degree, child->children.end());
    }

    child->keys.resize(degree - 1);
    child->children.resize(degree);

    node->children.insert(node->children.begin() + i + 1, newChild);
    node->keys.insert(node->keys.begin() + i, child->keys[degree - 1]);
}

// Inserts a key into a non-full node
template<typename T>
void BTree<T>::insertNonFull(shared_ptr<BTreeNode<T>> node, const T& key) {
    int i = node->keys.size() - 1;

    if (node->isLeaf) {
        node->keys.insert(lower_bound(node->keys.begin(), node->keys.end(), key), key);
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        if (node->children[i]->keys.size() == 2 * degree - 1) {
            splitChild(node, i, node->children[i]);

            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

// Inserts a key into the B-Tree
template<typename T>
void BTree<T>::insert(const T& key) {
    if (root->keys.size() == 2 * degree - 1) {
        auto newRoot = make_shared<BTreeNode<T>>(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0, root);
        root = newRoot;
    }
    insertNonFull(root, key);
}

// Searches for a key in the B-Tree
template<typename T>
bool BTree<T>::search(const T& key, shared_ptr<BTreeNode<T>> node) {
    if (!node) node = root;

    int i = 0;
    while (i < node->keys.size() && key > node->keys[i]) {
        i++;
    }

    if (i < node->keys.size() && key == node->keys[i]) {
        return true;
    }

    return !node->isLeaf && search(key, node->children[i]);
}

// Wildcard search in B-Tree
template<typename T>
vector<string> BTree<T>::wildcardSearch(const string& pattern) const {
    vector<string> matches;
    wildcardTraverse(root, pattern, matches);
    return matches;
}

template<typename T>
void BTree<T>::wildcardTraverse(shared_ptr<BTreeNode<T>> node, const string& pattern, vector<string>& matches) const {
    if (!node) return;
    for (const auto& key : node->keys) {
        if (matchesPattern(key, pattern)) {
            matches.push_back(key);
        }
    }
    for (const auto& child : node->children) {
        wildcardTraverse(child, pattern, matches);
    }
}

template<typename T>
bool BTree<T>::matchesPattern(const string& word, const string& pattern) const {
    size_t starPos = pattern.find('*');
    if (starPos == string::npos) return word == pattern;
    string prefix = pattern.substr(0, starPos);
    string suffix = pattern.substr(starPos + 1);
    return word.compare(0, prefix.size(), prefix) == 0 &&
           word.compare(word.size() - suffix.size(), suffix.size(), suffix) == 0;
}

#endif