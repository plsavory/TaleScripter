#ifndef PROJECT_BUILDER_EXCEPTION_INCLUDED
#define PROJECT_BUILDER_EXCEPTION_INCLUDED

#include "Exceptions/GeneralException.hpp"

// This exception is not needed within the main game engine, it will only be thrown in GameCompiler/ProjectBuilder.
class ProjectBuilderException : public GeneralException {
public:
    explicit ProjectBuilderException(const std::string &message) : GeneralException(std::string("Project Builder Exception"),
                                                                            message) {};
};

#endif