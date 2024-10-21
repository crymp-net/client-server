/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a registry for vehicle damages templates

-------------------------------------------------------------------------
History:
- 18:07:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGESTEMPLATEREGISTRY_H__
#define __VEHICLEDAMAGESTEMPLATEREGISTRY_H__

#include <map>
#include <vector>

class CVehicleDamagesGroup;

class CVehicleDamagesTemplateRegistry
	: public IVehicleDamagesTemplateRegistry
{
public:

	CVehicleDamagesTemplateRegistry() {}
	virtual ~CVehicleDamagesTemplateRegistry() {}

	virtual bool Init(const string& defaultDefFilename, const string& damagesTemplatesPath);
	virtual void Release() { delete this; }

	virtual bool RegisterTemplates(const string& filename, const string& defFilename);
	virtual bool UseTemplate(const string& templateName, IVehicleDamagesGroup* pDamagesGroup);

protected:

	string m_defaultDefFilename;

	struct STemplateFile
	{
		string filename;
		string defFilename;
		XmlNodeRef templateTable;
	};

	typedef std::vector<STemplateFile> TTemplateFileVector;
	TTemplateFileVector m_templateFiles;

	typedef std::map<string, XmlNodeRef> TTemplateMap;
	TTemplateMap m_templates;
};

#endif
