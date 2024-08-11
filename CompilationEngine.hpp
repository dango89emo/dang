#ifndef COMPILATION_ENGINE_HPP
#define COMPILATION_ENGINE_HPP

#include <string>
#include "XMLElement.hpp"
#include "XMLGenerator.hpp"

class CompilationEngine {
private:
    std::shared_ptr<XMLElement> input_root;
    std::shared_ptr<XMLGenerator> output;
    std::string xmlPath;
    std::string className;
    int i = 0;
    bool isInList(const std::vector<std::string>& list, const std::string& target);
    bool isType(std::shared_ptr<XMLElement> element);

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