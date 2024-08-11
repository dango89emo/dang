#ifndef XML_GENERATOR_HPP
#define XML_GENERATOR_HPP

#include "XMLElement.hpp"
#include <fstream>

class XMLGenerator {
private:
    std::shared_ptr<XMLElement> root;
    void writeElement(std::ofstream& file, const std::shared_ptr<XMLElement>& element, int indent = 0);

public:
    XMLGenerator(const std::string& rootTag);
    std::shared_ptr<XMLElement> getRoot();
    std::shared_ptr<XMLElement> addElement(const std::string& tagName, const std::string& content = "", std::shared_ptr<XMLElement> parent = nullptr);
    void generateXML(const std::string& filename);
    static std::shared_ptr<XMLElement> parseXML(const std::string& filename);
};

#endif // XML_GENERATOR_HPP