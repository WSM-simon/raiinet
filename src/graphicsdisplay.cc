module;
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>

export module graphicsdisplay;

import observer;
import game;
import board;
import player;
import link;
import cell;
import util.firewallInfo;
import util.linkType;

using std::string;

export class GraphicsDisplay : public Observer {
    Game& game_;
    Display* display_;
    Window window_;
    GC gc_;
    int screen_;
    int width_ = 800;
    int height_ = 800;
    bool initialized_ = false;

    // Colors (will be allocated from X11)
    unsigned long black_;
    unsigned long white_;
    unsigned long p1Color_;      // Player 1 links (alive)
    unsigned long p1Dead_;       // Player 1 links (dead)
    unsigned long p2Color_;      // Player 2 links (alive)
    unsigned long p2Dead_;       // Player 2 links (dead)
    unsigned long firewallColorP1_; // Player 1 firewall color (purple)
    unsigned long firewallColorP2_; // Player 2 firewall color (orange)
    unsigned long serverPortColor_; // Server port color
    unsigned long dataColor_;    // Data link highlight
    unsigned long virusColor_;   // Virus link highlight
    unsigned long bgColor_;      // Background color
    unsigned long textColor_;    // Text color

    void initializeX11() {
        if (initialized_) return;
        
        display_ = XOpenDisplay(nullptr);
        if (!display_) {
            // Cannot open display - silently fail (will check initialized_ in notify)
            return;
        }

        screen_ = DefaultScreen(display_);
        window_ = XCreateSimpleWindow(display_,
                                     RootWindow(display_, screen_),
                                     0, 0, width_, height_,
                                     2,
                                     BlackPixel(display_, screen_),
                                     WhitePixel(display_, screen_));

        XSelectInput(display_, window_, ExposureMask | KeyPressMask);
        
        // Set window title
        XStoreName(display_, window_, "RAIInet Game");
        
        XMapWindow(display_, window_);
        XFlush(display_);

        gc_ = XCreateGC(display_, window_, 0, nullptr);
        XSetLineAttributes(display_, gc_, 2, LineSolid, CapRound, JoinRound);

        // Allocate colors
        Colormap colormap = DefaultColormap(display_, screen_);
        XColor color;

        // Black and white
        black_ = BlackPixel(display_, screen_);
        white_ = WhitePixel(display_, screen_);

        // Player 1 (blue) - alive links
        color.red = 0x0000;
        color.green = 0x4444;
        color.blue = 0xffff;
        XAllocColor(display_, colormap, &color);
        p1Color_ = color.pixel;

        // Player 1 (dark gray) - dead links
        color.red = 0x4444;
        color.green = 0x4444;
        color.blue = 0x4444;
        XAllocColor(display_, colormap, &color);
        p1Dead_ = color.pixel;

        // Player 2 (red) - alive links
        color.red = 0xffff;
        color.green = 0x0000;
        color.blue = 0x0000;
        XAllocColor(display_, colormap, &color);
        p2Color_ = color.pixel;

        // Player 2 (dark gray) - dead links
        color.red = 0x4444;
        color.green = 0x4444;
        color.blue = 0x4444;
        XAllocColor(display_, colormap, &color);
        p2Dead_ = color.pixel;

        // Player 1 Firewall (purple)
        color.red = 0x8888;
        color.green = 0x0000;
        color.blue = 0xffff;
        XAllocColor(display_, colormap, &color);
        firewallColorP1_ = color.pixel;

        // Player 2 Firewall (orange)
        color.red = 0xffff;
        color.green = 0x8888;
        color.blue = 0x0000;
        XAllocColor(display_, colormap, &color);
        firewallColorP2_ = color.pixel;

        // Server port (yellow)
        color.red = 0xffff;
        color.green = 0xffff;
        color.blue = 0x0000;
        XAllocColor(display_, colormap, &color);
        serverPortColor_ = color.pixel;

        // Data (green)
        color.red = 0x0000;
        color.green = 0xffff;
        color.blue = 0x0000;
        XAllocColor(display_, colormap, &color);
        dataColor_ = color.pixel;

        // Virus (red)
        color.red = 0xffff;
        color.green = 0x0000;
        color.blue = 0x0000;
        XAllocColor(display_, colormap, &color);
        virusColor_ = color.pixel;

        // Background (light gray)
        color.red = 0xdddd;
        color.green = 0xdddd;
        color.blue = 0xdddd;
        XAllocColor(display_, colormap, &color);
        bgColor_ = color.pixel;

        // Text (black)
        textColor_ = black_;

        initialized_ = true;
    }

