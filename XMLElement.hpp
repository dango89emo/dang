#ifndef XML_ELEMENT_HPP
#define XML_ELEMENT_HPP

#include <string>
#include <vector>
#include <memory>

class XMLElement {
public:
    std::string tagName;
    std::string content;
    std::vector<std::shared_ptr<XMLElement>> children;

    XMLElement(const std::string& tag, const std::string& cont = "")
        : tagName(tag), content(cont) {}

    void addChild(const std::shared_ptr<XMLElement>& child) {
        children.push_back(child);
    }
};

#endif // XML_ELEMENT_HPP