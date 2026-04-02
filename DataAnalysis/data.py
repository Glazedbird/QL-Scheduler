import pandas as pd
import yaml
import os
rows = []

with open("config.yaml") as f:
    config = yaml.safe_load(f)

output_path = "./data/data" + config["model"] + ".csv"

with open("./rawdata/log.txt") as f:
    for line in f:
        if line.startswith("SUMMARY"):
            p = line.strip().split(",")
            n = int(p[2])
            rows.append({
                "mode": int(p[1]),
                "n": int(p[2]),
                "avg_turnaround": int(p[3]) / n,
                "avg_response": int(p[4]) / n,
                "avg_wait": int(p[5]) / n,
                "avg_run": int(p[6]) / n,
                "avg_sleep": int(p[7]) / n,
                "avg_sched": int(p[8]) / n,
                "total_turnaround": int(p[9]),
                "total_response": int(p[10]),
                "total_wait": int(p[11]),
                "total_run": int(p[12]),
                "total_sleep": int(p[13]),
                "total_sched": int(p[14]),
            })

df = pd.DataFrame(rows)
df.to_csv(
    output_path,
    mode='a',
    header=not os.path.exists(output_path),
    index=False
)