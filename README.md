# bce2 - BlockChain Export 2

Exports BTC blockchain into plain text data.

## Explanation

Bce2 parses blockchain's data files (blkXXXX.dat) *directly* and exports them into plain text format.  
Output data can be loaded into PostgreSQL further using [bcerq](https://github.com/tieugene/bcerq/).  
Key feature is enumerating blockchain objects (blocks, transactions, addresses) using interim key-value database.

_Note: see [python-based version](https://github.com/tieugene/bcepy) to compare_

## Documentation

- [Installation](doc/Install.md)
- [Usage](doc/Usage.md)
- [Output](doc/Output.md) format

## License

This application is distributing under GPL v3.0 [LICENSE](LICENSE)
