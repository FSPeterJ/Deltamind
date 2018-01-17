#pragma once

// Delete Later
enum ComponentsListing
{
	MESH,
	ANIMATION,
	MATERIAL,
	MAX_COMPONENTS
};

// Allows for covariant return types in Manager interface
class ComponentBase
{
	bool unique = false;
	//If you are creating this base, you are doing something wrong.
};