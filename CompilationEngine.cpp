#include "CompilationEngine.hpp"
#include "XMLGenerator.hpp"
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include "ClassNames.hpp"


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
    std::cout << element->tagName << element->content << std::endl;
    // 予約語の型名のパターン
    if (element->tagName == "keyword" && isInList(typeList, element->content)) {
        return true;
    }

    // クラス名のパターン
    if (element->tagName == "identifier" && isInList(classNames, element->content)){
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
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();

    while (true) {
        int startPoint = i;
        if (children[i]->content != "var") {
            break;
        }
        i++;

        if (!isType(children[i])) {
            std::cerr << "変数を定義する時には、次にtypeを指定してください" << std::endl;
            exit(1);
        }
        i++;
        while (true) {
            if (children[i]->tagName != "identifier") {
                std::cerr << "変数名を指定してください" << std::endl;
                exit(1);
            }
            i++;
            if (children[i]->tagName != "symbol" || children[i]->content != ",") {
                break;
            }
            i++;
        } 
        if (children[i]->tagName != "symbol" || children[i]->content != ";") {
            std::cerr << "変数の定義を終えるときには、最後に;が必要です。" << std::endl;
            exit(1);
        }
        i++;
        subroutineBody->addChild(std::make_shared<XMLElement>("varDec"));
        std::shared_ptr<XMLElement> varDec = subroutineBody->children.back();
        for (int j = startPoint; j < i; j++) {
            varDec->addChild(children[j]);
        }
    }
}

void CompilationEngine::compileStatements(){
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    subroutineBody->addChild(std::make_shared<XMLElement>("statements"));
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;

    while (true) {
        if (children[i]->tagName != "keyword") {
            break;
        }
        
        if (children[i]->content == "let") {
            compileLet();
        } else if (children[i]->content == "if") {
            compileIf();
        } else if (children[i]->content == "while") {
            compileWhile();
        } else if (children[i]->content == "do") {
            compileDo();
        } else if (children[i]->content == "return") {
            compileReturn();
        } else {
            break;
        }
    }
}

void CompilationEngine::compileLet() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    subroutineBody->addChild(std::make_shared<XMLElement>("statements"));
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    int startPoint = i;
    i++; // 'let' キーワードをスキップ

    if (children[i]->tagName != "identifier") {
        std::cerr << "変数名を指定してください" << std::endl;
        exit(1);
    }
    i++;

    if (children[i]->content == "[") {
        i++;
        compileExpression();
        if (children[i]->content != "]") {
            std::cerr << "配列アクセスには']'が必要です" << std::endl;
            exit(1);
        }
        i++;
    }

    if (children[i]->content != "=") {
        std::cerr << "代入には'='が必要です" << std::endl;
        exit(1);
    }
    i++;

    compileExpression();

    if (children[i]->content != ";") {
        std::cerr << "文の終わりには';'が必要です" << std::endl;
        exit(1);
    }
    i++;

    auto letStatement = std::make_shared<XMLElement>("letStatement");
    for (int j = startPoint; j < i; j++) {
        letStatement->addChild(children[j]);
    }
    currentElement->addChild(letStatement);
}

void CompilationEngine::compileIf() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    subroutineBody->addChild(std::make_shared<XMLElement>("statements"));
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    int startPoint = i;
    i++; // 'if' キーワードをスキップ

    if (children[i]->content != "(") {
        std::cerr << "条件式には'('が必要です" << std::endl;
        exit(1);
    }
    i++;

    compileExpression();

    if (children[i]->content != ")") {
        std::cerr << "条件式には')'が必要です" << std::endl;
        exit(1);
    }
    i++;

    if (children[i]->content != "{") {
        std::cerr << "if文の本体には'{'が必要です" << std::endl;
        exit(1);
    }
    i++;

    compileStatements();

    if (children[i]->content != "}") {
        std::cerr << "if文の本体には'}'が必要です" << std::endl;
        exit(1);
    }
    i++;

    if (children[i]->content == "else") {
        i++;
        if (children[i]->content != "{") {
            std::cerr << "else文の本体には'{'が必要です" << std::endl;
            exit(1);
        }
        i++;

        compileStatements();

        if (children[i]->content != "}") {
            std::cerr << "else文の本体には'}'が必要です" << std::endl;
            exit(1);
        }
        i++;
    }

    auto ifStatement = std::make_shared<XMLElement>("ifStatement");
    for (int j = startPoint; j < i; j++) {
        ifStatement->addChild(children[j]);
    }
    currentElement->addChild(ifStatement);
}


void CompilationEngine::compileWhile() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    subroutineBody->addChild(std::make_shared<XMLElement>("statements"));
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;

    int startPoint = i;
    i++; // 'while' キーワードをスキップ

    if (children[i]->content != "(") {
        std::cerr << "while文の条件式には'('が必要です" << std::endl;
        exit(1);
    }
    i++;

    compileExpression();

    if (children[i]->content != ")") {
        std::cerr << "while文の条件式には')'が必要です" << std::endl;
        exit(1);
    }
    i++;

    if (children[i]->content != "{") {
        std::cerr << "while文の本体には'{'が必要です" << std::endl;
        exit(1);
    }
    i++;

    compileStatements();

    if (children[i]->content != "}") {
        std::cerr << "while文の本体には'}'が必要です" << std::endl;
        exit(1);
    }
    i++;

    auto whileStatement = std::make_shared<XMLElement>("whileStatement");
    for (int j = startPoint; j < i; j++) {
        whileStatement->addChild(children[j]);
    }
    currentElement->addChild(whileStatement);
}

