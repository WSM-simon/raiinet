export module util.abilityResult;

import <string>;
import <vector>;
import util.resultHeader;
export class Link;

using std::string;
using std::vector;

export struct AbilityResult {
    ResultHeader header;   // success / error message

    bool used = false;         // ability take effect or not
    bool stateChanged = false;

    vector<Link*> affectedLinks;   // influenced link
};
