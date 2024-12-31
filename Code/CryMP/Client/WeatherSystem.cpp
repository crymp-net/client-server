#include <array>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/Cry3DEngine/I3DEngine.h"
#include "CryCommon/CryRenderer/IRenderer.h"
#include "CryGame/Game.h"

#include "WeatherSystem.h"

enum EWeatherVars {
	WEATHER_ENABLED,
	WEATHER_WIND,
	WEATHER_FX,
	WEATHER_LAYERS,
	WEATHER_VAR_COUNT
};

struct SEffectInfo {
	std::string effect;
	float scale = 1.0f;
	unsigned instances = 1;
	Vec3 dir = Vec3(0.0f, 0.0f, 1.0f);
	Vec3 pos = Vec3(512.0f, 512.0f, 64.0f);
};

static bool                          currently_enabled = false;
static std::map<int, float[3]>       original_weather_values;
static std::optional<Vec3>           original_wind;
static std::map<int, IStatInstGroup> original_groups;

static std::map<int, string>                                 active_values;
static std::set<std::string>                                 active_effects;
static std::optional<unsigned>                               active_mask;
static std::map<std::string, std::vector<IParticleEmitter*>> active_emmiters;

static constexpr std::array<EERType, 1> static_entities = {
	eERType_Brush,
};

static const int WEATHER_NAMESPACE		= 2000;
static const int WEATHER_ENV_NAMESPACE  = 2100;

static std::string GetFrozenGrassName(std::string_view path);
static std::string GetNormalGrassName(std::string_view path);
static SEffectInfo GetEffectInfo(std::string_view effect);

CWeatherSystem::CWeatherSystem() {
	original_weather_values.clear();
	m_time = 0;
	m_lastUpdate = -1000.0f;
}

void CWeatherSystem::Reset(bool deapply) {
	RestoreWeather(true);

	if (deapply) {
		if (original_wind) {
			gEnv->p3DEngine->SetWind(*original_wind);
		}

		if (active_mask) {
			RemoveLayer(*active_mask);
		}

		for (auto& k : active_effects) {
			if (k[0] == '-') SpawnEffect(k.c_str() + 1);
			else RemoveEffect(k.c_str());
		}
	}
	
	currently_enabled = false;
	original_groups.clear();
	original_weather_values.clear();
	original_wind.reset();

	active_effects.clear();
	active_emmiters.clear();
	active_values.clear();
	active_mask.reset();
}

void CWeatherSystem::Update(float frameTime) {
	CSynchedStorage *pSSS = g_pGame->GetSynchedStorage();
	if (!pSSS || !g_pGame->GetGameRules()) {
		return;
	}

	m_time += frameTime;

	bool b;
	if (!pSSS->GetGlobalValue(WEATHER_NAMESPACE + WEATHER_ENABLED, b) || !b) {
		if (currently_enabled) {
			Reset();
		}
		return;
	} else {
		currently_enabled = true;
	}

	if (m_time - m_lastUpdate >= 1.0f) {

		int changed = 0;
		ITimeOfDay* pTOD = gEnv->p3DEngine->GetTimeOfDay();
		if (pTOD) {
			int count = pTOD->GetVariableCount();
			for (int i = 0; i < count; i++) {
				string value;
				auto existing = active_values.find(WEATHER_ENV_NAMESPACE + i);
				if (pSSS->GetGlobalValue(WEATHER_ENV_NAMESPACE + i, value) && value.length() > 0) {
					if (existing == active_values.end() || existing->second != value) {
						float x, y, z;
						if (value[0] >= 'a' && value[0] <= 'z') {
							if (value[0] == 'i' && sscanf(value.c_str() + 1, "%f", &x) == 1) {
								SetWeatherVariable(i, x, true);
							} else if (value[0] == 'r' && sscanf(value.c_str() + 1, "%f", &x) == 1) {
								SetWeatherVariable(i, x, false);
							} else if (value[0] == 'v' && sscanf(value.c_str() + 1, "%f,%f,%f", &x, &y, &z) == 3) {
								SetWeatherVariable(i, x, y, z, false);
							}
						} else if(sscanf(value.c_str(), "%f", &x) == 1){
							SetWeatherVariable(i, x, true);
						}
						active_values[WEATHER_ENV_NAMESPACE + i] = value;
						changed++;
					}
				} else {
					RestoreWeatherVariable(i);
					if (existing != active_values.end()) {
						active_values.erase(existing);
						changed++;
					}
				}
			}
			if (changed > 0) {
				pTOD->Update();
			}
			for (int i = WEATHER_ENABLED + 1; i < WEATHER_VAR_COUNT; i++) {
				float f3[3];
				string value;
				auto existing = active_values.find(WEATHER_NAMESPACE + i);
				pSSS->GetGlobalValue(WEATHER_NAMESPACE + i, value);
				if (existing == active_values.end() || existing->second != value) {
					switch (i) {
					case WEATHER_WIND:
						if (value.length() > 0 && value[0] == 'v' && sscanf(value.c_str() + 1, "%f,%f,%f", f3 + 0, f3 + 1, f3 + 2) == 3) {
							if (!original_wind) original_wind = gEnv->p3DEngine->GetWind(AABB(), false);
							gEnv->p3DEngine->SetWind(Vec3(f3[0], f3[1], f3[2]));
						} else if (value.length() > 0 && sscanf(value.c_str(), "%f", f3 + 0) == 1) {
							if (!original_wind) original_wind = gEnv->p3DEngine->GetWind(AABB(), false);
							gEnv->p3DEngine->SetWind((*original_wind) * f3[0]);
						} else {
							if (original_wind) {
								gEnv->p3DEngine->SetWind(*original_wind);
								original_wind.reset();
							}
						}
						break;
					case WEATHER_FX:
						SyncEffects(value.c_str());
						break;
					case WEATHER_LAYERS:
						SyncLayers(value.c_str());
						break;
					}
					if (existing != active_values.end() && value.length() == 0) {
						active_values.erase(existing);
					}
				}
			}
		}
		m_lastUpdate = m_time;
	}
}

