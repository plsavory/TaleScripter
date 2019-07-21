//
// Created by Pete on 2019-07-18.
//

#ifndef TALESCRIPTER_DATAATTRIBUTE_HPP
#define TALESCRIPTER_DATAATTRIBUTE_HPP

struct DataAttribute {
public:
    DataAttribute(const std::string &pName, const std::string &pDefaultValue) {
        name = pName;
        defaultValue = pDefaultValue;
    }

    std::string getName() {
        return name;
    }

    std::string getDefaultValue() {
        return defaultValue;
    }
private:
    std::string name;
    std::string defaultValue;
};

struct DataAttributeGroup {
public:
    explicit DataAttributeGroup(const std::string &pName) {
        name = pName;
        setAcceptedValues();
    }

    std::vector<DataAttribute*> getAcceptedAttributes() {
        return acceptedAttributes;
    }

    std::string getName() {
        return name;
    }
private:
    void setAcceptedValues() {

        // There are probably better ways to do this, but here let's set default values that get used when nothing is specified in the json
        if (name == "textDisplay") {
            acceptedAttributes.push_back(new DataAttribute("framePositionX", "140"));
            acceptedAttributes.push_back(new DataAttribute("framePositionY", "585"));
            acceptedAttributes.push_back(new DataAttribute("frameFillAlpha", "255"));
            acceptedAttributes.push_back(new DataAttribute("frameBorderAlpha", "255"));
            acceptedAttributes.push_back(new DataAttribute("textOffsetX", "10"));
            acceptedAttributes.push_back(new DataAttribute("textOffsetY", "10"));
            acceptedAttributes.push_back(new DataAttribute("maxTextWidth", "980"));
            return;
        }

        if (name == "characterNameDisplay") {
            acceptedAttributes.push_back(new DataAttribute("framePositionX", "150"));
            acceptedAttributes.push_back(new DataAttribute("framePositionY", "550"));
            acceptedAttributes.push_back(new DataAttribute("frameFillAlpha", "255"));
            acceptedAttributes.push_back(new DataAttribute("frameBorderAlpha", "255"));
            acceptedAttributes.push_back(new DataAttribute("textOffsetX", "10"));
            acceptedAttributes.push_back(new DataAttribute("textOffsetY", "10"));
            return;
        }

        if (name == "menuButtons") {
            return;
        }

        throw MisuseException(Utils::implodeString({"Unknown attribute group name: ", name}));
    };
    std::string name;
    std::vector<DataAttribute*> acceptedAttributes;
};

#endif //TALESCRIPTER_DATAATTRIBUTE_HPP
