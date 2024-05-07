#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>

// Use standard namespace.
using namespace std;

class ShellUtils {
public:
    /**
     * Execute a shell command without capturing the output.
     *
     * @param cmd, the shell command to execute.
     * @return The exit status of the command.
     */
    static int exec(const string& cmd) {
        // Open a pipe to run the shell command.
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe == nullptr) {
            // Error opening the pipe.
            return -1;
        }

        // Get the exit status of the command.
        int status = pclose(pipe);
        return WEXITSTATUS(status);
    }

    /**
     * Execute a shell command and capture its output.
     *
     * @param cmd, the shell command to execute.
     * @return A pair containing the exit status and the command output.
     */
    static pair<int, string> execWithOutput(const string& cmd) {
        // Open a pipe to run the shell command.
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe == nullptr) {
            // Error opening the pipe.
            return {-1, ""};
        }

        stringstream output;
        constexpr size_t initialBufferSize = 4096; // Initial buffer size
        vector<char> buffer(initialBufferSize);

        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            size_t length = strlen(buffer.data());
            if (length == buffer.size() - 1) {
                // Resize the buffer if it's full.
                buffer.resize(buffer.size() * 2);
                continue;
            }
            output << buffer.data();
        }

        // Get the exit status of the command.
        int status = pclose(pipe);

        // Extract the result and remove the trailing newline, if any.
        string result = output.str();
        result.erase(result.find_last_not_of("\n") + 1);

        return {WEXITSTATUS(status), result};
    }
};