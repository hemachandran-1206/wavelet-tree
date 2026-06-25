/******************************************************************************
 * Wavelet Tree — C++ Implementation
 *
 * A succinct data structure for sequences over arbitrary alphabets.
 * Supports:
 *   - access(i)   : symbol at position i
 *   - rank(c, i)  : number of occurrences of c in s[0..i]
 *   - select(c, k): index of the k-th occurrence of c
 *
 * Reference: Grossi, Gupta, Vitter (2003); Navarro (2012) survey.
 ******************************************************************************/

#include <bits/stdc++.h>
using namespace std;

/* ------------------------------------------------------------------ */
/*  Node : one wavelet-tree node holding a bitmap + prefix sums        */
/* ------------------------------------------------------------------ */
struct Node {
    int lo, hi;                      // alphabet range [lo, hi] at this node
    vector<int> bits;                // bitmap: 0 = left, 1 = right
    vector<int> pref;                // pref[i] = number of 1s in bits[0..i-1]
    vector<int> zeros;               // positions of 0-bits (for select0)
    vector<int> ones;                // positions of 1-bits (for select1)
    Node *left, *right;

    Node(): lo(0), hi(0), left(nullptr), right(nullptr) {}

    // Build node from array over alphabet range [l..r]
    Node(vector<int> &arr, int l, int r) {
        lo = l; hi = r;
        left = right = nullptr;
        if (arr.empty()) return;
        if (lo == hi) return;        // leaf — nothing more to store

        int mid = (lo + hi) / 2;
        vector<int> L, R;
        pref.push_back(0);           // pref[0] = 0 (sentinel)

        for (int i = 0; i < (int)arr.size(); ++i) {
            if (arr[i] <= mid) {
                bits.push_back(0);
                zeros.push_back(i);
                L.push_back(arr[i]);
                pref.push_back(pref.back());   // 1-count unchanged
            } else {
                bits.push_back(1);
                ones.push_back(i);
                R.push_back(arr[i]);
                pref.push_back(pref.back() + 1);
            }
        }
        if (!L.empty()) left  = new Node(L, lo, mid);
        if (!R.empty()) right = new Node(R, mid + 1, hi);
    }

    // Number of 1s in bits[0..pos]  (inclusive)
    int rank1(int pos) const {
        if (pos < 0) return 0;
        if (pos >= (int)bits.size()) return pref.back();
        return pref[pos + 1];
    }

    // Number of 0s in bits[0..pos] (inclusive)
    int rank0(int pos) const {
        if (pos < 0) return 0;
        if (pos >= (int)bits.size()) pos = (int)bits.size() - 1;
        return (pos + 1) - rank1(pos);
    }

    // Position of the k-th 1 (k is 1-based)
    int select1(int k) const {
        if (k <= 0 || k > (int)ones.size()) return -1;
        return ones[k - 1];
    }

    // Position of the k-th 0 (k is 1-based)
    int select0(int k) const {
        if (k <= 0 || k > (int)zeros.size()) return -1;
        return zeros[k - 1];
    }
};

/* ------------------------------------------------------------------ */
/*  WaveletTree : top-level wrapper                                     */
/* ------------------------------------------------------------------ */
struct WaveletTree {
    Node *root;
    int lo, hi, n;

    WaveletTree() : root(nullptr), lo(0), hi(0), n(0) {}

    // Build from a std::string (treats chars as unsigned byte values)
    void build(const string &s) {
        if (s.empty()) return;
        n = (int)s.size();
        int mn = 255, mx = 0;
        vector<int> arr;
        arr.reserve(n);
        for (unsigned char c : s) {
            int v = c;
            mn = min(mn, v);
            mx = max(mx, v);
            arr.push_back(v);
        }
        lo = mn; hi = mx;
        root = new Node(arr, lo, hi);
    }

    // access(i) : return symbol at index i (0-based). O(log σ)
    char access(int idx) const {
        if (!root || idx < 0 || idx >= n) return '\0';
        Node *cur = root;
        int pos = idx;
        while (cur && cur->lo != cur->hi) {
            int b = cur->bits[pos];
            if (b == 0) {
                pos = cur->rank0(pos) - 1;
                cur = cur->left;
            } else {
                pos = cur->rank1(pos) - 1;
                cur = cur->right;
            }
        }
        return cur ? (char)cur->lo : '\0';
    }

