import { derive, signAndSubmit, utils } from "xrpl-accountlib";

import rippleAddressCodec from "ripple-address-codec";

import { sign } from "ripple-keypairs";

const wss = "wss://xahau-test.net";

const secret = "";
const account = derive.familySeed(secret);

/**
 * This is th evoucher from voucherCreate.mjs
 */
const giftVoucher =
  "ED72E3A4D29F893062339F5C6B99CBDB9281812569972412BE0929FB71F7DE6D3D";
const Voucher = {
  privateKey: giftVoucher,
  publicKey: null,
};

const derived = derive.privatekey(Voucher.privateKey);
Voucher.publicKey = Buffer.from(derived.keypair.publicKey, "hex");
//        ^^        Voucher pubkey / or hash thereof is the KEY for the Hook Store that tracks the balance that can be claimed

// Voucher claim balance is stored as Hook State under publicKey / pubkey hash

/**
 * This is the Invoke as generated
 */
console.log("Voucher.publicKey ", derived.keypair.publicKey);
const networkInfo = await utils.txNetworkAndAccountValues(wss, account);
const InvokeTx = {
  TransactionType: "Invoke",
  Account: account.address,
  Destination: "rvoucheredGC1mB4yd2CBjs7jGMRTLexe",
  Blob: null,
  ...networkInfo.txValues,
};

const accountId = rippleAddressCodec.decodeAccountID(InvokeTx.Account);
const payload = Buffer.concat([Voucher.publicKey, accountId]);
const signature = Buffer.from(
  sign(payload.toString("hex"), Voucher.privateKey),
  "hex"
);

const Blob = Buffer.concat([signature, payload]).toString("hex");

console.log(Blob);
//
//  [   - 64 signature -   ] [   - 33 voucher pubkey -   ] [   - 20 destination account id -   ]
//

Object.assign(InvokeTx, { Blob });
console.log(InvokeTx);

const result = await signAndSubmit(InvokeTx, wss, account);

console.log(JSON.stringify(result, null, "\t"));
