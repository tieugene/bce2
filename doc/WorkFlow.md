# WorkFlow

Всё будет не так.
cron.hourly (bce2) + daily sql update (if day changed)

## 1. Check state
- [ ] bitcoind:
   - [ ] working:
      - `status = os.system('systemctl is-active --quiet service-name')`
      - [pystemd](https://github.com/facebookincubator/pystemd)
   - [ ] progress:
      ```
      systemctl status bitcon | \
      grep progres | \
      tail -n 1 \
      gwk '{print $10,$15}'
      ```
   - [ ] blockcount:
      ```
      bitcoin-cli getblockcount
      ```
- [ ] postgres:
   - [ ] working
   - [ ] blockcount
- [ ] bce2

## 2. Update main DB
## 3. Update working DB
## 4. Make queries
