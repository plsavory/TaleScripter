#ifndef DATABASE_CONNECTION_INCLUDED
#define DATABASE_CONNECTION_INCLUDED

#include <sqlite3.h>
#include <iostream>
#include <vector>
#include "MisuseException.hpp"
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

        for (auto &currentColumn : column) {
            if (currentColumn->getName() == name) {
                return currentColumn;
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

    std::vector<DataSetRow *> getRows() {
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
     * Returns a row where the value stored within the column with columnName matches the given value
     * @param columnName The column name to look for
     * @param value The value to match on
     * @return A row
     */
    DataSetRow *getRowWithColumnValue(std::string columnName, std::string value) {

        for (auto &currentRow : row) {
            if (currentRow->getColumn(columnName)->getData()->asString() == value) {
                return currentRow;
            }
        }

        std::vector<std::string> errorVector = {
                "A row with value ",
                value,
                " in column ",
                columnName,
                " could not be found"
        };

        throw DataSetException(Utils::implodeString(errorVector));
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
    DataSetRow *findRow(const std::string &columnName, const std::string &columnValue) {

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
    DataSetRow *findRow(const std::string &columnName, const std::string &columnValue, bool throwNotFoundError) {
        auto *foundRow = findRow(columnName, columnValue);

        if (!foundRow && throwNotFoundError) {
            throw DataSetException(Utils::implodeString(
                    {"No row with value of ", columnValue, "in column ", columnName, " was found."}));
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
    DataSetRow *findRow(const std::string &columnName, int columnValue) {

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
    std::vector<DataSetRow *> row;
};

class DatabaseConnection {
public:
    explicit DatabaseConnection(const std::string &name) {

        // Initialise values and open database connection
        zErrMsg = nullptr;
        db = nullptr;

        std::string filename = "db/";
        filename.append(name);

        // Convert the string to a char array
        char *fName = new char[filename.length() + 1];
        std::strcpy(fName, filename.c_str());

        rc = sqlite3_open(fName, &db);
        delete[] fName;

        if (rc) {
            std::cout << "Error opening/creating database" << std::endl;
            usable = false;
        }

        usable = true;
    }

    ~DatabaseConnection() {
        sqlite3_close(db);
    }

    /**
    * [DatabaseConnection::executeQuery Execute a query on the database that this object is linked to]
    * @param query              [A query string]
    * @param destinationDataSet [A result set in which to save the results from the query]
    */
    void executeQuery(const std::string &query, DataSet *destinationDataSet) {
        sqlite3_stmt *statement;

#ifdef DATABASE_DEBUG
        std::cout<<"Execute SQL statement: "<<query<<std::endl;
#endif

        char *queryString = new char[query.length() + 1];
        std::strcpy(queryString, query.c_str());

        if (sqlite3_prepare_v2(db, queryString, -1, &statement, nullptr) == SQLITE_OK) {

            // Clear the data set, and populate it with the results of the query
            destinationDataSet->clear();

            int columns = sqlite3_column_count(statement);
            int result = 0;
            int rows = 0;

            // Loop until we have run out of rows
            while (true) {

                result = sqlite3_step(statement);

                if (result == SQLITE_ROW) {

                    // Add a row to the data set
                    DataSetRow *row = destinationDataSet->addRow();

                    for (int col = 0; col < columns; col++) {

                        bool isNull = sqlite3_column_type(statement, col) == SQLITE_NULL;

                        if (sqlite3_column_text(statement, col)) {
                            // Add a column to the row

                            const char *cName = sqlite3_column_name(statement, col);
                            const char *cData = reinterpret_cast<const char *>(sqlite3_column_text(statement, col));

                            std::string columnName((cName ? cName : ""));
                            std::string data(cData ? cData : "");

                            row->addColumn(columnName, data, false);
                        } else if (isNull) {
                            const char *cName = sqlite3_column_name(statement, col);
                            std::string columnName = (cName ? cName : "");
                            std::string data;
                            row->addColumn(columnName, data, true);
                        }
                    }

                    rows++;

                } else {
                    break;
                }

            }

#ifdef DATABASE_DEBUG
            destinationDataSet->debugOutputContents();
#endif

            sqlite3_finalize(statement);
            delete[](queryString);
            return;
        }

        std::string error = sqlite3_errmsg(db);

        if (error != "not an error") {
            std::vector<std::string> errorVector = {
                    "An SQL error has occurred:\n",
                    error,
                    "\n\n",
                    query
            };

            throw DatabaseException(Utils::implodeString(errorVector));
        }
    }

    /**
     * Execute a prepared statement, this function allows query parameters to be used.
     * @param query
     * @param destinationDataSet
     * @param parameters
     * @param types
     */
    void execute(const std::string &query, DataSet *destinationDataSet, const std::vector<std::string> &parameters,
                 const std::vector<int> &types) {
        sqlite3_stmt *statement;

        if (parameters.size() != types.size()) {
            throw DatabaseException("Parameters and Types vectors must contain the same number of values");
        }

#ifdef DATABASE_DEBUG
        std::cout<<"Execute SQL statement: "<<query<<std::endl;
#endif

        char *queryString = new char[query.length() + 1];
        std::strcpy(queryString, query.c_str());

        if (sqlite3_prepare_v2(db, queryString, -1, &statement, nullptr) == SQLITE_OK) {

            // Bind the parameters to the query
            for (int i = 0; i < parameters.size(); i++) {

                int errorCode = 0;

                std::string param = parameters[i];

                // Handle null values
                if (Utils::strToLower(parameters[i]) == "null") {
                    errorCode = sqlite3_bind_null(statement, i);
                } else {
                    switch (types[i]) {
                        case TYPE_INT:
                            errorCode = sqlite3_bind_int(statement, i + 1, std::stoi(parameters[i]));
                            break;
                        case TYPE_TEXT:
                            errorCode = sqlite3_bind_text(statement, i + 1, parameters[i].c_str(), -1, nullptr);
                            break;
                        case TYPE_BOOL:
                            if (Utils::strToLower(parameters[i]) != "true" &&
                                Utils::strToLower(parameters[i]) != "false") {
                                throw MisuseException("Boolean must be true or false (You don't say?)");
                            }
                            errorCode = sqlite3_bind_int(statement, i + 1, parameters[i] == "true" ? 1 : 0);
                            break;
                        case TYPE_DOUBLE:
                            errorCode = sqlite3_bind_double(statement, i + 1, std::stod(parameters[i]));
                            break;
                        default:
                            throw DatabaseException(
                                    Utils::implodeString({"Unknown data type id: ", std::to_string(types[i])}));
                    }

                }

                if (errorCode != SQLITE_OK) {
                    std::string reason;

                    switch (errorCode) {
                        case SQLITE_TOOBIG:
                            reason = "too big";
                            break;
                        case SQLITE_NOMEM:
                            reason = "no memory";
                            break;
                        default:
                            reason = "unknown reason";
                            break;
                    }

                    throw DatabaseException(Utils::implodeString(
                            {"An error occurred while binding parameter", std::to_string(i), "(", parameters[i],
                             "), reason: ", reason}));
                }

            }


            int columns = sqlite3_column_count(statement);
            int result = 0;
            int rows = 0;

            // Loop until we have run out of rows
            while (true) {

                result = sqlite3_step(statement);

                if (result == SQLITE_ROW) {

                    // Add a row to the data set
                    DataSetRow *row = destinationDataSet->addRow();

                    for (int col = 0; col < columns; col++) {

                        bool isNull = sqlite3_column_type(statement, col) == SQLITE_NULL;

                        if (sqlite3_column_text(statement, col)) {
                            // Add a column to the row

                            const char *cName = sqlite3_column_name(statement, col);
                            const char *cData = reinterpret_cast<const char *>(sqlite3_column_text(statement, col));

                            std::string columnName((cName ? cName : ""));
                            std::string data(cData ? cData : "");

                            row->addColumn(columnName, data, false);
                        } else if (isNull) {
                            const char *cName = sqlite3_column_name(statement, col);
                            std::string columnName = (cName ? cName : "");
                            std::string data;
                            row->addColumn(columnName, data, true);
                        }
                    }

                    rows++;

                } else {
                    break;
                }

            }

            sqlite3_finalize(statement);
            return;
        }

        std::string error = sqlite3_errmsg(db);

        if (error != "not an error") {
            std::vector<std::string> errorVector = {
                    "An SQL error has occurred:\n",
                    error,
                    "\n\n",
                    query
            };

            throw DatabaseException(Utils::implodeString(errorVector));
        }
    }

    /**
     * [DatabaseConnection::executeQuery Execute a query with no result data - this should be used for writing data]
     * @param query [Query string]
     * @return last insert ID
     */
    int executeQuery(const std::string &query) {
        sqlite3_stmt *statement;

#ifdef DATABASE_DEBUG
        std::cout<<"Execute SQL statement: "<<query<<std::endl;
#endif

        char *queryString = new char[query.length() + 1];
        std::strcpy(queryString, query.c_str());

        if (sqlite3_prepare_v2(db, queryString, -1, &statement, nullptr) == SQLITE_OK) {
            sqlite3_step(statement);
            sqlite3_finalize(statement);
            return this->getLastInsertId();
        }

        std::string error = sqlite3_errmsg(db);

        if (error != "not an error") {
            std::vector<std::string> errorVector = {
                    "An SQL error has occurred:\n",
                    error,
                    "\n\n",
                    query
            };

            throw DatabaseException(Utils::implodeString(errorVector));
        }

        return 0;
    }

    int getLastInsertId() {

        auto *dataSet = new DataSet();
        this->executeQuery("SELECT last_insert_rowid() as last_insert_id", dataSet);

        if (!dataSet->getRow(0)->doesColumnExist("last_insert_id")) {
            throw DatabaseException("Failed to get the last insert ID");
        }

        int lastInsertId = std::stoi(dataSet->getRow(0)->getColumn("last_insert_id")->getRawData());
        delete (dataSet);
        return lastInsertId;
    }

    /**
    * [DatabaseConnection::insert Inserts a row into the database]
    * @param tableName [The name of the table]
    * @param columns   [Array of strings with column names]
    * @param values    [Array of strings containing the values]
    * @param valuesCount [The number of values to insert]
    * @return last insert ID
    */
    int insert(const std::string &tableName, const std::vector<std::string> &columns,
               std::vector<std::string> &values, std::vector<int> types) {

        if (columns.size() != values.size() || types.size() != columns.size()) {
            throw DatabaseException("The number of given columns and the number of given values does not match.");
        }

        for (unsigned int i = 0; i < values.size(); i++) {

            std::stringstream ss;

            switch (types[i]) {
                case DATA_TYPE_STRING:

                    // If we actually want to set a string column to null...
                    if (values[i] == "NULL") {
                        continue;
                    }

                    ss << "'" << sanitizeString(values[i]) << "'";
                    values[i] = ss.str();
                    break;
                default:
                    break;
            }
        }

        std::string queryPortionColumns = Utils::implodeString(columns, ", ", 0);
        std::ostringstream columnsStream;
        columnsStream << "(" << queryPortionColumns << ")";
        queryPortionColumns = columnsStream.str();

        // TODO: Create an overloaded function which can handle inserting multiple rows at once
        std::string queryPortionValues = Utils::implodeString(values, ", ", 0);
        std::ostringstream valuesStream;
        valuesStream << "VALUES (" << queryPortionValues << ")";
        queryPortionValues = valuesStream.str();

        std::ostringstream ss;
        ss << "INSERT INTO `" << tableName << "` " << queryPortionColumns << " " << queryPortionValues << ";";

#ifdef PRINT_QUERIES
        std::cout<<ss.str()<<std::endl;
#endif

        return this->executeQuery(ss.str());
    }

    int insert(const std::string &tableName) {

        std::vector<std::string> query = {
                "INSERT INTO `",
                tableName,
                "` DEFAULT VALUES"
        };

        return executeQuery(Utils::implodeString(query, ""));
    }

    bool isUsable() {
        return usable;
    }

    // Data type constants for parameters
    static const int TYPE_INT = 0;
    static const int TYPE_TEXT = 1;
    static const int TYPE_DOUBLE = 2;
    static const int TYPE_BOOL = 3;

private:
    std::string name;
    sqlite3 *db;
    char *zErrMsg;
    int rc;
    bool usable;

    /**
     * [DatabaseConnection::sanitizeString Remove any offending characters and escape things that need it]
     * @param  string [The string]
     * @return        [The sanitized string]
     */
    static std::string sanitizeString(const std::string &string) {
        // TODO: There are probably more things that I need to put here.
        std::regex apostropheRegex = std::regex("\'");
        return regex_replace(string, apostropheRegex, "''");
    }
};

#endif
