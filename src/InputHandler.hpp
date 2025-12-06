#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include <iostream>
#include <string>
#include <spdlog/spdlog.h>

/**
 * The InputHandler class is primarily a wrapper class around standard input/output.
 * The class provides safety and helpful functionality for handling, validating, and using a user's keyboard input on the command line.
 */
class InputHandler
{
private:
    /**
     * Reads a user's single-line input in the terminal. I.e. any valid text entered until the "enter" key is pressed
     * @returns The user's input as a std::string
     */
    static std::string readLineInput()
    {
        // read input buffer into 'input' variable and return
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

public:
    /**
     * Displays the provided prompt and processes a user's single-line input in the terminal
     * @param prompt (OPTIONAL) The prompt to display to the user first before receiving input
     * @returns The user's input as a std::string
     */
    static std::string getLineInput(const std::string &prompt = "")
    {
        // prompt user if applicable
        if (!prompt.empty())
        {
            std::cout << prompt << std::endl;
        }
        // capture input
        return InputHandler::readLineInput();
    }

    /**
     * Get user integer input, does not continue until the user enter's a valid integer value
     * @returns The user's input as an int
     */
    static int getIntInput(const std::string &prompt = "", const std::string errorMsg = "")
    {
        // prompt user if applicable
        if (!prompt.empty())
        {
            std::cout << prompt << std::endl;
        }

        // read user input, continue if not valid integer
        while (true)
        {
            std::cout << prompt;
            std::string raw;
            std::getline(std::cin, raw);

            try
            {
                int value = std::stoi(raw);
                return value;
            }
            catch (...)
            {
            }

            std::cout << (errorMsg.empty() ? "Invalid option, try again." : errorMsg) << std::endl;
        }
    }
};

#endif