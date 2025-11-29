export module textdisplay;

import <iostream>;
import <string>;
import <vector>;
import <sstream>;

import observer;
import game;
import board;
import player;
import link;
import cell;
import ability;
import util.firewallInfo;

using std::ostream;
using std::string;
using std::vector;

export class TextDisplay : public Observer {
    Game& game_;
    ostream& out_;
    bool extraFeatures_;  // Enable colors and split lines

    // ANSI color codes
    const char* GREEN() { return extraFeatures_ ? "\033[32m" : ""; }
    const char* RED() { return extraFeatures_ ? "\033[31m" : ""; }
    const char* BLUE() { return extraFeatures_ ? "\033[34m" : ""; }
    const char* ORANGE() { return extraFeatures_ ? "\033[33m" : ""; }
    const char* CYAN() { return extraFeatures_ ? "\033[36m" : ""; }
    const char* RESET() { return extraFeatures_ ? "\033[0m" : ""; }
    
    // Get color for a link based on alive/dead status
    const char* getLinkColor(Link& link) {
        return link.isOnBoard() ? GREEN() : RED();
    }

    // Helper to get link character for display
    char getLinkChar(Link& link, int viewingPlayer) {
        int owner = link.getOwnerId();
        char id = link.getId();
        
        // Player 1's links are lowercase (a-h), Player 2's are uppercase (A-H)
        if (owner == 1) {
            return id;  // already lowercase
        } else {
            return id;  // already uppercase for player 2
        }
    }

    // Get string representation of a link for the info section
    string getLinkInfo(Link& link, int viewingPlayer) {
        int owner = link.getOwnerId();
        
        // If this link belongs to the opponent and is not revealed, show "?"
        if (owner != viewingPlayer && !link.isRevealedToOpponent()) {
            return "?";
        }
        
        // Show type and strength
        char type = link.isData() ? 'D' : 'V';
        return string(1, type) + std::to_string(link.getStrength());
    }

public:
    TextDisplay(Game& game, ostream& out, bool extraFeatures = false) 
        : game_{game}, out_{out}, extraFeatures_{extraFeatures} {}
    
    void printSplitLine() {
        if (extraFeatures_) {
            out_ << CYAN() << "----------------------------------------" << RESET() << "\n";
        }
    }

    void notify() override {
        printBoard(game_.getCurrentPlayerIndex());
    }

    // Helper to print a player's info section
    void printPlayerInfo(int playerId, int viewingPlayer) {
        Player& player = game_.getPlayer(playerId);
        
        out_ << GREEN() << "Player " << playerId << ":" << RESET() << "\n";
        out_ << BLUE() << "Downloaded: " << RESET() << player.getDownloadedData() << "D, " 
             << player.getDownloadedViruses() << "V\n";
        
        // Count unused abilities
        int unusedAbilities = 0;
        auto& abilities = game_.getAbilities(playerId);
        for (auto& ab : abilities) {
            if (!ab->isUsed()) ++unusedAbilities;
        }
        out_ << ORANGE() << "Abilities: " << RESET() << unusedAbilities << "\n";

        // Print links info
        // Show actual types for own links, ? for unrevealed opponent links
        // Hide invisible links from opponent
        vector<Link>& links = player.getLinks();
        
        // Build first line - only show visible links to opponent
        bool first1 = true;
        for (int i = 0; i < 4 && i < (int)links.size(); ++i) {
            // If viewing opponent's link and it's invisible, skip it
            if (playerId != viewingPlayer && !links[i].isVisible()) {
                continue;
            }
            if (!first1) out_ << " ";
            first1 = false;
            out_ << getLinkColor(links[i]) << links[i].getId() << ": " 
                 << getLinkInfo(links[i], viewingPlayer) << RESET();
        }
        if (!first1) out_ << "\n";
        
        // Build second line - only show visible links to opponent
        bool first2 = true;
        for (int i = 4; i < 8 && i < (int)links.size(); ++i) {
            // If viewing opponent's link and it's invisible, skip it
            if (playerId != viewingPlayer && !links[i].isVisible()) {
                continue;
            }
            if (!first2) out_ << " ";
            first2 = false;
            out_ << getLinkColor(links[i]) << links[i].getId() << ": " 
                 << getLinkInfo(links[i], viewingPlayer) << RESET();
        }
        if (!first2) out_ << "\n";
    }

