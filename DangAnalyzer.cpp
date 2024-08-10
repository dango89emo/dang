#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "DangTokenizer.hpp"
#include "XMLGenerator.hpp"

class DangAnalyzer {
private:
    std::string filePath;

public:
    DangAnalyzer(const std::string& path) : filePath(path) {}

    void run() {
        std::string rootTag = "tokens";
        XMLGenerator generator(rootTag);
        std::string input;
        DangTokenizer tokenizer(filePath);
        while (tokenizer.hasMoreTokens()) {
            tokenizer.advance();
            auto tokenType = tokenizer.tokenType();
            if (tokenType == "KEYWORD") {
                generator.addElement("keyword", tokenizer.currentToken);
            } else if (tokenType == "SYMBOL") {
                generator.addElement("symbol", tokenizer.currentToken);
            } else if (tokenType == "IDENTIFIER") {
                generator.addElement("identifier", tokenizer.currentToken);
            } else if (tokenType == "INT_CONST") {
                generator.addElement("integerConstant", std::to_string(tokenizer.intVal()));
            } else if (tokenType == "STRING_CONST") {
                generator.addElement("stringConstant", tokenizer.stringVal());
            }
        }
        generator.generateXML("output.xml");
    }
};



int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "使用方法: " << argv[0] << " <ファイル名>" << std::endl;
        return 1;
    }

    DangAnalyzer analyzer(argv[1]);
    analyzer.run();

    return 0;
}