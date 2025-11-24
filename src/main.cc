import <iostream>;

int main(int argc, char *argv[]) {
  bool useGraphics = true;
  int numPlayers = 2;

  // parse -text, abilities etc. here
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-text")
      useGraphics = false;
    // parse -ability1, -ability2, -seed, etc.
  }

  Game game{numPlayers};

  TextDisplay textDisplay{game};
  game.attach(&textDisplay);

  std::unique_ptr<GraphicsDisplay> graphicsDisplay;
  if (useGraphics) {
    graphicsDisplay = std::make_unique<GraphicsDisplay>(game);
    game.attach(graphicsDisplay.get());
  }

  Controller controller{game, std::cin, std::cout};
  controller.run();
}