    void cleanupX11() {
        if (initialized_) {
            if (gc_) XFreeGC(display_, gc_);
            if (window_) XDestroyWindow(display_, window_);
            if (display_) XCloseDisplay(display_);
            initialized_ = false;
        }
    }

    // Draw text at position (x, y)
    void drawText(int x, int y, const string& text, unsigned long color) {
        XSetForeground(display_, gc_, color);
        XDrawString(display_, window_, gc_, x, y, text.c_str(), text.length());
    }

    // Draw a filled rectangle
    void drawRect(int x, int y, int w, int h, unsigned long fillColor, unsigned long borderColor) {
        XSetForeground(display_, gc_, fillColor);
        XFillRectangle(display_, window_, gc_, x, y, w, h);
        XSetForeground(display_, gc_, borderColor);
        XDrawRectangle(display_, window_, gc_, x, y, w, h);
    }

    // Get color for a link based on owner and alive/dead status
    unsigned long getLinkColor(Link* link) {
        if (!link) return white_;
        
        int owner = link->getOwnerId();
        bool isAlive = link->isOnBoard();
        
        if (!isAlive) {
            // Dead links - gray
            return (owner == 1) ? p1Dead_ : p2Dead_;
        }
        
        // Alive links - use owner color (blue for P1, red for P2)
        // Type (Data/Virus) is indicated by the link ID display
        return (owner == 1) ? p1Color_ : p2Color_;
    }

public:
    GraphicsDisplay(Game& game) : game_{game}, display_{nullptr}, window_{0}, gc_{nullptr} {
        initializeX11();
    }

    ~GraphicsDisplay() {
        cleanupX11();
    }

    void notify() override {
        if (!initialized_) return;
        handleEvents();  // Process any pending events first
        draw();
        XFlush(display_);
    }

