# Wavelet Tree — C++ Implementation

A clean, interactive C++ implementation of the **Wavelet Tree**, one of the most elegant succinct data structures in modern algorithm design. This project ships with a fully functional CLI demo that lets you explore `access`, `rank`, and `select` queries in real time.

---

## What is a Wavelet Tree?

> *"A wavelet tree is a slight generalization of an old (1988) data structure by Chazelle, heavily used in Computational Geometry... since 2003, these views of wavelet trees, and their interactions, have been fruitful in a surprisingly wide range of problems."*  
> — *Navarro, "Wavelet Trees for All" (2012, §1)*

The **wavelet tree** was invented in **2003 by Grossi, Gupta, and Vitter** as a space-efficient data structure to represent a sequence and answer powerful queries on it. It generalizes the classic `rank` and `select` operations—traditionally defined only on bitvectors—to **arbitrary alphabets**.

### How it works (intuition)

1. Take a sequence `S[1…n]` over an alphabet `Σ` of size `σ`.
2. At the root, partition the alphabet into two halves (e.g., low vs. high symbols).
3. Store a **bitmap** that tells whether each symbol in `S` belongs to the left or right half.
4. Recursively build the same structure for the two subsequences.
5. The result is a balanced binary tree of height `⌈log₂ σ⌉`.

> *Only the bitmaps and tree topology are stored.* The subsequences themselves are **not** stored—they can be reconstructed on the fly by tracking bits down the tree.

The name derives from an analogy with the **wavelet transform** for signals: just as wavelet transforms recursively decompose a signal into low-frequency and high-frequency components, the wavelet tree recursively decomposes a sequence into low-symbol and high-symbol partitions.

---

## Why Wavelet Trees Are Special

### 1. Succinct Space
A balanced wavelet tree stores the sequence in **n·⌈log₂ σ⌉ + o(n·log σ)** bits—essentially the same asymptotic space as a plain array, but with **rich query support** built in. With entropy coding (e.g., Huffman-shaped trees), the space drops to **n·H₀(S) + o(n)** bits, where `H₀(S)` is the zero-order empirical entropy of the string. This means the tree **compresses** the data while preserving full random access.

> *"Even using plain bitmap representations, giving the wavelet tree a Huffman shape leads to a much faster implementation in practice while compressing the data."*  
> — *Navarro (2012, §3.2)*

### 2. Three Powerful Views
The same structure can be interpreted as:
- **A sequence** — supports `access(i)`, `rank(c, i)`, `select(c, k)`.
- **A reordering / permutation** — tracks where each element moves after sorting by symbol.
- **A grid of points** — enables 2D range counting and reporting in `O(log n)` time per query.

> *"These views of wavelet trees, and their interactions, have been fruitful in a surprisingly wide range of problems, extending well beyond the areas of text indexing and computational geometry where the structure was conceived."*  
> — *Navarro (2012, §1)*

### 3. Rank & Select in O(log σ) Time
On a balanced tree, all three core operations run in **O(log σ)** time. For byte alphabets (σ = 256), this is practically **O(1)**—at most 8 bitmap levels to traverse.

### 4. Entropy & High-Order Compression
The wavelet tree adapts naturally to data compressibility:
- **Zero-order entropy:** Using entropy-coded bitmaps (e.g., Raman et al.’s fully indexable dictionaries), total space becomes **n·H₀(S) + o(n)** bits.
- **High-order entropy:** Building the tree on the Burrows–Wheeler transform achieves **n·Hₖ(S) + o(n·log σ)** bits simultaneously for any `k ≤ α·logσ n`.

> *"Using a proper encoding on its bitmaps, a wavelet tree on the whole Burrows–Wheeler transform can reach k-th order entropy compression of a string."*  
> — *Navarro (2012, §3.3)*

### 5. Extensions & Modern Variants
- **Multi-ary wavelet trees** reduce height to `O(log_d σ)`.
- **Wavelet matrices** flatten the tree into a single bitmap per level for pointerless navigation.
- **Dynamic wavelet trees** support insertions and deletions, enabling dynamic FM-indexes.
- **Wavelet tries** extend the structure to sequences of strings.

> *"The data structure can be made dynamic, supporting insertions and deletions at arbitrary points of the string; this feature enables the implementation of dynamic FM-indexes."*  
> — *Wikipedia, "Wavelet Tree"*

---

## Applications

Wavelet trees have found use in an astonishingly diverse set of domains:

| Domain | Application |
|--------|-------------|
| **Text Indexing** | Compressed suffix arrays, FM-indexes, full-text search |
| **Bioinformatics** | DNA sequence analysis, short-read mapping, BWT-based indexes |
| **Computational Geometry** | 2D range counting / reporting, orthogonal range queries |
| **String Algorithms** | Range quantile, range mode, top-k in substrings |
| **Information Retrieval** | Document listing, term proximity queries, inverted indexes |
| **Data Compression** | Entropy-bound representation with direct random access |
| **Database Systems** | Succinct column stores, range histograms, indexable compression |

