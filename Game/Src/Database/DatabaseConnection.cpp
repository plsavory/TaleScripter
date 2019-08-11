#include <utility>

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include "Database/DatabaseConnection.hpp"
#include "Misc/Utils.hpp"
#include <regex>
#include <Exceptions/DatabaseException.hpp>
#include "MisuseException.hpp"

DatabaseConnection::DatabaseConnection(const std::string& name) {

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

DatabaseConnection::~DatabaseConnection() {
    sqlite3_close(db);
}

/**
 * [DatabaseConnection::executeQuery Execute a query on the database that this object is linked to]
 * @param query              [A query string]
 * @param destinationDataSet [A result set in which to save the results from the query]
 */
void DatabaseConnection::executeQuery(const std::string& query, DataSet *destinationDataSet) {

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
 * [DatabaseConnection::executeQuery Execute a query with no result data - this should be used for writing data]
 * @param query [Query string]
 * @return last insert ID
 */
int DatabaseConnection::executeQuery(const std::string& query) {
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

/**
 * Execute a prepared statement, this function allows query parameters to be used.
 * @param query
 * @param destinationDataSet
 * @param parameters
 * @param types
 */
void DatabaseConnection::execute(const std::string& query, DataSet *destinationDataSet, const std::vector<std::string> &parameters, const std::vector<int> &types) {
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
                        errorCode = sqlite3_bind_int(statement, i+1, std::stoi(parameters[i]));
                        break;
                    case TYPE_TEXT:
                        errorCode = sqlite3_bind_text(statement, i+1, parameters[i].c_str(), -1, nullptr);
                        break;
                    case TYPE_BOOL:
                        if (Utils::strToLower(parameters[i]) != "true" && Utils::strToLower(parameters[i]) != "false") {
                            throw MisuseException("Boolean must be true or false (You don't say?)");
                        }
                        errorCode = sqlite3_bind_int(statement, i+1, parameters[i] == "true" ? 1 : 0);
                        break;
                    case TYPE_DOUBLE:
                        errorCode = sqlite3_bind_double(statement, i+1, std::stod(parameters[i]));
                        break;
                    default:
                        throw DatabaseException(Utils::implodeString({"Unknown data type id: ", std::to_string(types[i])}));
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

                throw DatabaseException(Utils::implodeString({"An error occurred while binding parameter", std::to_string(i), "(", parameters[i], "), reason: ", reason}));
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

int DatabaseConnection::getLastInsertId() {

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
int DatabaseConnection::insert(const std::string &tableName, const std::vector<std::string>& columns,
                               std::vector<std::string>& values, std::vector<int> types) {

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

int DatabaseConnection::insert(const std::string& tableName) {
    std::vector<std::string> query = {
            "INSERT INTO `",
            tableName,
            "` DEFAULT VALUES"
    };

    return executeQuery(Utils::implodeString(query, ""));
}

/**
 * [DatabaseConnection::sanitizeString Remove any offending characters and escape things that need it]
 * @param  string [The string]
 * @return        [The sanitized string]
 */
std::string DatabaseConnection::sanitizeString(const std::string &string) {
    // TODO: There are probably more things that I need to put here.
    std::regex apostropheRegex = std::regex("\'");
    return regex_replace(string, apostropheRegex, "''");
}
