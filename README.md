# Voucher
Simple Voucher: Xahau (XAH)

Switch to **Xahau Testnet** and interact with the Hook account: `rKbftNCrmECA5Ab3YqitU5RNC9HtX6eEb8`

# Transactions
1. Payment: To create the voucher.
2. Invoke: To claim the voucher.


# Payment
To create a voucher: Send a payment(minimum 0.1 XAH) to the Hook account.
**Important:** Send voucher account_id as transaction parameter. Key: VC ( { 0x56U, 0x43U } )

# Invoke
To claim a voucher: Send a Invoke transaction with blob information.
Instructions on blob formate and size: [Voucher Claim](https://gist.github.com/WietseWind/5c3dfe291dd23bf2ca65edc39216dd42).


# TO DO
Keep track of the total vouchers created and total vouchers claimed for all the accounts.
Limit the creation and claiming of vouchers per month.
Ex: You can only create a total of 500 XAH vouchers per month. Similarly, you can only claim 500 XAH vouchers per month.
Limit yet to be decided.
