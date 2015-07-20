#include "util.h"

size_t getHashSize(size_t s) {
     if (s < 8) return 7;
     if (s < 16) return 13;
     if (s < 32) return 31;
     if (s < 64) return 61;
     if (s < 128) return 127;
     if (s < 512) return 509;
     if (s < 2048) return 1499;
     if (s < 8192) return 4999;
     if (s < 32768) return 13999;
     if (s < 131072) return 59999;
     if (s < 524288) return 100019;
     if (s < 2097152) return 300007;
     if (s < 8388608) return 900001;
     if (s < 33554432) return 1000003;
     if (s < 134217728) return 3000017;
     if (s < 536870912) return 5000011;
     return 7000003;
}
