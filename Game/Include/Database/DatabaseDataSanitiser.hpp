#ifndef DATABASE_DATA_SANITISER_INCLUDED
#define DATABASE_DATA_SANITISER_INCLUDED

#include <regex>
#include "Exceptions/DatabaseException.hpp"
#include "Misc/Utils.hpp"
#include <vector>

class DatabaseDataSanitiser {
public:
    /**
     * Ensures that only the correct values are taken when setting a boolean
     * @param value
     * @return
     */
    static std::string sanitiseBoolean(const std::string &value) {

        if (value == "true" || value == "TRUE") {
            return "TRUE";
        }

        if (value == "false" || value == "FALSE") {
            return "FALSE";
        }

        return "NULL";
    }

    /**
     * Throws an exception if the given string does not contain an integer value
     * @param value
     * @return
     */
    static std::string sanitiseInteger(const std::string &value) {
        std::regex validate("^\\d+$");

        if (!std::regex_match(value, validate)) {
            std::vector<std::string> error = {
                    "Value '", value, "' is not an integer."
            };

            throw DatabaseException(Utils::implodeString(error));
        }

        return value;
    }

    static std::string sanitiseDouble(const std::string &value) {
        std::regex validate(R"(\d+(\.\d{1,2})?)");

        if (!std::regex_match(value, validate)) {
            std::vector<std::string> error = {
                    "Value '", value, "' is not a double."
            };

            throw DatabaseException(Utils::implodeString(error));
        }

        return value;
    }
private:
};

#endif