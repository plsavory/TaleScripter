#ifndef DATABASE_CONNECTION_INCLUDED
#define DATABASE_CONNECTION_INCLUDED

#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <Exceptions/DatabaseException.hpp>
#include <Exceptions/DataSetException.hpp>
#include <regex>

#define DATA_SET_MAX_ROWS 1000
#define DATA_SET_MAX_COLUMNS 50

#define DATA_TYPE_NUMBER 0
#define DATA_TYPE_STRING 1
#define DATA_TYPE_DATE 2
#define DATA_TYPE_DATE_TIME 3
#define DATA_TYPE_BOOLEAN 4

enum FetchMode {
    None, All, One
};

struct DataContainer {
    DataContainer(const std::string &value, const std::string &parentColumnName, bool isNull) {
        data = isNull ? "" : value;
        columnName = parentColumnName;
    }

    /**
     * Returns the raw data contained within this object as a string
     * @return unformatted data
     */
    std::string getRawData() {
        return data;
    }

    /**
     * Does the same as getRawData for now, in its own function in case any extra processing is needed in future
     * @return
     */
    std::string asString() {
        return getRawData();
    }

    /**
     * Formats the data contained within this object as an integer and returns it
     * @return
     */
    int asInteger() {

        if (data.empty()) {
            return 0;
        }

        try {
            return std::stoi(data);
        } catch (std::exception &e) {
            std::vector<std::string> error = {
                    "DataContainer.asInteger() called on a datum which does not contain an integer.\n\n",
                    "Column name: '", columnName, "' \n",
                    "Value: '", data, "'"
            };
            throw DataSetException(Utils::implodeString(error));
        }
    }

    float asFloat() {

        if (data.empty()) {
            return 0;
        }

        try {
            return std::stof(data);
        } catch (std::exception &e) {
            std::vector<std::string> error = {
                    "DataContainer.asInteger() called on a datum which does not contain a float value.\n\n",
                    "Column name: '", columnName, "' \n",
                    "Value: '", data, "'"
            };
            throw DataSetException(Utils::implodeString(error));
        }

    }

    /**
     * Formats the data as a boolean and returns it, as long as it is one of the appropriate values
     * @return
     */
    bool asBoolean() {

        if (data.empty()) {
            return false;
        }

        // Validate that we are actually dealing with a boolean in a string...
        std::vector<std::string> acceptedValues = {"1", "0"};

        if (!Utils::isAcceptedValue(acceptedValues, data, false)) {
            std::vector<std::string> error{
                    "DataContainer.asBoolean called on a datum which does not contain a boolean. \n\n",
                    "Column name: '", columnName, "' \n",
                    "Value: '", data, "'"
            };
            throw DataSetException(Utils::implodeString(error));
        }

        return data == "1";

    }

    // TODO: Test this function... I got side tracked from something else and wrote it.
    std::string asDate(DateFormat format) {

        std::string date;

        if (data.empty()) {
            date = "1970-01-01";
        } else {
            date = data;
        }

        std::regex isoDateRegex = std::regex("^([0-9]{4})-([0-9][0-9])-([0-9][0-9])$");

        // Ensure that the string at least looks like a date
        if (!std::regex_match(date, isoDateRegex)) {
            std::vector<std::string> error = {
                    "DataContainer.asDate() called on a column which does not contain a date\n\n",
                    "Column name: '", columnName, "' \n",
                    "Data: ", date, "'\n"
            };

            throw DataSetException(Utils::implodeString(error));
        }

        // Ensure that the values of each part of the date are in range
        std::vector<std::string> datePortions = Utils::explodeString(date, '-');

        int year = std::stoi(datePortions[0]);
        int month = std::stoi(datePortions[1]);
        int day = std::stoi(datePortions[2]);

        // Initial range validation
        bool validDate = (month > 0 && month <= 12) && (day > 0 && day <= 31);

        if (!validDate) {
            std::vector<std::string> error = {
                    "DataContainer.asDate() called on a date which is out of range (Month or day out of range) \n\n",
                    "Column name: '", columnName, "'\n",
                    "Data: ", date, "'\n"
            };
            throw DataSetException(Utils::implodeString(error));
        }

        // Figure out if we are dealing with a leap year or not
        bool isLeapYear = (year % 100 == 0) ? (year % 400 == 0) : (year % 4 == 0);

        // Check to see if the days are within the length of the month
        int monthLength = Utils::getMonthLengthMatrix(isLeapYear)[month - 1];

        if (day > monthLength) {
            std::vector<std::string> error = {
                    "DataContainer.asDate() called on a date which is out of range (More days than the length of the month) \n\n",
                    "Column name: '", columnName, "'",
                    "Data: '", date, "'\n"
            };
        }

        // Format the date and return it
        if (format != DateFormat::FORMAT_ISO && format != DateFormat::FORMAT_DATETIME_ISO) {
            return Utils::formatDate(date, format);
        } else {
            return date;
        }
    }

private:
    std::string data;
    std::string columnName; // Used for printing errors
};

