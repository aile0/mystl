#include <iostream>

#include "mystl.h"

class less {
   public:
    bool operator()(int a, int b) const { return a < b; }
};

int main() {
    // test rb_tree
    mystl::rb_tree<int, less> tree;
    tree.insert_unique(10);
    tree.insert_unique(20);
    tree.insert_unique(30);
    tree.insert_unique(40);
    std::cout << "tree is empty: " << tree.empty() << std::endl;
    std::cout << "tree size: " << tree.size() << std::endl;
    // test rb_tree end

    std::cout << "hello world" << std::endl;
    return 0;
}