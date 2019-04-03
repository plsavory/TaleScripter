#ifndef DATABASE_CONNECTION_INCLUDED
#define DATABASE_CONNECTION_INCLUDED

#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <Exceptions/DatabaseException.hpp>
#include <Exceptions/DataSetException.hpp>

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

// Data set related stuff
struct DataSetColumn {
public:
    DataSetColumn(std::string cName, std::string cData) {
        name = std::move(cName);
        data = std::move(cData);
    };

    ~DataSetColumn() = default;

    std::string getName() {
        return name;
    }

    std::string getData() {
        return data;
    }

private:
    std::string name;
    std::string data;
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
    DataSetColumn *addColumn(const std::string &name, const std::string &data) {

        if (name.empty()) {
            return nullptr;
        }

        for (auto &potentialNewColumn : column) {
            if (!potentialNewColumn) {
                potentialNewColumn = new DataSetColumn(name, data);
                return potentialNewColumn;
            }
        }

        return nullptr;
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
                std::cout << "Column: " << currentColumn->getName() << " | " << currentColumn->getData();
                std::cout << std::endl;
            }
        }

    }

private:
    DataSetColumn *column[DATA_SET_MAX_COLUMNS]{};
};

struct DataSet {
public:
    DataSet() {

        for (auto &emptyRow : row) {
            emptyRow = nullptr;
        }

        rowCount = 0;

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

        for (auto &currentRow : row) {
            if (!currentRow) {
                currentRow = new DataSetRow();
                rowCount++;
                return currentRow;
            }
        }

        return nullptr;

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
        return rowCount;
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
                    "could not be found"
            };

            throw DataSetException(Utils::implodeString(errorVector));
        }
        return row[index];
    }

private:
    DataSetRow *row[DATA_SET_MAX_ROWS]{};
    int rowCount;
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
