export module util.resultHeader;

import <string>;

/**
 * ResultHeader - Contains success status and message for operations
 * Used in MoveResult to indicate if an operation succeeded and provide error messages
 */
export struct ResultHeader {
    bool success = true;              // true if operation succeeded, false otherwise
    std::string msg = "";            // Message describing the result (error message if failed)
};

