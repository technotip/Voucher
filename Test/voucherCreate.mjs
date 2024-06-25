import { generate, derive, signAndSubmit, utils } from "xrpl-accountlib";

const wss = "wss://xahau-test.net";

const main = async () => {
  const voucher = generate.familySeed({ algorithm: "ed25519" });
  const secret = "";
  const account = derive.familySeed(secret);

  const networkInfo = await utils.txNetworkAndAccountValues(wss, account);
  const payload = {
    TransactionType: "Payment",
    Destination: "rvoucheredGC1mB4yd2CBjs7jGMRTLexe",
    Amount: "100000",
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

  const result = await signAndSubmit(payload, wss, account);

  console.log(JSON.stringify(result, null, "\t"));
  console.log("Voucher code for Claiming", voucher.keypair.privateKey);
};

main();
