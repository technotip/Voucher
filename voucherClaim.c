#include "hookapi.h"

#define BYTES20_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint64_t*)(buf + 16) = *(uint64_t*)(i + 16);\
    *(uint8_t*)(buf + 4) = *(uint8_t*)(i + 4);\
}

#define BYTES32_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint64_t*)(buf + 16) = *(uint64_t*)(i + 16);\
    *(uint64_t*)(buf + 24) = *(uint64_t*)(i + 24);\
    *(uint64_t*)(buf + 32) = *(uint64_t*)(i + 32);\
}

uint8_t txn[260] =
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
/* 22,  78, account              */   0x81U, 0x14U, 0xA5U, 0xCAU, 0x0EU, 0xA2U, 0x57U, 0x4AU, 0xBEU, 0x2FU, 0x09U, 0x80U, 0xEAU, 0x71U, 0x6CU, 0xE7U, 0x97U, 0xA7U, 0xCEU, 0xD4U, 0xEDU, 0xB6U,
/* 22,  100, destination         */   0x83U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 138, 122  emit details        */ 
/* 0,   260                      */ 
};

#define FLS_OUT    (txn + 15U) 
#define LLS_OUT    (txn + 21U) 
#define FEE_OUT    (txn + 35U) 
#define AMOUNT_OUT (txn + 26U)
#define DEST_OUT   (txn + 102U) 
#define EMIT_OUT   (txn + 122U) 

int64_t cbak(uint32_t reserve) {
  uint8_t limit_ns[32]    = { 0xB0U, 0x44U, 0xC7U, 0xA1U, 0x64U, 0x7AU, 0xB9U, 0x06U, 0x5FU, 0x68U, 0x5DU, 0xAEU, 0x11U, 0xB8U, 0xE3U, 0xFCU, 0xDDU, 0x11U, 0x94U, 0xF8U, 0x8CU, 0x21U, 0x19U, 0x00U, 0x2BU, 0xD8U, 0x98U, 0xD2U, 0x91U, 0x3FU, 0x29U, 0xC0U };
  uint8_t hook_acc_id[20] = { 0xA5U, 0xCAU, 0x0EU, 0xA2U, 0x57U, 0x4AU, 0xBEU, 0x2FU, 0x09U, 0x80U, 0xEAU, 0x71U, 0x6CU, 0xE7U, 0x97U, 0xA7U, 0xCEU, 0xD4U, 0xEDU, 0xB6U };
  uint8_t txn_id[32];
  otxn_id(txn_id, 32, 0);
  int64_t meta_slot_no = meta_slot(0);
  if(slot_subfield(meta_slot_no, sfTransactionResult, 1) == 1) {
    int8_t result;
    if(slot(SVAR(result), 1) == 1) {

        typedef struct TempState {
            int64_t amount_xfl;
            int8_t pubKey[32];
        } InterState;

        InterState tempState;
        
      if(state(SVAR(tempState),SBUF(txn_id)) == 40) {
            if (result != 0 || reserve != 0) {
                state_set(SVAR(tempState.amount_xfl), SBUF(tempState.pubKey));
                    typedef struct myState {
                        int64_t  total_sent;
                        int64_t  total_received;
                        int64_t  sent;
                        int64_t  received;        
                        int32_t  ledger_create;
                        int32_t  ledger_claim;
                    } State;

                    State S;

                    int8_t dest_acc[20];
                    otxn_field(SBUF(dest_acc), sfDestination);

                    if(state_foreign(SVAR(S), dest_acc, 32, SBUF(limit_ns), SBUF(hook_acc_id)) == 40) {
                        int64_t amount_xfl = float_negate(tempState.amount_xfl);
                        S.total_received = float_sum(S.total_received, amount_xfl);   
                        S.received       = float_sum(S.received, amount_xfl);  
                        state_foreign_set(SVAR(S), dest_acc, 32, SBUF(limit_ns), SBUF(hook_acc_id));

                        typedef struct voucherTotal {
                            int64_t created;
                            int64_t claimed;
                        } VoucherTotal;
                        VoucherTotal Total;
                        uint8_t total_ns[32] = { 0x5DU, 0x8FU, 0xF9U, 0xC2U, 0x82U, 0xE7U, 0xD9U, 0x68U, 0xD4U, 0x40U, 0xBEU, 0x77U, 0x02U, 0xA5U, 0x5AU, 0x42U, 0x49U, 0x1EU, 0x3AU, 0x7EU, 0x56U, 0xDFU, 0xA2U, 0x06U, 0xB2U, 0xE6U, 0xEAU, 0xD7U, 0x90U, 0xBEU, 0xB7U, 0x10U }; //TOTAL
                        uint8_t tkey[32]     = { 0x54U, 0x4FU, 0x54U, 0x41U, 0x4CU }; // TOTAL
                        if(state_foreign(SVAR(Total), SBUF(tkey), SBUF(total_ns), SBUF(hook_acc_id)) == 16){
                            Total.claimed = float_sum(Total.claimed, amount_xfl);
                            state_foreign_set(SVAR(Total), SBUF(tkey), SBUF(total_ns), SBUF(hook_acc_id));
                        }
                    }    
            }
      }
    }
  }


  state_set(0, 0, txn_id, 32);
  accept(SBUF(reserve), __LINE__);
  return 0;
}