bool CWeatherSystem::SetWeatherVariable(int variableId, float value, bool interpolate, bool update) {
	ITimeOfDay* pTOD = gEnv->p3DEngine->GetTimeOfDay();
	if (pTOD) {
		ITimeOfDay::SVariableInfo info;
		if (pTOD->GetVariableInfo(variableId, info)) {
			auto existing = original_weather_values.find(variableId);
			float fMin = info.fValue[1];
			float fMax = info.fValue[2];
			if (existing == original_weather_values.end()) {
				memcpy(original_weather_values[variableId], info.fValue, sizeof(info.fValue));
			}
			else {
				fMin = existing->second[1];
				fMax = existing->second[2];
			}
			if (interpolate) {
				value = fMax * value + fMin * (1.0 - value);
			}
			info.fValue[1] = value;
			info.fValue[2] = value;
			pTOD->SetVariableValue(variableId, info.fValue);
			if (update) {
				pTOD->Update();
			}
			return true;
		}
	}
	return false;
}

bool CWeatherSystem::SetWeatherVariable(int variableId, float x, float y, float z, bool update) {
	ITimeOfDay* pTOD = gEnv->p3DEngine->GetTimeOfDay();
	if (pTOD) {
		ITimeOfDay::SVariableInfo info;
		if (pTOD->GetVariableInfo(variableId, info)) {
			auto existing = original_weather_values.find(variableId);
			if (existing == original_weather_values.end()) {
				memcpy(original_weather_values[variableId], info.fValue, sizeof(info.fValue));
			}
			info.fValue[0] = x;
			info.fValue[1] = y;
			info.fValue[2] = z;
			pTOD->SetVariableValue(variableId, info.fValue);
			if (update) {
				pTOD->Update();
			}
			return true;
		}
	}
	return false;
}

std::optional<WeatherVariable> CWeatherSystem::GetWeatherVariable(int variableId) {
	ITimeOfDay* pTOD = gEnv->p3DEngine->GetTimeOfDay();
	if (pTOD) {
		ITimeOfDay::SVariableInfo info;
		if (pTOD->GetVariableInfo(variableId, info)) {
			return WeatherVariable{
				.value = info.fValue[0],
				.min = info.fValue[1],
				.max = info.fValue[2]
			};
		}
	}
	return {};
}

