#ifndef INPUT_HANDLER_EXCEPTION_INCLUDED
#define INPUT_HANDLER_EXCEPTION_INCLUDED

#include "Exceptions/GeneralException.hpp"

// This exception is not needed within the main game engine, it will only be thrown in GameCompiler/ProjectBuilder.
class InputHandlerException : public GeneralException {
public:
    explicit InputHandlerException(const std::string &message) : GeneralException(std::string("Input Handler Exception"),
                                                                                    message) {};
};

#endif