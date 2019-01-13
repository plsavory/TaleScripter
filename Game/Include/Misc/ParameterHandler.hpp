/**
 * [ParameterHandler This class will handle any command line parameters if they were passed to the program]
 */
class ParameterHandler {
public:
  ParameterHandler(int argc, char* argv[]);
  ~ParameterHandler();
  bool shouldExit();
private:
  bool shouldExitProgram;
  void printVersionInformation();
  void printLicenceInformation();
};