std::optional<WeatherVariable> CWeatherSystem::GetWeatherVariableRange(int variableId) {
	ITimeOfDay* pTOD = gEnv->p3DEngine->GetTimeOfDay();
	if (pTOD) {
		ITimeOfDay::SVariableInfo info;
		if (pTOD->GetVariableInfo(variableId, info)) {
			auto existing = original_weather_values.find(variableId);
			if (existing != original_weather_values.end()) {
				return WeatherVariable{
					.value = -1.0,
					.min = existing->second[1],
					.max = existing->second[2]
				};
			}
			return WeatherVariable{
				.value = -1.0,
				.min = info.fValue[1],
				.max = info.fValue[2]
			};
		}
	}
	return {};
}

bool CWeatherSystem::RestoreWeatherVariable(int variableId, bool update) {
	ITimeOfDay* pTOD = gEnv->p3DEngine->GetTimeOfDay();
	if (pTOD) {
		ITimeOfDay::SVariableInfo info;
		if (pTOD->GetVariableInfo(variableId, info)) {
			auto existing = original_weather_values.find(variableId);
			if (existing != original_weather_values.end()) {
				memcpy(info.fValue, original_weather_values[variableId], sizeof(info.fValue));
				original_weather_values.erase(existing);
			}
			pTOD->SetVariableValue(variableId, info.fValue);
			if (update) {
				pTOD->Update();
			}
			return true;
		}
	}
	return false;
}

bool CWeatherSystem::RestoreWeather(bool update) {
	ITimeOfDay* pTOD = gEnv->p3DEngine->GetTimeOfDay();
	if (pTOD) {
		ITimeOfDay::SVariableInfo info;
		for (auto& [variableId, value] : original_weather_values) {
			if (pTOD->GetVariableInfo(variableId, info)) {
				memcpy(info.fValue, value, sizeof(info.fValue));
				pTOD->SetVariableValue(variableId, info.fValue);
			}
		}
		original_weather_values.clear();
		if (update) {
			pTOD->Update();
		}
	}
	return false;
}

bool CWeatherSystem::UpdateWeather() {
	ITimeOfDay* pTOD = gEnv->p3DEngine->GetTimeOfDay();
	if (pTOD) {
		pTOD->Update();
		return true;
	}
	else {
		return false;
	}
}


void CWeatherSystem::SyncEffects(const char* effects) {
	std::stringstream ss;
	std::string item;
	std::set<std::string> effects_now;

	std::set<std::string> to_remove;
	std::set<std::string> to_add;

	ss << effects;

	while ((ss >> item) && item.length() > 0) {
		effects_now.insert(item);
	}

	for (auto& k : effects_now) {
		if (!active_effects.contains(k)) to_add.insert(k);
	}

	for (auto& k : active_effects) {
		if (!effects_now.contains(k)) to_remove.insert(k);
	}

	for (auto& k : to_add) {
		if (k[0] == '-') {
			RemoveEffect(k.c_str() + 1);
		}
		else {
			SpawnEffect(k.c_str());
		}
	}

	for (auto& k : to_remove) {
		if (k[0] == '-') {
			SpawnEffect(k.c_str() + 1);
		}
		else {
			RemoveEffect(k.c_str());
		}
	}

	active_effects = effects_now;
}

void CWeatherSystem::SpawnEffect(const char *eff) {
	std::string effect_name{ eff };
	SEffectInfo info = GetEffectInfo(eff);

	IParticleEffect* pEffect = gEnv->p3DEngine->FindParticleEffect(info.effect.c_str(), "", "Particle.SpawnEffect");
	if (pEffect) {
		auto emitters = active_emmiters.find(effect_name);
		if (emitters != active_emmiters.end()) {
			for (auto emitter : emitters->second) {
				emitter->Activate(true);
			}
		}
		else {
			pEffect->SetEnabled(true);
			Vec3& pos = info.pos;
			pos.z = gEnv->p3DEngine->GetTerrainElevation(pos.x, pos.y);
			for (unsigned i = 0; i < info.instances; i++) {
				if (i > 0) {
					pos.x += (((float)rand()) / RAND_MAX) * 4.0f - 2.0f;
					pos.y += (((float)rand()) / RAND_MAX) * 4.0f - 2.0f;
				}
				IParticleEmitter* pEmitter = pEffect->Spawn(true, IParticleEffect::ParticleLoc(pos, info.dir, info.scale));
				if (pEmitter) {
					active_emmiters[effect_name].push_back(pEmitter);
				}
			}
		}
	}
}

