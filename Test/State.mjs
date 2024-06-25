import { XrplClient } from "xrpl-client";

const main = async () => {
  const client = new XrplClient("wss://xahau.network");

  const { account_data } = await client.send({
    command: "account_info",
    account: "rvoucheredGC1mB4yd2CBjs7jGMRTLexe",
  });
  //console.log("account_data ", JSON.stringify(account_data, null, "\t"));
  console.log("namespace ", account_data.HookNamespaces);

  const data = await client.send({
    command: "account_namespace",
    account: "rvoucheredGC1mB4yd2CBjs7jGMRTLexe",
    namespace_id: account_data.HookNamespaces[0],
  });

  console.log(JSON.stringify(data, null, "\t"));
  client.close();
};
main();
