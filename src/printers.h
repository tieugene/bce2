#ifndef PRINTERS_H
#define PRINTERS_H

// results
void        out_vin(void);
void        out_vout(void);
void        out_addr(uint32_t, uint160_t &);
void        out_xaddr(uint32_t);
void        out_tx(void);
void        out_bk(void);
// debug
void        __prn_vin(void);
void        __prn_vout(void);
void        __prn_addr(void);
void        __prn_tx(void);
void        __prn_bk(void);
void        __prn_file(string &);
void        __prn_summary(void);
bool        __prn_trace(void);

#endif // PRINTERS_H
