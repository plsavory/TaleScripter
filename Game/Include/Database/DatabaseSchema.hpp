/*
  DatabaseSchema.hpp - Written by P.L Savory
  This file contains classes and structures which will provide a view of a database to the application
  This allows for quick table and row creation, as well as the ability to create an entire database with one function call.
  There will be lots of TODOs here as this will take some time to develop

  These classes do not actually run any queries, they will just generate queries and return data types to be used within the database management class.

  These classes won't be the only way to interface with the database, the Database manager class will also accept raw SQL query strings.
 */

// TODO: See if this file and DatabaseSchema.cpp require to be split up on a class per class basis.

#ifndef DATABASE_SCHEMA_INCLUDED
#define DATABASE_SCHEMA_INCLUDED

#define MAX_TABLES 0xFF
#define MAX_COLUMNS 0xFF

#include <iostream>

enum ColumnType {
    tText, tInteger, tDouble, tDate, tTimestamp, tBoolean
};

class DatabaseSchemaUtils {
public:
    static std::string getType(ColumnType columnType);
};

class DatabaseColumn {
public:
    DatabaseColumn(const std::string &cName, ColumnType cType, bool cNotNull, const std::string &cDefaultValue,
                   bool cPrimaryKey);

    ColumnType getType();

    std::string getTypeAsString();

    void outputColumnInfo();

    std::string getCreationQuery();

private:
    std::string name;
    ColumnType type;
    bool primaryKey;
    bool notNull;
    std::string defaultValue;
};

class DatabaseTable {
public:
    explicit DatabaseTable(const std::string &tName);

    std::string getName();

    DatabaseColumn *
    addColumn(const std::string &cName, ColumnType cType, bool cNotNull, const std::string &cDefaultValue);

    DatabaseColumn *addPrimaryKey();

    DatabaseColumn *
    addForeignKey(const std::string &cName, const std::string &foreignTableName, const std::string &foreignTableId);

    void outputTableInfo();

    std::string getCreationQuery();

private:
    std::string name;
    DatabaseColumn *column[MAX_COLUMNS]{};

    DatabaseColumn *
    addColumnInternal(const std::string &cName, ColumnType cType, bool cNotNull, const std::string &cDefaultValue,
                      bool cPrimaryKey);
};

class DatabaseSchema {
public:
    explicit DatabaseSchema(const std::string &dName);

    std::string getName();

    DatabaseTable *addTable(const std::string &tableName);

    DatabaseTable *getTable(const std::string &tableName);

    bool createDatabase();

private:
    std::string name;
    DatabaseTable *table[MAX_TABLES]{};
};

#endif
