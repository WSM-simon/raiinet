export module util.resultHeader;

import <string>;
using std::string;

export struct ResultHeader {
    bool success = true;   // default success
    string msg = "";       // write error message when failed
};