// Compile with:
// g++ -std=c++17 -g -O2 -Wall -Wextra -Wno-sign-compare -o checker checker.cc

// For plotting solutions, the dot program should be installed
// https://graphviz.org

#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

using namespace std;
using Word = vector<int>;

int n;
vector<Word> A;
vector<Word> R;

vector<vector<int>>       trans;
vector<int>               final;

ostream& operator <<(ostream& out, const Word& w) {
  for (int wi : w) out << wi;
  return out;
}

void read_block(vector<Word>& X) {
  int x;
  cin >> x;
  X.resize(x);
  for (auto& w : X) {
    int m;
    cin >> m;
    assert(m >= 0);
    w.resize(m);
    for (int& wi : w) {
      cin >> wi;
      assert(wi == 0 or wi == 1);
    }
  }
}

void read_input() {
  read_block(A);
  read_block(R);
}


void read_output() {

  cin >> n;
  trans = vector<vector<int>>(n, vector<int>(2));
  final = vector<int>(n);
  for (int s = 0; s < n; ++s) {
    for (int b = 0; b <= 1; ++b) {
      int t;
      cin >> t;
      assert(0 <= t and t < n);
      trans[s][b] = t;
    }
    int f;
    cin >> f;
    assert(f == 0 or f == 1);
    final[s] = f;
  }
}


bool is_accepted(const Word& w) {
  int s = 0;
  for (int wi : w) {
    assert(0 <= s and s < n);
    assert(wi == 0 or wi == 1);
    s = trans[s][wi];
  }
  return final[s];
}


bool check_solution() {

  bool ok = true;
  
  for (const auto& w : A)
    if (not is_accepted(w)) {
      cout << "Error: word " << w << " should be accepted but is not" << endl;
      ok = false;
    }

  for (const auto& w : R)
    if (is_accepted(w)) {
      cout << "Error: word " << w << " should be rejected but is not" << endl;
      ok = false;
    }

  if (ok) cout << "OK" << endl;
  return ok;
}


void plot_solution() {
  cout << "Plotting solution to file tmp.png" << endl;
  ofstream ost("tmp.dot");

  int n = final.size();
  int m = trans[0].size();
  ost << "digraph finite_automaton {" 		    << endl;
  ost << "    rankdir=LR;"            		    << endl;
  ost << "    size=\"8,5\""                               << endl;
  ost << ""                                               << endl;

  for (int s = 0; s < n; ++s) {
    if (final[s])
      ost << "    node [shape = doublecircle, fillcolor = gold, style = filled]; q" << s << ";" << endl;
    else
      ost << "    node [shape =       circle, fillcolor = gold, style = filled]; q" << s << ";"           << endl;
  }
  
  ost << "    node [shape = point ]; qi"                  << endl;
  ost << ""                                               << endl;
  ost << "    qi -> q0;"                                  << endl;
  ost << ""                                               << endl;

  for (int s = 0; s < n; ++s) {
    for (int b = 0; b < m; ++b) {
      int t = trans[s][b];
      ost << "    q" << s << " -> q" << t << " [ label = \"" << b << "\" ];" << endl;
    }
  }
  ost << "}"                          << endl;
  
  ost.close();
  // int res = system("dot -Teps tmp.dot -o tmp.eps");
  int res = system("dot -Tpng tmp.dot -o tmp.png");
  assert(res == 0);
}


int main(int argc, char *argv[]) {

  // Write help message.
  if (argc != 1) {
    cout << "Makes a sanity check of a solution to the Minimum Consistent Finite Automaton Problem" << endl;
    cout << "Usage: " << argv[0] << " < output_file" << endl;
    exit(0);
  }
  
  read_input();
  read_output();
  bool ok = check_solution();
  if (ok) plot_solution();
}
