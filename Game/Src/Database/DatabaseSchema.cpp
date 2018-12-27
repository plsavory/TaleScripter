#include "SQLite/sqlite3.h"
#include "Database/DatabaseSchema.hpp"
#include <cstring>
#include <string>
#include <sstream>

// DatabaseSchema class
DatabaseSchema::DatabaseSchema(std::string dName) {
  // TODO: Allow entire schema and tables to be loaded from JSON files
  name = dName;

  // Make all table pointers be null
  for (int i = 0; i < MAX_TABLES; i++) {
    table[i] = NULL;
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
  std::cout<<"Creating database " << name << ".gfdb..." << std::endl;

  // Database connection
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  std::string filename = name;
  filename.append(".gfdb");

  // Convert the string to a char array TODO: Move this into its own function
  char *fName = new char[filename.length() + 1];
  std::strcpy(fName, filename.c_str());

  rc = sqlite3_open(fName, &db);
  delete[] fName;

  if (rc) {
    std::cout<<"Error opening/creating database"<<std::endl;
    return false;
  } else {
    std::cout<<"Database created/opened"<<std::endl;
  }

  // Create all of the tables
  for (int i = 0; i < MAX_TABLES; i++) {
    if (table[i]) {

      std::string query = table[i]->getCreationQuery();
      char *sql = new char[query.length() + 1];
      std::strcpy(sql, query.c_str());

      // Callback argument is a function which will get called for each step/row
      // Here, we are passing NULL as we are creating tables and not fetching data.
      rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

      delete[] sql;

      if (rc != SQLITE_OK) {
        std::cout<<"-----------"<<std::endl<<"Sql error:"<<std::endl<< zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
      } else {
        std::cout<<"Table "<<table[i]->getName()<<" created successfully."<<std::endl;
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
DatabaseTable* DatabaseSchema::addTable(std::string name) {
  // Create a table in an empty slot
  for (int i = 0; i<MAX_TABLES; i++) {

    if (table[i] == NULL) {
      table[i] = new DatabaseTable(name);
      return table[i];
    }

  }

  return NULL;
}

// DatabaseTable class
DatabaseTable::DatabaseTable(std::string tName) {
  name = tName;

  // Set all columns to null
  for (int i = 0; i < MAX_COLUMNS; i++) {
    column[i] = NULL;
  }
}

std::string DatabaseTable::getName() {
  return name;
}

DatabaseColumn* DatabaseTable::addColumn(std::string cName, ColumnType cType, bool cNotNull, std::string cDefaultValue) {
  return addColumnInternal(cName, cType, cNotNull, cDefaultValue, false);
}

DatabaseColumn* DatabaseTable::addPrimaryKey() {
  return addColumnInternal("id", ColumnType::tInteger, true, "", true);
}

DatabaseColumn* DatabaseTable::addForeignKey(std::string cName, std::string foreignTableName, std::string foreignTableId) {
  // TODO: Implement this to enforce strict relations
  return NULL;
}

/**
 * [DatabaseTable::addColumnInternal Adds a column to the table schema]
 * @param  cName         [Name of the column]
 * @param  cType         [Column data type]
 * @param  cNotNull      [Not null restriction on column]
 * @param  cDefaultValue [Column's default value]
 * @param  cPrimaryKey   [Primary key of the column]
 * @return               [Success: Pointer to column object, failure: NULL]
 */
DatabaseColumn* DatabaseTable::addColumnInternal(std::string cName, ColumnType cType, bool cNotNull, std::string cDefaultValue, bool cPrimaryKey) {

  // TODO: Error if a column with the same name already exists

  // Find an empty slot to add a column to, if one exists, create it
  for (int i = 0; i < MAX_COLUMNS; i++) {
    if (column[i] == NULL) {
      column[i] = new DatabaseColumn(cName, cType, cNotNull, cDefaultValue, cPrimaryKey);
      return column[i];
    }
  }

  return NULL;
}

/**
 * [DatabaseTable::getCreationQuery Creates a query string to create this table]
 * @return [Query string]
 */
std::string DatabaseTable::getCreationQuery() {

  // TODO: Do some sanity checking/validation on the table before creation
  std::stringstream ss;

  ss << "CREATE TABLE "<<name<<"( \n";

  for (int i = 0; i < MAX_COLUMNS; i++) {
    if (column[i]) {
      // TODO: Come up with a better way to add the commas depending on whether it's the last item or not
      ss << column[i]->getCreationQuery() << ((i == MAX_COLUMNS || !column[i+1]) ? "" : ",")<<std::endl;
    }
  }

  ss << ");";

  return ss.str();
}

/**
 * [DatabaseTable::outputTableInfo Outputs the table information to the console]
 */
void DatabaseTable::outputTableInfo() {
  std::cout<<"~~~~~ Table ~~~~~"<<std::endl;
  std::cout<<"Name: "<<name<<std::endl<<std::endl;

  for (int i = 0; i < MAX_COLUMNS; i++) {
    if (column[i]) {
      column[i]->outputColumnInfo();
    }
  }

  std::cout<<"~~~~~~~~~~~~~~~~~"<<std::endl;
}

// DatabaseColumn class
DatabaseColumn::DatabaseColumn(std::string cName, ColumnType cType, bool cNotNull, std::string cDefaultValue, bool cPrimaryKey) {

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
  <<"---- Column ----"<<std::endl
  <<"Name: "<<name<<std::endl
  <<"Type: "<< getTypeAsString() <<std::endl
  <<"Primary: "<< (primaryKey ? "True" : "False")<<std::endl
  <<"----------------"<<std::endl
  <<std::endl;
}

/**
 * [DatabaseColumn::getCreationQuery Generate part of a query to create this column]
 * @return [Query string]
 */
std::string DatabaseColumn::getCreationQuery() {
  std::stringstream ss;

  ss
  <<"\t"
  <<name<<" "
  <<getTypeAsString()
  <<(primaryKey ? " PRIMARY KEY" : "")
  <<(notNull ? " NOT NULL" : "");

  return ss.str();
}

// DatabaseSchemaUtils class
std::string DatabaseSchemaUtils::getType(ColumnType columnType) {

  switch(columnType) {
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
  };

}
