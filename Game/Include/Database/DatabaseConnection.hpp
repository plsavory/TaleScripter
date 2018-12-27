#include "SQLite/sqlite3.h"

#define DATA_SET_MAX_ROWS 1000
#define DATA_SET_MAX_COLUMNS 50
#define DATABASE_DEBUG

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
  std::string name;
  std::string data;
private:
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
        if (column[i]->name == name) {
          return getColumn(i);
        }
      }
    }

    return NULL;
  }
  bool doesColumnExist(std::string name) {
    for (int i = 0; i<DATA_SET_MAX_COLUMNS; i++) {
      if (column[i]) {
        if (column[i]->name == name) {
          return true;
        }
      }
    }

    return false;
  }
  void debugOutputContents() {

    for (int i = 0; i < DATA_SET_MAX_COLUMNS; i++) {
      if (column[i]) {
        std::cout<<"Column: " << column[i]->name << " | "<< column[i]->data;
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
private:
  DataSetRow *row[DATA_SET_MAX_ROWS];
};

class DatabaseConnection {
public:
  DatabaseConnection(std::string name);
  ~DatabaseConnection();
  void executeQuery(std::string query, DataSet *destinationDataSet);
private:
  std::string name;
  sqlite3 *db;
  char *zErrMsg;
  int rc;
};