// Data set related stuff
struct DataSetColumn {
public:
    DataSetColumn(std::string cName, std::string cData, bool isNull) {
        name = std::move(cName);
        dataContainer = new DataContainer(cData, name, isNull);
    };

    ~DataSetColumn() {
        delete (dataContainer);
    };

    std::string getName() {
        return name;
    }

    /**
     * Returns the DataContainer object containing the data in this column
     * @return - DataContainer object
     */
    DataContainer *getData() {
        return dataContainer;
    }

    /**
     * @deprecated - Functions on the DataContainer should be used instead, I have left this here for now as removing it would break everything.
     * @return
     */
    std::string getRawData() {
        // TODO: Remove this function when all usages have been changed
        return dataContainer->getRawData();
    }

private:
    std::string name;
    DataContainer *dataContainer;
};

struct DataSetRow {
public:
    DataSetRow() {
        for (auto &emptyColumn : column) {
            emptyColumn = nullptr;
        }
    };

    ~DataSetRow() {
        for (auto &currentColumn : column) {
            delete currentColumn;
        }
    };

    /**
     * Adds a column to the row
     *
     * @param name
     * @param data
     * @return
     */
    DataSetColumn *addColumn(const std::string &name, const std::string &data, bool isNull) {

        if (name.empty()) {
            return nullptr;
        }

        DataSetColumn *newColumn = new DataSetColumn(name, data, isNull);
        column.push_back(newColumn);
        return newColumn;
    }

    /**
     * Returns a column from the row with the given index
     *
     * @param index
     * @return
     */
    DataSetColumn *getColumn(int index) {

        // Throw an exception when this column does not exist so that we never can end up with null pointer errors.
        if (!column[index]) {
            std::vector<std::string> errorVector = {
                    "No column with index ",
                    std::to_string(index),
                    "was found"
            };
            throw DataSetException(std::string(Utils::implodeString(errorVector)));
        }
        return column[index];
    }

    /**
     * Returns a column from the row with the given name
     *
     * @param name
     * @return
     */
    DataSetColumn *getColumn(const std::string &name) {
        for (int i = 0; i < DATA_SET_MAX_COLUMNS; i++) {
            if (column[i]) {
                if (column[i]->getName() == name) {
                    return getColumn(i);
                }
            }
        }

        // No column with this name found - throw an error.
        std::vector<std::string> errorVector = {
                "No column with name '",
                name,
                "' was found"
        };

        throw DataSetException(Utils::implodeString(errorVector));
    }

