#pragma once

#include <unistd.h>
#include <csignal>
#include <cstring>
#include <sys/resource.h>
#include <sched.h>

#include "logger.hpp"
#include "shell_utils.hpp"
#include "type_converter_utils.hpp"

// Use standard namespace.
using namespace std;

class ThreadProcessUtils {
public:
    /**
     * Get the process ID (PID) of the specified process name.
     *
     * @param processName, the name of the process.
     * @return The PID of the process, or -1 if not found.
     */
    static pid_t getPid(const string& processName) {
        pid_t pid = -1;

        // Execute command to retrieve process ID based on process name.
        auto result = ShellUtils::execWithOutput("ps -Ao pid,args | grep -iE '" + processName + "' | sed 's/^[ \\t]*//' | cut -d' ' -f1 | sed '$d' | head -n 1");

        // Check if the result is not empty and convert it to pid_t.
        if (!result.second.empty()) {
            pid = TypeConverterUtils::to<pid_t>(result.second);
        }

        return pid;
    }

    /**
     * Get a vector of process IDs (PIDs) for the specified process name.
     *
     * @param processName, the name of the process.
     * @return A vector of PIDs corresponding to the process name.
     */
    static vector<pid_t> getPids(const string& processName) {
        vector<pid_t> pids;

        // Execute command to retrieve all PIDs based on process name.
        auto result = ShellUtils::execWithOutput("ps -Ao pid,args | grep -iE '" + processName + "' | sed 's/^[ \\t]*//' | cut -d' ' -f1 | sed '$d'");

        // Convert the result to a vector of string PIDs.
        vector<string> stringPids = TypeConverterUtils::stringToVectorString(result.second);

        // Convert each string PID to pid_t and add to the vector.
        for (const string& stringPid : stringPids) {
            if (!stringPid.empty()) {
                pids.push_back(TypeConverterUtils::to<pid_t>(stringPid));
            }
        }

        return pids;
    }

    /**
     * Get the thread ID (TID) of the specified thread name within a process.
     *
     * @param processName, the name of the process.
     * @param threadName, the name of the thread.
     * @return The TID of the thread, or -1 if not found.
     */
    static pid_t getProcTid(const string& processName, const string& threadName) {
        // Initialize variable to store TID.
        pid_t tid = -1;

        // Find PID of the process.
        pid_t pid = getPid(processName);

        // Execute command to get the list of TIDs of the current process.
        auto result = ShellUtils::execWithOutput("ls /proc/" + to_string(pid) + "/task/");

        // Convert the result to a vector of string TIDs.
        vector<string> stringTids = TypeConverterUtils::stringToVectorString(result.second);

        // Iterate through each string TID and check for the desired thread name.
        for (const string& stringTid : stringTids) {
            if (!stringTid.empty()) {
                // Read the thread name from the corresponding /comm file.
                string tidPath = "/proc/" + to_string(pid) + "/task/" + stringTid + "/comm";
                ifstream tidFile(tidPath);
                string currentThreadName;
                tidFile >> currentThreadName;

                // Check if the current thread name matches the desired one.
                if (currentThreadName.find(threadName) != string::npos) {
                    tid = TypeConverterUtils::to<pid_t>(stringTid);
                    break;
                }
            }
        }

        return tid;
    }

    /**
     * Get a vector of thread IDs (TIDs) for the specified thread name within a process.
     * @param processName, the name of the process
     * @param threadName, the name of the thread.
     * @return A vector of TIDs corresponding to the thread name.
     */
    static vector<pid_t> getProcTids(const string& processName, const string& threadName) {
        vector<pid_t> tids;

        // Find PID of the process.
        pid_t pid = getPid(processName);

        // Execute command to get the list of TIDs of the current process.
        auto result = ShellUtils::execWithOutput("ls /proc/" + to_string(pid) + "/task/");

        // Convert the result to a vector of string TIDs.
        vector<string> stringTids = TypeConverterUtils::stringToVectorString(result.second);

        // Iterate through each string TID and check for the desired thread name.
        for (const string& stringTid : stringTids) {
            if (!stringTid.empty()) {
                // Read the thread name from the corresponding /comm file.
                string tidPath = "/proc/" + to_string(pid) + "/task/" + stringTid + "/comm";
                ifstream tidFile(tidPath);
                string currentThreadName;
                tidFile >> currentThreadName;

                // Check if the current thread name matches the desired one.
                if (currentThreadName.find(threadName) != string::npos) {
                    tids.push_back(TypeConverterUtils::to<pid_t>(stringTid));
                }
            }
        }

        return tids;
    }
    
