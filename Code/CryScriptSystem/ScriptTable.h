#pragma once

#include "ScriptSystem.h"

class ScriptTable : public IScriptTable
{
	enum
	{
		DELETED_REF = -1,
		NULL_REF    = 0,
	};

	int m_ref = NULL_REF;
	int m_refCount = 0;
	lua_State *m_L = nullptr;
	CScriptSystem *m_pSS = nullptr;

	void Init();

	void CloneTable(int srcTableIndex, int dstTableIndex);
	void CloneTableRecursive(int srcTableIndex, int dstTableIndex);

	void DumpTable(IScriptTableDumpSink *pSink);

	static int StdCFunction(lua_State *L);
	static int StdCUserDataFunction(lua_State *L);

public:
	ScriptTable() = default;

	static ScriptTable *Create(CScriptSystem *pSS, lua_State *L, bool empty);

	void Attach();
	void AttachToObject(IScriptTable *pObject);

	void PushRef();
	void PushRef(IScriptTable *pObject);

	void SetMetatable(IScriptTable *pMetatable);

	IScriptSystem *GetScriptSystem() const override;

	void AddRef() override;
	void Release() override;

	void Delegate(IScriptTable *pMetatable) override;

	void *GetUserDataValue() override;

	void SetValueAny(const char *key, const ScriptAnyValue & any, bool isChain = false) override;
	bool GetValueAny(const char *key, ScriptAnyValue & any, bool isChain = false) override;

	bool BeginSetGetChain() override;
	void EndSetGetChain() override;

	ScriptVarType GetValueType(const char *key) override;
	ScriptVarType GetAtType(int index) override;

	void SetAtAny(int index, const ScriptAnyValue & any) override;
	bool GetAtAny(int index, ScriptAnyValue & any) override;

	Iterator BeginIteration() override;
	bool MoveNext(Iterator & it) override;
	void EndIteration(const Iterator & it) override;

	void Clear() override;

	int Count() override;

	bool Clone(IScriptTable *pSourceTable, bool isDeepCopy = false) override;

	void Dump(IScriptTableDumpSink *pSink) override;

	bool AddFunction(const SUserFunctionDesc & fd) override;
};
