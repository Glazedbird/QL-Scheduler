#define EPOSILON 10

#define ALPHA   100    // 0.1
#define GAMMA   900    // 0.9
#define SCALE   1000

#define NWAIT_BUCKET 3
#define NRUN_BUCKET  3
#define NSTATE       (NRUN_BUCKET * NWAIT_BUCKET)
#define NACTION 4