#!/usr/bin/env bash
set -euo pipefail

LOG=/tmp/dvfs_full.log
TXT=/tmp/dvfs_txt.log

sh runDVFS.sh | tee "$LOG" >/dev/null
strings "$LOG" > "$TXT"

echo "=== Clean Summary ==="
echo

# Print each function's Slack classification + DVFS decision + energy saved
for f in _Z12memory_boundP4Nodei _Z13compute_boundfi _Z13streaming_memPfi _Z11mixed_boundP4Nodefi; do
  echo "Function: $f"

  # Slack classification
  grep -a "^\[SlackPass\] $f" "$TXT" | tail -n 1

  # DVFS decision
  grep -a "^\[DVFSPass\] $f" "$TXT" | tail -n 1
    awk -v target="$f" '
    $0 ~ /^===== SlackEnergy Report =====/ {inrep=1; cur=""}
    inrep && $0 ~ /^Function[[:space:]]*:/ {cur=$0}
    inrep && cur ~ target && $0 ~ /^Energy saved/ {print $0; exit}
    inrep && $0 ~ /^================================/ {inrep=0}
  ' "$TXT"


  echo
done

echo "Runtime DVFS events:"
grep -a "^\[DVFS\]" "$TXT" || true
