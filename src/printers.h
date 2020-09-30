#ifndef PRINTERS_H
#define PRINTERS_H

// results
void        out_bk(void);
void        out_tx(void);
void        out_vin(void);
void        out_vout(void);
void        out_addr(uint32_t const, uint160_t const &);
void        out_xaddr(uint32_t const);
// debug
void        __prn_bk(void);
void        __prn_tx(void);
void        __prn_vin(void);
void        __prn_vout(void);
void        __prn_addr(const string&, const string&);

void        __prn_file(const string&);
void        __prn_head(void);
void        __prn_tail(void);
void        __prn_interim(void);
void        __prn_summary(void);
bool        __prn_trace(void);

#endif // PRINTERS_H
