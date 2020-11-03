# TODO
## 201029:
Test [bech32 encoding](https://slowli.github.io/bech32-buffer/)
Comparing bcepy vs bce2.
A: 96 (48 utems):
```
-a 295234989 "bc1q34aq5drpuwy3wgl9lhup9892qp6svr8ldzyy7c" 1
+a 295234989 "bc1qq2xh5z35v83cj9eruh7lsy5u4gq82psv8h3qw3" 1
-a 295244991 "bc1qpkuy609cpcl7dpvrgkpavgtdqumtcynxp33z0h" 1
+a 295244991 "bc1qqgxmsnfuhq8rle59sdzc843pd5rnd0qjra45p7" 1
-a 295254920 "bc1q47fes6vk5l2klxgvl80q6neymj4vwvp5tlgdzq" 1
+a 295254920 "bc1qq2he8xrfj6na2muepnuaur20ynw243esth766y" 1
```

data (src, 481824.):
```
t 249036899 481824 dfcec48bb8491856c353306ab5febeb7e99e4d783eedf3de98f3ee0812b92bad
a 295234989 "bc1q34aq5drpuwy3wgl9lhup9892qp6svr8ldzyy7c" 1
o 249036899 0 194300 295234989
```

480-490:

44061219: *
7: nonstandard$
421134: nulldata$
----
421141: -e nulldata$ -e nonstandard$
43640078: 