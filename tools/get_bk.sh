# get bk by height
if [ $# -ne 1 ]; then
  echo "Usage: $0 <bk no>" >> /dev/stderr
  exit 1
fi
BKCLI=$(which bitcoin-cli)
if [ -z "$BKCLI" ]; then
  echo "Cannot find 'bitcoin-cli'" >> /dev/stderr
  exit 1
fi
$BKCLI getblock $($BKCLI getblockhash "$1") 2 > $1.json
