/* A general exception class.
 * Should be extended by other TaleScripter exceptions or thrown as a general error.
 * Put as much functionality for error handling in this class as possible, as we should
 * use this class to write error logs and to display information to the user in future.
 */

#ifndef GENERAL_EXCEPTION_INCLUDED
#define GENERAL_EXCEPTION_INCLUDED

#include <exception>
#include <stdexcept>
#include <vector>
#include "Misc/Utils.hpp"

class GeneralException : public std::exception {
public:

    /**
     * Very basic constructor, probably shouldn't be used outside of quick debugging.
     */
    GeneralException() {
        fullMessage = "No information provided (This is not helpful.)";
    };
    /**
     * Simple constructor - To be used when we don't care about displaying the type of error.
     *
     * @param message
     */
    GeneralException(const std::string &message) {
        fullMessage = message;
    };

    /**
     * Should be called by exceptions which inherit from this one.
     *
     * @param errorType - The type of error, will usually be the class name of the inheriting exception
     * @param message - The exception message/reason
     */
    GeneralException(const std::string &errorType, const std::string &message) {

        std::vector<std::string> errorStrings = {
                errorType,
                ": ",
                message
        };

        fullMessage = Utils::implodeString(errorStrings);
    }
    const char* what() {
        return fullMessage.c_str();
    }
private:
    std::string fullMessage;
};


#endif