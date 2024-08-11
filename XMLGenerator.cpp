#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "XMLGenerator.hpp"

XMLGenerator::XMLGenerator(const std::string& rootTag) : root(std::make_shared<XMLElement>(rootTag)) {}

std::shared_ptr<XMLElement> XMLGenerator::getRoot() { return root; }

std::shared_ptr<XMLElement> XMLGenerator::addElement(const std::string& tagName, const std::string& content, std::shared_ptr<XMLElement> parent) {
    auto newElement = std::make_shared<XMLElement>(tagName, content);
    if (parent) {
        parent->addChild(newElement);
    } else {
        root->addChild(newElement);
    }
    return newElement;
}

void XMLGenerator::writeElement(std::ofstream& file, const std::shared_ptr<XMLElement>& element, int indent) {
    std::string indentation(indent * 2, ' ');
    file << indentation << "<" << element->tagName << ">";
    
    if (!element->children.empty()) {
        file << "\n";
        for (const auto& child : element->children) {
            writeElement(file, child, indent + 1);
        }
        file << indentation << "</" << element->tagName << ">\n";
    } else {
        std::string content;
        if (element->content =="<"){
            content = "&lt;";
        } else if (element->content == ">"){
            content = "&gt;";
        } else if (element->content == "&"){
            content = "&amp;";
        } else {
            content = element->content;
        }
        file << " " << content << " " << "</" << element->tagName << ">\n";
    }
}

void XMLGenerator::generateXML(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        writeElement(file, root);
        file.close();
        std::cout << "XML file generated successfully: " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

std::shared_ptr<XMLElement> XMLGenerator::parseXML(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::stack<std::shared_ptr<XMLElement>> elementStack;
    std::shared_ptr<XMLElement> currentElement = nullptr;
    std::shared_ptr<XMLElement> root = nullptr;

    size_t pos = 0;
    while (pos < content.length()) {
        size_t startTag = content.find('<', pos);
        if (startTag == std::string::npos) break;

        size_t endTag = content.find('>', startTag);
        if (endTag == std::string::npos) break;

        std::string tag = content.substr(startTag + 1, endTag - startTag - 1);
        
        if (tag[0] == '/') {
            // Closing tag
            if (!elementStack.empty()) {
                elementStack.pop();
                currentElement = elementStack.empty() ? nullptr : elementStack.top();
            }
        } else {
            // Opening tag
            auto newElement = std::make_shared<XMLElement>(tag);
            if (!root) root = newElement;

            if (currentElement) {
                currentElement->addChild(newElement);
            }
            elementStack.push(newElement);
            currentElement = newElement;

            // Extract content
            size_t contentStart = endTag + 1;
            size_t contentEnd = content.find('<', contentStart);
            if (contentEnd != std::string::npos) {
                std::string elementContent = content.substr(contentStart, contentEnd - contentStart);
                currentElement->content = elementContent;
            }
        }

        pos = endTag + 1;
    }

    return root;
}