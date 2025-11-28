module polarizeability;

import ability;
import board;
import player;
import link;
import util.linkType;
import util.abilityParams;
import util.abilityResult;


PolarizeAbility::PolarizeAbility() : Ability("Polarize", 'P') {}

AbilityResult PolarizeAbility::apply(Board& board, Player& user, Player& opponent,
                    AbilityParams& userParams, 
                    AbilityParams& opponentParams) {

    (void)board;
    (void)opponent;
    (void)opponentParams; // Don't need these for PolarizeAbility

    AbilityResult res;

    Link* userLink = user.getLinkById(userParams.targetLinkId);

    if (!userLink) {
        res.header.success = false;
        res.header.msg = "No such link for this player.";
        return res;
    }

    if (!userLink->isOnBoard()) {
        res.header.success = false;
        res.header.msg = "Link already downloaded.";
        return res;
    }

    // Polarize: change Data to Virus or Virus to Data
    if (userLink->isData()) {
        userLink->setType(LinkType::Virus);
    } else {
        userLink->setType(LinkType::Data);
    }
    markUsed();
    res.used = true;
    return res;
}