> *"Originally introduced to represent compressed suffix arrays, it has found application in several contexts."*  
> — *Wikipedia, "Wavelet Tree"*

---

## Working Demo

This repository includes an **interactive command-line tool** (`src/wavelet_tree.cpp`) that lets you build a wavelet tree over any string and query it live.

### Build & Run

```bash
# Clone the repository
git clone https://github.com/hemachandran-1206/wavelet-tree.git
cd wavelet-tree

# Compile
g++ -std=c++17 -O2 -o wavelet_tree src/wavelet_tree.cpp

# Run the interactive demo
./wavelet_tree
```

### Example Session

```
=== Wavelet Tree (string) interactive tool ===

Enter your string now:
abracadabra

Wavelet Tree built for string of length 11.
Type 'help' to see commands again.

> access 0
character at index 0 : 'a'
> access 3
character at index 3 : 'a'
> rank a 10
rank('a', 10) = 5
> select a 3
select('a', 3) = 5
> rank b 10
rank('b', 10) = 2
> select b 2
select('b', 2) = 8
> exit
Goodbye!
```

### Supported Commands

| Command | Description |
|---------|-------------|
| `access i` | Returns the character at **0-based** index `i` |
| `rank c p` | Returns number of occurrences of character `c` in `s[0..p]` (inclusive, 0-based) |
| `select c k` | Returns the **0-based** index of the `k`-th occurrence of `c` (`k` is 1-based) |
| `help` | Show command reference |
| `exit` | Quit the program |

---

## Algorithm & Complexity

| Operation | Time | Space (balanced) |
|-----------|------|-----------------|
| Build | `O(n log σ)` | `n·⌈log σ⌉ + o(n·log σ)` bits |
| `access(i)` | `O(log σ)` | — |
| `rank(c, i)` | `O(log σ)` | — |
| `select(c, k)` | `O(log σ)` | — |

For a **byte alphabet** (σ = 256), all operations traverse at most **8 levels**—effectively constant time in practice.

### Implementation Details

- **Bitmaps:** Each node stores a `vector<int>` of bits and prefix sums (`pref`), enabling `rank₁` and `rank₀` in **O(1)** time per level.
- **Select support:** `zeros` and `ones` vectors store the positions of each bit value, allowing `select₀` and `select₁` in **O(1)**.
- **Recursion:** The tree is built recursively by partitioning the current alphabet range and filtering the subsequence into left/right children.
- **Memory model:** Each node owns its bitmaps and child pointers; the tree is fully deallocatable via standard `delete` ( RAII extension is straightforward).

---

## Project Structure

```
wavelet-tree/
├── src/
│   └── wavelet_tree.cpp      # Main implementation + interactive demo
├── .gitignore                # Ignores binaries, build artifacts, IDE files
├── README.md                 # This file
└── LICENSE                   # (optional, add your own)
```

---

## Key References

| Citation | Year | Contribution |
|----------|------|-------------|
| **R. Grossi, A. Gupta, J. S. Vitter** — *High-order entropy-compressed text indexes* (SODA) | 2003 | **Original invention** of the wavelet tree |
| **P. Ferragina, R. Giancarlo, G. Manzini** — *The myriad virtues of Wavelet Trees* (Information and Computation) | 2009 | Comprehensive survey of applications and extensions |
| **G. Navarro** — *Wavelet Trees for All* (CPM) | 2012 | Deep survey covering structure, compression, variants, and use cases |
| **H.-L. Chan et al.** — *Compressed Indexes for dynamic text collections* (ACM TALG) | 2007 | Dynamic wavelet trees / FM-indexes |
| **R. Grossi, G. Ottaviano** — *The Wavelet Trie* (PODS) | 2012 | Extending wavelet trees to string sequences |

- **Wikipedia:** [Wavelet Tree](https://en.wikipedia.org/wiki/Wavelet_Tree) — Succinct overview, access algorithm, and extensions.
- **ScienceDirect Survey:** [Navarro (2012) — Wavelet Trees for All](https://www.sciencedirect.com/science/article/pii/S1570866713000610) — The definitive modern survey (§1–3 cover structure, compression, and tree-shape variants).

---

## License

This project is provided for educational and research purposes. Feel free to fork, extend, and contribute improvements—dynamic wavelet trees, wavelet matrices, and multi-ary extensions are all natural next steps!

---

## Contributions & Ideas

Possible extensions:
- **Wavelet Matrix** — flatten the tree to pointerless level bitmaps for faster cache performance.
- **Dynamic Operations** — support `insert(i, c)` and `delete(i)` at arbitrary positions.
- **Range Quantile** — answer "what is the k-th smallest symbol in substring S[i..j]?"
- **Multi-ary Nodes** — reduce tree height using `d`-ary splits instead of binary.
- **Huffman Shaping** — rebalance the tree by symbol frequency for better average-case time and compression.

> *"We are ourselves big fans of wavelet trees, and having squeezed them out for several years, it is inevitable that there will be many references to our own work in this survey."*  
> — *Navarro (2012, §1)*  
> **We hope you become a fan too.** 🌲
