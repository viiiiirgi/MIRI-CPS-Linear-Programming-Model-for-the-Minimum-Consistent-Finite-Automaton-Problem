#!/bin/bash

SOLVER="./main"
INSTANCE_DIR="instances"
OUTPUT_DIR="out"
INSTANCE_EXTENSION=".inp"
TIMEOUT_SECONDS=60
CHECKER_SCRIPT="./checker"

if [ ! -f "$SOLVER" ]; then
    echo "Error: Solver executable '$SOLVER' not found."
    echo "Please compile your C++ code first (e.g., using 'make')."
    exit 1
elif [ ! -x "$SOLVER" ]; then
    echo "Error: Solver executable '$SOLVER' is not executable."
    echo "Please make it executable (e.g., using 'chmod +x $SOLVER')."
    exit 1
fi

if [ ! -d "$INSTANCE_DIR" ]; then
    echo "Error: Instances directory '$INSTANCE_DIR/' not found."
    echo "Please create it and place your instance files there."
    exit 1
fi

mkdir -p "$OUTPUT_DIR"
echo "Solver output will be saved in '$OUTPUT_DIR/'"
echo "---"

process_count=0
success_count=0
timeout_count=0
error_count=0

for infile in "$INSTANCE_DIR"/*"$INSTANCE_EXTENSION"; do
    if [ -f "$infile" ]; then
        process_count=$((process_count + 1))
        base=$(basename "$infile" "$INSTANCE_EXTENSION")
        outfile="$OUTPUT_DIR/${base}.out"

        echo "Processing '$infile' -> '$outfile'"

        timeout "$TIMEOUT_SECONDS"s "$SOLVER" < "$infile" > "$outfile"
        status=$?

        if [ $status -eq 0 ]; then
            echo "  Finished successfully for '$base$INSTANCE_EXTENSION'."
            if [ -s "$outfile" ]; then
                 success_count=$((success_count + 1))
            else
                echo "  WARNING: Solver exited successfully, but output file '$outfile' is empty."
                error_count=$((error_count + 1))
            fi
        elif [ $status -eq 124 ]; then
            echo "  TIMED OUT for '$base$INSTANCE_EXTENSION' after ${TIMEOUT_SECONDS}s."
            timeout_count=$((timeout_count + 1))
            rm -f "$outfile"
        else
            echo "  ERROR: Solver exited with status $status for '$base$INSTANCE_EXTENSION'."
            error_count=$((error_count + 1))
            rm -f "$outfile"
        fi
        echo "---"
    fi
done

echo "Finished processing $process_count instances."
echo "Solver Summary: Success=$success_count, Timeout=$timeout_count, Error=$error_count"
echo ""

if [ ! -f "$CHECKER_SCRIPT" ]; then
    echo "Warning: Checker script '$CHECKER_SCRIPT' not found. Skipping checks."
elif [ ! -x "$CHECKER_SCRIPT" ]; then
    echo "Warning: Checker script '$CHECKER_SCRIPT' is not executable. Skipping checks."
else
    echo "Starting checker script '$CHECKER_SCRIPT' on outputs in '$OUTPUT_DIR/'..."
    checked_count=0
    valid_count=0
    invalid_count=0

    for outfile_to_check in "$OUTPUT_DIR"/*.out; do
        if [ -f "$outfile_to_check" ]; then
            checked_count=$((checked_count + 1))
            base_out=$(basename "$outfile_to_check")
            echo "Checking '$outfile_to_check'..."

            if "$CHECKER_SCRIPT" < "$outfile_to_check"; then
                echo "  Check PASSED for '$base_out'."
                valid_count=$((valid_count + 1))
            else
                echo "  Check FAILED for '$base_out'."
                invalid_count=$((invalid_count + 1))
            fi
        fi
    done

    echo "Finished checking $checked_count output files."
    echo "Check Summary: Valid=$valid_count, Invalid=$invalid_count"
fi

if [ "$process_count" -eq 0 ]; then
    echo "No instance files with extension '$INSTANCE_EXTENSION' were found in '$INSTANCE_DIR/'."
fi

exit 0
