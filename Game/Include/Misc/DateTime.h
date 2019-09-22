//
// Created by Pete on 2019-08-11.
//

#ifndef TALESCRIPTER_DATETIME_H
#define TALESCRIPTER_DATETIME_H

#include <iostream>
#include "GeneralException.hpp"

class DateTime {
public:
    /**
     *
     * @param date The date & time in string form
     * @param format The input format of the date
     */
    DateTime(const std::string &date, int format) {
        // Validate the date format
        switch (format) {
            case FORMAT_ISO:
                constructFromIsoDate(date);
                break;
            case FORMAT_UK:
            case FORMAT_US:
            default:
                throw GeneralException(Utils::implodeString({"Unknown date format: ", std::to_string(format)}));
        }
    }

    /**
     * Returns a string containing the date in a specific format
     * @param format
     * @return
     */
    std::string format(int format) {

    }
    ~DateTime() = default;

    // Consts for using the format function
    static const int FORMAT_ISO = 0;
    static const int FORMAT_UK = 1;
    static const int FORMAT_US = 2;
private:
    std::tm tm;

    void constructFromIsoDate(const std::string &date) {

    }
};

#endif //TALESCRIPTER_DATETIME_H
