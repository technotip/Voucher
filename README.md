# Voucher
Simple Voucher: Xahau (XAH)

Switch to **Xahau Testnet** and interact with the Hook account: `rvoucheredGC1mB4yd2CBjs7jGMRTLexe`

# Transactions
1. Payment: To create the voucher.
2. Invoke: To claim the voucher.


# Payment
To create a voucher: Send a payment(minimum 2 XAH) to the Hook account.

**Important:** Send voucher publicKey as transaction parameter. Key: VC ( { 0x56U, 0x43U } )

**Example Payload:**
```
  const payload = {
    TransactionType: "Payment",
    Destination: "rvoucheredGC1mB4yd2CBjs7jGMRTLexe",
    Amount: "5000000",
    NetworkID: "21338",
    HookParameters: [
      {
        HookParameter: {
          HookParameterName: "5643",
          HookParameterValue: voucher.keypair.publicKey.toString("hex"),
        },
      },
    ],
    ...networkInfo.txValues,
  };
```

# Invoke
To claim a voucher: Send a Invoke transaction with blob information.
Instructions on blob formate and size: [Voucher Claim](https://gist.github.com/WietseWind/5c3dfe291dd23bf2ca65edc39216dd42).

**Example Payload:**
```
  const payload = {
          "TransactionType": "Invoke",
          "Account": "rnRSDQ43dA1EtupNREhs7d4Cuhya3KtBwD",
          "Destination": "rvoucheredGC1mB4yd2CBjs7jGMRTLexe",
          "Blob": "ED9E132EF9F5867A4C54D2063A846B7A0486657F62BB8C52B7D80D573651ED887661485C21679E1D7306C49921BAE3B59EB79A7D31832ECCC6C96A433A490F05EDF37F7FE00247F8179C5BB8EE95398F0A431B454282208BF022A909C06535D5C6307F889ED17D34062F38F7D0798C9ED309F78AA6",
           
           ...networkInfo.txValues,
  };
```



# Features
Keeps track of the total vouchers created and total vouchers claimed for all the individual accounts.
Limit the creation and claiming of vouchers per month per user. 
Ex: You can only create a total of 500 XAH vouchers per month. Similarly, you can only claim 500 XAH vouchers per month.
Limit can be set using an Admin account.
Keeps track of global total vouchers created and claimed via the hook.


# SetHook
We are using [hooks-toolkit](https://hooks-toolkit.com/quickstart) to build and install the hook.
Build command:
```
c2wasm-cli contracts build
ts-node install.ts
```
install.ts
```
import { createHookPayload } from "@transia/hooks-toolkit";
import { SetHookFlags } from "@transia/xrpl";
import { derive, utils, sign } from "xrpl-accountlib";

const wss = "wss://xahau-test.net";   // For mainnet, use: wss://xahau.network

const main = async () => {
  const secret = ""; //use proper familySeed
  const account = derive.familySeed(secret);

  const hook1 = createHookPayload({
    version: 0,
    createFile: "voucherClaim",
    namespace: "VC",
    flags: SetHookFlags.hsfOverride,
    hookOnArray: ["Invoke"],
  });

  const hook2 = createHookPayload({
    version: 0,
    createFile: "voucherCreate",
    namespace: "VC",
    flags: SetHookFlags.hsfOverride,
    hookOnArray: ["Payment"],
  });

  const networkInfo = await utils.txNetworkAndAccountValues(wss, account.address);

  const payload = {
    TransactionType: "SetHook",
    Flags: 0,
    Hooks: [{ Hook: hook1 }, { Hook: hook2 }],
    ...networkInfo.txValues,
    Account: "rvoucheredGC1mB4yd2CBjs7jGMRTLexe",
  };

  const temp = sign(payload, account);
  // const result = await signAndSubmit(payload, wss, account);
  // console.log(JSON.stringify(result, null, "\t"));
  console.log(JSON.stringify(temp, null, "\t"));
};

main();

```
