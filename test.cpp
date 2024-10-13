#include <iostream>

#include "mystl.h"  // 假设 rb_tree 的定义在这个头文件中

int main() {
    // // 创建一个 rb_tree 实例
    // mystl::rb_tree<int, std::less<int>> tree;

    // // 测试插入操作
    // auto result1 = tree.insert_unique(10);
    // auto result2 = tree.insert_unique(20);
    // auto result3 = tree.insert_unique(10);  // 插入重复元素

    // // 输出插入结果
    // std::cout << "Insert 10: " << (result1.second ? "Success" : "Fail")
    //           << std::endl;
    // std::cout << "Insert 20: " << (result2.second ? "Success" : "Fail")
    //           << std::endl;
    // std::cout << "Insert 10 again: " << (result3.second ? "Success" : "Fail")
    //           << std::endl;

    // // 测试查找操作
    // auto it1 = tree.find(10);
    // auto it2 = tree.find(30);  // 查找不存在的元素

    // // 输出查找结果
    // if (it1 != tree.end()) {
    //     std::cout << "Find 10: Success" << std::endl;
    // } else {
    //     std::cout << "Find 10: Fail" << std::endl;
    // }

    // if (it2 != tree.end()) {
    //     std::cout << "Find 30: Success" << std::endl;
    // } else {
    //     std::cout << "Find 30: Fail" << std::endl;
    // }

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

    // 创建一个空的map
    mystl::map<int, std::string> myMap;

    // 插入一些元素
    myMap.insert(mystl::make_pair(1, "one"));
    myMap.insert(mystl::make_pair(2, "two"));
    myMap.insert(mystl::make_pair(3, "three"));

    // 使用operator[]访问和插入元素
    std::cout << "Using operator[]:" << std::endl;
    std::cout << "myMap[1] = " << myMap[1] << std::endl;  // 输出: one
    std::cout << "myMap[4] = " << myMap[4]
              << std::endl;  // 插入键为4的元素，值为默认值

    // // 使用at访问元素，如果键不存在则抛出异常
    try {
        std::cout << "myMap.at(2) = " << myMap.at(2) << std::endl;  // 输出: two
        std::cout << "myMap.at(5) = " << myMap.at(5) << std::endl;  // 抛出异常
    } catch (const std::out_of_range& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    // // 遍历map
    std::cout << "Iterating over map:" << std::endl;
    for (const auto& pair : myMap) {
        std::cout << pair.first << " => " << pair.second << std::endl;
    }

    // // 删除元素
    myMap.erase(2);  // 删除键为2的元素

    // // 再次遍历map，验证删除操作
    std::cout << "Map after erasing key 2:" << std::endl;
    for (const auto& pair : myMap) {
        std::cout << pair.first << " => " << pair.second << std::endl;
    }

    return 0;
}