#ifndef DATA_SET_EXCEPTION_INCLUDED
#define DATA_SET_EXCEPTION_INCLUDED

#include "Exceptions/GeneralException.hpp"

class DataSetException : public GeneralException {
public:
    explicit DataSetException(const std::string &message) : GeneralException(std::string("DataSet Exception"),
                                                                             message) {};
};

#endif