#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "DangTokenizer.hpp"
#include "CompilationEngine.hpp"
#include "XMLGenerator.hpp"
#include "ClassNames.hpp"

std::vector<std::string> classNames;

class DangAnalyzer {
private:
    std::vector<std::string> filePaths;

    void findDangFiles(const std::string& path) {
        if (path.substr(path.length() - 5) == ".dang") {
            filePaths.push_back(path);
        } else if (std::filesystem::is_directory(path)) {
            for (const auto & entry : std::filesystem::directory_iterator(path)) {
                if (entry.path().extension() == ".dang") {
                    filePaths.push_back(entry.path().string());
                }
            }
        }
    }

    void registerClassNames() {
        for (auto c :{"Math", "String", "Array", "Output", "Screen", "Keyboard", "Memory", "Sys"}){
            classNames.push_back(c);
        }
        for (const auto& filePath : filePaths) {
            DangTokenizer tokenizer(filePath);
            bool foundClass = false;
            while (tokenizer.hasMoreTokens()) {
                tokenizer.advance();
                if (foundClass) {
                    if (tokenizer.tokenType() == "IDENTIFIER") {
                        classNames.push_back(tokenizer.currentToken);
                        break;
                    }
                } else if (tokenizer.tokenType() == "KEYWORD" && tokenizer.currentToken == "class") {
                    foundClass = true;
                }
            }
        }
    }

public:
    DangAnalyzer(const std::string& path) {
        findDangFiles(path);
    }

    void run() {
        registerClassNames();

        for (const auto& filePath : filePaths) {
            std::string rootTag = "tokens";
            XMLGenerator generator(rootTag);
            std::string pathToTokensXML = filePath + ".tokens.xml";
            std::string pathToParsedXML = filePath + ".parsed.xml";

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
            generator.generateXML(pathToTokensXML);
            
            CompilationEngine parser(pathToTokensXML, pathToParsedXML);
            parser.compileClass();
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "使用方法: " << argv[0] << " <ファイル名またはディレクトリ>" << std::endl;
        return 1;
    }
    DangAnalyzer analyzer(argv[1]);
    analyzer.run();
    return 0;
}
