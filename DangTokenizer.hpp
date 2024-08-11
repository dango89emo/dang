#ifndef DANG_TOKENIZER_HPP
#define DANG_TOKENIZER_HPP

#include <vector>
#include <string>
#include <regex>

class DangTokenizer {
private:
    std::vector<std::string> tokens;
    size_t currentIndex;

    static const std::vector<std::string> keyWords;
    static const std::string symbolsRegex;
    static const std::regex tokenRegex;

    std::string readFile(const std::string& filePath);
    void tokenize(const std::string& input);
    std::string removeComments(const std::string& input);

public:
    std::string currentToken;

    DangTokenizer(const std::string& filePath);

    bool hasMoreTokens() const;
    void advance();
    std::string tokenType() const;
    std::string keyWord() const;
    std::string symbol() const;
    std::string identifier() const;
    int intVal() const;
    std::string stringVal() const;
};

#endif // DANG_TOKENIZER_HPP