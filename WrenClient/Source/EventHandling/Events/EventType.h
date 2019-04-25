#pragma once

enum class EventType
{
	LeftMouseDownEvent,
	LeftMouseUpEvent,
	RightMouseDownEvent,
	RightMouseUpEvent,
	MiddleMouseDownEvent,
	MiddleMouseUpEvent,
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
	DeleteCharacterSuccess,
	PlayerCorrection
};