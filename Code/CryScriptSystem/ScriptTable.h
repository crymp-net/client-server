#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct lua_State;

class ScriptSystem;

class ScriptTable : public IScriptTable
{
	int m_ref = 0;
	int m_refCount = 0;
	lua_State *m_L = nullptr;
	ScriptSystem *m_pSS = nullptr;

	ScriptTable() = default;

public:
	static ScriptTable *Create(ScriptSystem *pSS, lua_State *L, bool empty);

	void Init();
	void Attach();

	void PushRef();

	void SetMetatable(IScriptTable *pMetatable);

	//////////////////
	// IScriptTable //
	//////////////////

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

	bool GetValueRecursive(const char *path, IScriptTable *pTable) override;

private:
	void CloneTable(int srcTableIndex, int dstTableIndex);
	void CloneTableRecursive(int srcTableIndex, int dstTableIndex);

	void DumpTable(IScriptTableDumpSink *pSink);

	static ScriptTable *AllocateTable(ScriptSystem *pSS);
	static void DeallocateTable(ScriptTable *pTable, ScriptSystem *pSS);

	static int StdCFunction(lua_State *L);
	static int StdCUserDataFunction(lua_State *L);
};
