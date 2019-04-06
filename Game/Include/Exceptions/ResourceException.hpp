#ifndef RESOURCE_HANDLER_EXCEPTION_INCLUDED
#define RESOURCE_HANDLER_EXCEPTION_INCLUDED

#include "Exceptions/GeneralException.hpp"

// This exception is not needed within the main game engine, it will only be thrown in GameCompiler/ProjectBuilder.
class ResourceException : public GeneralException {
public:
    explicit ResourceException(const std::string &message) : GeneralException(std::string("Resource Handler Exception"),
                                                                                  message) {};
};

#endif