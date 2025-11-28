export module cell;

import link;
import util.firewallInfo;
import util.position;

export class Cell {
    Link* link_ = nullptr;          // non-owning pointer to Link
    FirewallInfo firewall_;         // stored BY VALUE
    bool hasFirewall_ = false;

public:
    // ---- link operations ----
    bool hasLink() { return link_ != nullptr; }

    Link* getLink() { return link_; }

    void setLink(Link* ln) { link_ = ln; }

    void clearLink() { link_ = nullptr; }

    // ---- firewall operations ----
    bool hasFirewall() { return hasFirewall_; }

    FirewallInfo* getFirewall() {
        return hasFirewall_ ? &firewall_ : nullptr;
    }

    // ownerId comes as int; save directly in firewall_
    void setFirewall(int ownerId) {
        firewall_.ownerId = ownerId;
        hasFirewall_ = true;
    }

    void clearFirewall() {
        hasFirewall_ = false;
    }
};