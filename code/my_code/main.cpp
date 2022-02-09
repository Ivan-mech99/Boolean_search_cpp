#include <fcntl.h>
#include <sys/io.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <cwctype>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <string>

#include "gzstream.h"
#include "utf8.h"

void first_parsing(std::wstring unparsed, std::list<std::wstring> &parsed) {
  std::wstring token = L"";
  std::set<wchar_t> ops;
  std::wstring unparsed1 = L"";
  unparsed1 += unparsed;
  for (int k = 0; k < unparsed.size(); k++) {
    unparsed1[k] = towlower(unparsed1[k]);
  }
  ops.insert('&');
  ops.insert('|');
  ops.insert('!');
  ops.insert('(');
  ops.insert(')');
  for (auto symb : unparsed1) {
    if (ops.find(symb) != ops.end()) {
      if (token != L"") {
        parsed.push_back(token);
        token = L"";
        token += symb;
        parsed.push_back(token);
        token = L"";
      } else {
        token += symb;
        parsed.push_back(token);
        token = L"";
      }
    } else if (ops.find(symb) == ops.end() && symb == ' ') {
      if (token != L"") {
        parsed.push_back(token);
        token = L"";
      }
    } else {
      token += symb;
    }
  }
  if (token != L"") {
    parsed.push_back(token);
  }
}

bool cond(std::list<std::wstring> &stack, std::wstring &elem,
          std::map<std::wstring, int> &priority) {
  if (stack.size() > 0) {
    if (priority[stack.back()] >= priority[elem]) {
      return true;
    }
  }
  return false;
}

void polish_notation(std::list<std::wstring> &parsed,
                     std::list<std::wstring> &pol_not) {
  std::map<std::wstring, int> priority;
  std::list<std::wstring> stack;
  std::wstring tmp;
  priority[L"!"] = 3;
  priority[L"&"] = 2;
  priority[L"|"] = 1;
  priority[L"("] = 0;
  for (auto elem : parsed) {
    if (elem == L"(") {
      stack.push_back(elem);
    } else if (elem == L")") {
      while (stack.back() != L"(") {
        pol_not.push_back(stack.back());
        stack.pop_back();
      }
      stack.pop_back();
    } else if (priority.find(elem) != priority.end()) {
      while (cond(stack, elem, priority)) {
        pol_not.push_back(stack.back());
        stack.pop_back();
      }
      stack.push_back(elem);
    } else {
      pol_not.push_back(elem);
    }
  }
  if (stack.size() > 0) {
    stack.reverse();
    for (auto op : stack) {
      pol_not.push_back(op);
    }
  }
}

// type = 2, 1, 0
// elem code ! -3 & -2 | -1
void create_node(int &key, int elem_type, int elem_code, std::list<int> &stack,
                 std::map<int, std::list<int>> &tree) {
  tree[key].push_back(elem_type);
  tree[key].push_back(elem_code);
  int a1;
  int a2;
  if (elem_type == 0) {
    tree[key].push_back(-1);
    tree[key].push_back(-1);
  } else if (elem_type == 1) {
    a1 = stack.back();
    stack.pop_back();
    tree[key].push_back(-1);
    tree[key].push_back(a1);
  } else if (elem_type == 2) {
    a1 = stack.back();
    stack.pop_back();
    a2 = stack.back();
    stack.pop_back();
    tree[key].push_back(a2);
    tree[key].push_back(a1);
  }
  stack.push_back(key);
  key++;
}

int build_tree(std::map<std::wstring, unsigned int> &mp,
               std::list<std::wstring> &pol_not,
               std::map<int, std::list<int>> &tree) {
  std::list<int> stack;
  int key = 1;
  for (std::wstring elem : pol_not) {
    if (elem == L"&") {
      create_node(key, 2, -2, stack, tree);
    } else if (elem == L"|") {
      create_node(key, 2, -1, stack, tree);
    } else if (elem == L"!") {
      create_node(key, 1, -3, stack, tree);
    } else {
      create_node(key, 0, mp[elem], stack, tree);
    }
  }
  return stack.front();
}

int max(int a, int b) {
  if (a > b)
    return a;
  else
    return b;
}

int min(int a, int b) {
  if (a < b)
    return a;
  else
    return b;
}

