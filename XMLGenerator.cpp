#include "XMLGenerator.hpp"
#include <iostream>

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
        file << element->content << "</" << element->tagName << ">\n";
    }
}

void XMLGenerator::generateXML(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        writeElement(file, root);
        file.close();
        std::cout << "XML file generated successfully: " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}