    void printBoard(int viewingPlayer) {
        Board& board = game_.getBoard();
        
        // Determine who goes at top and bottom
        // With textFlip: viewer at top, opponent at bottom
        // Without textFlip: Player 1 always at top, Player 2 always at bottom
        bool textFlip = game_.hasTextFlip();
        int topPlayer = textFlip ? viewingPlayer : 1;
        int bottomPlayer = textFlip ? (3 - viewingPlayer) : 2;

        // Print top player's info
        printPlayerInfo(topPlayer, viewingPlayer);

        // Print board (flipped when Player 2 is viewing, only if textFlip enabled)
        out_ << "========\n";
        
        // If textFlip: Player 1 sees rows 0->7, Player 2 sees rows 7->0 (flipped)
        // Without textFlip: always show rows 0->7
        bool shouldFlip = game_.hasTextFlip() && (viewingPlayer == 2);
        int startRow = shouldFlip ? board.getRows() - 1 : 0;
        int endRow = shouldFlip ? -1 : board.getRows();
        int rowStep = shouldFlip ? -1 : 1;
        
        for (int r = startRow; r != endRow; r += rowStep) {
            // If textFlip: Player 2 sees cols 7->0 (mirrored)
            // Without textFlip: always show cols 0->7
            int startCol = shouldFlip ? board.getCols() - 1 : 0;
            int endCol = shouldFlip ? -1 : board.getCols();
            int colStep = shouldFlip ? -1 : 1;
            
            for (int c = startCol; c != endCol; c += colStep) {
                Cell& cell = board.getCell(r, c);
                
                // Check if this is a server port
                bool isServerPort = false;
                if ((r == 0 && (c == 3 || c == 4)) || (r == 7 && (c == 3 || c == 4))) {
                    isServerPort = true;
                }
                
                if (cell.hasLink()) {
                    Link* link = cell.getLink();
                    int linkOwner = link->getOwnerId();
                    // If link belongs to opponent and is invisible, don't show it
                    if (linkOwner != viewingPlayer && !link->isVisible()) {
                        out_ << '.';
                    } else {
                        // Links on board are always alive (green)
                        out_ << GREEN() << link->getId() << RESET();
                    }
                } else if (isServerPort) {
                    out_ << 'S';
                } else if (cell.hasFirewall()) {
                    // Show firewall: 'm' for Player 1, 'w' for Player 2
                    FirewallInfo* fw = cell.getFirewall();
                    if (fw->ownerId == 1) {
                        out_ << ORANGE() << 'm' << RESET();
                    } else {
                        out_ << ORANGE() << 'w' << RESET();
                    }
                } else {
                    out_ << '.';
                }
            }
            out_ << "\n";
        }
        
        out_ << "========\n";

        // Print bottom player's info
        printPlayerInfo(bottomPlayer, viewingPlayer);
    }

    void printAbilities(int playerIdx) {
        auto& abilities = game_.getAbilities(playerIdx);
        out_ << ORANGE() << "Abilities for " << GREEN() << "Player " << playerIdx << RESET() << ":\n";
        for (int i = 0; i < (int)abilities.size(); ++i) {
            out_ << (i + 1) << ": " << abilities[i]->getName();
            if (abilities[i]->isUsed()) {
                out_ << " [USED]";
            }
            out_ << "\n";
        }
    }

    void printMessage(const string& msg) {
        out_ << msg << "\n";
    }

    void printWinner(int winnerId) {
        out_ << GREEN() << "Player " << winnerId << " wins!" << RESET() << "\n";
    }
};

