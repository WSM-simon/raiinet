export module util.moveresult;

// Forward declare Link because MoveResult references it
export class Link;       
import util.resultHeader;
import util.firewallinfo;

export struct MoveResult {
    ResultHeader header;              // Success status and message

    bool moved = false;
    bool battle = false;
    bool downloaded = false;
    bool firewallTriggered = false;

    Link* attacker = nullptr;
    Link* defender = nullptr;
    Link* downloadedLink = nullptr;

    FirewallInfo* firewall = nullptr;
};