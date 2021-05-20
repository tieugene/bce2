# bce2 - BlockChain Export 2

Exports BTC blockchain into plain text data.

## Description

Bce2 exports blockchain data into text ready for loading into SQL DB.  
Main goal is translating blockchain hashes (transactions, addresses) into their order numers using intermediate key-value storage.  
Output data can be loaded into PostgreSQL using [bcerq](https://github.com/tieugene/bcerq/).  

_Note: see [python-based version](https://github.com/tieugene/bcepy) to compare_

## Documentation

- [Installation](doc/Install.md)
- [Usage](doc/Usage.md) and companions ([locs-file](doc/MkLocs.md), [tuning](doc/Tuning.md))
- [Output](doc/Output.md) format

## License

This application is distributing under GPL v3.0 [LICENSE](LICENSE)
