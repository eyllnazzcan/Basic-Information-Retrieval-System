# Basic Information Retrieval System

This project implements a modular and extensible information retrieval system developed in C++, capable of performing Boolean search, wildcard search, and spelling correction. It features a functional graphical user interface (GUI) built with FLTK.

---

## System Components

### Data Structures
- **B-Tree**
  
- **Inverted Index**  

- **K-Gram Index**  
---

### Preprocessing Pipeline
1. **Case Folding**: All words converted to lowercase.
3. **Tokenization**: Splits text into individual words.
4. **Stop Word Removal**: Excludes common, non-informative words.
5. **Stemming**: Reduces words to their root forms (Porter stemmer).

---

### Retrieval Features

- **Boolean Search (AND, OR)**  

- **Wildcard Search**  

- **Spelling Correction**  

- **Ranked Output**
  
---

## GUI – FLTK Interface

- Dark-themed, minimal layout (900x700 window)
- Search box and button
- Real-time display of ranked results
- Built with Fast Light Toolkit (FLTK)

---

## How to Run

### Requirements
- C++ compiler (e.g., g++)
- [FLTK library](https://www.fltk.org/doc-1.3/index.html)

### Build Instructions
```bash
g++ -std=c++17 -I$(brew --prefix fltk)/include -Iinclude -L$(brew --prefix fltk)/lib \ -o main main.cpp -lfltk
