#ifndef DATABASE_EXCEPTION_INCLUDED
#define DATABASE_EXCEPTION_INCLUDED

#include <Exceptions/GeneralException.hpp>

class DatabaseException : public GeneralException {
public:
    explicit DatabaseException(const std::string &message) : GeneralException(
            std::string("Database Exception"), message) {};
};

#endif