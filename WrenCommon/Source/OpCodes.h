#pragma once

const std::string CHECKSUM = { '6', '5', '8', '3', '6', '2', '1', '6' };

constexpr char OPCODE_CONNECT[2] = { '0', '0' };
constexpr char OPCODE_DISCONNECT[2] = { '0', '1' };
constexpr char OPCODE_LOGIN_SUCCESSFUL[2] = { '0', '2' };
constexpr char OPCODE_LOGIN_UNSUCCESSFUL[2] = { '0', '3' };
constexpr char OPCODE_CREATE_ACCOUNT[2] = { '0', '4' };
constexpr char OPCODE_CREATE_ACCOUNT_SUCCESSFUL[2] = { '0', '5' };
constexpr char OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL[2] = { '0', '6' };
constexpr char OPCODE_CREATE_CHARACTER[2] = { '0', '7' };
constexpr char OPCODE_CREATE_CHARACTER_SUCCESSFUL[2] = { '0', '8' };
constexpr char OPCODE_CREATE_CHARACTER_UNSUCCESSFUL[2] = { '0', '9' };
constexpr char OPCODE_HEARTBEAT[2] = { '1', '0' };
constexpr char OPCODE_ENTER_WORLD[2] = { '1', '1' };
constexpr char OPCODE_ENTER_WORLD_SUCCESSFUL[2] = { '1', '2' };
constexpr char OPCODE_DELETE_CHARACTER[2] = { '1', '3' };
constexpr char OPCODE_DELETE_CHARACTER_SUCCESSFUL[2] = { '1', '4' };
constexpr char OPCODE_PLAYER_UPDATE[2] = { '1', '5' };
constexpr char OPCODE_PLAYER_CORRECTION[2] = { '1', '6' };
constexpr char OPCODE_GAMEOBJECT_UPDATE[2] = { '1', '7' };
constexpr char OPCODE_ACTIVATE_ABILITY[2] = { '1', '8' };
constexpr char OPCODE_SEND_CHAT_MESSAGE[2] = { '1', '9' };
constexpr char OPCODE_PROPAGATE_CHAT_MESSAGE[2] = { '2', '0' };
constexpr char OPCODE_SET_TARGET[2] = { '2', '1' };
constexpr char OPCODE_UNSET_TARGET[2] = { '2', '2' };