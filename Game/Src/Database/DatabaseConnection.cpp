#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include "Database/DatabaseConnection.hpp"

DatabaseConnection::DatabaseConnection(std::string name) {

  // Initialise values and open database connection
  zErrMsg = 0;

  std::string filename = name;
  filename.append(".gfdb");

  // Convert the string to a char array TODO: Move this into its own function
  char *fName = new char[filename.length() + 1];
  std::strcpy(fName, filename.c_str());

  rc = sqlite3_open(fName, &db);
  delete[] fName;

  if (rc) {
    std::cout<<"Error opening/creating database"<<std::endl;
  } else {
    std::cout<<"Database Opened"<<std::endl;
  }
}

DatabaseConnection::~DatabaseConnection() {
  sqlite3_close(db);
}

/**
 * [DatabaseConnection::executeQuery Execute a query on the database that this object is linked to]
 * @param query              [A query string]
 * @param destinationDataSet [A result set in which to save the results from the query]
 */
void DatabaseConnection::executeQuery(std::string query, DataSet *destinationDataSet) {

  sqlite3_stmt *statement;

  #ifdef DATABASE_DEBUG
    std::cout<<"Execute SQL statement: "<<query<<std::endl;
  #endif

  char *queryString = new char[query.length() + 1];
  std::strcpy(queryString, query.c_str());

  if (sqlite3_prepare_v2(db, queryString, -1, &statement, 0) == SQLITE_OK) {

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

          if (sqlite3_column_text(statement,col)) {
            // Add a column to the row

            const char* cName = sqlite3_column_name(statement,col);
            const char* cData = reinterpret_cast<const char*>(sqlite3_column_text(statement,col));

            std::string columnName(cName);
            std::string data(cData);

            row->addColumn(cName, cData);
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
    return;
  }

  // TODO: Add some nicer error handling here
  std::string error = sqlite3_errmsg(db);

  if (error != "not an error") {
    std::cout<<"An SQL error occurred: " << error << std::endl;
  }

  return;

}
