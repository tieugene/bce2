# Get block hashes
# Input: nothing (bitcoind must be running)
# Output: block hashes (stdout)
# Time:

BKFROM=0
BKTO=""
BKCLI=$(which bitcoin-cli)
verbose=""

message() {
  echo "$1" >> /dev/stderr
}

debug() {
  if [ -n "$verbose" ]; then
    message "$1"
  fi
}

help() {
  message "Usage: $0 [-v] [-f <bkno>] [-t <bkno>]
  -v - verbose
  -f - block start from (default=0)
  -t - block run to (default=max)"
}

get_bk_max() {
  BKTO=$($BKCLI getblockcount)
}

if [ -z "$BKCLI" ]; then
  message "Cannot find 'bitcoin-cli'"
  exit 1
fi
while getopts vhf:t: opt
do
  case "${opt}" in
    v) verbose=1;;
    f) BKFROM=${OPTARG};;
    t) BKTO=${OPTARG};;
    h) help && exit;;
    *) help && exit 1;;
  esac
done
if [ -z "$BKTO" ]; then
  get_bk_max
fi
if [ -z "$BKTO" ]; then
  message "Cannot get max block"
  exit 1
fi
if [ "$BKFROM" -gt "$BKTO" ]; then
  message "'from' > 'to'"
  exit 1
fi
debug "Get bk hashes from $BKFROM to $BKTO"
for i in $(seq "$BKFROM" "$BKTO"); do
  if [[ "$i" == *000 ]]; then
    message "$i"
  fi
  # echo "$i"
  $BKCLI getblockhash "$i"
done
