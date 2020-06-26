### badchessengine

uci compliant chess engine in c, initially with VICE as a base and inspired by Stockfish, Ethereal, and Weiss.

I haven't super extensively tested it  but I'd estimate based on myself being an avid chess player + basic testing that it's ~1900-2000 elo

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


