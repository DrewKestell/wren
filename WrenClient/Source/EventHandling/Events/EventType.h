#pragma once

enum class EventType
{
	MouseDownEvent,
	MouseUpEvent,
	MouseMoveEvent,
	KeyDownEvent,
	SystemKeyDownEvent,
	SystemKeyUpEvent,
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
	DeleteCharacterSuccess
};