    /**
     * Returns true if a column with the given name exists in this row
     *
     * @param name
     * @return
     */
    bool doesColumnExist(const std::string &name) {
        for (auto &currentColumn : column) {
            if (currentColumn) {
                if (currentColumn->getName() == name) {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Outputs the content of the row to the console
     */
    void debugOutputContents() {

        for (auto &currentColumn : column) {
            if (currentColumn) {
                std::cout << "Column: " << currentColumn->getName() << " | " << currentColumn->getRawData();
                std::cout << std::endl;
            }
        }

    }

    int getColumnCount() {
        return column.size();
    }

private:
    std::vector<DataSetColumn *> column;
};

struct DataSet {
public:
    DataSet() {

        for (auto &emptyRow : row) {
            emptyRow = nullptr;
        }

    };

    ~DataSet() {

        for (auto &currentRow : row) {
            delete currentRow;
        }

    };

    void clear() {
        for (auto &currentRow : row) {
            delete currentRow;
            currentRow = nullptr;
        }
    };

    /**
     * Adds a row to the data set
     *
     * @return Pointer to the newly-created row
     */
    DataSetRow *addRow() {
        auto *newRow = new DataSetRow();
        row.push_back(newRow);
        return newRow;
    }

    /**
     * Outputs the content of the data set to the console
     */
    void debugOutputContents() {

        int rows = 0;

        for (auto &currentRow : row) {
            if (currentRow) {
                rows++;
            }
        }

        std::cout << "Contents of DataSet -  Rows: " << rows << std::endl;

        for (auto &currentRow : row) {
            if (currentRow) {
                currentRow->debugOutputContents();
            }
        }
    }

    /**
     * @return count of rows within the data set
     */
    int getRowCount() {
        return row.size();
    }

    std::vector<DataSetRow*> getRows() {
        return row;
    }

    /**
     * Returns the row with the given index from the data set
     * @param index
     * @return Pointer to the row object
     */
    DataSetRow *getRow(int index) {

        if (!row[index]) {
            std::vector<std::string> errorVector = {
                    "A row with index ",
                    std::to_string(index),
                    " could not be found"
            };

            throw DataSetException(Utils::implodeString(errorVector));
        }
        return row[index];
    }

    /**
     * Returns a data set row where the given column has the given value
     * Does not throw exceptions if a row was not found.
     * Does throw an exception if the column does not exist on a row or if the data type is incorrect and unable to be cast
     *
     * @param columnName The name of the column to search
     * @param columnValue The value of the column to search
     * @return nullptr if nothing found, otherwise a DataSetRow.
     */
    DataSetRow* findRow(const std::string &columnName, const std::string &columnValue) {

        for (auto &currentRow : row) {
            if (currentRow->getColumn(columnName)->getData()->asString() == columnValue) {
                return currentRow;
            }
        }

        return nullptr;
    }

    /**
 * Returns a data set row where the given column has the given value
 * Does not throw exceptions if a row was not found.
 * Does throw an exception if the column does not exist on a row or if the data type is incorrect and unable to be cast
 *
 * @param columnName The name of the column to search
 * @param columnValue The value of the column to search
 * @param throwNotFoundError - If set to true, will throw an error if the row is not found
 * @return nullptr if nothing found, otherwise a DataSetRow.
 */
    DataSetRow* findRow(const std::string &columnName, const std::string &columnValue, bool throwNotFoundError) {
        auto *foundRow = findRow(columnName, columnValue);

        if (!foundRow && throwNotFoundError) {
            throw DataSetException(Utils::implodeString({"No row with value of ", columnValue, "in column ", columnName, " was found."}));
        }

        return foundRow;
    }

    /**
    * Returns a data set row where the given column has the given value
    * Does not throw exceptions if a row was not found.
    * Does throw an exception if the column does not exist on a row or if the data type is incorrect and unable to be cast
    *
    * @param columnName The name of the column to search
    * @param columnValue The value of the column to search
    * @return nullptr if nothing found, otherwise a DataSetRow.
    */
    DataSetRow* findRow(const std::string &columnName, int columnValue) {

        for (auto &currentRow : row) {
            if (currentRow->getColumn(columnName)->getData()->asInteger() == columnValue) {
                return currentRow;
            }
        }

        return nullptr;
    }

    /**
    * Returns a data set row where the given column has the given value
    * Does not throw exceptions if a row was not found.
    * Does throw an exception if the column does not exist on a row or if the data type is incorrect and unable to be cast
    *
    * @param columnName The name of the column to search
    * @param columnValue The value of the column to search
    * @return nullptr if nothing found, otherwise a DataSetRow.
    */
//    DataSetRow* findRow(const std::string &columnName, bool columnValue) {
//
//        for (auto &currentRow : row) {
//            if (currentRow->getColumn(columnName)->getData()->asBoolean() == columnValue) {
//                return currentRow;
//            }
//        }
//
//        return nullptr;
//    }

private:
    std::vector<DataSetRow*> row;
};

class DatabaseConnection {
public:
    explicit DatabaseConnection(const std::string &name);

    ~DatabaseConnection();

    void executeQuery(const std::string &query, DataSet *destinationDataSet);

    int executeQuery(const std::string &query);

    int getLastInsertId();

    int insert(const std::string &tableName, const std::vector<std::string> &columns,
               std::vector<std::string> &values, std::vector<int> types);

    int insert(const std::string &tableName);

    bool isUsable() {
        return usable;
    }

private:
    std::string name;
    sqlite3 *db;
    char *zErrMsg;
    int rc;
    bool usable;

    static std::string sanitizeString(const std::string &string);
};

#endif
