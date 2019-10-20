#pragma once

#include <OpCodes.h>

constexpr auto CLIENT_WIDTH = 1400.0f;
constexpr auto CLIENT_HEIGHT = 900.0f;
constexpr unsigned __int64 TIMEOUT_DURATION = 30000; // 30000ms == 30s
constexpr auto UPDATE_FREQUENCY = 0.01666666666f;
constexpr auto PLAYER_SPEED = 60.0f;
constexpr auto MESSAGE_BUFFER_SIZE = 100;
constexpr auto TILE_SIZE = 30.0f;
constexpr unsigned int MAP_WIDTH = 100;
constexpr unsigned int MAP_HEIGHT = 100;
constexpr unsigned int MAP_SIZE = MAP_WIDTH * MAP_HEIGHT;

constexpr XMFLOAT3 VEC_ZERO      = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
constexpr XMFLOAT3 VEC_SOUTHWEST = XMFLOAT3{ -1.0f, 0.0f, -1.0f };
constexpr XMFLOAT3 VEC_SOUTH     = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
constexpr XMFLOAT3 VEC_SOUTHEAST = XMFLOAT3{ 1.0f, 0.0f, -1.0f };
constexpr XMFLOAT3 VEC_EAST      = XMFLOAT3{ 1.0f, 0.0f, 0.0f };
constexpr XMFLOAT3 VEC_NORTHEAST = XMFLOAT3{ 1.0f, 0.0f, 1.0f };
constexpr XMFLOAT3 VEC_NORTH     = XMFLOAT3{ 0.0f, 0.0f, 1.0f };
constexpr XMFLOAT3 VEC_NORTHWEST = XMFLOAT3{ -1.0f, 0.0f, 1.0f };
constexpr XMFLOAT3 VEC_WEST      = XMFLOAT3{ -1.0f, 0.0f, 0.0f };

const OpCode CHECKSUM{ OpCode::Checksum };
constexpr auto PACKET_SIZE = 1024;
constexpr auto SERVER_IP_ADDRESS = "127.0.0.1";
constexpr auto SERVER_PORT_NUMBER = 27016;

constexpr auto INVENTORY_SIZE = 16;