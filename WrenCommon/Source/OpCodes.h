#pragma once

enum class OpCode : int
{
	Connect,
	Disconnect,
	LoginSuccess,
	LoginFailure,
	CreateAccount,
	CreateAccountSuccess,
	CreateAccountFailure,
	CreateCharacter,
	CreateCharacterSuccess,
	CreateCharacterFailure,
	Heartbeat,
	EnterWorld,
	EnterWorldSuccess,
	DeleteCharacter,
	DeleteCharacterSuccess,
	PlayerUpdate,
	SkillIncrease,
	NpcUpdate,
	ActivateAbility,
	SendChatMessage,
	PropagateChatMessage,
	SetTarget,
	UnsetTarget,
	ServerMessage,
	ActivateAbilitySuccess,
	AttackHit,
	AttackMiss,
	Ping,
	Pong,
	PlayerRightMouseDown,
	PlayerRightMouseUp,
	PlayerRightMouseDirChange,

	Checksum = 65836216
};
