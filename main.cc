#include "cptree.h"

int main() {
  std::vector<int> a {1, 2, 1, 2, 3, 4, 5, 2};

  cp_node *root = new cp_node(0);

  for(auto i:a) {
    if(!root->hasChild(i))
      root->addChild(i);
  }

  root->removeChild(2);
  root->removeChild(4);

  root->printItems();

  delete root;
  return 0;
}

