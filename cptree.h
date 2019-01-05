
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <iostream>
#include <algorithm>

class cptree;
class cp_node {
  public:
    cp_node(int item, cp_node* p=nullptr): key(item), parent(p) { }
    ~cp_node() {
      for(auto c:children)
        delete c;
    }

    cp_node* addChild(int key) {
      auto node = new cp_node(key, this);
      children.push_back(node);
      return node;
    }

    cp_node* getChild(int key) {
      auto find = std::find_if( children.begin(), children.end(),
                  [&key](const cp_node* x) { return x->key == key;});
      if(find != children.end())
        return *find;
      else
        return nullptr;
    }

    std::vector<cp_node*> getChildren() {
      return children;
    }

    bool hasChild(int key) {
      return this->getChild(key) != nullptr;
    }

    void removeChild(int key) {
      cp_node* item = getChild(key);
      if(item) {
        children.erase(std::remove(children.begin(), children.end(), item), children.end());
        delete item;
      }
    }

    void printItems() {
      for(auto c:children)
        std::cout<<c->key<<"\t";
      std::cout<<std::endl;
    }

    cp_node *getParent() {
      return parent;
    }

  private:
    friend cptree;
    int key;
    cp_node *parent;
    std::vector<cp_node*> children;
};

class cptree {
  public:
    cptree() { root = new cp_node(-1); }
    ~cptree() { delete root; }

    void addSeq(std::vector<int> seq) {
      cp_node * cursornode = root;
      bool newseq = false;
      for(auto s : seq) {
        if(!cursornode->hasChild(s)) {
          cursornode = cursornode->addChild(s);
          newseq = true;
        } else {
          cursornode = cursornode->getChild(s);
        }
      }

      if(newseq) {
        seqNum++;
        items.insert(seqNum);
        for(auto s : seq) {
          II[s].insert(seqNum);
        }
        LT[seqNum] = cursornode;
        seqCount[seqNum] = 1;
      } else {
        // update count
        seqCount[seqNum] += 1;
      }
    }

    std::vector<int> predict(std::vector<int> seq) {
      std::vector<int> preds;
      if(seq.empty()) return preds;

      std::set<int> intersect = II[seq.front()];
      std::vector<int>::const_iterator it = seq.begin()+1;
      for(; it!=seq.end(); ++it) {
        std::set<int> data;
        std::set_intersection(intersect.begin(), intersect.end(),
            II[*it].begin(), II[*it].end(), 
            std::inserter(data, data.begin()));
        intersect = data;
      }

      if(intersect.empty()) return preds;

      int key = seq.back();
      std::vector<std::vector<int>> retSeq;
      for(auto i : intersect) {
        std::vector<int> sseq;
        cp_node *node = LT[i];
        while(node->key >= 0) {
          sseq.push_back(node->key);
          node = node->parent;
        }
        std::reverse(sseq.begin(), sseq.end());
        
        int index = 0, next_index = 0;
        for(auto k : seq) {
          auto it = std::find(sseq.begin(), sseq.end(), k);
          next_index = it - sseq.begin();
          index = std::max(index, next_index);
        }
        if(next_index < index) // last item in seq is not the last one in sseq
          continue;

        sseq.erase(sseq.begin(), sseq.begin()+next_index);
        retSeq.push_back(sseq);
      }

      std::map<int, float> counttable;
      for(auto sseq : retSeq) {
        int count = 1;
        for(auto s : sseq) {
          if(std::find(seq.begin(), seq.end(), s) != seq.end()) continue;
          float level = 1.0/retSeq.size(); 
          float dist = 1e-3/count;
          float score = 1+level+dist;
          if(counttable.count(s) > 0) {
            counttable[s] *= score;
          }else{
            counttable[s] = score;
          }
          count += 1;
        }
      }

      std::priority_queue<std::pair<double, int>> q;
      for (auto kv : counttable) {
        std::pair<float, int> p(kv.second, kv.first);
        q.push(p);
      }
      int k = 5; // number of indices we need
      for (int i = 0; i < k; ++i) {
        int ki = q.top().second;
        preds.push_back(ki);
        q.pop();
      }

      return preds;
    }

  private:
    int seqNum = 0;
    cp_node * root;
    std::set<int> items;
    std::map<int, int> seqCount;
    std::map<int, std::set<int>> II;
    std::map<int, cp_node *> LT;
};
