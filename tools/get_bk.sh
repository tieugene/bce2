#!/bin/sh
# get bk by height
bitcoin-cli getblock `bitcoin-cli getblockhash $1` 2 > $1.json