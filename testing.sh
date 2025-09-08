#!/bin/bash

mkdir -p logs
start_time=$(date +%s%3N)

for f in tests/files/*.txt; do
    echo "Starting client for file: $f"
    
    # Read the file content into a variable
    payload=$(<"$f")

    # Send job type, filename, payload to client non-interactively
    {
        echo "count-words"
        echo "$(basename "$f")"
        echo "$payload"
    } | ./output/client >> "logs/$(basename "$f").log" 2>&1 &
done

wait

end_time=$(date +%s%3N)
echo "All clients finished."
echo "Total time (ms): $((end_time - start_time))"
