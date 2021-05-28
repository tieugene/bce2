# Making locs-file

Direct processing blockchain requires to know block "locations" - what block in what blk*.dat at what position is stored.  
To achieve this a special utility prepares helping locs-file (block **loc**ation**s** file) from blockchain LevelDB database.

Let's make locs-file for **670k** blocks. Blockchain is stored in `$BTCDIR`.

1. run bitcoind:

   ```bash
   sudo systemctl start bitcoin
   ```

1. get block count (*to check how many blocks we have*):

   ```bash
   bitcoin-cli getblockcount
   ```

1. get 1-st 670k block hashes into `tmp.hex` (filename is not matter):

   ```bash
   (for i in `seq 0 669999`; do bitcoin-cli getblockhash $i; done) > tmp.hex
   ```

1. stop bitcoind:

   ```bash
   sudo systemctl stop bitcoin
   ```

1. fix blockchain permissions:

   ```bash
   sudo chmod -R a+rX $BTCDIR
   ```

1. copy LevelDB folder into somewhere (e.g. here):

   ```bash
   cp $BTSDIR/blocks/index .
   ```

1. make locs-file `bk.locs.670k.bin` (filename is not matter):

   ```bash
   btcbklocs -i tmp.hex index bk.locs.670k.bin
   ```

   or (requires python3-plyvel):

   ```bash
   tools/btcbklocs.py -i tmp.hex index bk.locs.670k.bin
   ```

*Note: steps 1&hellip;4 can be done on any host having bitcoin-cli; steps 5&hellip;7 must be done _exactly_ from those \$BTCDIR that bce2 will use.*
