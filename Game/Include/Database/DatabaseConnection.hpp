#ifndef DATABASE_CONNECTION_INCLUDED
#define DATABASE_CONNECTION_INCLUDED

#include <sqlite3.h>
#include <iostream>
#include <vector>

#define DATA_SET_MAX_ROWS 1000
#define DATA_SET_MAX_COLUMNS 50

#define DATA_TYPE_NUMBER 0
#define DATA_TYPE_STRING 1
#define DATA_TYPE_DATE 2
#define DATA_TYPE_DATE_TIME 3
#define DATA_TYPE_BOOLEAN 4

enum FetchMode {None, All, One};

// Data set related stuff
struct DataSetColumn {
public:
  DataSetColumn(std::string cName, std::string cData) {
    name = cName;
    data = cData;
  };

  ~DataSetColumn() {

  };
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
    for (int i = 0; i<DATA_SET_MAX_COLUMNS; i++) {
      column[i] = NULL;
    }
  };
  ~DataSetRow() {
    for (int i = 0; i<DATA_SET_MAX_COLUMNS; i++) {
      if (column[i]) {
        delete column[i];
      }
    }
  };
  DataSetColumn* addColumn(std::string name, std::string data) {

    if (name == "") {
      return NULL;
    }

    for (int i = 0; i<DATA_SET_MAX_COLUMNS; i++) {
      if (!column[i]) {
        column[i] = new DataSetColumn(name,data);
        return column[i];
      }
    }

    return NULL;
  }
  DataSetColumn* getColumn(int id) {
    return column[id];
  }
  DataSetColumn* getColumn(std::string name) {
    for (int i = 0; i<DATA_SET_MAX_COLUMNS; i++) {
      if (column[i]) {
        if (column[i]->getName() == name) {
          return getColumn(i);
        }
      }
    }

    return NULL;
  }
  bool doesColumnExist(std::string name) {
    for (int i = 0; i<DATA_SET_MAX_COLUMNS; i++) {
      if (column[i]) {
        if (column[i]->getName() == name) {
          return true;
        }
      }
    }

    return false;
  }
  void debugOutputContents() {

    for (int i = 0; i < DATA_SET_MAX_COLUMNS; i++) {
      if (column[i]) {
        std::cout<<"Column: " << column[i]->getName() << " | "<< column[i]->getData();
        std::cout<<std::endl;
      }
    }

  }
private:
  DataSetColumn *column[DATA_SET_MAX_COLUMNS];
};

struct DataSet {
public:
  DataSet() {

    for (int i = 0; i < DATA_SET_MAX_ROWS; i++) {
      row[i] = NULL;
    }

    rowCount = 0;

  };

  ~DataSet() {

    for (int i = 0; i < DATA_SET_MAX_ROWS; i++) {
      if (row[i]) {
        delete row[i];
      }
    }

  };
  void clear() {
    for (int i = 0; i < DATA_SET_MAX_ROWS; i++) {

      if (row[i]) {
        delete row[i];
      }

      row[i] = NULL;
    }
  };
  DataSetRow* addRow() {

    for (int i = 0; i < DATA_SET_MAX_ROWS; i++) {
      if (!row[i]) {
        row[i] = new DataSetRow();
        rowCount++;
        return row[i];
      }
    }

    return NULL;

  }
  void debugOutputContents() {

    int rows = 0;

    for (int i = 0; i < DATA_SET_MAX_ROWS; i++) {
      if (row[i]) {
        rows++;
      }
    }

    std::cout<<"Contents of DataSet-  Rows: "<<rows<<std::endl;

    for (int i = 0; i < DATA_SET_MAX_ROWS; i++) {
      if (row[i]) {
        row[i]->debugOutputContents();
      }
    }
  }
  int getRowCount() {
    return rowCount;
  }
  DataSetRow* getRow(int id) {
    return row[id];
  }
private:
  DataSetRow *row[DATA_SET_MAX_ROWS];
  int rowCount;
};

class DatabaseConnection {
public:
  DatabaseConnection(std::string name);
  ~DatabaseConnection();
  void executeQuery(std::string query, DataSet *destinationDataSet);
  int executeQuery(std::string query);
  int getLastInsertId();
  int insert(std::string tableName, std::vector<std::string> columns, std::vector<std::string> values, std::vector<int> types);
  int insert(std::string tableName);
  bool isUsable() {
    return usable;
  }
private:
  std::string name;
  sqlite3 *db;
  char *zErrMsg;
  int rc;
  bool usable;
  std::string sanitizeString(std::string);
};

#endif
