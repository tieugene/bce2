#include "misc.h"

#include <stdio.h>
#include <openssl/sha.h>

uint32_t    read_v(void)   ///<read 1..4-byte int and forward;
{
    auto retvalue = static_cast<uint32_t>(*CUR_PTR.u8_ptr++);
    if ((retvalue & 0xFC) == 0xFC) {
        switch (retvalue & 0x03) {
            case 0: // 0xFC
                break;
            case 1: // 0xFD
                retvalue = static_cast<uint32_t>(*CUR_PTR.u16_ptr++);
                break;
            case 2: // 0xFE
                retvalue = *CUR_PTR.u32_ptr++;
                break;
            case 3: // 0xFF
                throw "Value too big";
        }
    }
    return retvalue;
}

uint32_t    read_32(void)  ///< Read 4-byte int and go forward
{
    return *CUR_PTR.u32_ptr++;
}

uint64_t    read_64(void)  ///< Read 8-byte int and go forward
{
    return *CUR_PTR.u64_ptr++;
}

uint256_t * read_256_ptr(void)
{
    return CUR_PTR.u256_ptr++;
}

uint8_t *   read_u8_ptr(uint32_t size)
{
    auto retvalue = CUR_PTR.u8_ptr;
    CUR_PTR.u8_ptr += size;
    return retvalue;
}

void    sha256(void *src, uint32_t size, uint256_t &dst)
{
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, src, size);
    SHA256_Final(dst.begin(), &context);
}

void    mk_hash(void *src, uint32_t size, uint256_t &dst)
{
    uint256_t tmp;
    sha256(src, size, tmp);
    sha256(&tmp, sizeof(uint256_t), dst);
}

string    hash2str(uint256_t &h)
{
    char tmp[257];
    tmp[256] = '\0';
    for (uint_fast8_t i=0; i < 32; i++)
        sprintf(tmp+(i<<1), "%02x", h[i]);
    return string(tmp);
}

void    out_vin(void)   // FIXME: compare w/ COINBASE_txid too
{
    if (CUR_VIN.vout != COINBASE_vout)  // skip coinbase
        cout << "i" << TAB << "..." << TAB << CUR_VIN.vout << endl;
}

void    out_vout(void)
{
    cout << "o" << TAB << CUR_TX.no << TAB << CUR_VOUT.no << TAB << CUR_VOUT.satoshi << endl;
}

void    out_tx(void)
{
    cout << "t" << TAB << CUR_BK.no << TAB << CUR_TX.no << TAB << endl;
}

void    out_bk(void)    ///< Output bk data for DB
{
    time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    cout << "b" << TAB << CUR_BK.no << TAB << "'" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << "'" << TAB << hash2str(CUR_BK.hash) << endl;
}

void    __prn_vin(void)
{
    cerr << "\t\tVin:" << endl
         << "\t\t\tVout_n:\t" << CUR_VIN.vout << endl
         << "\t\t\tSSize:\t" << CUR_VIN.ssize << endl
         << "\t\t\tSeq:\t" << CUR_VIN.seq << endl
         ;
}

void    __prn_vout(void)
{
    cerr << "\t\tVout:" << endl
         << "\t\t\t$:\t" << CUR_VOUT.satoshi << endl
         << "\t\t\tSSize:\t" << CUR_VOUT.ssize << endl
         ;
}

void    __prn_tx(void)
{
    cerr << "\tTx:" << endl
         << "\t\tVer:\t" << CUR_TX.ver << endl
         << "\t\tvins:\t" << CUR_TX.vins << endl
         << "\t\tvouts:\t" << CUR_TX.vouts << endl
         << "\t\tlock:\t" << CUR_TX.locktime << endl
         ;
}

void    __prn_bk(void)
{
    time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    cerr    << "Block: " << CUR_BK.no << endl
            << "\tSize:\t" << CUR_BK.head_ptr->size << endl
            << "\tVer:\t" << CUR_BK.head_ptr->ver << endl
            << "\tTime:\t" << CUR_BK.head_ptr->time << " ("
            << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << ")" << endl
            << "\tTxs:\t" << CUR_BK.txs << endl
            ;
}
