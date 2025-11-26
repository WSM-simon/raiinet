export module util.moveresult;

// Forward declare Link because MoveResult references it
export class Link;       
import util.firewallinfo;

export struct MoveResult {
    bool moved = false;
    bool battle = false;
    bool downloaded = false;
    bool firewallTriggered = false;

    Link* attacker = nullptr;
    Link* defender = nullptr;
    Link* downloadedLink = nullptr;

    FirewallInfo* firewall = nullptr;
};