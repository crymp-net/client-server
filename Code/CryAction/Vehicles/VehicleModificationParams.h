#ifndef __Vehicle_Modification_Params__h__
#define __Vehicle_Modification_Params__h__

#include <map>

class CVehicleModificationParams
{
public:
	CVehicleModificationParams();
	CVehicleModificationParams(XmlNodeRef xmlVehicleData, const char* modificationName);
	virtual ~CVehicleModificationParams();

	template<typename T>
	void ApplyModification(const char* nodeId, const char* attrName, T& attrValueOut) const
	{
		XmlNodeRef modificationNode = GetModificationNode(nodeId, attrName);
		if (modificationNode)
		{
			modificationNode->getAttr("value", attrValueOut);
		}
	}

	template<>
	void ApplyModification<const char**>(const char* nodeId, const char* attrName, const char**& attrValueOut) const
	{
		XmlNodeRef modificationNode = GetModificationNode(nodeId, attrName);
		if (modificationNode)
		{
			*attrValueOut = modificationNode->getAttr("value");
		}
	}

private:
	void               InitModification(XmlNodeRef xmlModificationData);

	static XmlNodeRef  FindModificationNodeByName(const char* name, XmlNodeRef xmlModificationsGroup);

	void               InitModificationElem(XmlNodeRef xmlElem);

	virtual XmlNodeRef GetModificationNode(const char* nodeId, const char* attrName) const;

private:
	struct Implementation;
	Implementation* m_pImpl;
};

#endif
