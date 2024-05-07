#pragma once

#include "shell_utils.hpp"

class VMTouchUtils {
public:
    /**
     * Preloads the specified item into memory using VMTouch.
     *
     * @param itemType, type of the item (e.g., "obj" for object, "app" for application).
     * @param item, the item to be preloaded.
     */
    static void preload(string itemType, string item) {
        // Initialize variable.
        string cmd;

        // Preload depending on item type.
        if (itemType == "obj") {
            if (FSUtils::isPathExists(item)) {
                cmd = "/data/local/tmp/vmtouch -dl " + item;
            } else {
                // Handle non-existent path.
                return;
            }
        } else if (itemType == "app") {
            cmd = "/data/local/tmp/vmtouch -dl $(pm path " + item + " | cut -d: -f2 | xargs dirname)";
        } else {
            // Handle unsupported item type.
            return;
        }

        // Execute the command and check for success.
        if (ShellUtils::exec(cmd) != 0) {
            // Log error message.
            Logger(LogType::INFO) << "Failed to preload " << item << " into memory." << endl;
        } else {
            // Log success message.
            Logger(LogType::INFO) << "Preloaded " << item << " into memory." << endl;
        }
    }

    /**
     * Preloads the specified item into memory using VMTouch in full mode.
     *
     * @param itemType, type of the item (e.g., "obj" for object, "app" for application).
     * @param item, the item to be preloaded.
     */
    static void preloadFull(string itemType, string item) {
        // Initialize variable.
        string cmd;

        // Preload depending on item type.
        if (itemType == "obj") {
            if (FSUtils::isPathExists(item)) {
                cmd = "/data/local/tmp/vmtouch -dL " + item;
            } else {
                // Handle non-existent path.
                return;
            }
        } else if (itemType == "app") {
            cmd = "/data/local/tmp/vmtouch -dL $(pm path " + item + " | cut -d: -f2 | xargs dirname)";
        } else {
            // Handle unsupported item type.
            return;
        }

        // Execute the command and check for success.
        if (ShellUtils::exec(cmd) != 0) {
            // Log error message.
            Logger(LogType::INFO) << "Failed to preload " << item << " into memory." << endl;
        } else {
            // Log success message.
            Logger(LogType::INFO) << "Preloaded " << item << " into memory." << endl;
        }
    }
};