void CWeatherSystem::RemoveEffect(const char *eff) {
	std::string effect_name{ eff };
	SEffectInfo info = GetEffectInfo(eff);

	IParticleEffect* pEffect = gEnv->p3DEngine->FindParticleEffect(info.effect.c_str(), "", "Particle.SpawnEffect");
	if (pEffect) {
		auto emitters = active_emmiters.find(effect_name);
		if (emitters != active_emmiters.end()) {
			for (auto emitter : emitters->second) {
				emitter->Activate(false);
			}
			active_emmiters.erase(emitters);
		}
		else {
			pEffect->SetEnabled(false);
		}
	}
}

void CWeatherSystem::SyncLayers(const char* layers) {
	std::stringstream ss;
	std::string layer;
	ss << layers;

	std::optional<unsigned> mask;

	while ((ss >> layer) && layer.length() > 0) {
		if (layer == "frozen") {
			if (!mask) mask.emplace(static_cast<unsigned>(MTL_LAYER_FROZEN));
			else mask.emplace(*mask | static_cast<unsigned>(MTL_LAYER_FROZEN));
		}
		else if (layer == "wet") {
			if (!mask) mask.emplace(static_cast<unsigned>(MTL_LAYER_WET));
			else mask.emplace(*mask | static_cast<unsigned>(MTL_LAYER_WET));
		}
	}

	if (mask && (!active_mask || *active_mask != *mask)) {
		if (active_mask) RemoveLayer(*active_mask);
		ApplyLayer(*mask);
		active_mask = std::move(mask);
	} else if(!mask && active_mask) {
		RemoveLayer(*active_mask);
		active_mask.reset();
	}
}

void CWeatherSystem::ApplyLayer(unsigned layer) {
	for (auto type : static_entities) {
		uint32_t count = gEnv->p3DEngine->GetObjectsByType(type, NULL);
		if (count == 0) continue;
		std::vector<IRenderNode*> nodes;
		nodes.resize(count);
		count = gEnv->p3DEngine->GetObjectsByType(type, nodes.data());
		for (auto node : nodes) {
			node->SetMaterialLayers(node->GetMaterialLayers() | layer);
		}
	}

	for (int i = 0; i < 65536; i++) {
		IStatInstGroup group;
		if (gEnv->p3DEngine->GetStatInstGroup(i, group)) {
			if (!group.szFileName || strlen(group.szFileName) == 0 || !group.pStatObj) continue;

			std::string_view sv_name{ group.pStatObj->GetFilePath() };
			auto original = original_groups.find(i);
			if (original == original_groups.end()) {
				original_groups[i] = group;
			}
			group.nMaterialLayers = group.nMaterialLayers | layer;
			if (layer & MTL_LAYER_FROZEN) {
				// nicegrass and bigpatch need special treatment, so check for those
				// and if so, replace the underlying cgf model with a new model
				auto new_name = GetFrozenGrassName(sv_name);
				if (new_name != sv_name) {
					group.nMaterialLayers = group.nMaterialLayers & (~MTL_LAYER_FROZEN);
					strncpy(group.szFileName, new_name.c_str(), sizeof(group.szFileName) - 1);
					IStatObj* pClone = group.pStatObj->Clone();
					pClone->SetFilePath(group.szFileName);
					pClone->Refresh(FRO_GEOMETRY);
					group.pStatObj = pClone;
					group.pMaterial = NULL;
				} else {
					group.fBending = 0.0f;
				}
			}
			gEnv->p3DEngine->SetStatInstGroup(i, group);
		}
	}
}

void CWeatherSystem::RemoveLayer(unsigned layer) {
	for (auto type : static_entities) {
		uint32_t count = gEnv->p3DEngine->GetObjectsByType(type, NULL);
		if (count == 0) continue;
		std::vector<IRenderNode*> nodes;
		nodes.resize(count);
		count = gEnv->p3DEngine->GetObjectsByType(type, nodes.data());
		for (auto node : nodes) {
			node->SetMaterialLayers(node->GetMaterialLayers() & (~layer));
		}
	}

	for (int i = 0; i < 65536; i++) {
		IStatInstGroup group;
		if (gEnv->p3DEngine->GetStatInstGroup(i, group)) {
			if (!group.szFileName || strlen(group.szFileName) == 0 || !group.pStatObj) continue;

			std::string_view sv_name{ group.pStatObj->GetFilePath() };
			auto pStatBefore = group.pStatObj;
			auto original = original_groups.find(i);
			if (original != original_groups.end()) {
				group = original->second;
				original_groups.erase(original);
			} else {
				// this shouldn't ever happen, but just in case it occurs
				// handle it by replacing original grasses back
				group.nMaterialLayers = group.nMaterialLayers & (~layer);
				if (layer & MTL_LAYER_FROZEN) {
					auto new_name = GetNormalGrassName(sv_name);
					if (new_name != sv_name) {
						strncpy(group.szFileName, new_name.c_str(), sizeof(group.szFileName) - 1);
						IStatObj* pClone = group.pStatObj->Clone();
						pClone->SetFilePath(group.szFileName);
						pClone->Refresh(FRO_GEOMETRY);
						group.pStatObj = pClone;
					}
				}
			}

			if (pStatBefore != group.pStatObj) {
				// if stat object changed (nicegrass handling), force bUseTerrainColor
				group.bUseTerrainColor = true;
			}

			gEnv->p3DEngine->SetStatInstGroup(i, group);

		}
	}
}

