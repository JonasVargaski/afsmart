const express = require("express");
const socket = require("socket.io");
const Clients = require("./clients");

const PORT = 3333;
const app = express();
app.use(express.json());

app.post("/session", (req, res) => {
  console.log(req.body);
  res.json({ user: { name: "Jonas", id: "54584848117" } });
});

const server = app.listen(PORT, () => {
  console.log(`Listening on port ${PORT}`);
});

const io = socket(server);
const clients = new Clients(io);

io.on("connection", (client) => {
  console.log("new client", client.id);
  client.emit("s->d.sync", {});
  client.on("d->s.sync", (state) => clients.syncDeviceState(client, state));
  client.on("disconnect", () => clients.removeClient(client.id));

  client.on("a->s.sync", (state) => console.log(state));
});
