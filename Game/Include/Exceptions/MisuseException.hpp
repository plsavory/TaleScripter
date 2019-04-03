#ifndef MISUSE_EXCEPTION_INCLUDED
#define MISUSE_EXCEPTION_INCLUDED

#include "Exceptions/GeneralException.hpp"

// Misuse exceptions should be used in the instance where they will be thrown because of programmer error.
// These exceptions should not be thrown during normal program operation.
class MisuseException : public GeneralException {
public:
    explicit MisuseException(const std::string &message) : GeneralException(std::string("Misuse Exception"),
                                                                            message) {};
};

#endif