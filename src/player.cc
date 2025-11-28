export module player;

import <string>;
import <vector>;
import <memory>;

import link;                 // Link class
import util.linkType;        // if needed for construction

using std::string, std::vector, std::unique_ptr, std::move;

export class Player {
    int id_;                             // index of the player (0..numPlayers-1)
    vector<Link> links_;                 // owns its 8 links
    int downloadedData_ = 0;             // # of data downloaded
    int downloadedViruses_ = 0;          // # of viruses downloaded

public:
    // ---- constructors ----
    
    Player() : id_{-1} {}  // default constructor for array initialization

    Player(int id, vector<Link> initialLinks)
        : id_{id}
        , links_{move(initialLinks)}
    {}

    // ---- identity ----
    int getId() { return id_; }

    // ---- links ----
    vector<Link>& getLinks() { return links_; }

    Link* getLinkById(char ch) {
        for (auto &ln : links_) {
            if (ln.getId() == ch) return &ln;
        }
        return nullptr;
    }

    // ---- download tracking ----
    int getDownloadedData() { return downloadedData_; }
    int getDownloadedViruses() { return downloadedViruses_; }

    void incrementDownloadedData() { ++downloadedData_; }
    void incrementDownloadedViruses() { ++downloadedViruses_; }

    // ---- victory status ----
    int victoryStatus() {
        if (downloadedData_ >= 4 && downloadedViruses_ < 4) return 1;
        if (downloadedViruses_ >= 4 && downloadedData_ < 4) return 0;
        if (downloadedData_ >= 4 && downloadedViruses_ >= 4) return -1; // if this happens, this should be an error
        return 0; // still playing
    }
};