export module abilityFactory;

import <memory>;
import ability;
import util.abilityKind;
import downloadability;
import firewallability;
import scanability;
import polarizeability;
import linkboostability;

using std::unique_ptr;
using std::make_unique;

export unique_ptr<Ability> makeAbility(AbilityKind kind) {
    switch (kind) {
        case AbilityKind::Download:
            return make_unique<DownloadAbility>();
        case AbilityKind::Firewall:
            return make_unique<FirewallAbility>();
        case AbilityKind::Scan:
            return make_unique<ScanAbility>();
        case AbilityKind::Polarize:
            return make_unique<PolarizeAbility>();
        case AbilityKind::LinkBoost:
            return make_unique<LinkboostAbility>();
    }
    return nullptr;
}