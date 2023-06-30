#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

// 此类读入并存储了候选词和可用词集合
// 调用此类的两个成员函数可以判断一个单词是否属于候选词/可用词集合
class WordSet {
public:

    // 在构造时，从文件读入候选集和可用集。
    // 你需要在代码中实例化一个 WordSet 类型变量。
    WordSet() {
        std::ifstream FINAL_FILE; 
        FINAL_FILE.open("./FINAL.txt", std::ifstream::in);
        while(!FINAL_FILE.eof()) {
            std::string str;
            FINAL_FILE >> str;
            final_set.insert(str);
        } 
        
        std::ifstream ACCEPTABLE_FILE;
        ACCEPTABLE_FILE.open("./ACC.txt", std::ifstream::in);
        while(!ACCEPTABLE_FILE.eof()) {
            std::string str;
            ACCEPTABLE_FILE >> str;
            acceptable_set.insert(str);
        }

    }

    // 判断输入参数 word 是否属于候选集合
    bool in_final_set(std::string word) {
        return final_set.find(word) != final_set.end();
    }
    
    // 判断输入参数 word 是否属于可用集合
    bool in_acceptable_set(std::string word) {
        return acceptable_set.find(word) != acceptable_set.end();
    }

private:
    std::set<std::string> final_set;
    std::set<std::string> acceptable_set;
};

//TODO：你可以添加类或者函数

int main() {
    //TODO：程序从这里开始运行
    return 0;
}
