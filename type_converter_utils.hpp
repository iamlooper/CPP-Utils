#pragma once

#include <sstream>
#include <vector>

// Use standard namespace.
using namespace std;

class TypeConverterUtils {
public:
    /**
     * Converts a value from the source type to the target type.
     *
     * @tparam targetType, the type to convert to.
     * @tparam sourceType, the type to convert from.
     * @param source, the value to convert.
     * @return The converted value of the target type.
     */
    template<typename targetType, typename sourceType>
    static targetType to(const sourceType& source) {
        // Initialize.    
        targetType result;
        stringstream stream;
        
        // Convert the source value to a string
        stream << source;
        
        // Extract the converted value from the stringstream
        stream >> result;
        
        return result;
    }

    /**
     * Converts a string into a vector of strings, splitting by line.
     *
     * @param source, the input string to convert.
     * @return A vector of strings where each element corresponds to a line in the input string.
     */
    static vector<string> stringToVectorString(const string& source) {
        // Initialize.
        vector<string> result;
        stringstream stream(source);
        string line;

        // Process each line of the input string and add it to the result vector.
        while (getline(stream, line)) {
            result.push_back(line);
        }

        return result;
    }
};