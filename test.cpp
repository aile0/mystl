#include <iostream>

#include "mystl.h"  // 假设 rb_tree 的定义在这个头文件中

int main() {
    // 创建一个 rb_tree 实例
    mystl::rb_tree<int, std::less<int>> tree;

    // 测试插入操作
    auto result1 = tree.insert_unique(10);
    auto result2 = tree.insert_unique(20);
    auto result3 = tree.insert_unique(10);  // 插入重复元素

    // 输出插入结果
    std::cout << "Insert 10: " << (result1.second ? "Success" : "Fail")
              << std::endl;
    std::cout << "Insert 20: " << (result2.second ? "Success" : "Fail")
              << std::endl;
    std::cout << "Insert 10 again: " << (result3.second ? "Success" : "Fail")
              << std::endl;

    // 测试查找操作
    auto it1 = tree.find(10);
    auto it2 = tree.find(30);  // 查找不存在的元素

    // 输出查找结果
    if (it1 != tree.end()) {
        std::cout << "Find 10: Success" << std::endl;
    } else {
        std::cout << "Find 10: Fail" << std::endl;
    }

    if (it2 != tree.end()) {
        std::cout << "Find 30: Success" << std::endl;
    } else {
        std::cout << "Find 30: Fail" << std::endl;
    }

    tree.erase(it1);
    
    tree.clear();
    // // 测试删除操作
    // auto tmp = tree.erase(it1);
    // std::cout << "tmp: " << *tmp << std::endl;
    // auto it3 = tree.find(10);

    // // 输出删除结果
    // if (it3 != tree.end()) {
    //     std::cout << "Erase 10: Fail" << std::endl;
    // } else {
    //     std::cout << "Erase 10: Success" << std::endl;
    // }

    return 0;
}