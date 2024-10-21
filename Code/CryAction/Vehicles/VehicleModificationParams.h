#ifndef __Vehicle_Modification_Params__h__
#define __Vehicle_Modification_Params__h__


class CVehicleModificationParams
{
public:
	CVehicleModificationParams() {}

	CVehicleModificationParams( XmlNodeRef xmlVehicleData, const char* modificationName )
	{
		assert( modificationName != NULL );
		if ( modificationName[ 0 ] == 0 )
		{
			return;
		}

		XmlNodeRef xmlModificationsGroup = xmlVehicleData->findChild( "Modifications" );
		if ( ! xmlModificationsGroup )
		{
			return;
		}

		XmlNodeRef xmlModification = FindModificationNodeByName( modificationName, xmlModificationsGroup );
		if ( ! xmlModification )
		{
			return;
		}

		InitModification( xmlModification );
	}


	virtual ~CVehicleModificationParams() {}


	template< typename T >
	void ApplyModification( const char* nodeId, const char* attrName, T& attrValueOut ) const
	{
		XmlNodeRef modificationNode = GetModificationNode( nodeId, attrName );
		if ( modificationNode )
		{
			modificationNode->getAttr( "value", attrValueOut );
		}
	}

private:
	void InitModification( XmlNodeRef xmlModificationData )
	{
		assert( xmlModificationData );

		bool hasParentModification = xmlModificationData->haveAttr( "parent" );
		if ( hasParentModification )
		{
			XmlNodeRef xmlModificationsGroup = xmlModificationData->getParent();

			const char* parentModificationName = xmlModificationData->getAttr( "parent" );
			XmlNodeRef xmlParentModificationData = FindModificationNodeByName( parentModificationName, xmlModificationsGroup );
			if ( xmlParentModificationData && ( xmlParentModificationData != xmlModificationData ) )
			{
				InitModification( xmlParentModificationData );
			}
		}

		XmlNodeRef xmlElemsGroup = xmlModificationData->findChild( "Elems" );
		if ( ! xmlElemsGroup )
		{
			return;
		}

		for ( int i = 0; i < xmlElemsGroup->getChildCount(); ++i )
		{
			XmlNodeRef xmlElem = xmlElemsGroup->getChild( i );

			InitModificationElem( xmlElem );
		}
	}


	static XmlNodeRef FindModificationNodeByName( const char* name, XmlNodeRef xmlModificationsGroup )
	{
		assert( name != NULL );
		assert( xmlModificationsGroup );

		int numNodes = xmlModificationsGroup->getChildCount();
		for ( int i = 0; i < numNodes; i++ )
		{
			XmlNodeRef xmlModification = xmlModificationsGroup->getChild( i );
			const char* modificationName = xmlModification->getAttr( "name" );
			if ( modificationName && ( strcmpi( name, modificationName ) == 0 ) )
			{
				return xmlModification;
			}
		}

		return XmlNodeRef();
	}


	void InitModificationElem( XmlNodeRef xmlElem )
	{
		assert( xmlElem != NULL );

		bool valid = true;
		valid &= xmlElem->haveAttr( "idRef" );
		valid &= xmlElem->haveAttr( "name" );
		valid &= xmlElem->haveAttr( "value" );

		if ( ! valid )
		{
			CryLog( "Vehicle modification element at line %i invalid, skipping.", xmlElem->getLine() );
			return;
		}

		const char* id = xmlElem->getAttr( "idRef" );
		const char* attrName = xmlElem->getAttr( "name" );

		TModificationKey key( id, attrName );
		m_modifications[ key ] = xmlElem;
	}


	XmlNodeRef GetModificationNode( const char* nodeId, const char* attrName ) const
	{
		assert( nodeId != NULL );
		assert( attrName != NULL );

		TModificationKey key( nodeId, attrName );
		TModificationMap::const_iterator cit = m_modifications.find( key );
		bool modificationFound = ( cit != m_modifications.end() );
		if ( ! modificationFound )
		{
			return XmlNodeRef();
		}

		return cit->second;
	}

private:
	typedef std::pair< string, string > TModificationKey;
	typedef std::map< TModificationKey, XmlNodeRef > TModificationMap;
	TModificationMap m_modifications;
};

#endif