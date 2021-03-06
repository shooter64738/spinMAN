#define BitSet_(arg,posn) ((arg) |= (1UL << (posn)))
#define BitClr_(arg,posn) ((arg) &= ~(1UL << (posn)))
#define BitTst(arg,posn) bool((arg) & (1UL << (posn)))
#define BitLong(n) (1UL << (n))
#define BitGet(p,m) bool((p) & (1UL << (m)))