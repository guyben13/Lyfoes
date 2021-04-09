#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

using namespace std;

#define DEB std::cerr << __FILE__ << ":" << __LINE__ << ":" << std::endl
#define out(x) #x << "=" << (x) << "; "

std::string INPUT =
    "wppg,rkgk,BcGr,bGBB,Gbwp,YbPo,brYc,ocwy|"
    "gYgP,wPpc,yoBY,okky,GyPr,    ,    ";

string Char(char c) {
  static const map<char, string> colors = {
      {'y', "33"},       {'p', "38;5;200"}, {'P', "38;5;55"}, {'Y', "90"},
      {'b', "38;5;32"},  {'B', "34"},       {'g', "92"},      {'G', "32"},
      {'o', "38;5;214"}, {'w', "97"},       {'r', "91"},      {'c', "36"},
      {'k', "38;5;168"},
  };
  static const string NO_COLOR = "\e[m";
  auto iter = colors.find(c);
  string color = iter == colors.end() ? "" : iter->second;
  return "\e[" + color + "m" + c + NO_COLOR;
}

struct Step {
  Step(char c, size_t from, size_t to) : c(c), from(from), to(to) {}
  char c;
  size_t from;
  size_t to;
  size_t count = 0;
};

class State {
 public:
  vector<Step> steps;
  State(size_t n) : data(n, 0) {}
  State(string s) : State((s.size() + 4) / 5) {
    if (s.size()+1 != data.size() * 5)
      cerr << "Error: " << s.size() << " " << data.size() << std::endl;
    map<char, size_t> cc;
    for (size_t i = 0; i < data.size(); ++i) {
      for (size_t j = 0; j < 4; ++j) {
        char c = s.at(5 * i + j);
        if (c != ' ') {
          cc.emplace(c, 0).first->second++;
          Push(c, i);
        }
      }
    }
    bool error = false;
    for (const auto &p : cc) {
      if (p.second != 4) {
        cerr << "Error: " << p.first << " -> " << p.second << endl;
        error = true;
      }
    }
    if (error) exit(-1);
  }

  vector<uint32_t> Sorted() const {
    auto res = data;
    sort(res.begin(), res.end());
    return res;
  }

  bool IsDone(size_t i) const { return IsDoneInternal(data.at(i)); }

  static bool IsDoneInternal(uint32_t x) {
    uint8_t a = x;
    uint8_t b = x >> 8;
    uint8_t c = x >> 16;
    uint8_t d = x >> 24;
    return a == b && a == c && a == d;
  }

  bool IsDone() const {
    for (uint32_t x : data) {
      if (!IsDoneInternal(x)) return false;
    }
    return true;
  }

  bool HasSpace(size_t i) const { return data.at(i) < 0x1000000; }

  size_t Size() const { return data.size(); }

  void Push(char c, size_t i) {
    data.at(i) = (data.at(i) << 8) | (unsigned char)(c);
  }

  char Top(size_t i) const { return char((unsigned char)(data.at(i))); }

  void Pop(size_t i) { data.at(i) >>= 8; }

  vector<State> Neighbors() const {
    vector<State> res, r2;
    char pc = steps.empty() ? 0 : steps.back().c;
    for (size_t i = 0; i < Size(); ++i) {
      if (IsDone(i)) continue;
      char c = Top(i);
      if (!c) continue;
      for (size_t j = 0; j < Size(); ++j) {
        if (i == j) continue;
        if (!HasSpace(j)) continue;
        if (Top(j) != c && Top(j)) continue;
        State n = *this;
        n.steps.emplace_back(c, i + 1, j + 1);
        bool was_empty = !Top(j);
        while (n.HasSpace(j) && n.Top(i) == c && (n.Top(j) == c || !n.Top(j))) {
          n.Pop(i);
          n.Push(c, j);
          ++n.steps.back().count;
          if (was_empty && (n.Top(i) == c || !n.Top(i))) continue;
          if (n.HasSpace(j) && n.Top(i) == c) continue;
          if (c == pc)
            res.push_back(n);
          else
            r2.push_back(n);
        }
      }
    }
    res.insert(res.end(), r2.begin(), r2.end());
    return res;
  }

  void Print() const {
    cout << endl;
    for (size_t i = 0; i < data.size(); i++) {
      cout << ((i + 1) % 10) << ' ';
    }
    cout << endl;
    for (size_t i = 0; i < 4; i++) {
      for (uint32_t d : data) {
        if (d)
          while (d < 0x1000000) d <<= 8;
        char c = d >> (8 * i);
        if (c)
          cout << Char(c);
        else
          cout << ' ';
        cout << '|';
      }
      cout << endl;
    }
  }

