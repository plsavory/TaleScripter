#include <sqlite3.h>
#include "Database/DatabaseSchema.hpp"
#include <cstring>
#include <string>
#include <sstream>

// DatabaseSchema class
DatabaseSchema::DatabaseSchema(const std::string &dName) {
    // TODO: Allow entire schema and tables to be loaded from JSON files
    name = dName;

    // Make all table pointers be null
    for (auto &emptyTable : table) {
        emptyTable = nullptr;
    }

}

std::string DatabaseSchema::getName() {
    return name;
}

/**
 * [DatabaseSchema::createDatabase Creates the database]
 * @return [True if successful, false if failure]
 */
bool DatabaseSchema::createDatabase() {
    std::cout << "Creating database " << name << std::endl;

    // Database connection
    sqlite3 *db;
    char *zErrMsg = nullptr;
    int rc;

    std::string filename = std::string("db/");
    filename.append(name);

    // Convert the string to a char array TODO: Move this into its own function
    char *fName = new char[filename.length() + 1];
    std::strcpy(fName, filename.c_str());

    rc = sqlite3_open(fName, &db);
    delete[] fName;

    if (rc) {
        std::cout << "Error opening/creating database" << std::endl;
        return false;
    } else {
        std::cout << "Database created/opened" << std::endl;
    }

    // Create all of the tables
    for (auto &currentTable : table) {
        if (currentTable) {

            // Drop the table if it already exists
            std::stringstream ss;
            ss << "DROP TABLE IF EXISTS " << currentTable->getName() << ";";
            std::string dropQuery = ss.str();
            char *dropSql = new char[dropQuery.length() + 1];
            std::strcpy(dropSql, dropQuery.c_str());
            rc = sqlite3_exec(db, dropSql, nullptr, nullptr, &zErrMsg);

            if (rc != SQLITE_OK) {
                std::cout << "-----------" << std::endl << "Sql error:" << std::endl << zErrMsg << std::endl;
                std::cout << std::endl << "---------------------" << std::endl;
                std::cout << dropQuery << std::endl;
                sqlite3_free(zErrMsg);
                continue;
            }

            // Create the table
            std::string query = currentTable->getCreationQuery();
            char *sql = new char[query.length() + 1];
            std::strcpy(sql, query.c_str());

            // Callback argument is a function which will get called for each step/row
            // Here, we are passing nullptr as we are creating tables and not fetching data.
            rc = sqlite3_exec(db, sql, nullptr, nullptr, &zErrMsg);

            delete[] sql;

            if (rc != SQLITE_OK) {
                std::cout << "-----------" << std::endl << "Sql error:" << std::endl << zErrMsg << std::endl;
                std::cout << std::endl << "---------------------" << std::endl;
                std::cout << query << std::endl;
                sqlite3_free(zErrMsg);
            } else {
                std::cout << "Table " << currentTable->getName() << " created successfully." << std::endl;
            }
        }
    }

    sqlite3_close(db);
    return true;
}

/**
 * [DatabaseSchema::addTable Create a table within the schema]
 * @param  name [Name of the table]
 * @return      [ID of the table stored within the schema, null on failure]
 */
DatabaseTable *DatabaseSchema::addTable(const std::string &tableName) {
    // Create a table in an empty slot
    for (auto &potentialNewTable : table) {

        if (potentialNewTable == nullptr) {
            potentialNewTable = new DatabaseTable(tableName);
            return potentialNewTable;
        }

    }

    return nullptr;
}

/**
 * Returns a pointer to the table object with the given name
 *
 * @param tableName
 * @return
 */
DatabaseTable *DatabaseSchema::getTable(const std::string &tableName) {

    for (auto &currentTable : table) {
        if (currentTable) {
            if (currentTable->getName() == tableName) {
                return currentTable;
            }
        }
    }
    return nullptr;
}

// DatabaseTable class
DatabaseTable::DatabaseTable(const std::string &tName) {
    name = tName;

    // Set all columns to null
    for (auto &emptyColumn : column) {
        emptyColumn = nullptr;
    }
}

std::string DatabaseTable::getName() {
    return name;
}

DatabaseColumn *
DatabaseTable::addColumn(const std::string &cName, ColumnType cType, bool cNotNull, const std::string &cDefaultValue) {
    return addColumnInternal(cName, cType, cNotNull, cDefaultValue, false);
}

DatabaseColumn *DatabaseTable::addPrimaryKey() {
    return addColumnInternal("id", ColumnType::tInteger, true, "", true);
}