// elem code ! -3 & -2 | -1
int srch(int key, int doc_num,
         std::map<unsigned int, std::set<unsigned int>> &rev_ind,
         std::map<int, std::list<int>> &tree, int ms) {
  std::list<int>::iterator it0 = tree[key].begin();
  int cur_type = *it0;
  it0++;
  int cur_code = *it0;
  if (cur_type == 2) {
    if (cur_code == -2) {
      std::list<int>::iterator it1 = tree[key].begin();
      it1++;
      it1++;
      std::list<int>::iterator it2 = tree[key].begin();
      it2++;
      it2++;
      it2++;
      return max(srch(*it1, doc_num, rev_ind, tree, ms),
                 srch(*it2, doc_num, rev_ind, tree, ms));
    } else {
      std::list<int>::iterator it1 = tree[key].begin();
      it1++;
      it1++;
      std::list<int>::iterator it2 = tree[key].begin();
      it2++;
      it2++;
      it2++;
      return min(srch(*it1, doc_num, rev_ind, tree, ms),
                 srch(*it2, doc_num, rev_ind, tree, ms));
    }
  } else if (cur_type == 1) {
    if (cur_code == -3) {
      std::list<int>::iterator it1 = tree[key].begin();
      it1++;
      it1++;
      it1++;
      int tmp = srch(*it1, doc_num, rev_ind, tree, ms);
      if (tmp == doc_num) {
        return tmp + 1;
      } else {
        return doc_num;
      }
    }
  } else {
    std::list<int>::iterator it0 = tree[key].begin();
    it0++;
    auto res = rev_ind[*it0].find(doc_num);
    std::set<unsigned int>::iterator it1 = rev_ind[*it0].begin();
    std::set<unsigned int>::iterator it2 = rev_ind[*it0].end();
    auto res1 = std::lower_bound(it1, it2, doc_num);
    return (*res1);
  }
  return ms;
}

void process_stream_search(
    std::list<int> &res, std::map<std::wstring, unsigned int> &mp,
    std::list<std::wstring> &pol_not,
    std::map<unsigned int, std::set<unsigned int>> &rev_ind, int ms) {
  std::map<int, std::list<int>> tree;
  int doc_num = 1;
  int q;
  int head = build_tree(mp, pol_not, tree);
  while (doc_num < ms) {
    q = srch(head, doc_num, rev_ind, tree, ms);
    if (doc_num == q) {
      res.push_back(doc_num);
      doc_num += 1;
    } else if (doc_num < q) {
      doc_num = q;
    } else
      doc_num += 1;
  }
}

void vb1(unsigned int num, std::ofstream &out) {
  int ind = 0;
  int tmp = 0;
  int pw;
  char x;
  int z = num;
  while (z >= 128) {
    z = z / 128;
    ind += 1;
  }
  z = num;
  while (ind >= 0) {
    pw = (int)pow(128, ind);
    tmp = z / pw;
    x = tmp;
    if (ind != 0) {
      out.write((char *)&x, sizeof(x));
    } else {
      x |= 1 << 7;
      out.write((char *)&x, sizeof(x));
    }
    z = z - tmp * pw;
    ind -= 1;
  }
}

unsigned int vb1_symb_dec(std::ifstream &in) {
  char y;
  char z;
  int ind = 0;
  std::list<char> l;
  do {
    in.read((char *)&y, sizeof(y));
    if ((y & 128)) {
      z = y;
      z &= ~(1 << 7);
      l.push_front(z);
      ind++;
    } else {
      l.push_front(y);
      ind++;
    }
  } while (!(y & 128));
  unsigned int res = 0;
  unsigned int hold = 0;
  for (int i = 0; i < ind; i++) {
    hold = l.front();
    l.pop_front();
    res += (unsigned int)(hold * pow(128, i));
  }
  return res;
}

void code_dict(std::ofstream &out,
               std::map<unsigned int, std::set<unsigned int>> &mp) {
  int sz;
  sz = mp.size();
  vb1(sz, out);
  std::set<unsigned int>::iterator it;
  unsigned int prev = 0;
  for (const auto &myPair : mp) {
    vb1(myPair.first, out);
    vb1(myPair.second.size(), out);
    prev = 0;
    for (it = myPair.second.begin(); it != myPair.second.end(); it++) {
      vb1((*it) - prev, out);
      prev = (*it);
    }
  }
}

void decode_dict(std::ifstream &in,
                 std::map<unsigned int, std::set<unsigned int>> &mp) {
  unsigned int sz = vb1_symb_dec(in);
  unsigned int key;
  unsigned int cur_size;
  unsigned int prev;
  for (int i = 0; i < sz; i++) {
    key = vb1_symb_dec(in);
    cur_size = vb1_symb_dec(in);
    prev = 0;
    for (unsigned int j = 0; j < cur_size; j++) {
      prev += vb1_symb_dec(in);
      mp[key].insert(prev);
    }
  }
}

