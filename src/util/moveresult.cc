export module util.moveResult;

import util.resultHeader;
import util.firewallInfo;
import link;

export struct MoveResult {
    ResultHeader header;              // Success status and message

    bool moved = false;
    bool battle = false;
    bool downloaded = false;
    bool firewallTriggered = false;

    Link* attacker = nullptr;
    Link* defender = nullptr;
    Link* downloadedLink = nullptr;
    int downloaderId = -1;            // Who gets credit for the download (1 or 2)

    FirewallInfo* firewall = nullptr;
};