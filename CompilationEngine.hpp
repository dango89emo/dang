#ifndef COMPILATION_ENGINE_HPP
#define COMPILATION_ENGINE_HPP

#include <string>
#include <vector>
#include <memory>
#include "XMLElement.hpp"
#include "XMLGenerator.hpp"

class CompilationEngine {
private:
    std::vector<std::shared_ptr<XMLElement>> children;
    std::shared_ptr<XMLGenerator> output;
    std::shared_ptr<XMLElement> currentElement;
    std::string xmlPath;
    int i = 0;

    bool isInList(const std::vector<std::string>& list, const std::string& target);
    bool isType(std::shared_ptr<XMLElement> element);
    bool isOperator(const std::string& symbol);
    bool isKeywordConstant(const std::string& keyword);
    void compileSubroutineCall();

public:
    CompilationEngine(const std::string& inputPath, const std::string& outputPath);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();
};

#endif // COMPILATION_ENGINE_HPP