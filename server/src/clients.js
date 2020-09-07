const database = require("./database");

class Clients {
  constructor(io) {
    this.io = io;
    this.clients = {};
  }

  sync() {
    Object.values(this.clients).forEach((client) => {
      const socket = this.io.to(client.connectionId);
      const stateDevice = client.state.outputs;
      socket.emit("s->d.state", stateDevice);
    });
  }

  syncDeviceState(client, { mac, pwd, ...state }) {
    const deviceId = mac.replace(/(:)/gi, "").toUpperCase();
    const connectionId = client.id;

    console.log(state);

    // database.setState(deviceId, state);

    this.clients[deviceId] = {
      pwd,
      deviceId,
      connectionId,
      connetionDate: new Date(),
      state: { ...state },
    };

    this.sync();
  }

  removeClient(client) {
    delete this.clients[client.id];
    console.log("CLIENT DISCONNECTED");
  }
}

module.exports = Clients;
