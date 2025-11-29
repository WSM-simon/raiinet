module exchangeability;

import ability;
import board;
import player;
import cell;
import link;
import util.abilityParams;
import util.abilityResult;
import util.position;


ExchangeAbility::ExchangeAbility() : Ability("Exchange", 'E') {}

AbilityResult ExchangeAbility::apply(Board& board, Player& user, Player& opponent,
                    AbilityParams& userParams, 
                    AbilityParams& opponentParams) {

    AbilityResult res;

    Link* userLink = user.getLinkById(userParams.targetLinkId);
    Link* opponentLink = opponent.getLinkById(opponentParams.targetLinkId);

    if (!opponentLink || !userLink) {
        res.header.success = false;
        res.header.msg = "No such link exist.";
        return res;
    }

    if (!opponentLink->isOnBoard() || !userLink->isOnBoard()) {
        res.header.success = false;
        res.header.msg = "Link already downloaded.";
        return res;
    }

    Position userPos = userLink->getPosition();
    Position opponentPos = opponentLink->getPosition();

    Cell& userCell = board.getCell(userPos.row, userPos.col);
    Cell& opponentCell = board.getCell(opponentPos.row, opponentPos.col);

    Link* tmp = userCell.getLink();
    userCell.setLink(opponentCell.getLink());
    opponentCell.setLink(tmp);

    userLink->setPosition(opponentPos);
    opponentLink->setPosition(userPos);

    markUsed();
    res.used = true;
    res.header.success = true;
    res.header.msg = "";
    return res;
}