#include <vector>

class Utils {
public:
  static void setBit(int bit, bool val, unsigned char &variable);
  static void setBit(int bit, bool val, unsigned short &variable);
  static bool testBit(int bit, const unsigned char variable);
  static bool testBit(int bit, const unsigned short variable);
  static std::string getVersionString(bool verboseString);
  static bool fileExists(const std::string &fileName);
  static std::vector<std::string> explodeString(const std::string &s, char delimiter);
  static std::string implodeString(std::vector<std::string> explodedString, std::string glue, unsigned int count);
  static std::string implodeString(std::vector<std::string> explodedString, std::string glue);
  static std::string implodeString(std::vector<std::string> explodedString);
  static std::string removeQuotationsFromString(std::string string);
private:
};