int main() {
  std::setlocale(LC_ALL, "");
  std::string line;
  std::map<std::wstring, unsigned int> word_to_num;
  std::map<std::wstring, unsigned int> url_to_num;
  std::map<unsigned int, std::set<unsigned int>> index;
  std::map<unsigned int, std::set<unsigned int>> rev_ind;
  unsigned int word_cur_num = 0;
  unsigned int url_cur_num = 0;
  unsigned int last_url = 0;
  unsigned int save_url_num = 0;
  int i = 0;
  int ii;
  int start1 = clock();
  for (ii = 1; ii < 9; ii++) {
    last_url = url_cur_num;
    std::string filename = "dump" + std::to_string(ii) + ".gz";
    const char *filename1 = filename.c_str();
    igzstream in(filename1);
    while (std::getline(in, line)) {
      std::wstring word1;
      std::wstring word2;
      std::wstring word3 = L"";
      i++;
      std::string::iterator end_it =
          utf8::find_invalid(line.begin(), line.end());
      utf8::utf8to16(line.begin(), end_it, std::back_inserter(word1));
      size_t sz = word1.size();
      if (word1.find(L"http://lenta") != std::string::npos &&
          word1.find(' ') == std::string::npos) {
        std::wstring buf = L"";
        size_t num = 0;
        int flag = 0;
        for (auto &c : word1) {
          num++;
          if (num != 1) {
            if (c != '/') {
              buf += c;
            } else {
              word3 += buf;
              word3 += c;
              buf = L"";
            }
          }
        }
        buf = L"";
        url_cur_num++;
        url_to_num[word3] = url_cur_num;
      } else {
        std::string::iterator end_it1 =
            utf8::find_invalid(line.begin(), line.end());
        utf8::utf8to16(line.begin(), end_it1, std::back_inserter(word2));
        for (int k = 0; k < word2.size(); k++) {
          word2[k] = towlower(word2[k]);
        }
        word2 = std::regex_replace(word2, std::wregex(L"[^а-яА-Яa-zA-Z0-9-]"),
                                   L" ");
        size_t num = 0;
        for (auto &c : word2) {
          num++;
          if (num == 1 && c == '-') {
            continue;
          }
          if (c != ' ' && num != sz) {
            word3 += c;
          } else if (c != ' ' && num == sz) {
            word3 += c;
            if (word_to_num.find(word3) == word_to_num.end() &&
                url_cur_num != last_url) {
              word_cur_num++;
              word_to_num[word3] = word_cur_num;
              index[url_cur_num].insert(word_cur_num);
              rev_ind[word_cur_num].insert(url_cur_num);
              word3 = L"";
            } else if (word_to_num.find(word3) != word_to_num.end() &&
                       url_cur_num != last_url) {
              index[url_cur_num].insert(word_to_num.find(word3)->second);
              rev_ind[word_to_num.find(word3)->second].insert(url_cur_num);
              word3 = L"";
            } else {
              word3 = L"";
            }
          } else if (c == ' ' && word3 != L"") {
            if (word_to_num.find(word3) == word_to_num.end() &&
                url_cur_num != last_url) {
              word_cur_num++;
              word_to_num[word3] = word_cur_num;
              index[url_cur_num].insert(word_cur_num);
              rev_ind[word_cur_num].insert(url_cur_num);
              word3 = L"";
            } else if (word_to_num.find(word3) != word_to_num.end() &&
                       url_cur_num != last_url) {
              index[url_cur_num].insert(word_to_num.find(word3)->second);
              rev_ind[word_to_num.find(word3)->second].insert(url_cur_num);
              word3 = L"";
            } else {
              word3 = L"";
            }
          } else if (c == ' ' && word3 == L"") {
            continue;
          }
        }
      }
    }
    std::wcout << "to_help_indexes: " << ii << std::endl;
  }
  std::map<unsigned int, std::wstring> word_to_num_rev;
  std::map<unsigned int, std::wstring> url_to_num_rev;
  for (const auto &myPair : url_to_num) {
    url_to_num_rev[myPair.second] = myPair.first;
  }
  for (const auto &myPair : word_to_num) {
    word_to_num_rev[myPair.second] = myPair.first;
  }
  int end1 = clock();
  int t = (end1 - start1) / CLOCKS_PER_SEC;
  int start2 = clock();
  std::wcout << L"Построение индекса: " << t << std::endl;
  std::ofstream out1("index.txt", std::ios::binary);
  code_dict(out1, index);
  out1.close();
  std::ofstream out2("rev_ind.txt", std::ios::binary);
  code_dict(out2, rev_ind);
  out2.close();
  rev_ind.clear();
  int end2 = clock();
  t = (end2 - start2) / CLOCKS_PER_SEC;
  int start3 = clock();
  std::wcout << L"Сжатие: " << t << std::endl;
  std::ifstream in2("rev_ind.txt", std::ios::binary);
  decode_dict(in2, rev_ind);
  in2.close();
  std::ifstream in1("index.txt", std::ios::binary);
  decode_dict(in1, index);
  in2.close();
  int end3 = clock();
  t = (end3 - start3) / CLOCKS_PER_SEC;
  std::wcout << L"Распаковка: " << t << std::endl;
  std::wstring wk = L"1";
  while (wk != L"0") {
    std::list<std::wstring> parsed;
    std::list<std::wstring> pol_not;
    std::wstring unparsed;
    std::wcout << L"Введите запрос: " << std::endl;
    getline(std::wcin, unparsed);
    std::wcout << unparsed << std::endl;
    first_parsing(unparsed, parsed);
    polish_notation(parsed, pol_not);
    std::list<int> res;
    process_stream_search(res, word_to_num, pol_not, rev_ind, index.size() + 1);
    for (auto elem : res) {
      std::wcout << url_to_num_rev[elem] << std::endl;
    }
    std::wcout << L"Результатов: " << res.size() << std::endl;
    std::wcout << L"Продолжить работу?" << std::endl;
    getline(std::wcin, wk);
  }
}

