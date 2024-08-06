#pragma once

#include "CryCommon/CryAction/IItemSystem.h"

class ItemParamsNode : public IItemParamsNode
{
	void* m_reserved[10 - 1] = {};

public:
	ItemParamsNode();
	virtual ~ItemParamsNode();

	////////////////////////////////////////////////////////////////////////////////
	// IItemParamsNode
	////////////////////////////////////////////////////////////////////////////////

	void AddRef() const override;
	unsigned int GetRefCount() const override;
	void Release() const override;

	int GetAttributeCount() const override;
	const char* GetAttributeName(int i) const override;
	const char* GetAttribute(int i) const override;
	bool GetAttribute(int i, Vec3& attr) const override;
	bool GetAttribute(int i, Ang3& attr) const override;
	bool GetAttribute(int i, float& attr) const override;
	bool GetAttribute(int i, int& attr) const override;
	int GetAttributeType(int i) const override;

	const char* GetAttribute(const char* name) const override;
	bool GetAttribute(const char* name, Vec3& attr) const override;
	bool GetAttribute(const char* name, Ang3& attr) const override;
	bool GetAttribute(const char* name, float& attr) const override;
	bool GetAttribute(const char* name, int& attr) const override;
	int GetAttributeType(const char* name) const override;

	const char* GetNameAttribute() const override;

	int GetChildCount() const override;
	const char* GetChildName(int i) const override;
	const IItemParamsNode* GetChild(int i) const override;
	const IItemParamsNode* GetChild(const char* name) const override;

	void SetAttribute(const char* name, const char* attr) override;
	void SetAttribute(const char* name, const Vec3& attr) override;
	void SetAttribute(const char* name, float attr) override;
	void SetAttribute(const char* name, int attr) override;

	void SetName(const char* name) override;
	const char* GetName() const override;

	IItemParamsNode* InsertChild(const char* name) override;
	void ConvertFromXML(XmlNodeRef& root) override;

	int GetMemorySize() const override;

	////////////////////////////////////////////////////////////////////////////////
};
