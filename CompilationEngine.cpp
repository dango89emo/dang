#include "CompilationEngine.hpp"
#include "XMLGenerator.hpp"
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>


CompilationEngine::CompilationEngine(const std::string& inputPath, const std::string& outputPath){
    input_root = XMLGenerator::parseXML(inputPath);
    std::shared_ptr<XMLGenerator> p(new XMLGenerator("class"));
    output = p;
    xmlPath = outputPath;
}

void CompilationEngine::compileClass(){
    
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    if (children[i]->tagName != "keyword" | children[i]->content != "class") {
        std::cerr << "モジュールのトップレベルはclassにしてください" << std::endl;
        exit(1);
    }
    i++;
    if (children[i]->tagName != "identifier"){
        std::cerr << "classの名前を指定してください" << std::endl;
        exit(1);
    }
    i++;
    if (children[i]->tagName!="symbol" | children[i]->content != "{"){
        std::cerr << "classの定義には最初に'{'が必要です" << std::endl; 
        exit(1);
    }
    i++;
    std::shared_ptr<XMLElement> classElement = output->getRoot(); 

    for (int j=0; j < i; j++){
        classElement->addChild(children[j]);
    }
     
    compileClassVarDec();
    compileSubroutine();

    // if(children[i]->tagName!="symbol" | children[i]->content != "}"){
    //     std::cerr << "classの定義には最後に'}'が必要です" << std::endl; 
    //     exit(1);
    // }
    // classElement->addChild(children[i]);
    output->generateXML(xmlPath);
}

void CompilationEngine::compileClassVarDec(){
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;

    while (true) {
        int startPoint = i;
        if (children[i]->content != "static" && children[i]->content != "field"){
            break;
        }
        i++;

        if(!isType(children[i])){
            std::cerr << "クラス変数を定義する時には、次にtypeを指定してください" << std::endl;
            exit(1);
        }
        i++;
        while (true) {
            if(children[i]->tagName != "identifier"){
                std::cerr << "クラス変数名を指定してください" << std::endl;
                exit(1);
            }
            i++;
            std::cout << i << std::endl;
            if(children[i]->tagName != "symbol" | children[i]->content != ","){
                break;
            }
            i++;
        } 
        if(children[i]->tagName!="symbol" | children[i]->content!=";"){
            std::cerr << "クラス変数の定義を終えるときには、最後に;が必要です。" << std::endl;
            exit(1);
        }
        i++;
        output->addElement("classVarDec");
        std::shared_ptr<XMLElement> classVarDec = output->getRoot()->children.back(); 
        for (int j = startPoint; j < i; j++) {
            classVarDec->addChild(children[j]);
        }
    }
}

bool CompilationEngine::isType(std::shared_ptr<XMLElement> element) {
    std::vector<std::string> typeList = {"int", "char", "boolean"};
    std::vector<std::string> classNameList = {className};  // TODO: プログラム全体のクラス名のリストを取得する
    std::cout << element->tagName << element->content << std::endl;
    // 予約語の型名のパターン
    if (element->tagName == "keyword" && isInList(typeList, element->content)) {
        return true;
    }

    // クラス名のパターン
    if (element->tagName == "identifier" && isInList(classNameList, element->content)){
        return true;
    }
    return false;
}

void CompilationEngine::compileSubroutine(){
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    int startPoint = i;
    std::vector<std::string> subroutineKeywords = {"constructor", "function", "method"};
    while (true) {
        if (i == startPoint) {
            if (children[i]->tagName != "keyword" | !isInList(subroutineKeywords, children[i]->content)){
                std::cerr << "サブルーチンの種類を指定してください" << std::endl;
                exit(1);
            }
        } else {
            if (children[i]->tagName != "keyword" | !isInList(subroutineKeywords, children[i]->content)){
                break;
            }
        }
        i++;

        if (!isType(children[i]) && (children[i]->tagName != "keyword" | children[i]->content != "void")) {
            std::cerr << "戻り値型を指定してください" << std::endl;
            exit(1);
        }
        i++;

        if (children[i]->tagName != "identifier"){
            std::cerr << "サブルーチンの名前を指定してください" << std::endl;
            exit(1);
        }
        i++;
        if (children[i]->tagName != "symbol" | children[i]->content != "("){
            std::cerr << "パラメータの指定のために'('が必要です。" << std::endl;
            exit(1);
        }
        i++;
        output->addElement("subroutineDec");
        std::shared_ptr<XMLElement> subroutineDec = output->getRoot()->children.back(); 
        for (int j = startPoint; j < i; j++) {
            subroutineDec->addChild(children[j]);
        }
        compileParameterList();
        startPoint = i;
        if (children[i]->tagName != "symbol" | children[i]->content != ")"){
            std::cerr << "パラメータの指定のために')'が必要です。" << std::endl;
            exit(1);
        }
        i++;
        if (children[i]->tagName != "symbol" | children[i]->content != "{"){
            std::cerr << "サブルーチンの記述には'{'が必要です。" << std::endl;
            exit(1);
        }
        i++;
        for (int j = startPoint; j < i; j++) {
            subroutineDec->addChild(children[j]);
        }
        subroutineDec->addChild(
            std::shared_ptr<XMLElement>(new XMLElement("subroutineBody"))
            );
        compileVarDec();
        compileStatements();
        // if (children[i]->tagName != "symbol" | children[i]->content != "}"){
        //     std::cerr << "サブルーチンの記述には'}'が必要です。" << std::endl;
        //     exit(1);
        // }
        // subroutineDec->addChild(children[i]);
        i++;
    }
}

void CompilationEngine::compileParameterList(){
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    std::shared_ptr<XMLElement> subroutineDec = output->getRoot()->children.back(); 
    subroutineDec->addChild(
            std::shared_ptr<XMLElement>(new XMLElement("parameterList"))
    );
    int startPoint = i;
    while(true){
        if (!isType(children[i])){break;}
        i++;
        if (children[i]->tagName != "identifier") {
            std::cerr << "パラメータ名を指定してください" << std::endl;
            exit(1);
        }
        i++;
        if (children[i]->tagName !="symbol" | children[i]->content!=","){break;}
    }
    if(startPoint==i){return;}
    for(int j = startPoint ; j < i; j++){
        subroutineDec->children.back()->addChild(children[j]);
    }
}
void CompilationEngine::compileVarDec(){

}

void CompilationEngine::compileStatements(){

}

bool CompilationEngine::isInList(const std::vector<std::string>& list, const std::string& target) {
    return std::find(list.begin(), list.end(), target) != list.end();
}
