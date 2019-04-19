#ifndef CONFIGURATION_EXCEPTION_INCLUDED
#define CONFIGURATION_EXCEPTION_INCLUDED

#include <Exceptions/GeneralException.hpp>

class ConfigurationException : public GeneralException {
public:
    explicit ConfigurationException(const std::string &message) : GeneralException(
            std::string("Configuration Exception"), message) {};
};

#endif