int64_t hook(uint32_t reserve) {

    // if(otxn_type() != ttINVOKE)
    //     rollback(SBUF("Voucher Claim: Set only on INVOKE Transaction."), __LINE__);

    int64_t current_ledger  =  ledger_seq();
    uint8_t hook_acc_id[20] = { 0xA5U, 0xCAU, 0x0EU, 0xA2U, 0x57U, 0x4AU, 0xBEU, 0x2FU, 0x09U, 0x80U, 0xEAU, 0x71U, 0x6CU, 0xE7U, 0x97U, 0xA7U, 0xCEU, 0xD4U, 0xEDU, 0xB6U };
    uint8_t config_ns[32]   = { 0xF4U, 0x34U, 0x37U, 0xFCU, 0xA1U, 0xD5U, 0xF3U, 0xD0U, 0x38U, 0x10U, 0x73U, 0xEDU, 0x3EU, 0xECU, 0x9AU, 0xE4U, 0x2BU, 0xF8U, 0x69U, 0x88U, 0x55U, 0x9EU, 0x98U, 0x00U, 0x97U, 0x95U, 0xA9U, 0x69U, 0x91U, 0x9CU, 0xBEU, 0xCAU };
    uint8_t limit_ns[32]    = { 0xB0U, 0x44U, 0xC7U, 0xA1U, 0x64U, 0x7AU, 0xB9U, 0x06U, 0x5FU, 0x68U, 0x5DU, 0xAEU, 0x11U, 0xB8U, 0xE3U, 0xFCU, 0xDDU, 0x11U, 0x94U, 0xF8U, 0x8CU, 0x21U, 0x19U, 0x00U, 0x2BU, 0xD8U, 0x98U, 0xD2U, 0x91U, 0x3FU, 0x29U, 0xC0U };
    uint8_t total_ns[32]    = { 0x5DU, 0x8FU, 0xF9U, 0xC2U, 0x82U, 0xE7U, 0xD9U, 0x68U, 0xD4U, 0x40U, 0xBEU, 0x77U, 0x02U, 0xA5U, 0x5AU, 0x42U, 0x49U, 0x1EU, 0x3AU, 0x7EU, 0x56U, 0xDFU, 0xA2U, 0x06U, 0xB2U, 0xE6U, 0xEAU, 0xD7U, 0x90U, 0xBEU, 0xB7U, 0x10U }; //TOTAL
    uint8_t key[32]         = { 0x4CU, 0x49U, 0x4DU, 0x49U, 0x54U }; // LIMIT    
    uint8_t tkey[32]        = { 0x54U, 0x4FU, 0x54U, 0x41U, 0x4CU }; // TOTAL

    typedef struct voucherTotal {
        int64_t created;
        int64_t claimed;
    } VoucherTotal;
    VoucherTotal Total;

    uint8_t blob[117];
    if(otxn_field(blob - 1, 118, sfBlob) != 118) {
      uint8_t wietse[20] = { 0xB6U, 0x75U, 0xB3U, 0xDEU, 0x01U, 0x0AU, 0x7CU, 0x7AU, 0x2DU, 0xF6U, 0x55U, 0xC5U, 0x4CU, 0x28U, 0x4AU, 0x21U, 0x13U, 0xFFU, 0xD7U, 0x6BU };
     
      uint8_t user[20];
      otxn_field(SBUF(user),sfAccount);
      if(!BUFFER_EQUAL_20(wietse, user)) {
        rollback(SBUF("Voucher Claim: Blob must be 64 + 33 + 20 = 117 bytes."), __LINE__);
      }

      typedef struct Voucher_Config {
          int8_t min[8];
          int8_t max[8];
          int8_t cap[8];
      } Config;

      Config c;
      uint8_t min[3]     = { 0x4DU, 0x49U, 0x4EU }; // MIN  2 XAH
      uint8_t max[3]     = { 0x4DU, 0x41U, 0x58U }; // MAX  500 XAH
      uint8_t cap[3]     = { 0x43U, 0x41U, 0x50U }; // CAP  5000 XAH
      uint8_t claim[3]   = { 0x43U, 0x4CU, 0x4DU }; // CLM 0 or 1
      uint8_t create[3]  = { 0x43U, 0x52U, 0x54U }; // CRT 0 or 1


      if(otxn_param(SBUF(c.min), SBUF(min)) != 8) {
          rollback(SBUF("Voucher: Couldn't Set Minimum Voucher Value."), 2);
      }

      if(otxn_param(SBUF(c.max), SBUF(max)) != 8) {
          rollback(SBUF("Voucher: Couldn't Set Maximum Voucher Value."), 4);
      }

      if(otxn_param(SBUF(c.cap), SBUF(cap)) != 8) {
          rollback(SBUF("Voucher: Couldn't Set CAP on Voucher."), 1);
      }

      if(state_foreign_set(SVAR(c), SBUF(key), SBUF(config_ns), SBUF(hook_acc_id)) != 24){
          rollback(SBUF("Voucher: Couldn't Set The Configurations."), 1);
      }

      if(state_foreign(SVAR(Total), SBUF(tkey), SBUF(total_ns), SBUF(hook_acc_id)) != 16){
          Total.created = 0;
          Total.claimed = 0;
          state_foreign_set(SVAR(Total), SBUF(tkey), SBUF(total_ns), SBUF(hook_acc_id));
      }

      accept(SBUF("Voucher: Config Success."), __LINE__);
    }
        
    BYTES20_TO_BUF(DEST_OUT, blob + 97);

    if(util_verify(blob + 64, 53, blob, 64, blob + 64, 33) != 1) {
      rollback(SBUF("Voucher Claim: Not Authorized"), __LINE__);   
    }
      

    typedef struct TempState {
      int64_t amount_xfl;
      int8_t pubKey[32];
    } InterState;

    InterState tempState;
    BYTES32_TO_BUF(tempState.pubKey, blob + 65);

    if(state(SVAR(tempState.amount_xfl), SBUF(tempState.pubKey)) != 8)
       rollback(SBUF("Voucher Claim: Voucher claimed or not present."), __LINE__);

    {
        uint64_t drops = float_int(tempState.amount_xfl, 6, 1); 
        uint8_t *b = AMOUNT_OUT; 
        *b++ = 0b01000000 + ((drops >> 56) & 0b00111111); 
        *b++ = (drops >> 48) & 0xFFU; 
        *b++ = (drops >> 40) & 0xFFU; 
        *b++ = (drops >> 32) & 0xFFU; 
        *b++ = (drops >> 24) & 0xFFU; 
        *b++ = (drops >> 16) & 0xFFU; 
        *b++ = (drops >> 8) & 0xFFU; 
        *b++ = (drops >> 0) & 0xFFU; 
    }


    typedef struct myState {
        int64_t  total_sent;
        int64_t  total_received;
        int64_t  sent;
        int64_t  received;        
        int32_t  ledger_create;
        int32_t  ledger_claim;
    } State;

    State S;

    typedef struct Voucher_Config {
        int64_t min;
        int64_t max;
        int64_t cap;
    } Config;

    Config c;

    if(state_foreign(SVAR(c), SBUF(key), SBUF(config_ns), SBUF(hook_acc_id)) != 24){
      rollback(SBUF("Voucher: Configuration Not Set."), 2);
    }


   if(state_foreign(SVAR(S), DEST_OUT, 32, SBUF(limit_ns), SBUF(hook_acc_id)) == 40) {
        if(current_ledger <= S.ledger_claim + 500000) {
            S.total_received = float_sum(S.total_received, tempState.amount_xfl);    
            int64_t total = float_sum(S.received, tempState.amount_xfl);
            if (float_compare(total, c.cap, COMPARE_GREATER) == 1){
                rollback(SBUF("You have exceeded the daily claim limit."), 5000);
            }
            S.received = total;
        } else {
            S.total_received = float_sum(S.total_received, tempState.amount_xfl);      
            S.received = tempState.amount_xfl;
            S.ledger_claim = current_ledger;
        }
    } else {
        S.received       = tempState.amount_xfl;
        S.total_received = tempState.amount_xfl;
        S.ledger_claim = current_ledger;
    }

    if(state_foreign(SVAR(Total), SBUF(tkey), SBUF(total_ns), SBUF(hook_acc_id)) == 16){
          Total.claimed = float_sum(Total.claimed, tempState.amount_xfl);
          state_foreign_set(SVAR(Total), SBUF(tkey), SBUF(total_ns), SBUF(hook_acc_id));
    }
    
    if(state_foreign_set(SVAR(S), DEST_OUT, 32, SBUF(limit_ns), SBUF(hook_acc_id)) != 40) {
        rollback(SBUF("Voucher Create: Failed at creating the user limit."), __LINE__);
    }    


   etxn_reserve(1);
    
   uint32_t fls = (uint32_t)current_ledger + 1;
   *((uint32_t *)(FLS_OUT)) = FLIP_ENDIAN(fls);

   // TXN PREPARE: LastLedgerSequense
   uint32_t lls = fls + 4;
   *((uint32_t *)(LLS_OUT)) = FLIP_ENDIAN(lls);

   etxn_details(EMIT_OUT, 138U);

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
   if(emit(SBUF(emithash), SBUF(txn)) != 32)
       rollback(SBUF("Voucher Claim: Failed To Emit."), 1);

   state_set(0, 0, SBUF(tempState.pubKey));
   state_set(SVAR(tempState), SBUF(emithash));

   accept(SBUF("Voucher Claim: Successful."), __LINE__);
  _g(1,1);
  return 0;
}