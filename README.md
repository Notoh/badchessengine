### badchessengine

uci compliant chess engine in c, initially with VICE as a base and inspired by Stockfish, Ethereal, and Weiss.

I haven't super extensively tested it and i'm nowhere near happy with it enough to the point I'm ready to submit to CCRL (even though I can make further versions after) or anything, but I'd estimate based on myself being an avid chess player that it is about 2000 elo in strength currently

if you want to learn how to make a chess engine, VICE by bluefever software is great for teaching you techniques

## UCI settings
currently BCE supports Hash - the size of the hash table in MiB (tested up to 1.5 GiB)



# Up next
quarantine is going on so i have lots of time to codernerd

finishing opening book

lots and lots of refactoring

tablebase support

time management

uh making the search way better with lmr & futility & lots more pruning

tapered eval

just making the eval non shitty (better terms and better tuning)

(maybe) full conversion to bitboards


