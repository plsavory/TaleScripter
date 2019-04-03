#ifndef PROJECT_DATA_EXCEPTION
#define PROJECT_DATA_EXCEPTION

#include <string>
#include <exception>
#include <Misc/Utils.hpp>

class ProjectDataException : public std::exception {
public:
    ProjectDataException(const std::string& exceptionFileName, const std::string& exceptionMessage) : exception() {

    }
private:
    std::string message;
    std::string fileName;
};

#endif