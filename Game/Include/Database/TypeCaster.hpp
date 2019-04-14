#ifndef TYPE_CASTER_INCLUDED
#define TYPE_CASTER_INCLUDED

#include <string>

// This class handles casting from any type in to a string to be inserted into the database
class TypeCaster {
public:
    static std::string cast(bool value) {
        return value ? "TRUE" : "FALSE";
    }

    static std::string cast(int value) {
        return std::to_string(value);
    }

    static std::string cast(double value) {
        return std::to_string(value);
    }

    static std::string cast(float value) {
        return std::to_string(value);
    }
};

#endif