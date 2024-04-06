#include "hookapi.h"

#define BYTES20_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint64_t*)(buf + 16) = *(uint64_t*)(i + 16);\
    *(uint8_t*)(buf + 4) = *(uint8_t*)(i + 4);\
}

uint8_t txn[238] =
{
/* size,upto */
/* 3,  0, tt = Payment           */   0x12U, 0x00U, 0x00U,
/* 5,  3, flags                  */   0x22U, 0x00U, 0x00U, 0x00U, 0x00U,
/* 5,  8, sequence               */   0x24U, 0x00U, 0x00U, 0x00U, 0x00U,
/* 6,  13, firstledgersequence   */   0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,
/* 6,  19, lastledgersequence    */   0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,
/* 9,  25, amount                */   0x61U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
/* 9,  34, fee                   */   0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
/* 35,  43, signingpubkey        */   0x73U, 0x21U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 22,  78, account              */   0x81U, 0x14U, 0xCBU, 0xE6U, 0x27U, 0x1BU, 0x0BU, 0x90U, 0x61U, 0x5BU, 0xD3U, 0xF4U, 0x03U, 0x0CU, 0xD1U, 0x1AU, 0x09U, 0x32U, 0xC6U, 0xC4U, 0x6BU, 0x6AU,
/* 22,  100, destination         */   0x83U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 116, 122  emit details        */ 
/* 0,   238                      */ 
};

#define FLS_OUT    (txn + 15U) 
#define LLS_OUT    (txn + 21U) 
#define FEE_OUT    (txn + 35U) 
#define AMOUNT_OUT (txn + 26U)
#define DEST_OUT   (txn + 102U) 
#define EMIT_OUT   (txn + 122U) 

int64_t hook(uint32_t) {

    if(otxn_type() != ttINVOKE)
        rollback(SBUF("Voucher Claim: Set only on INVOKE Transaction."), __LINE__);

    uint8_t blob[117];
    if(otxn_field(blob - 1, 118, sfBlob) != 118)
        rollback(SBUF("Voucher Claim: Blob must be 64 + 33 + 20 = 117 bytes."), __LINE__);

   BYTES20_TO_BUF(DEST_OUT, blob + 97);

   if(util_verify(blob + 64, 53, blob, 64, blob + 64, 33) != 1)
        rollback(SBUF("Voucher Claim: Not Authorized"), __LINE__);        
  
   if(state(AMOUNT_OUT, 8, blob + 65, 32) != 8)
       rollback(SBUF("Voucher Claim: Voucher claimed or not present."), __LINE__);

   etxn_reserve(1);
    
   uint32_t fls = (uint32_t)ledger_seq() + 1;
   *((uint32_t *)(FLS_OUT)) = FLIP_ENDIAN(fls);

   // TXN PREPARE: LastLedgerSequense
   uint32_t lls = fls + 4;
   *((uint32_t *)(LLS_OUT)) = FLIP_ENDIAN(lls);

   etxn_details(EMIT_OUT, 116U);

   // TXN PREPARE: Fee
   {
       int64_t fee = etxn_fee_base(SBUF(txn));
       uint8_t *b = FEE_OUT;
       *b++ = 0b01000000 + ((fee >> 56) & 0b00111111);
       *b++ = (fee >> 48) & 0xFFU;
       *b++ = (fee >> 40) & 0xFFU;
       *b++ = (fee >> 32) & 0xFFU;
       *b++ = (fee >> 24) & 0xFFU;
       *b++ = (fee >> 16) & 0xFFU;
       *b++ = (fee >> 8) & 0xFFU;
       *b++ = (fee >> 0) & 0xFFU;
   }

   uint8_t emithash[32]; 
   if(emit(SBUF(emithash), SBUF(txn)) < 0 )
       rollback(SBUF("Voucher Claim: Failed To Emit."), 1);

   state_set(0, 0, blob + 65, 32);
   accept(SBUF("Voucher Claim: Successful."), __LINE__);

  _g(1,1);
  return 0;
}