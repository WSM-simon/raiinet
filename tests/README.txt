RAIInet Test Cases
==================

Usage:
------
1. Start the game: ./raiinet
2. Use the sequence command: sequence tests/test_basic_movement.txt

Or run from command line with input redirection:
  ./raiinet < tests/test_basic_movement.txt

With extra features (colors, board flip):
  ./raiinet -enableExtraFeature < tests/test_basic_movement.txt

Test Files:
-----------
BASIC TESTS:
1.  test_basic_movement.txt       - Basic up/down movement for both players
2.  test_horizontal_movement.txt  - Left/right movement
3.  test_invalid_moves.txt        - Invalid move attempts (edge, own server port)

BATTLE TESTS:
4.  test_battle.txt               - Two links collide and battle
5.  test_strength_matters.txt     - Higher strength wins (V3 vs V1)
6.  test_defender_wins.txt        - Defender wins when stronger (V1 vs V4)
7.  test_reveal_on_battle.txt     - Links revealed after battle
8.  test_dead_link_display.txt    - Dead links shown differently

ABILITY TESTS:
9.  test_abilities.txt            - Using Scan and LinkBoost abilities
10. test_abilities_command.txt    - Show abilities list
11. test_linkboost.txt            - LinkBoost makes link move 2 squares
12. test_scan_ability.txt         - Scan reveals opponent's link
13. test_polarize.txt             - Polarize flips link type
14. test_download_ability.txt     - Download steals opponent's link
15. test_ability_already_used.txt - Used abilities cannot be reused
16. test_one_ability_per_turn.txt - Only 1 ability allowed per turn

FIREWALL TESTS:
17. test_firewall.txt             - Firewall traps opponent's virus
18. test_firewall_data_pass.txt   - Data passes through firewall (revealed only)
19. test_multiple_firewalls.txt   - Both players place firewalls

ESCAPE/DOWNLOAD TESTS:
20. test_escape.txt               - Link escapes off board edge
21. test_escape_proper.txt        - Clean escape without collision
22. test_boosted_escape.txt       - LinkBoost helps escape faster
23. test_server_port.txt          - Link moves onto server port
24. test_server_port_capture.txt  - Server port captures link

WIN CONDITION TESTS:
25. test_win_data.txt             - Win by escaping data links
26. test_cannot_move_own_link.txt - Cannot attack your own link

Default Link Setup (V1V2V3V4D1D2D3D4):
--------------------------------------
Player 1: a=V1, b=V2, c=V3, d=V4, e=D1, f=D2, g=D3, h=D4
Player 2: A=V1, B=V2, C=V3, D=V4, E=D1, F=D2, G=D3, H=D4

Starting Positions:
  Row 0: a b c S S f g h  (Player 1)
  Row 1: . . . d e . . .
  Row 6: . . . D E . . .
  Row 7: A B C S S F G H  (Player 2)

Default Abilities (LFDSP):
--------------------------
1 = LinkBoost (move 2 squares)
2 = Firewall (place trap at row col)
3 = Download (steal opponent's link)
4 = Scan (reveal opponent's link)
5 = Polarize (flip your link's type)

Notes:
------
- Lines starting with # are comments (ignored)
- Empty lines are skipped
- Use 'board' command to print current state
- Use 'abilities' command to show available abilities
- Use 'quit' to exit
