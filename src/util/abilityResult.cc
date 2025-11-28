export module util.abilityResult;

import <string>;
import <vector>;
import util.resultHeader;

using std::string;
using std::vector;

export struct AbilityResult {
    ResultHeader header;  // success / error message

    bool used = false; // ability used or not
};
