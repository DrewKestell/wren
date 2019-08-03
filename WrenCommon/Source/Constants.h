#pragma once

static constexpr auto CLIENT_WIDTH = 1400;
static constexpr auto CLIENT_HEIGHT = 900;
static constexpr auto TIMEOUT_DURATION = 30000; // 30000ms == 30s
static constexpr auto UPDATE_FREQUENCY = 0.01666666666f;
static constexpr auto PLAYER_SPEED = 60.0f;
static constexpr auto MESSAGE_BUFFER_SIZE = 100;
static constexpr auto TILE_SIZE = 30.0f;
static constexpr unsigned int MAP_WIDTH = 100;
static constexpr unsigned int MAP_HEIGHT = 100;
static constexpr unsigned int MAP_SIZE = MAP_WIDTH * MAP_HEIGHT;

static constexpr XMFLOAT3 VEC_ZERO      = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
static constexpr XMFLOAT3 VEC_SOUTHWEST = XMFLOAT3{ -1.0f, 0.0f, -1.0f };
static constexpr XMFLOAT3 VEC_SOUTH     = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
static constexpr XMFLOAT3 VEC_SOUTHEAST = XMFLOAT3{ 1.0f, 0.0f, -1.0f };
static constexpr XMFLOAT3 VEC_EAST      = XMFLOAT3{ 1.0f, 0.0f, 0.0f };
static constexpr XMFLOAT3 VEC_NORTHEAST = XMFLOAT3{ 1.0f, 0.0f, 1.0f };
static constexpr XMFLOAT3 VEC_NORTH     = XMFLOAT3{ 0.0f, 0.0f, 1.0f };
static constexpr XMFLOAT3 VEC_NORTHWEST = XMFLOAT3{ -1.0f, 0.0f, 1.0f };
static constexpr XMFLOAT3 VEC_WEST      = XMFLOAT3{ -1.0f, 0.0f, 0.0f };