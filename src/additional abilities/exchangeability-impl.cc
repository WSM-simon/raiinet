module exchangeability;

import ability;
import game;
import player;
import board;
import cell;
import link;
import util.abilityParams;
import util.abilityResult;
import util.position;


ExchangeAbility::ExchangeAbility() : Ability("Exchange", 'E') {}

AbilityResult ExchangeAbility::apply(Game& game, Player& user, Player& opponent,
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

    Board& board = game.getBoard();
    Cell& userCell = board.getCell(userPos.row, userPos.col);
    Cell& opponentCell = board.getCell(opponentPos.row, opponentPos.col);

    Link* tmp = userCell.getLink();
    userCell.setLink(opponentCell.getLink());
    opponentCell.setLink(tmp);

    userLink->setPosition(opponentPos);
    opponentLink->setPosition(userPos);

    markUsed();
    res.used = true;
    return res;
}