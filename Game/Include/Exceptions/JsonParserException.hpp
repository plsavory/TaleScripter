#ifndef JSON_PARSER_EXCEPTION_INCLUDED
#define JSON_PARSER_EXCEPTION_INCLUDED

#include "Exceptions/GeneralException.hpp"

class JsonParserException : public GeneralException {
public:
    explicit JsonParserException(const std::string &message) : GeneralException(std::string("JSON Parser Exception"),
                                                                             message) {};
};

#endif