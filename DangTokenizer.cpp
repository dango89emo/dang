#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include "DangTokenizer.hpp"


std::string DangTokenizer::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "ファイルを開けませんでした: " << filePath << std::endl;
        exit(1);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void DangTokenizer::tokenize(const std::string& input) {
    std::string cleanedInput = removeComments(input); 
    std::sregex_iterator it(cleanedInput.begin(), cleanedInput.end(), tokenRegex);
    std::sregex_iterator end;
    while (it != end) {
        for (size_t i = 1; i < it->size(); ++i) {
            if (it->str(i) != "") {
                tokens.push_back(it->str(i));
            }
        }
        ++it;
    }
}

std::string DangTokenizer::removeComments(const std::string& input) {
    std::regex singleLineComment(R"(//[^\n]*(?:\n|$))");
    std::regex multiLineComment(R"(/\*[\s\S]*?\*/)");
    std::string noSingleLineComments = std::regex_replace(input, singleLineComment, "");
    return std::regex_replace(noSingleLineComments, multiLineComment, "");
}


DangTokenizer::DangTokenizer(const std::string& filePath) : currentIndex(0) {
    std::string fileContent = readFile(filePath);
    tokenize(fileContent);
}

bool DangTokenizer::hasMoreTokens() const {
    return currentIndex < tokens.size();
}

void DangTokenizer::advance() {
    if (!hasMoreTokens()) {
        std::cerr << "トークンが存在しません" << std::endl;
        exit(1);
    }
    currentToken = tokens[currentIndex++];
}

std::string DangTokenizer::tokenType() const {
    if (std::find(keyWords.begin(), keyWords.end(), currentToken) != keyWords.end()) {
        return "KEYWORD";
    } else if (std::regex_match(currentToken, std::regex(symbolsRegex))) {
        return "SYMBOL";
    } else if (std::regex_match(currentToken, std::regex(R"(^\d+$)"))) {
        return "INT_CONST";
    } else if (std::regex_match(currentToken, std::regex(R"(^".*"$)"))) {
        return "STRING_CONST";
    } else {
        return "IDENTIFIER";
    }
}

std::string DangTokenizer::keyWord() const {
    if (tokenType() == "KEYWORD") {
        return currentToken;
    } else {
        std::cerr << "キーワードではありません" << std::endl;
        exit(1);
    }
    return currentToken;
}

std::string DangTokenizer::symbol() const {
    if (tokenType() == "SYMBOL") {
        return currentToken;
    } else {
        std::cerr << "シンボルではありません" << std::endl;
        exit(1);
    }
    return currentToken;
}

std::string DangTokenizer::identifier() const {
    if (tokenType() == "IDENTIFIER") {
        return currentToken;
    } else {
        std::cerr << "識別子ではありません" << std::endl;
        exit(1);
    }
    return currentToken;
}

int DangTokenizer::intVal() const {
    if (tokenType() == "INT_CONST") {
        return std::stoi(currentToken);
    } else {
        std::cerr << "整数ではありません" << std::endl;
        exit(1);
    }
    return std::stoi(currentToken);
}
std::string DangTokenizer::stringVal() const {
    if (tokenType() == "STRING_CONST") {
        return currentToken.substr(1, currentToken.size() - 2);
    } else {
        std::cerr << "文字列ではありません" << std::endl;
        exit(1);
    }
    return currentToken.substr(1, currentToken.size() - 2);
}


const std::vector<std::string> DangTokenizer::keyWords = {
    "class", "constructor", "function", "method", "field", "static", "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do", "if", "else", "while", "return"
};

const std::string DangTokenizer::symbolsRegex = R"([{}()\[\].,;+\-*/&|<>=~])";

const std::regex DangTokenizer::tokenRegex(
    "(" + symbolsRegex + ")"
    R"(|(\b\d+\b))"
    R"(|("[^"\n]*")|)"
    R"((\b[a-zA-Z_]\w*\b))"
);