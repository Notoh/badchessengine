### badchessengine

uci compliant chess engine in c, initially with VICE as a base and inspired by Stockfish, Ethereal, and Weiss.

I haven't super extensively tested it and i'm nowhere near happy with it enough to the point I'm ready to submit to CCRL (even though I can make further versions after) or anything, but I'd estimate based on myself being an avid chess player that it is about 2000 elo in strength currently that it's ~1800 elo

if you want to learn how to make a chess engine, VICE by bluefever software is great for teaching you techniques

## UCI settings
currently BCE supports Hash - the size of the hash table in MiB (tested up to 1.5 GiB)



# Up next
in summer i'll start a lot of work on this, trying to get it to 2200

lots and lots of refactoring

tapered eval

uh making the search way better with lmr & futility & lots more pruning

just making the eval non shitty (better terms and better tuning)

tablebase support

time management

conversion to bitboards


