export module cell;

import link;
import util.firewallinfo;
import util.position;

export class Cell {
    Link* link_ = nullptr;                     // non-owning pointer
    FirewallInfo* firewall_ = nullptr;         // non-owning pointer to firewall info

public:
    // ---- link operations ----
    bool hasLink() { return link_ != nullptr; }
    Link* getLink() { return link_; }
    void setLink(Link* ln) { link_ = ln; }
    void clearLink() { link_ = nullptr; }

    // ---- firewall operations ----
    bool hasFirewall() { return firewall_ != nullptr; }
    FirewallInfo* getFirewall() { return firewall_; }

    void setFirewall(FirewallInfo* fw) {
        firewall_ = fw;
    }

    void clearFirewall() {
        firewall_ = nullptr;
    }
};