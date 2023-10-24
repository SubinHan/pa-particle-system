#include "Ui/UiLink.h"

UiLink::UiLink(int id, int from, int to) :
	_id(id),
	_from(from),
	_to(to)
{
}

int UiLink::getId() const
{
	return _id;
}

int UiLink::getFromId() const
{
	return _from;
}

int UiLink::getToId() const
{
	return _to;
}

int UiLink::getOther(int id) const
{
	return _from == id ? _to : _from;
}