    // rank(c, pos) : occurrences of c in s[0..pos] (inclusive). O(log σ)
    int rank(char c, int pos) const {
        if (!root) return 0;
        if (pos < 0) return 0;
        if (pos >= n) pos = n - 1;
        int x = (unsigned char)c;
        if (x < lo || x > hi) return 0;

        Node *cur = root;
        int l = lo, r = hi;
        int p = pos;
        while (cur && l < r) {
            int mid = (l + r) / 2;
            if (x <= mid) {
                p = cur->rank0(p) - 1;
                r = mid;
                cur = cur->left;
            } else {
                p = cur->rank1(p) - 1;
                l = mid + 1;
                cur = cur->right;
            }
            if (!cur) return 0;
        }
        return (p < 0) ? 0 : p + 1;
    }

    // select(c, k) : index of k-th occurrence of c (k is 1-based). O(log σ)
    int select(char c, int k) const {
        if (!root) return -1;
        int total = rank(c, n - 1);
        if (k > total || k <= 0) return -1;
        int x = (unsigned char)c;

        Node *cur = root;
        int l = lo, r = hi;
        vector<pair<Node*, int>> path;   // (node, direction) pairs from root to leaf
        while (cur && l < r) {
            int mid = (l + r) / 2;
            if (x <= mid) {
                path.push_back({cur, 0});
                cur = cur->left;
                r = mid;
            } else {
                path.push_back({cur, 1});
                cur = cur->right;
                l = mid + 1;
            }
        }

        int pos = -1;
        int now = k;
        for (int i = (int)path.size() - 1; i >= 0; --i) {
            Node *nd = path[i].first;
            int dir = path[i].second;
            if (dir == 0) pos = nd->select0(now);
            else          pos = nd->select1(now);
            if (pos == -1) return -1;
            now = pos + 1;   // convert to 1-based count for next level up
        }
        return pos;
    }
};

/* ------------------------------------------------------------------ */
/*  Interactive CLI Demo                                                */
/* ------------------------------------------------------------------ */
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "=== Wavelet Tree (string) interactive tool ===\n\n";
    cout << "Instructions:\n";
    cout << "1) Enter the string for which you want to build the wavelet tree (press Enter).\n";
    cout << "   The string may include spaces.\n";
    cout << "2) After the tree is built you can run the following commands (one per line):\n";
    cout << "   access i       -> character at index i (0-based)\n";
    cout << "   rank c p       -> occurrences of character c in s[0..p] (p is 0-based, inclusive)\n";
    cout << "   select c k     -> index (0-based) of the k-th occurrence of c (k is 1-based)\n";
    cout << "   help           -> show this help message\n";
    cout << "   exit           -> quit the program\n\n";
    cout << "Examples:\n";
    cout << "  access 0\n";
    cout << "  rank a 10\n";
    cout << "  select x 3\n\n";
    cout << "Enter your string now:\n";

    string s;
    if (!getline(cin, s)) return 0;

    WaveletTree wt;
    wt.build(s);

    cout << "\nWavelet Tree built for string of length " << (int)s.size() << ".\n";
    cout << "Type 'help' to see commands again.\n\n";

    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "exit") break;

        if (cmd == "help") {
            cout << "Commands:\n";
            cout << "  access i       -> character at index i (0-based)\n";
            cout << "  rank c p       -> occurrences of c in s[0..p] (p 0-based, inclusive)\n";
            cout << "  select c k     -> index (0-based) of k-th occurrence of c (k 1-based)\n";
            cout << "  exit           -> quit\n";
            continue;
        }

        if (cmd == "access") {
            int i;
            if (!(ss >> i)) {
                cout << "Invalid usage. Example: access 3\n";
                continue;
            }
            char x = wt.access(i);
            if (x == '\0') cout << "Index out of range or tree empty\n";
            else           cout << "character at index " << i << " : '" << x << "'\n";
        }
        else if (cmd == "rank") {
            char c; int p;
            if (!(ss >> c >> p)) {
                cout << "Invalid usage. Example: rank a 10\n";
                continue;
            }
            int ans = wt.rank(c, p);
            cout << "rank('" << c << "', " << p << ") = " << ans << "\n";
        }
        else if (cmd == "select") {
            char c; int k;
            if (!(ss >> c >> k)) {
                cout << "Invalid usage. Example: select a 2\n";
                continue;
            }
            int idx = wt.select(c, k);
            if (idx == -1)
                cout << "select('" << c << "', " << k << ") -> not found (fewer occurrences)\n";
            else
                cout << "select('" << c << "', " << k << ") = " << idx << "\n";
        }
        else {
            cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }

    cout << "Goodbye!\n";
    return 0;
}
