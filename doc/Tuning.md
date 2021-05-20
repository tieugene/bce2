# Tuning

K-V tuning with '*-t* _num_' can dramatically increase its perfomance and depends on k-v engine used.

## kc\*, tk\*

For these engines _num_ set log<sub>2</sub>() of reserved records for _creating_ k-v files (so '*-t*' not affects on already created k-v files).  
Best result is achiving when `reserverd` &sime; `stored`: too many `reserved` increase storage size and slow down starting job; too few `reserved` stay bottleneck when `reserverd` &Lt; `stored`.  
Default value is 10 (2<sup>10</sup> = 1M records) - enough for first 100&hellip;200k blocks but no more.  
With 680k blocks _num_ = 29&hellip;30 will be optimal (0.5&hellip;1G records).

In case of too small `reserved` in already created DB rebuilding these DBs is welcomed.

## bdb

BerkeleyDB perfomnce depends on exactly cache size, _num_ means *GB* RAM dedicated as cache. Default cache size is 256KB.  
It is possible to set 90% of free RAM, BDB eat as much cache as it requires and no more (but no less).  
Unlike engines above this not affects on DB structure.
