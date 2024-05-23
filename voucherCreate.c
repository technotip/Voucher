#include "hookapi.h"


int64_t hook(uint32_t) {

    // if(otxn_type() != ttPAYMENT)
    //     rollback(SBUF("Voucher Create: Set only on Payment Transaction."), __LINE__);

    uint8_t user_acc_id[20];
    otxn_field(SBUF(user_acc_id), sfAccount);

    uint8_t hook_acc_id[20] = { 0x0AU, 0x2DU, 0x33U, 0xB2U, 0xB1U, 0x6AU, 0x05U, 0x8DU, 0x91U, 0x75U, 0x84U, 0x63U, 0x55U, 0xDDU, 0x73U, 0x1FU, 0xA2U, 0xC9U, 0x19U, 0x41U };
    int8_t equal = 0; BUFFER_EQUAL(equal, hook_acc_id, user_acc_id, 20);

    if(equal)
        accept(SBUF("Voucher Create: Outgoing Transaction."), __LINE__);

    uint64_t current_ledger =  ledger_seq();
    uint8_t config_ns[32] = { 0xF4U, 0x34U, 0x37U, 0xFCU, 0xA1U, 0xD5U, 0xF3U, 0xD0U, 0x38U, 0x10U, 0x73U, 0xEDU, 0x3EU, 0xECU, 0x9AU, 0xE4U, 0x2BU, 0xF8U, 0x69U, 0x88U, 0x55U, 0x9EU, 0x98U, 0x00U, 0x97U, 0x95U, 0xA9U, 0x69U, 0x91U, 0x9CU, 0xBEU, 0xCAU };
    uint8_t limit_ns[32]  = { 0xB0U, 0x44U, 0xC7U, 0xA1U, 0x64U, 0x7AU, 0xB9U, 0x06U, 0x5FU, 0x68U, 0x5DU, 0xAEU, 0x11U, 0xB8U, 0xE3U, 0xFCU, 0xDDU, 0x11U, 0x94U, 0xF8U, 0x8CU, 0x21U, 0x19U, 0x00U, 0x2BU, 0xD8U, 0x98U, 0xD2U, 0x91U, 0x3FU, 0x29U, 0xC0U };
    uint8_t key[32]    = { 0x4CU, 0x49U, 0x4DU, 0x49U, 0x54U }; // LIMIT

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

    uint8_t amount[8];
    if(otxn_field(SBUF(amount), sfAmount) != 8)
        rollback(SBUF("Voucher Create: Non XAH Transaction not allowed."), __LINE__);

   uint64_t otxn_drops = AMOUNT_TO_DROPS(amount);
   int64_t amount_xfl = float_set(-6, otxn_drops);

   if(float_compare(amount_xfl, c.min, COMPARE_LESS) == 1)
       rollback(SBUF("Voucher Create: Less than allowed minimum."), 2);

   if(float_compare(amount_xfl, c.max, COMPARE_GREATER) == 1)
       rollback(SBUF("Voucher Create: Greater than allowed maximum."), 500);       


    uint8_t pname[2] = { 0x56U, 0x43U };
    uint8_t state_key[32];
    if(otxn_param(state_key - 1, 33, pname, 2) != 33)
        rollback(SBUF("Voucher Create: Please pass valid account id."), __LINE__);

    uint8_t vault[8];
    if(state(SBUF(vault), SBUF(state_key)) == 8)
        rollback(SBUF("Voucher Create: Voucher already present."), __LINE__);


    if(state_foreign(SVAR(S), user_acc_id, 32, SBUF(limit_ns), SBUF(hook_acc_id)) == 40) {
        if(current_ledger <= S.ledger_create + 500000) {
            S.total_sent = float_sum(S.total_sent, amount_xfl);            
            int64_t total = float_sum(S.sent, amount_xfl);
            if (float_compare(total, c.cap, COMPARE_GREATER) == 1){
                rollback(SBUF("Try sending less XAH. You are exceeding the monthly limit."), 5000);
            }
            S.sent = total;
        } else {
            S.total_sent = float_sum(S.total_sent, amount_xfl);      
            S.sent = amount_xfl;
            S.ledger_create = current_ledger;
        }
    } else {
        S.sent = amount_xfl;
        S.total_sent = amount_xfl;
        S.ledger_create = current_ledger;
    }

    typedef struct voucherTotal {
        int64_t created;
        int64_t claimed;
    } VoucherTotal;
    VoucherTotal Total;
    uint8_t total_ns[32]    = { 0x5DU, 0x8FU, 0xF9U, 0xC2U, 0x82U, 0xE7U, 0xD9U, 0x68U, 0xD4U, 0x40U, 0xBEU, 0x77U, 0x02U, 0xA5U, 0x5AU, 0x42U, 0x49U, 0x1EU, 0x3AU, 0x7EU, 0x56U, 0xDFU, 0xA2U, 0x06U, 0xB2U, 0xE6U, 0xEAU, 0xD7U, 0x90U, 0xBEU, 0xB7U, 0x10U }; //TOTAL
    uint8_t tkey[32]        = { 0x54U, 0x4FU, 0x54U, 0x41U, 0x4CU }; // TOTAL

    if(state_foreign(SVAR(Total), SBUF(tkey), SBUF(total_ns), SBUF(hook_acc_id)) == 16){
          Total.created = float_sum(Total.created, amount_xfl);
          state_foreign_set(SVAR(Total), SBUF(tkey), SBUF(total_ns), SBUF(hook_acc_id));
    }    
    
    if(state_foreign_set(SVAR(S), user_acc_id, 32, SBUF(limit_ns), SBUF(hook_acc_id)) != 40) {
        rollback(SBUF("Voucher Create: Failed at creating the user limit."), __LINE__);
    }

    if (state_set(SVAR(amount_xfl), SBUF(state_key)) != 8)
	   rollback(SBUF("Voucher Create: Failed to create Voucher."), __LINE__);

   accept(SBUF("Voucher Created Successful."), __LINE__);

   _g(1,1);
   return 0;
}