    /**
     * Change the priority of a process.
     *
     * @param processName, the name of the process.
     * @param newPriority, the new priority value to set.
     */
    static void reniceProcess(const string& processName, int newPriority) {
        // Get the PID of the process.
        pid_t pid = getPid(processName);

        // Get the current priority of the process.
        int oldPriority = getpriority(PRIO_PROCESS, pid);

        // Set the new priority for the process.
        if (setpriority(PRIO_PROCESS, pid, newPriority) == -1) {
            // Log an error if setting priority fails.
            Logger(LogType::ERROR) << "Failure setting priority for process " << processName << " (PID: " << pid << ")" << endl;
        } else {
            // Display the change in priority.
            Logger(LogType::INFO) << "Changed priority of process " << processName << " (PID: " << pid << ") from " << oldPriority << " to " << newPriority << endl;
        }
    }
    
    /**
     * Change the priority of a thread.
     *
     * @param processName, the name of the process.  
     * @param threadName, the name of the thread.
     * @param newPriority, the new priority value to set.
     */
    static void reniceThread(const string& processName, const string& threadName, int newPriority) {
        // Get the TID of the thread.
        pid_t tid = getProcTid(processName, threadName);

        // Get the current priority of the thread.
        int oldPriority = getpriority(PRIO_PROCESS, tid);

        // Set the new priority for the thread.
        if (setpriority(PRIO_PROCESS, tid, newPriority) == -1) {
            // Log an error if setting priority fails.
            Logger(LogType::ERROR) << "Failure setting priority for thread " << threadName << " (TID: " << tid << ")" << endl;
        } else {
            // Display the change in priority.
            Logger(LogType::INFO) << "Changed priority of thread " << threadName << " (TID: " << tid << ") from " << oldPriority << " to " << newPriority << endl;
        }
    }
    
    /**
     * Change the CPU affinity of a process.
     *
     * @param processName, the name of the process.
     * @param start, the starting CPU index.
     * @param end, the ending CPU index.
     */
    static void changeProcessCpuAffinity(const string& processName, int start, int end) {
        // Get the PID of the given process name.
        pid_t pid = getPid(processName);

        // Initialize the mask to all zeros.
        cpu_set_t new_mask;
        CPU_ZERO(&new_mask);

        // Set the specified range of CPUs in the mask.
        for (int i = start; i <= end; i++) {
            CPU_SET(i, &new_mask);
        }

        // Set the CPU affinity of the process using the new mask.
        int result = sched_setaffinity(pid, sizeof(new_mask), &new_mask);

        // Check the return value to see if the call was successful.
        if (result == -1) {
            // Log an error if setting CPU affinity fails.
            Logger(LogType::ERROR) << "Failure setting CPU affinity for process " << processName << " (PID: " << pid << ")" << endl;
        } else {
            // Display the change in CPU affinity.
            Logger(LogType::INFO) << "Changed CPU affinity of process " << processName << " (PID: " << pid << ") to CPU " << start << "-" << end << endl;
        }
    }
    
    /**
     * Change the CPU affinity of a thread.
     *
     * @param processName, the name of the process.
     * @param threadName, the name of the thread.
     * @param start, the starting CPU index.
     * @param end, the ending CPU index.
     */
    static void changeThreadCpuAffinity(const string& processName, const string& threadName, int start, int end) {
        // Get the TID of the given thread name.
        pid_t tid = getProcTid(processName, threadName);

        // Initialize the mask to all zeros.
        cpu_set_t new_mask;
        CPU_ZERO(&new_mask);

        // Set the specified range of CPUs in the mask.
        for (int i = start; i <= end; i++) {
            CPU_SET(i, &new_mask);
        }

        // Set the CPU affinity of the thread using the new mask.
        int result = sched_setaffinity(tid, sizeof(new_mask), &new_mask);

        // Check the return value to see if the call was successful.
        if (result == -1) {
            // Log an error if setting CPU affinity fails.
            Logger(LogType::ERROR) << "Failure setting CPU affinity for thread " << threadName << " (TID: " << tid << ")" << endl;
        } else {
            // Display the change in CPU affinity.
            Logger(LogType::INFO) << "Changed CPU affinity of thread " << threadName << " (TID: " << tid << ") to CPU " << start << "-" << end << endl;
        }
    }
    