    void draw() {
        if (!initialized_) return;

        Board& board = game_.getBoard();
        
        // Clear window
        XSetForeground(display_, gc_, bgColor_);
        XFillRectangle(display_, window_, gc_, 0, 0, width_, height_);

        // Board cell size
        int cellSize = 60;
        int boardStartX = 250;
        int boardStartY = 150;
        
        // Draw Player 1 info at top
        Player& p1 = game_.getPlayer(1);
        drawPlayerInfo(1, p1, 50, 220);
        
        // Draw board (8x8 grid)
        // Player 1 is at top (row 0), Player 2 at bottom (row 7)
        // NO FLIPPING - always show row 0 at top
        
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                int x = boardStartX + c * cellSize;
                int y = boardStartY + r * cellSize;
                
                Cell& cell = board.getCell(r, c);
                
                // Check if server port
                bool isServerPort = ((r == 0 && (c == 3 || c == 4)) || 
                                     (r == 7 && (c == 3 || c == 4)));
                
                unsigned long cellColor = bgColor_;
                unsigned long borderColor = black_;
                
                if (isServerPort) {
                    cellColor = serverPortColor_;
                } else if (cell.hasFirewall()) {
                    // Get firewall owner to determine color
                    FirewallInfo* fw = cell.getFirewall();
                    if (fw && fw->ownerId == 1) {
                        cellColor = firewallColorP1_;  // Purple for Player 1
                    } else {
                        cellColor = firewallColorP2_;  // Orange for Player 2
                    }
                }
                
                // Draw cell background
                drawRect(x, y, cellSize, cellSize, cellColor, borderColor);
                
                // Draw link if present
                if (cell.hasLink()) {
                    Link* link = cell.getLink();
                    int linkOwner = link->getOwnerId();
                    int viewingPlayer = game_.getCurrentPlayerIndex();
                    
                    // If link belongs to opponent and is invisible, don't show it
                    if (linkOwner != viewingPlayer && !link->isVisible()) {
                        // Don't draw the link - it's invisible to the opponent
                    } else {
                        unsigned long linkColor = getLinkColor(link);
                        
                        // Draw link as filled circle
                        XSetForeground(display_, gc_, linkColor);
                        XFillArc(display_, window_, gc_,
                                x + 10, y + 10, cellSize - 20, cellSize - 20,
                                0, 360 * 64);
                        
                        // Draw link ID character in white
                        string linkId(1, link->getId());
                        XSetForeground(display_, gc_, white_);
                        int textX = x + cellSize/2 - 4;
                        int textY = y + cellSize/2 + 5;
                        XDrawString(display_, window_, gc_, textX, textY, linkId.c_str(), 1);
                    }
                }
            }
        }
        
        // Draw Player 2 info at bottom
        Player& p2 = game_.getPlayer(2);
        drawPlayerInfo(2, p2, 50, boardStartY + 8 * cellSize - 170);
        
        // Draw current player indicator
        int currentPlayer = game_.getCurrentPlayerIndex();
        string currentText = "Current Player: " + std::to_string(currentPlayer);
        drawText(450, 85, currentText, textColor_);
    }

    void drawPlayerInfo(int playerId, Player& player, int x, int y) {
        string playerText = "Player " + std::to_string(playerId) + ":";
        drawText(x, y, playerText, textColor_);
        
        // Downloaded info
        string downloadText = "Downloaded: " + 
                             std::to_string(player.getDownloadedData()) + "D, " +
                             std::to_string(player.getDownloadedViruses()) + "V";
        drawText(x, y + 20, downloadText, textColor_);
        
        // Abilities count
        int unusedAbilities = 0;
        auto& abilities = game_.getAbilities(playerId);
        for (auto& ab : abilities) {
            if (!ab->isUsed()) ++unusedAbilities;
        }
        string abilitiesText = "Abilities: " + std::to_string(unusedAbilities);
        drawText(x, y + 40, abilitiesText, textColor_);
        
        // Links info
        y += 60;
        std::vector<Link>& links = player.getLinks();
        string linksLine1 = "";
        string linksLine2 = "";
        
        for (int i = 0; i < 4 && i < (int)links.size(); ++i) {
            // If viewing opponent's link and it's invisible, skip it
            int currentPlayer = game_.getCurrentPlayerIndex();
            if (playerId != currentPlayer && !links[i].isVisible()) {
                continue;
            }
            
            if (!linksLine1.empty()) linksLine1 += " ";
            char id = links[i].getId();
            string info = "?";
            
            if (playerId == currentPlayer || links[i].isRevealedToOpponent()) {
                char type = links[i].isData() ? 'D' : 'V';
                info = string(1, type) + std::to_string(links[i].getStrength());
            }
            
            linksLine1 += string(1, id) + ":" + info;
        }
        
        for (int i = 4; i < 8 && i < (int)links.size(); ++i) {
            // If viewing opponent's link and it's invisible, skip it
            int currentPlayer = game_.getCurrentPlayerIndex();
            if (playerId != currentPlayer && !links[i].isVisible()) {
                continue;
            }
            
            if (!linksLine2.empty()) linksLine2 += " ";
            char id = links[i].getId();
            string info = "?";
            
            if (playerId == currentPlayer || links[i].isRevealedToOpponent()) {
                char type = links[i].isData() ? 'D' : 'V';
                info = string(1, type) + std::to_string(links[i].getStrength());
            }
            
            linksLine2 += string(1, id) + ":" + info;
        }
        
        if (!linksLine1.empty()) {
            drawText(x, y, linksLine1, textColor_);
        }
        if (!linksLine2.empty()) {
            drawText(x, y + 20, linksLine2, textColor_);
        }
    }

    void handleEvents() {
        if (!initialized_) return;
        
        XEvent event;
        while (XPending(display_)) {
            XNextEvent(display_, &event);
            if (event.type == Expose) {
                draw();
            }
            // Ignore other events for now
        }
    }

public:
    // Public method to handle events (can be called from main loop)
    void processEvents() {
        handleEvents();
    }
};

