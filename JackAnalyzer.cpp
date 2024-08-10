#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>

class JackTokenizer {
private:
    std::vector<std::string> tokens;
    size_t currentIndex;

    static const std::vector<std::string> keyWords;
    static const std::string symbolsRegex;
    static const std::regex tokenRegex;

    std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file) {
            std::cerr << "ファイルを開けませんでした: " << filePath << std::endl;
            exit(1);
        }
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    void tokenize(const std::string& input) {
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

    std::string removeComments(const std::string& input) {
        std::regex singleLineComment(R"(//.*?(?:\n|$))");
        std::regex multiLineComment(R"(/\*[\s\S]*?\*/)");
        std::string noSingleLineComments = std::regex_replace(input, singleLineComment, "\n");
        return std::regex_replace(noSingleLineComments, multiLineComment, "");
    }

public:
    std::string currentToken;

    JackTokenizer(const std::string& filePath) : currentIndex(0) {
        std::string fileContent = readFile(filePath);
        tokenize(fileContent);
    }

    bool hasMoreTokens() const {
        return currentIndex < tokens.size();
    }

    void advance() {
        if (!hasMoreTokens()) {
            std::cerr << "トークンが存在しません" << std::endl;
            exit(1);
        }
        currentToken = tokens[currentIndex++];
    }

    std::string tokenType() const {
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
};

const std::vector<std::string> JackTokenizer::keyWords = {
    "class", "constructor", "function", "method", "field", "static", "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do", "if", "else", "while", "return"
};

const std::string JackTokenizer::symbolsRegex = R"([{}()\[\].,;+\-*/&|<>=~])";

const std::regex JackTokenizer::tokenRegex(
    "(" + symbolsRegex + ")"
    R"(|(\b\d+\b))"
    R"(|("[^"\n]*")|)"
    R"((\b[a-zA-Z_]\w*\b))"
);

class JackAnalyzer {
private:
    std::string filePath;

public:
    JackAnalyzer(const std::string& path) : filePath(path) {}

    void run() {
        JackTokenizer tokenizer(filePath);
        while (tokenizer.hasMoreTokens()) {
            tokenizer.advance();
            std::cout << "Token: " << tokenizer.currentToken << ", Type: " << tokenizer.tokenType() << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "使用方法: " << argv[0] << " <ファイル名>" << std::endl;
        return 1;
    }

    JackAnalyzer analyzer(argv[1]);
    analyzer.run();

    return 0;
}