    /**
     * Change the scheduling policy of a process.
     *
     * @param processName, the name of the process.
     * @param schedPolicy, the new scheduling policy.
     * @param priority, the priority value to set.
     */
    static void changeProcessScheduler(const string& processName, const int& schedPolicy, int priority) {
        // Get the PID of the given process name.
        pid_t pid = getPid(processName);

        // Get the current scheduling policy and priority of the process.
        int oldSchedPolicy;
        sched_param oldParam;
        sched_getparam(pid, &oldParam);
        oldSchedPolicy = sched_getscheduler(pid);

        // Set the new scheduling policy and priority for the process.
        sched_param new_param;
        new_param.sched_priority = priority;
        int result = sched_setscheduler(pid, schedPolicy, &new_param);

        // Check the return value to see if the call was successful.
        if (result == -1) {
            // Log an error if changing scheduler fails.
            Logger(LogType::ERROR) << "Failure changing scheduler for process " << processName << " (PID: " << pid << ")" << endl;
        } else {
            // Display the change in scheduling policy and priority.
            Logger(LogType::INFO) << "Changed scheduling policy of process " << processName << " (PID: " << pid << ") from Policy: " << oldSchedPolicy << ", Priority: " << oldParam.sched_priority << " to " << "Policy: " << schedPolicy << ", Priority: " << priority << endl;
        }
    }
    
    /**
     * Change the scheduling policy of a thread.
     *
     * @param processName, the name of the process.
     * @param threadName, the name of the thread. 
     * @param schedPolicy, the new scheduling policy.
     * @param priority, the priority value to set.
     */
    static void changeThreadScheduler(const string& processName, const string& threadName, const int& schedPolicy, int priority) {
        // Get the TID of the given thread name.
        pid_t tid = getProcTid(processName, threadName);

        // Get the current scheduling policy and priority of the thread.
        int oldSchedPolicy;
        sched_param oldParam;
        sched_getparam(tid, &oldParam);
        oldSchedPolicy = sched_getscheduler(tid);

        // Set the new scheduling policy and priority for the thread.
        sched_param new_param;
        new_param.sched_priority = priority;
        int result = sched_setscheduler(tid, schedPolicy, &new_param);

        // Check the return value to see if the call was successful.
        if (result == -1) {
            // Log an error if changing scheduler fails.
            Logger(LogType::ERROR) << "Failure changing scheduler for thread " << threadName << " (TID: " << tid << ")" << endl;
        } else {
            // Display the change in scheduling policy and priority.
            Logger(LogType::INFO) << "Changed scheduling policy of thread " << threadName << " (TID: " << tid << ") from Policy: " << oldSchedPolicy << ", Priority: " << oldParam.sched_priority << " to " << "Policy: " << schedPolicy << ", Priority: " << priority << endl;
        }
    }   
   
    /**
     * Kill all processes with the given name.
     *
     * @param processName, the name of the process to kill.
     */
    static void killProcess(const string& processName) {
        // Get all PIDs of the specified process name.
        vector<pid_t> pids = getPids(processName);

        // Kill each process with SIGKILL.
        for (const pid_t& pid : pids) {
            Logger(LogType::INFO) << "Killing process " << processName << " (PID: " << pid << ")..." << endl;
            kill(pid, SIGKILL);
        }
    }

    /**
     * Kill all threads with the given name within a process.
     *
     * @param processName, the name of the process.
     * @param threadName, the name of the thread to kill.
     */
    static void killThread(const string& processName, const string& threadName) {
        // Get all TIDs of the specified thread name within the process.
        vector<pid_t> tids = getProcTids(processName, threadName);

        // Kill each thread with SIGKILL.
        for (const pid_t& tid : tids) {
            Logger(LogType::INFO) << "Killing thread " << threadName << " (TID: " << tid << ")..." << endl;
            kill(tid, SIGKILL);
        }
    }   
};