DatabaseColumn *DatabaseTable::addForeignKey(const std::string &cName, const std::string &foreignTableName,
                                             const std::string &foreignTableId) {
    // TODO: Implement this to enforce strict relations
    return nullptr;
}

/**
 * [DatabaseTable::addColumnInternal Adds a column to the table schema]
 * @param  cName         [Name of the column]
 * @param  cType         [Column data type]
 * @param  cNotNull      [Not null restriction on column]
 * @param  cDefaultValue [Column's default value]
 * @param  cPrimaryKey   [Primary key of the column]
 * @return               [Success: Pointer to column object, failure: nullptr]
 */
DatabaseColumn *DatabaseTable::addColumnInternal(const std::string &cName, ColumnType cType, bool cNotNull,
                                                 const std::string &cDefaultValue, bool cPrimaryKey) {

    // TODO: Error if a column with the same name already exists

    // Find an empty slot to add a column to, if one exists, create it
    for (auto &potentialNewColumn : column) {
        if (potentialNewColumn == nullptr) {
            potentialNewColumn = new DatabaseColumn(cName, cType, cNotNull, cDefaultValue, cPrimaryKey);
            return potentialNewColumn;
        }
    }

    return nullptr;
}

/**
 * [DatabaseTable::getCreationQuery Creates a query string to create this table]
 * @return [Query string]
 */
std::string DatabaseTable::getCreationQuery() {

    // TODO: Do some sanity checking/validation on the table before creation
    std::stringstream ss;

    ss << "CREATE TABLE " << name << "( \n";

    for (int i = 0; i < MAX_COLUMNS; i++) {
        if (column[i]) {
            // TODO: Come up with a better way to add the commas depending on whether it's the last item or not
            ss << column[i]->getCreationQuery() << ((i == MAX_COLUMNS || !column[i + 1]) ? "" : ",") << std::endl;
        }
    }

    ss << ");";

    return ss.str();
}

/**
 * [DatabaseTable::outputTableInfo Outputs the table information to the console]
 */
void DatabaseTable::outputTableInfo() {
    std::cout << "~~~~~ Table ~~~~~" << std::endl;
    std::cout << "Name: " << name << std::endl << std::endl;

    for (auto &currentColumn : column) {
        if (currentColumn) {
            currentColumn->outputColumnInfo();
        }
    }

    std::cout << "~~~~~~~~~~~~~~~~~" << std::endl;
}

// DatabaseColumn class
DatabaseColumn::DatabaseColumn(const std::string &cName, ColumnType cType, bool cNotNull,
                               const std::string &cDefaultValue, bool cPrimaryKey) {

    name = cName;
    type = cType;
    primaryKey = cPrimaryKey;
    notNull = cNotNull;
    defaultValue = cDefaultValue;

}

ColumnType DatabaseColumn::getType() {
    return type;
}

std::string DatabaseColumn::getTypeAsString() {
    return DatabaseSchemaUtils::getType(type);
}

/**
 * [DatabaseColumn::outputColumnInfo Writes column info to the console]
 */
void DatabaseColumn::outputColumnInfo() {

    std::cout
            << "---- Column ----" << std::endl
            << "Name: " << name << std::endl
            << "Type: " << getTypeAsString() << std::endl
            << "Primary: " << (primaryKey ? "True" : "False") << std::endl
            << "----------------" << std::endl
            << std::endl;
}

/**
 * [DatabaseColumn::getCreationQuery Generate part of a query to create this column]
 * @return [Query string]
 */
std::string DatabaseColumn::getCreationQuery() {
    std::stringstream ss;

    ss
            << "\t"
            << name << " "
            << getTypeAsString()
            << (primaryKey ? " PRIMARY KEY" : "")
            << (notNull ? " NOT NULL " : "")
            << "DEFAULT NULL";

    return ss.str();
}

// DatabaseSchemaUtils class
std::string DatabaseSchemaUtils::getType(ColumnType columnType) {

    switch (columnType) {
        case ColumnType::tText:
            return "TEXT";
        case ColumnType::tInteger:
            return "INTEGER";
        case ColumnType::tDouble:
            return "DOUBLE";
        case ColumnType::tDate:
            return "DATE";
        case ColumnType::tTimestamp:
            return "TIMESTAMP";
        case ColumnType::tBoolean:
            return "BOOLEAN";
        default:
            return "UNKNOWN";
    }

}
