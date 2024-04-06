import { XrplClient } from "xrpl-client";

const main = async () => {
  const client = new XrplClient("wss://xahau.network");

  const { account_data } = await client.send({
    command: "account_info",
    account: "rKbftNCrmECA5Ab3YqitU5RNC9HtX6eEb8",
  });
  //console.log("account_data ", JSON.stringify(account_data, null, "\t"));
  console.log("namespace ", account_data.HookNamespaces);

  const data = await client.send({
    command: "account_namespace",
    account: "rKbftNCrmECA5Ab3YqitU5RNC9HtX6eEb8",
    namespace_id: account_data.HookNamespaces[0],
  });

  console.log(JSON.stringify(data, null, "\t"));
  client.close();
};
main();
