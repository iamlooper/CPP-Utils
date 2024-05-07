#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <sys/stat.h>

#include "logger.hpp"
#include "shell_utils.hpp"

// Use standard namespace.
using namespace std;

class FSUtils {
public:
    /**
     * Reads the content of a file and returns it as a string.
     *
     * @param filename, the path to the file.
     * @return The content of the file as a string.
     */
    static string readFile(const string& filename) {
        ifstream file(filename);
        string content;
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                content += line + "\n";
            }
            file.close();
        } else {
            Logger(LogType::ERROR, cerr) << "Unable to open file " << filename << endl;
        }
        return content;
    }

    /**
     * Writes the given content to a file.
     *
     * @param filename, the path to the file.
     * @param content, the content to be written to the file.
     */
    static void writeFile(const string& filename, const string& content) {
        ofstream file(filename);
        if (file.is_open()) {
            file << content;
            file.close();
        } else {
            Logger(LogType::ERROR, cerr) << "Unable to write to file " << filename << endl;
        }
    }
    
    /**
     * Modifies the value of a file.
     *
     * @param fileName, the name of the file to mutate.
     * @param value, the new value of the file.
     * @return true if the mutate operation is successful, false otherwise.
     */
    static bool mutateFile(const string& fileName, const string& value) {
        // Check if the file exists.
        if (!isPathExists(fileName)) {
            // File does not exist, return false.
            Logger(LogType::ERROR) << fileName << " does not exist." << endl;
            return false;
        }

        // Write the new value.
        ofstream outFile(fileName.c_str());
        if (!outFile.is_open()) {
            // Failed to open the file for writing, return false.
            Logger(LogType::ERROR) << "Failed to open " << fileName << " for writing." << endl;
            return false;
        }
        outFile << value;
        outFile.close();

        // Log the success.
        Logger(LogType::INFO) << fileName << " -> " << value << endl;
        return true;
    }    

    /**
     * Creates a folder at the specified path.
     *
     * @param folderPath, the path where the folder should be created.
     * @return true if the folder is created successfully, false otherwise.
     */
    static bool createFolder(const string& folderPath) {
        if (mkdir(folderPath.c_str(), 0777) == 0) {
            return true;
        } else {
            Logger(LogType::ERROR, cerr) << "Unable to create folder " << folderPath << endl;
            return false;
        }
    }

    /**
     * Deletes a folder at the specified path.
     *
     * @param folderPath, the path to the folder to be deleted.
     * @return true if the folder is deleted successfully, false otherwise.
     */
    static bool deleteFolder(const string& folderPath) {
        if (rmdir(folderPath.c_str()) == 0) {
            return true;
        } else {
            Logger(LogType::ERROR, cerr) << "Unable to delete folder " << folderPath << endl;
            return false;
        }
    }

    /**
     * Checks if the given path exists.
     *
     * @param path, the path to a file or folder.
     * @return true if the path exists, false otherwise.
     */    
    static bool isPathExists(const string& path) {
        struct stat info;
        int ret = stat(path.c_str(), &info);
        return ret == 0;
    }
    
    /**
     * Get a list of paths from a wildcard path.
     *
     * @param wildcardPath, the path containing a wildcard (*).
     * @return A vector of expanded paths.
     */    
    static vector<string> getPathsFromWp(const string& wildcardPath) {
        // Execute a shell command to retrieve the output of the wildcard expansion.
        auto result = ShellUtils::execWithOutput("for i in " + wildcardPath + "; do echo $i; done");

        // Split the output by line and store it in a vector.
        vector<string> paths;
        stringstream stream(result.second);
        string path;
        while (getline(stream, path)) { 
            paths.push_back(path); 
        }
        return paths;    
    }

    /**
     * Remove a path with support for wildcard path deletion.
     *
     * @param path, the path or wildcard path to be removed.
     */
    static void removePath(const string& path) {
        // If the path contains an asterisk, then use the getPathsFromWp() function to get a list of paths and remove them.
        if (path.find('*') != string::npos) { 
            vector<string> paths = getPathsFromWp(path);
            for (const string& p : paths) {
                remove(p.c_str());
            }
        } else {
            // If path does not contain an asterisk, then continue with normal method.
            remove(path.c_str());
        }
    }    
};