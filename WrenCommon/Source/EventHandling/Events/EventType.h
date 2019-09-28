#pragma once

enum class EventType
{
	LeftMouseDown,
	DoubleLeftMouseDown,
	LeftMouseUp,
	RightMouseDown,
	RightMouseUp,
	MiddleMouseDown,
	MiddleMouseUp,
	MouseMove,
	KeyDown,
	SystemKeyDown,
	SystemKeyUp,
	ChangeActiveLayer,
	CreateAccountFailed,
	CreateAccountSuccess,
	LoginFailed,
	LoginSuccess,
	CreateCharacterFailed,
	CreateCharacterSuccess,
	EnterWorldSuccess,
	WrongChecksum,
	OpcodeNotImplemented,
	DeleteCharacterSuccess,
	PlayerCorrection,
	DeleteGameObject,
	NpcUpdate,
	PlayerUpdate,
	UIAbilityDropped,
	ActivateAbility,
	StartDraggingUIAbility,
	SetTarget,
	UnsetTarget,
	Logout,
	DeselectCharacterListing,
	ReorderUIComponents,
	SendChatMessage,
	PropagateChatMessage,
	ServerMessage,
	ActivateAbilitySuccess,
	AttackHit,
	AttackMiss,
	Pong,
	SkillIncrease,
	NpcDeath
};