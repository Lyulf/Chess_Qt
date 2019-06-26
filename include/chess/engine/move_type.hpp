#pragma once

namespace chess {

	enum class MoveType {
		None,
		Normal,
		PawnDouble,
		Promotion,
		EnPassantCapture,
		Castle
	};

}