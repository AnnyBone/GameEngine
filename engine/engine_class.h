/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

/*	Base Abstract Classes	*/

class CoreObject
{
	virtual void Simulate() = 0;	// Simulate the object.
	virtual void Draw() = 0;		// Draw the object.
};

class CoreObjectManager
{
	virtual void Add(CoreObject *object) = 0;		// Adds object to manager.
	virtual void Remove(CoreObject *object) = 0;	// Removes object from manager.
	virtual void Delete(CoreObject *object) = 0;	// Deletes object entirely.
	virtual void Clear() = 0;						// Clears objects from manager.

	virtual void Draw() = 0;		// Draws all objects assigned.
	virtual void Simulate() = 0;	// Simulates all objects assigned.
};

class CoreManager
{
	virtual void Simulate() {}
	virtual void Draw()	{}
};