void CompilationEngine::compileDo() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    subroutineBody->addChild(std::make_shared<XMLElement>("statements"));
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    int startPoint = i;
    i++; // 'do' キーワードをスキップ

    compileSubroutineCall();

    if (children[i]->content != ";") {
        std::cerr << "do文の終わりには';'が必要です" << std::endl;
        exit(1);
    }
    i++;

    auto doStatement = std::make_shared<XMLElement>("doStatement");
    for (int j = startPoint; j < i; j++) {
        doStatement->addChild(children[j]);
    }
    currentElement->addChild(doStatement);
}

void CompilationEngine::compileReturn() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    subroutineBody->addChild(std::make_shared<XMLElement>("statements"));
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    int startPoint = i;
    i++; // 'return' キーワードをスキップ

    if (children[i]->content != ";") {
        compileExpression();
    }

    if (children[i]->content != ";") {
        std::cerr << "return文の終わりには';'が必要です" << std::endl;
        exit(1);
    }
    i++;

    auto returnStatement = std::make_shared<XMLElement>("returnStatement");
    for (int j = startPoint; j < i; j++) {
        returnStatement->addChild(children[j]);
    }
    currentElement->addChild(returnStatement);
}

void CompilationEngine::compileExpression() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    subroutineBody->addChild(std::make_shared<XMLElement>("statements"));
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    auto expressionElement = std::make_shared<XMLElement>("expression");
    
    currentElement->addChild(expressionElement);
    
    auto prevElement = currentElement;
    currentElement = expressionElement;

    compileTerm();

    while (children[i]->tagName == "symbol" && isOperator(children[i]->content)) {
        expressionElement->addChild(children[i]);
        i++;
        compileTerm();
    }

    currentElement = prevElement;
}

void CompilationEngine::compileTerm() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    subroutineBody->addChild(std::make_shared<XMLElement>("statements"));
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    auto termElement = std::make_shared<XMLElement>("term");
    currentElement->addChild(termElement);

    if (children[i]->tagName == "integerConstant" || children[i]->tagName == "stringConstant" || 
        (children[i]->tagName == "keyword" && isKeywordConstant(children[i]->content))) {
        termElement->addChild(children[i]);
        i++;
    } else if (children[i]->tagName == "identifier") {
        termElement->addChild(children[i]);
        i++;
        if (children[i]->content == "[") {
            termElement->addChild(children[i]);
            i++;
            compileExpression();
            if (children[i]->content != "]") {
                std::cerr << "配列アクセスには']'が必要です" << std::endl;
                exit(1);
            }
            termElement->addChild(children[i]);
            i++;
        } else if (children[i]->content == "(" || children[i]->content == ".") {
            i--; // サブルーチン呼び出しの開始位置に戻る
            compileSubroutineCall();
        }
    } else if (children[i]->content == "(") {
        termElement->addChild(children[i]);
        i++;
        compileExpression();
        if (children[i]->content != ")") {
            std::cerr << "括弧式には')'が必要です" << std::endl;
            exit(1);
        }
        termElement->addChild(children[i]);
        i++;
    } else if (children[i]->content == "-" || children[i]->content == "~") {
        termElement->addChild(children[i]);
        i++;
        compileTerm();
    } else {
        std::cerr << "不正な項です" << std::endl;
        exit(1);
    }
}

void CompilationEngine::compileSubroutineCall() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    if (children[i + 1]->content == ".") {
        // クラス名またはオブジェクト名
        currentElement->addChild(children[i]);
        i++;
        // '.'
        currentElement->addChild(children[i]);
        i++;
    }
    // サブルーチン名
    currentElement->addChild(children[i]);
    i++;
    // '('
    if (children[i]->content != "(") {
        std::cerr << "サブルーチン呼び出しには'('が必要です" << std::endl;
        exit(1);
    }
    currentElement->addChild(children[i]);
    i++;

    compileExpressionList();

    // ')'
    if (children[i]->content != ")") {
        std::cerr << "サブルーチン呼び出しには')'が必要です" << std::endl;
        exit(1);
    }
    currentElement->addChild(children[i]);
    i++;
}

void CompilationEngine::compileExpressionList() {
    std::shared_ptr<XMLElement> subroutineBody = output->getRoot()->children.back()->children.back();
    std::shared_ptr<XMLElement> currentElement = subroutineBody->children.back();
    std::vector<std::shared_ptr<XMLElement>> children = input_root->children;
    auto expressionListElement = std::make_shared<XMLElement>("expressionList");
    currentElement->addChild(expressionListElement);

    auto prevElement = currentElement;
    currentElement = expressionListElement;

    if (children[i]->content != ")") {
        compileExpression();
        while (children[i]->content == ",") {
            currentElement->addChild(children[i]);
            i++;
            compileExpression();
        }
    }

    currentElement = prevElement;
}

bool CompilationEngine::isOperator(const std::string& symbol) {
    static const std::vector<std::string> operators = {"+", "-", "*", "/", "&", "|", "<", ">", "="};
    return std::find(operators.begin(), operators.end(), symbol) != operators.end();
}

bool CompilationEngine::isKeywordConstant(const std::string& keyword) {
    static const std::vector<std::string> keywordConstants = {"true", "false", "null", "this"};
    return std::find(keywordConstants.begin(), keywordConstants.end(), keyword) != keywordConstants.end();
}

bool CompilationEngine::isInList(const std::vector<std::string>& list, const std::string& target) {
    return std::find(list.begin(), list.end(), target) != list.end();
}