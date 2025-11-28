export module game;

import <string>;

import subject;              // Game inherits from Subject for observer pattern
import board;                // Board class
import player;               // Player class
import link;                 // Link class
import util.direction;       // Direction enum
import util.abilityParams;  // AbilityParams struct
import util.moveResult;      // MoveResult struct
import util.abilityResult;   // AbilityResult struct
import ability;              // Ability class (forward declaration)

/**
 * Game class - Coordinates the game state and player actions
 * 
 * This class manages:
 * - The game board
 * - Two players
 * - Current player turn
 * - Win condition checking
 * - Link movement and downloads
 * - Ability usage
 * 
 * Game inherits from Subject to notify observers (displays) of state changes
 */
export class Game : public Subject {
    Board board_;                          // The game board
    Player players_[2];        // Two players (0 and 1)
    int currentPlayer_ = 0;               // Current player index (0 or 1)
    int winnerId_ = -1;         // -1 = no winner yet, 0 or 1 = winner
    bool gameOver_ = false;
    bool useGraphics_ = true;

    void handleDownload(Link& link);
    void handleMoveResult(MoveResult& result);
    void switchPlayer();
    void checkWinCondition();

public:
    Game(bool useGraphics = true);

    void initializeGame(PlayerSetup& p0, PlayerSetup& p1);

    Board& getBoard() { return board_; }

    Player& getPlayer(int idx) { return players_[idx]; }

    Player& getCurrentPlayer() { return players_[currentPlayer_]; }

    int getCurrentPlayerIndex() const { return currentPlayer_; }

    MoveResult moveLink(char linkId, Direction dir);

    AbilityResult useAbility(int abilityIndex, AbilityParams& params);

    void endTurn();

    void reset();

    bool isGameOver();

    int getWinnerId();
};

