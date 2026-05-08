#include "kernel/types.h"
#include "user/user.h"

void
spawn(char *prog)
{
    int pid = fork();

    if(pid < 0){
        printf("fork failed\n");
        return;
    }

    if(pid == 0){
        char *argv[] = { prog, 0 };
        exec(prog, argv);
        printf("exec %s failed\n", prog);
        exit(1);
    }
}

void exec_once(int mode) {
    int n = 0;
    uint64 total_turnaround = 0;
    uint64 total_response = 0;
    uint64 total_wait = 0;
    uint64 total_run = 0;
    uint64 total_sleep = 0;
    uint64 total_sched = 0;

    struct pstat st;
    int status;

    printf("WORKLOAD START mode=%d\n", mode);

    if(mode == 0){
        spawn("cpu_long");
        spawn("cpu_long");
        spawn("cpu_long");
        spawn("cpu_long");
    } else if(mode == 1){
        for(int i = 0; i < 8; i++)
            spawn("cpu_short");
    } else if(mode == 2){
        spawn("cpu_long");
        spawn("cpu_long");
        for(int i = 0; i < 6; i++)
            spawn("cpu_short");
    } else if(mode == 3){
        spawn("cpu_long");
        for(int i = 0; i < 4; i++)
            spawn("sleepy_job");
    } else {
        printf("invalid mode\n");
        exit(1);
    }

    int pid;
    while((pid = waitstat(&status, &st)) != -1){
        uint64 turnaround = 0;
        uint64 response = 0;

        if(st.etime >= st.ctime)
            turnaround = st.etime - st.ctime;

        if(st.first_run_time >= st.ctime)
            response = st.first_run_time - st.ctime;
        else
            response = 0;

        total_turnaround += turnaround;
        total_response += response;
        total_wait += st.wait_ticks;
        total_run += st.run_ticks;
        total_sleep += st.sleep_ticks;
        total_sched += st.sched_count;
        n++;

        // printf("STAT mode=%d pid=%d status=%d "
        //        "ctime=%ld first=%ld etime=%ld "
        //        "run=%ld wait=%ld sleep=%ld sched=%ld "
        //        "turnaround=%ld response=%ld\n",
        //        mode,
        //        pid,
        //        status,
        //        st.ctime,
        //        st.first_run_time,
        //        st.etime,
        //        st.run_ticks,
        //        st.wait_ticks,
        //        st.sleep_ticks,
        //        st.sched_count,
        //        turnaround,
        //        response);

    }

    if(n > 0){
        // printf("SUMMARY mode=%d n=%d "
        //        "avg_turnaround=%ld avg_response=%ld avg_wait=%ld "
        //        "avg_run=%ld avg_sleep=%ld avg_sched=%ld "
        //        "total_turnaround=%ld total_response=%ld total_wait=%ld "
        //        "total_run=%ld total_sleep=%ld total_sched=%ld\n",
        //        mode,
        //        n,
        //        total_turnaround / n,
        //        total_response / n,
        //        total_wait / n,
        //        total_run / n,
        //        total_sleep / n,
        //        total_sched / n,
        //        total_turnaround,
        //        total_response,
        //        total_wait,
        //        total_run,
        //        total_sleep,
        //        total_sched);
        printf("SUMMARY,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
                mode,
                n,
                total_turnaround,
                total_response,
                total_wait,
                total_run,
                total_sleep,
                total_sched,
                total_turnaround,
                total_response,
                total_wait,
                total_run,
                total_sleep,
                total_sched);
    } else {
        printf("SUMMARY mode=%d n=0\n", mode);
    }
}

int
main(int argc, char *argv[])
{
    if(argc < 2){
        printf("usage: workload <mode>\n");
        exit(1);
    }

    int mode = atoi(argv[1]);
    int times = atoi(argv[2]);

    while(times --) {
        exec_once(mode);
    }

    printf("WORKLOAD END mode=%d\n", mode);
    exit(0);
}