bool CWeatherSystem::IsWet() const {
	return active_mask && ((*active_mask & MTL_LAYER_WET) == MTL_LAYER_WET);
}

bool CWeatherSystem::IsFrozen() const {
	return active_mask && ((*active_mask & MTL_LAYER_FROZEN) == MTL_LAYER_FROZEN);
}

//------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------

static std::string GetFrozenGrassName(std::string_view path) {
	auto pos = path.rfind("/nicegrass");
	if (pos != std::string::npos) {
		std::string name;
		pos += 1;
		name += path.substr(0, pos);
		name += "frozen_";
		name += path.substr(pos);
		return name;
	}
	pos = path.rfind("/bigpatch");
	if (pos != std::string::npos) {
		pos = path.rfind("._frozen.cgf");
		if (pos != std::string::npos) {
			return std::string{ path };
		}
		pos = path.rfind(".cgf");
		if (pos != std::string::npos) {
			std::string name;
			name += path.substr(0, pos);
			name += "_frozen.cgf";
			return name;
		}
	}
	return std::string{ path };
}

static std::string GetNormalGrassName(std::string_view path) {
	auto pos = path.rfind("/frozen_nicegrass");
	if (pos != std::string::npos) {
		std::string name;
		pos += 1;
		name += path.substr(0, pos);
		name += path.substr(pos + 7);
		return name;
	}
	pos = path.rfind("/bigpatch");
	if (pos != std::string::npos) {
		pos = path.rfind("_frozen.cgf");
		if (pos != std::string::npos) {
			std::string name;
			name += path.substr(0, pos);
			name += ".cgf";
			return name;
		}
	}
	return std::string{ path };
}

static SEffectInfo GetEffectInfo(std::string_view effect) {
	/*
		Parse effect info from the following format "effect_name;key1=value1;key2=value2;key3=value3..."
		Valid keys are:
			x=123.45 (scale)
			i=123 (instances)
			p=123.45,123.45,123.45 (position)
			d=123.45,123.45,123.45 (direction)
	*/
	auto pos = effect.find(";");
	float x, y, z;
	unsigned u = 1;
	if (pos == std::string::npos) {
		return SEffectInfo{
			.effect = std::string{ effect }
		};
	}
	else {
		auto effect_name = effect.substr(0, pos);
		auto tail = effect.substr(pos + 1);
		SEffectInfo info{
			.effect = std::string{ effect_name }
		};
		do {
			pos = tail.find(";");
			auto kv{ tail };
			if (pos != std::string::npos) {
				kv = kv.substr(0, pos);
				tail = tail.substr(pos + 1);
			}
			auto eq = kv.find("=");
			if (eq == std::string::npos) break;
			auto key = kv.substr(0, eq);
			std::string value{ kv.substr(eq + 1) };

			if (key == "x" && sscanf(value.c_str(), "%f", &x) == 1) {
				info.scale = x;
			}
			else if (key == "p" && sscanf(value.c_str(), "%f,%f,%f", &x, &y, &z) == 3) {
				info.pos = Vec3(x, y, z);
			}
			else if (key == "d" && sscanf(value.c_str(), "%f,%f,%f", &x, &y, &z) == 3) {
				info.dir = Vec3(x, y, z);
			}
			else if (key == "i" && sscanf(value.c_str(), "%u", &u) == 1) {
				info.instances = u;
			}
		} while (pos != std::string::npos);
		return info;
	}
}