const fs = require("fs");
const path = require("path");

const file = path.resolve(__dirname, "states.json");

function setState(id, state) {
  const states = JSON.parse(fs.readFileSync(file));
  states[id] = state;
  fs.writeFileSync(file, JSON.stringify(states));
}

function getState(id, defaultState) {
  const states = JSON.parse(fs.readFileSync(file));
  const state = states[id] || defaultState;
  return state;
}

module.exports = { setState, getState };
