#include "CompilationEngine.hpp"
#include "XMLGenerator.hpp"
#include <iostream>

#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

// 方法1: std::remove_ifとerase
std::string removeWhitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

CompilationEngine::CompilationEngine(const std::string& inputPath, const std::string& outputPath){
    input_root = XMLGenerator::parseXML(inputPath);
    std::shared_ptr<XMLGenerator> p(new XMLGenerator("class"));
    output = p;
    xmlPath = outputPath;
}

void CompilationEngine::compileClass(){
    
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;

    if (
        removeWhitespace(children[0]->content) != "class" |
        children[1]->tagName != "identifier" |
        removeWhitespace(children[2]->content) !="{" |
        removeWhitespace(children.back()->content) != "}"
        ) {
        std::cerr << "モジュールのトップレベルはclassにしてください" << std::endl;
        exit(1);
    }
    output->addElement("keyword", "class");
    output->addElement("identifier", children[1]->content);
    output->addElement("symbol", "{");
    children.erase(children.begin());
    children.erase(children.begin());
    children.erase(children.begin());
    children.pop_back();
    compileClassVarDec();
    compileSubroutine();
    output->addElement("symbol","}");
    output->generateXML(xmlPath);
}

void CompilationEngine::compileClassVarDec(){

}

void CompilationEngine::compileSubroutine(){

}