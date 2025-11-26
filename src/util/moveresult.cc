export module util.moveResult;

// Forward declare Link because MoveResult references it
export class Link;       
import util.resultHeader;
import util.firewallInfo;

export struct MoveResult {
    ResultHeader header;

    bool moved = false;
    bool battle = false;
    bool downloaded = false;
    bool firewallTriggered = false;

    Link* attacker = nullptr;
    Link* defender = nullptr;
    Link* downloadedLink = nullptr;

    FirewallInfo* firewall = nullptr;
};