#include "hookapi.h"


int64_t hook(uint32_t) {

    if(otxn_type() != ttPAYMENT)
        rollback(SBUF("Voucher Create: Set only on Payment Transaction."), __LINE__);

    uint8_t user_acc_id[20];
    otxn_field(SBUF(user_acc_id), sfAccount);

    uint8_t hook_acc_id[20] = { 0xCBU, 0xE6U, 0x27U, 0x1BU, 0x0BU, 0x90U, 0x61U, 0x5BU, 0xD3U, 0xF4U, 0x03U, 0x0CU, 0xD1U, 0x1AU, 0x09U, 0x32U, 0xC6U, 0xC4U, 0x6BU, 0x6AU };
    short equal = 0; BUFFER_EQUAL(equal, hook_acc_id, user_acc_id, 20);

    if(equal)
        accept(SBUF("Voucher Create: Outgoing Transaction."), __LINE__);

    uint8_t amount[8];
    int64_t amount_len = otxn_field(SBUF(amount), sfAmount);

    if(amount_len != 8)
        rollback(SBUF("Voucher Create: Non XAH Transaction not allowed."), __LINE__);

   uint64_t otxn_drops = AMOUNT_TO_DROPS(amount);

   if(otxn_drops < (uint64_t)100000)
       rollback(SBUF("Voucher Create: Voucher should be minimum 0.1 XAH"), __LINE__);

    uint8_t pname[2] = { 0x56U, 0x43U };
    uint8_t state_key[32];
    if(otxn_param(state_key, 20, pname, 2) != 20)
        rollback(SBUF("Voucher Create: Please pass valid account id."), __LINE__);

    uint8_t vault[8];
    if(state(SBUF(vault), SBUF(state_key)) == 8)
        rollback(SBUF("Voucher Create: Voucher already present."), __LINE__);

    if (state_set(SBUF(amount), SBUF(state_key)) < 0)
	   rollback(SBUF("Voucher Create: Failed to create Voucher."), __LINE__);

   accept(SBUF("Voucher Created Successful."), __LINE__);

   _g(1,1);
   return 0;
}