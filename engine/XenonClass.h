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

class XObject
{
	virtual void Simulate() {}	// Simulate the object.
	virtual void Draw() {}		// Draw the object.
};

class XObjectManager
{
	virtual void Add(XObject *object) = 0;		// Adds object to manager.
	virtual XObject* Add() = 0;
	virtual void Remove(XObject *object) = 0;	// Removes object from manager.
	virtual void Delete(XObject *object) = 0;	// Deletes object entirely.
	virtual void Clear() = 0;					// Clears objects from manager.
};

class XManager
{
	virtual void Simulate() {}
	virtual void Draw()	{}
};
