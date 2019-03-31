#pragma once

enum class EventType
{
	MouseDownEvent,
	MouseUpEvent,
	MouseMoveEvent,
	ButtonPressEvent,
	KeyDownEvent,
	SystemKeyDownEvent,
	SystemKeyUpEvent,
	SelectCharacterListing,
	DeselectCharacterListing,
	ChangeActiveLayer,
	CreateAccountFailed,
	CreateAccountSuccess,
	LoginFailed,
	LoginSuccess,
	CreateCharacterFailed,
	CreateCharacterSuccess,
	EnterWorldSuccess,
	WrongChecksum,
	OpcodeNotImplemented
};