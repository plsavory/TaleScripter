#include <iostream>
#include <vector>
#include <string>
#include "Database/DatabaseConnection.hpp"
#include "GameCompiler/FileHandler.hpp"
#include "GameCompiler/ProjectBuilder.hpp"
#include "GameCompiler/GameCompiler.hpp"
#include "Misc/ParameterHandler.hpp"
#include "Misc/Utils.hpp"
#include <sstream>
#include <Exceptions/ProjectBuilderException.hpp>
#include <Exceptions/MisuseException.hpp>

int main(int argc, char *argv[]) {

    try {

        std::cout << Utils::getVersionString(true) << std::endl << std::endl;

        // TODO: Recreate something like this parameter handling code inside the ParameterHandler class, I think this method is better.
        auto *compilerOptions = new GameCompilerOptions(); // Is this really a compiler? Maybe I should rename this at some point.

        // Parse the arguments provided to the executable
        int count = 1;
        std::vector<std::string> errorMessage;

        // This is really ugly, but for now I don't have the time to be implementing a switch on string solution.
        const char *supportedParameters[] = {"-f", "-p"};
        int countOfSupportedParameters = sizeof(supportedParameters) / sizeof(*supportedParameters);

        while (count < argc) {

            bool supportedParameter = false;
            std::string currentParameter(argv[count]);
            std::string path;

            const char *argumentChar = (count < (argc - 1)) ? argv[count + 1] : "none";

            std::string currentArgument(argumentChar);

            int matchedParameter = 0;

            for (int i = 0; i < countOfSupportedParameters; i++) {

                std::string currentSupportedParameter(supportedParameters[i]);

                if (currentParameter == currentSupportedParameter) {
                    supportedParameter = true;
                    matchedParameter = i;
                }
            }

            if (!supportedParameter) {
                throw ProjectBuilderException("An unsupported parameter has been passed.");
            }

            switch (matchedParameter) {
                case 0: // -f
                    if (currentArgument == "none") {
                        throw ProjectBuilderException(
                                "The -f parameter requires an argument containing the file path to the project.");
                    }

                    if (!Utils::fileExists(currentArgument)) {
                        std::stringstream ss;
                        ss << "Unable to find project file: " << currentArgument;
                        throw ProjectBuilderException(ss.str());
                    }

                    compilerOptions->setProjectFilePath(currentArgument);
                    break;
                case 1: // -p
                    if (currentArgument == "none") {
                        throw ProjectBuilderException("The -p parameter requires a project name");
                    }

                    path.append("projects/");
                    path.append(currentArgument);
                    path.append("/project.json");

                    if (!Utils::fileExists(path)) {
                        std::stringstream ss;
                        ss << "Unable to find project file: " << path;
                        throw ProjectBuilderException(ss.str());
                    }

                    compilerOptions->setProjectFilePath(path);
                    break;
                default:
                    throw MisuseException("A parameter is listed as supported, however it is not being handled.");
            }

            count += 2;

        }

        auto fileHandler = new FileHandler();

        try {
            auto *compiler = new GameCompiler(compilerOptions, fileHandler);
            compiler->process();
            std::cout << std::endl << "Done! Run TaleScripter-Runner to run your project." << std::endl;
        } catch (ProjectBuilderException &e) {

            // My own exceptions
            // At this point, we can print the file name that we were attempting to process when the error occurred to be more helpful to the user.
            std::cout <<
                      "A data error occurred while building your project." << std::endl <<
                      std::endl << "File: " << fileHandler->getLastReadFileName() <<
                      std::endl << "Message: " << e.what() << std::endl;

            exit(EXIT_FAILURE);

        } catch (nlohmann::json::exception &e) {

            // JSON exceptions
            std::cout <<
                      "A JSON error occurred while building your project." << std::endl <<
                      std::endl << "File: " << fileHandler->getLastReadFileName() <<
                      std::endl << "Message: " << e.what() << std::endl;

            exit(EXIT_FAILURE);

        }

    } catch (GeneralException &e) {
        std::cout << "An error occurred while building your project: " << e.what() << std::endl;
    }
}
