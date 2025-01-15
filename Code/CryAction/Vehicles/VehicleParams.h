#ifndef __Vehicle_Params__h__
#define __Vehicle_Params__h__

#include "VehicleModificationParams.h"
#include "CryCommon/CrySystem/IXml.h"

class CVehicleParams
{
public:
	CVehicleParams(XmlNodeRef root, const CVehicleModificationParams& modificationParams)
		: m_xmlNode(root)
		, m_modificationParams(modificationParams)
	{
	}

	virtual ~CVehicleParams() {}

	const char* getTag() const
	{
		return m_xmlNode->getTag();
	}

	bool haveAttr(const char* name) const
	{
		return m_xmlNode->haveAttr(name);
	}

	const char* getAttr(const char* name) const
	{
		const char* attributeValue = m_xmlNode->getAttr(name);
		const char** attributeValueAddress = &attributeValue;
		ApplyModification(name, attributeValueAddress);

		return attributeValue;
	}

	bool getAttr(const char* name, const char** valueOut) const
	{
		return GetAttrImpl(name, valueOut);
	}

	bool getAttr(const char* name, int& valueOut) const
	{
		return GetAttrImpl(name, valueOut);
	}

	bool getAttr(const char* name, float& valueOut) const
	{
		return GetAttrImpl(name, valueOut);
	}

	bool getAttr(const char* name, bool& valueOut) const
	{
		return GetAttrImpl(name, valueOut);
	}

	bool getAttr(const char* name, Vec3& valueOut) const
	{
		return GetAttrImpl(name, valueOut);
	}

	int getChildCount() const
	{
		return m_xmlNode->getChildCount();
	}

	CVehicleParams getChild(int i) const
	{
		XmlNodeRef childNode = m_xmlNode->getChild(i);
		return CVehicleParams(childNode, m_modificationParams);
	}

	CVehicleParams findChild(const char* id) const
	{
		XmlNodeRef childNode = m_xmlNode->findChild(id);
		return CVehicleParams(childNode, m_modificationParams);
	}

	operator bool() const { return m_xmlNode != NULL; }

	bool IsValid() const { return m_xmlNode != NULL; }

	void Dump(int level = 0) const
	{
		if (!IsValid())
		{
			CryLogAlways("%*s<Invalid Node>", level * 2, "");
			return;
		}

		// Print current node's tag
		CryLogAlways("%*s<%s>", level * 2, "", getTag());

		// Print attributes
		int attributeCount = m_xmlNode->getNumAttributes();
		for (int i = 0; i < attributeCount; ++i)
		{
			const char* key;
			const char* value;
			m_xmlNode->getAttributeByIndex(i, &key, &value);
			CryLogAlways("%*s  @%s=\"%s\"", level * 2, "", key, value);
		}

		// Print child nodes recursively
		int childCount = getChildCount();
		for (int i = 0; i < childCount; ++i)
		{
			getChild(i).Dump(level + 1);
		}

		// Closing tag
		CryLogAlways("%*s</%s>", level * 2, "", getTag());
	}

private:
	template<typename T>
	bool GetAttrImpl(const char* name, T& valueOut) const
	{
		bool attributeGetSuccess = m_xmlNode->getAttr(name, valueOut);
		ApplyModification(name, valueOut);

		return attributeGetSuccess;
	}

	template<>
	bool GetAttrImpl<const char**>(const char* name, const char**& valueOut) const
	{
		const bool exists = m_xmlNode->haveAttr(name);
		*valueOut = m_xmlNode->getAttr(name);
		ApplyModification(name, valueOut);

		return exists;
	}

	template<typename T>
	void ApplyModification(const char* name, T& valueOut) const
	{
		const char* id = m_xmlNode->getAttr("id");
		if (id && *id)
		{
			m_modificationParams.ApplyModification(id, name, valueOut);
		}
	}

private:
	XmlNodeRef                        m_xmlNode;
	const CVehicleModificationParams& m_modificationParams;
};




#endif