 private:
  vector<uint32_t> data;
};

class Graph {
 public:
  template <typename I>
  struct Range_ {
    pair<I, I> p;
    Range_(pair<I, I> &&p) : p(move(p)) {}
    I begin() { return p.first; }
    I end() { return p.second; }
  };
  template <typename I>
  static Range_<I> Range(pair<I, I> &&p) {
    return Range_<I>(move(p));
  }
  using Node = vector<uint32_t>;
  using Map = map<Node, size_t>;
  using Edges = multimap<size_t, size_t>;
  using Vec = vector<bool>;

  Map nodes;
  Edges edges;
  vector<size_t> GetPossible(const State &s) const {
    vector<size_t> res(nodes.size(), 0);
    size_t o = GetIndex(s);
    deque<size_t> q;
    q.push_back(o);
    res.at(o) = 1;
    while (!q.empty()) {
      size_t x = q.front();
      q.pop_front();
      size_t prev = res.at(x);
      if (!prev) {
        cerr << "error" << endl;
        exit(-1);
      }
      for (const auto &p : Range(edges.equal_range(x))) {
        size_t y = p.second;
        if (res.at(y)) continue;
        res.at(y) = prev + 1;
        q.push_back(y);
      }
    }
    return res;
  }

  size_t NumNodes() const { return nodes.size(); }
  size_t NumEdges() const { return edges.size(); }

  bool AddEdge(const State &from, size_t to) {
    auto iter_and_suc = nodes.emplace(from.Sorted(), nodes.size());
    edges.emplace(iter_and_suc.first->second, to);
    return iter_and_suc.second;
  }

  size_t GetIndex(const State &node) const {
    auto iter = nodes.find(node.Sorted());
    if (iter == nodes.end()) {
      cerr << "Error, node not found" << endl;
      exit(-1);
    }
    return iter->second;
  }
};

ostream &operator<<(ostream &out, const Step &step) {
  if (step.count > 1)
    out << step.count;
  else
    out << " ";
  return out << Char(step.c) << " " << setw(2) << step.from << " -> " << setw(2)
             << step.to;
}

int main(int argc, char *argv[]) {
  string input = INPUT;  //"kyyr,bkwg,gBYG,cpww,PPrG,pBrg,kwyy,bgYo|"
                         //   "BGcY,YbPb,koop,PocG,pBcr,    ,    .";
  if (argc >= 2) {
    input = argv[1];
  }

  cout << input << endl;

  State origin(input);
  State destination(0);
  deque<State> q;
  q.push_back(origin);
  Graph graph;
  graph.AddEdge(origin, 0);
  size_t checked = 0;
  size_t max_q = 0;
  while (!q.empty()) {
    max_q = max(max_q, q.size());
    State state = move(q.front());
    size_t state_index = graph.GetIndex(state);
    q.pop_front();
    ++checked;
    if (state.IsDone()) {
      destination = state;
    }

    for (auto &n : state.Neighbors()) {
      if (!graph.AddEdge(n, state_index)) continue;
      q.emplace_back(move(n));
    }
  }

  if (!destination.Size()) {
    cerr << "No Solution\n";
    return -1;
  }
  vector<size_t> possible = graph.GetPossible(destination);
  cout << out(destination.steps.size()) << endl;
  cout << out(checked) << out(graph.NumNodes()) << out(graph.NumEdges())
       << out(q.size()) << out(max_q) << endl;
  cout << "Steps:" << endl;
  int cnt = 0;
  for (const Step &step : destination.steps) {
    cout << step;

    ++cnt;
    if (cnt % 4)
      cout << "; ";
    else
      cout << endl;
  }
  cout << endl;

  State s = origin;
  while (!s.IsDone()) {
    s.Print();
    size_t curr = possible[graph.GetIndex(s)];
    auto ns = s.Neighbors();
    if (ns.empty()) {
      cout << "stuck" << endl;
      break;
    }
    for (size_t i = 0; i < ns.size(); ++i) {
      const State &n = ns.at(i);
      size_t next = possible[graph.GetIndex(n)];
      cout << (!next ? '-' : next < curr ? '+' : '=') << i << " "
           << n.steps.back() << endl;
    }
    while (true) {
      cout << "move: ";
      size_t m;
      cin >> m;
      if (m >= ns.size()) {
        cout << "Bad selection " << m << endl;
        continue;
      }
      if (!possible[graph.GetIndex(ns.at(m))]) {
        cout << "No! that's impossible! " << m << endl;
        continue;
      }
      s = ns.at(m);
      break;
    }
  }

  return 0;
}

