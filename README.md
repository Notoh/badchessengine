### badchessengine

uci compliant chess engine in c, initially with VICE as a base and inspired by Stockfish, Ethereal, and Weiss.

[It has a CCRL 40/2 Blitz Rating of 2008.](https://ccrl.chessdom.com/ccrl/404/cgi/engine_details.cgi?match_length=30&print=Details&each_game=1&eng=BadChessEngine%200.4.4%2064-bit#BadChessEngine_0_4_4_64-bit) 

if you want to learn how to make a chess engine, VICE by bluefever software is great for teaching you techniques

## UCI settings
currently BCE supports Hash - the size of the hash table in MiB (tested up to 1.5 GiB)



# Up next

working a lot on this recently -

more refactoring

tapered eval

continuing to make the search way better with futility, SEE, probcut, etc.

just making the eval non shitty (better terms and better tuning)

tablebase support

time management

conversion to bitboards


