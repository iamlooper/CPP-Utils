#pragma once

#include "shell_utils.hpp"

// Use standard namespace.
using namespace std;

class AndroidUtils {
public:
    /**
     * Get the package name of the default home activity.
     * @return The home package name.
     */
    static string getHomePkgName() {
        auto result = ShellUtils::execWithOutput("pm resolve-activity -a android.intent.action.MAIN -c android.intent.category.HOME | grep packageName | head -n 1 | cut -d= -f2");
        return result.second;
    }

    /**
     * Get the package name of the current input method editor (IME).
     * @return The IME package name.
     */
    static string getImePkgName() {
        auto result = ShellUtils::execWithOutput("ime list | grep packageName | head -n 1 | cut -d= -f2");
        return result.second;
    }

    /**
     * Check if an app with the given package name exists.
     * @param pkgName, the package name to check.
     * @return true if the app exists, false otherwise.
     */
    static bool isAppExists(const string& pkgName) {
        if (ShellUtils::exec("pm path " + pkgName) == 0) {
            return true;
        } else {
            return false;
        }
    }
    
    /**
     * Stop the given system service.
     *
     * @param serviceName, the name of the service to stop.
     */
    static void stopSystemService(const string& serviceName) {
        // Log and stop system service.
        Logger(LogType::INFO) << "Stopping system service " << serviceName << "..." << endl;
        ShellUtils::exec("stop " + serviceName);
    }
};