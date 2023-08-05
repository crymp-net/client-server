#include <cstdint>

#include "ItemParamsNode.h"

ItemParamsNode::ItemParamsNode()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305277b0;
#else
	std::uintptr_t ctor = 0x3051f5f0;
#endif

	(this->*reinterpret_cast<void(ItemParamsNode::*&)()>(ctor))();
}

ItemParamsNode::~ItemParamsNode()
{
}

////////////////////////////////////////////////////////////////////////////////
// IItemParamsNode
////////////////////////////////////////////////////////////////////////////////

void ItemParamsNode::AddRef() const
{
}

unsigned int ItemParamsNode::GetRefCount() const
{
	return {};
}

void ItemParamsNode::Release() const
{
}

int ItemParamsNode::GetAttributeCount() const
{
	return {};
}

const char* ItemParamsNode::GetAttributeName(int i) const
{
	return {};
}

const char* ItemParamsNode::GetAttribute(int i) const
{
	return {};
}

bool ItemParamsNode::GetAttribute(int i, Vec3& attr) const
{
	return {};
}

bool ItemParamsNode::GetAttribute(int i, Ang3& attr) const
{
	return {};
}

bool ItemParamsNode::GetAttribute(int i, float& attr) const
{
	return {};
}

bool ItemParamsNode::GetAttribute(int i, int& attr) const
{
	return {};
}

int ItemParamsNode::GetAttributeType(int i) const
{
	return {};
}

const char* ItemParamsNode::GetAttribute(const char* name) const
{
	return {};
}

bool ItemParamsNode::GetAttribute(const char* name, Vec3& attr) const
{
	return {};
}

bool ItemParamsNode::GetAttribute(const char* name, Ang3& attr) const
{
	return {};
}

bool ItemParamsNode::GetAttribute(const char* name, float& attr) const
{
	return {};
}

bool ItemParamsNode::GetAttribute(const char* name, int& attr) const
{
	return {};
}

int ItemParamsNode::GetAttributeType(const char* name) const
{
	return {};
}

const char* ItemParamsNode::GetNameAttribute() const
{
	return {};
}

int ItemParamsNode::GetChildCount() const
{
	return {};
}

const char* ItemParamsNode::GetChildName(int i) const
{
	return {};
}

const IItemParamsNode* ItemParamsNode::GetChild(int i) const
{
	return {};
}

const IItemParamsNode* ItemParamsNode::GetChild(const char* name) const
{
	return {};
}

void ItemParamsNode::SetAttribute(const char* name, const char* attr)
{
}

void ItemParamsNode::SetAttribute(const char* name, const Vec3& attr)
{
}

void ItemParamsNode::SetAttribute(const char* name, float attr)
{
}

void ItemParamsNode::SetAttribute(const char* name, int attr)
{
}

void ItemParamsNode::SetName(const char* name)
{
}

const char* ItemParamsNode::GetName() const
{
	return {};
}

IItemParamsNode* ItemParamsNode::InsertChild(const char* name)
{
	return {};
}

void ItemParamsNode::ConvertFromXML(XmlNodeRef& root)
{
}

int ItemParamsNode::GetMemorySize() const
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
