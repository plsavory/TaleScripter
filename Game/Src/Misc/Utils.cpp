#include <iostream>
#include "Misc/Utils.hpp"
#include "Misc/ProjectInfo.hpp"
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <regex>

/**
 * [Utils::setBit Sets or unsets a bit on the provided 8-bit variable]
 * @param bit      [Which bit to operate on]
 * @param val      [New value]
 * @param variable [The variable to set/unset the bit on]
 */
void Utils::setBit(int bit, bool val, unsigned char &variable)
{
  // Can't set a bit greater than the size of the variable
  if (bit > 7) {
    return;
  }

  if (val) {
    variable |= (1 << bit);
  } else {
    variable &= ~(1 << bit);
  }

}

/**
 * [Utils::setBit Sets or unsets a bit on the provided 16-bit variable]
 * @param bit      [Which bit to operate on]
 * @param val      [New value]
 * @param variable [The variable to set/unset the bit on]
 */
void Utils::setBit(int bit, bool val, unsigned short &variable)
{
  // Can't set a bit greater than the size of the variable
  if (bit > 15) {
    return;
  }

  if (val) {
    variable |= (1 << bit);
  } else {
    variable &= ~(1 << bit);
  }
}

/**
 * [Utils::testBit Tests a bit]
 * @param  bit      [The bit to test]
 * @param  variable [The data to do the test on]
 * @return          [True if bit is set, otherwise false]
 */
bool Utils::testBit(int bit, const unsigned char variable)
{
  if (bit > 7)
    return false;

  return (variable & (1 << bit));
}

/**
 * [Utils::testBit Tests a bit]
 * @param  bit      [The bit to test]
 * @param  variable [The data to do the test on]
 * @return          [True if bit is set, otherwise false]
 */
bool Utils::testBit(int bit, const unsigned short variable)
{
  if (bit > 15)
    return false;

  return (variable & (1 << bit));
}

/**
 * [getVersionString Returns a version string for the project, relies on ProjectInfo.h]
 * @param  verboseString [Whether to post the long string with OS/CPU info]
 * @return               [The string]
 */
std::string Utils::getVersionString(bool verboseString)
{
  std::stringstream stringStream;

  if (verboseString) {
    stringStream<<PROJECT_NAME<<" "<<PROJECT_VERSION<<PROJECT_OS<<PROJECT_ARCH<< " (Compiled: " << __DATE__
      << " - " << __TIME__ << ") " << "branch: " << BRANCH_STRING << " - commit: " << CURRENT_COMMIT_STRING;
  } else {
    stringStream<<PROJECT_NAME<<" "<<PROJECT_VERSION;
  }

    return stringStream.str();
}

/**
 * [Utils::fileExists Checks to see if a file exists or not]
 * @param  fileName [Path to the file]
 * @return          [True if file exists, otherwise false.]
 */
bool Utils::fileExists(const std::string &fileName) {

  if (FILE *file = fopen(fileName.c_str(), "r")) {
    fclose(file);
    return true;
  };

  return false;

}

/**
 * [Utils::explodeString Splits a string into pieces and returns a vector containing them]
 * @param string    [The string to split]
 * @param delimiter [The character to split on]
 */
std::vector<std::string> Utils::explodeString(const std::string &string, char delimiter) {

  std::vector<std::string> returnValue;
  std::stringstream ss(string);
  std::string currentItem;

  while (std::getline(ss, currentItem, delimiter)) {
    returnValue.push_back(currentItem);
  }

  return returnValue;

}

/**
 * [Utils::implodeString Takes a vector of strings and a delimiter, returns them all as one string]
 * @param  explodedString [The vector of strings to join]
 * @param  glue           [A string to separate each element]
 * @param  count          [Max number of elements to add to the string, 0 if no limit]
 * @return                [The imploded string]
 */
std::string Utils::implodeString(std::vector<std::string> explodedString, std::string glue, unsigned int count) {

  std::string returnValue;
  unsigned int wordCount = 0;

  while (!explodedString.empty()) {

    std::string thisElement = "";
    thisElement.append(explodedString.front());

    explodedString.erase(explodedString.begin());

    if (!explodedString.empty()) {

      // Also check to ensure that we aren't appending to a newline
      if (thisElement != "\n") {
        thisElement.append(glue);
      }
    }

    returnValue.append(thisElement);

    // If we've reached the end of the number of characters we want to append, stop here
    if (count != 0) {
      if ((++wordCount) == count) {
        return returnValue;
      }
    }

  }

  return returnValue;

}

/**
 * [Utils::implodeString Takes a vector of strings and a delimiter, returns them all as one string]
 * @param  explodedString [The vector of strings to join]
 * @param  glue           [A string to separate each element]
 * @return                [The imploded string]
 */
std::string Utils::implodeString(std::vector<std::string> explodedString, std::string glue) {
  return implodeString(explodedString, glue, 0); // I got tired of writing the extra 0...
}

/**
 * [Utils::implodeString Takes a vector of strings and implodes them]
 * @param  explodedString [Vector of strings to join]
 * @return                [The imploded string]
 */
std::string Utils::implodeString(std::vector<std::string> explodedString) {
  return implodeString(explodedString, "", 0); // I got tired of writing the extra 0...
}

/**
 * [Utils::removeQuotationsFromString Used often when using JSON, as the JSON library that I use returns quotation marks with strings in arrays]
 * @param  string [The string to operate on]
 * @return        [String with quotation marks removed from the start and end]
 */
std::string Utils::removeQuotationsFromString(std::string string) {

  std::regex start("$\"");
  std::regex end("\"^");

  return std::regex_replace(std::regex_replace(string, end, ""), start, "");
}
