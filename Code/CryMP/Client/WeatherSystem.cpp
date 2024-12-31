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

static bool                          currently_enabled = false;
static std::map<int, float[3]>       original_weather_values;
static std::optional<Vec3>           original_wind;
static std::string                   original_map_name;
static std::map<int, IStatInstGroup> original_groups;

static std::map<int, string>    active_values;
static std::set<std::string>    active_effects;
static std::optional<unsigned>  active_mask;

static constexpr std::array<EERType, 3> static_entities = {
	eERType_Brush,
	eERType_RoadObject_NEW,
	eERType_Rope
};

static const int WEATHER_NAMESPACE		= 2000;
static const int WEATHER_ENV_NAMESPACE  = 2100;

static std::string GetFrozenGrassName(std::string_view path);
static std::string GetNormalGrassName(std::string_view path);

enum EWeatherVars {
	WEATHER_ENABLED,
	WEATHER_WIND,
	WEATHER_FX,
	WEATHER_LAYERS,
	WEATHER_VAR_COUNT
};

CWeatherSystem::CWeatherSystem() {
	original_weather_values.clear();
	m_time = 0;
	m_lastUpdate = -1000.0f;
}

void CWeatherSystem::Reset() {
	RestoreWeather(true);
	if (original_wind) {
		gEnv->p3DEngine->SetWind(*original_wind);
	}

	for (auto& k : active_effects) {
		if (k[0] == '-') SpawnEffect(k.c_str() + 1);
		else RemoveEffect(k.c_str());
	}

	if (active_mask) {
		RemoveLayer(*active_mask);
		active_mask.reset();
	}
	
	currently_enabled = false;
	original_weather_values.clear();
	original_wind.reset();
	original_groups.clear();
	active_effects.clear();
	active_values.clear();
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
						if (value[0] >= 'a' && value[0] <= 'z') {
							if (value[0] == 'i') {
								SetWeatherVariable(i, atof(value.c_str() + 1), true);
							} else if (value[0] == 'r') {
								SetWeatherVariable(i, atof(value.c_str() + 1), false);
							} else if (value[0] == 'v') {
								float x, y, z;
								if (sscanf(value.c_str() + 1, "%f,%f,%f", &x, &y, &z) == 3) {
									SetWeatherVariable(i, x, y, z, false);
								}
							}
						} else {
							SetWeatherVariable(i, atof(value.c_str()), true);
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
			for (int i = WEATHER_ENABLED + 1; i < WEATHER_VAR_COUNT; i++) {
				string value;
				float f3[3];
				auto existing = active_values.find(WEATHER_NAMESPACE + i);
				if (pSSS->GetGlobalValue(WEATHER_NAMESPACE + i, value)) {
					if (existing == active_values.end() || existing->second != value) {
						switch (i) {
						case WEATHER_WIND:
							if (value[0] == 'v' && sscanf(value.c_str() + 1, "%f,%f,%f", f3 + 0, f3 + 1, f3 + 2) == 3) {
								if (!original_wind) original_wind = gEnv->p3DEngine->GetWind(AABB(), false);
								gEnv->p3DEngine->SetWind(Vec3(f3[0], f3[1], f3[2]));
							} else if (sscanf(value.c_str(), "%f", f3 + 0) == 1) {
								if (!original_wind) original_wind = gEnv->p3DEngine->GetWind(AABB(), false);
								gEnv->p3DEngine->SetWind((*original_wind) * f3[0]);
							} else {
								if (original_wind) gEnv->p3DEngine->SetWind(*original_wind);
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
						changed++;
					}
				}
			}
			if (changed > 0) {
				pTOD->Update();
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
	IParticleEffect* pEffect = gEnv->p3DEngine->FindParticleEffect(eff, "", "Particle.SpawnEffect");
	if (pEffect)
	{
		float scale = 1.0f;
		ScriptHandle entityId(0);
		int partId = 0;

		pEffect->SetEnabled(true);
		pEffect->Spawn(true, IParticleEffect::ParticleLoc(Vec3(512.0f, 512.0f, 64.0f), Vec3(0.0f, 0.0f, 1.0f), scale));
	}
}

void CWeatherSystem::RemoveEffect(const char *eff) {
	IParticleEffect* pEffect = gEnv->p3DEngine->FindParticleEffect(eff, "", "Particle.SpawnEffect");
	if (pEffect)
	{
		pEffect->SetEnabled(false);
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

	auto current_map_name = g_pGame->GetIGameFramework()->GetLevelName();
	if (!current_map_name) {
		original_groups.clear();
		return;
	} else if (current_map_name != original_map_name) {
		original_map_name = current_map_name;
		original_groups.clear();
	}
	for (int i = 0; i < 65536; i++) {
		IStatInstGroup group;
		if (gEnv->p3DEngine->GetStatInstGroup(i, group)) {
			// ignore not a real groups
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
					group.pStatObj->SetFilePath(group.szFileName);
					group.pStatObj->Refresh(FRO_GEOMETRY);
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

	auto current_map_name = g_pGame->GetIGameFramework()->GetLevelName();
	if (!current_map_name || original_map_name != current_map_name) {
		original_groups.clear();
	}
	else {
		for (int i = 0; i < 65536; i++) {
			IStatInstGroup group;
			if (gEnv->p3DEngine->GetStatInstGroup(i, group)) {
				// ignore not a real groups
				if (!group.szFileName || strlen(group.szFileName) == 0 || !group.pStatObj) continue;

				std::string_view sv_name{ group.pStatObj->GetFilePath() };
				auto original = original_groups.find(i);
				if (original != original_groups.end()) {
					group = original->second;
				} else {
					group.nMaterialLayers = group.nMaterialLayers & (~layer);
				}

				if (layer & MTL_LAYER_FROZEN) {
					auto new_name = GetNormalGrassName(sv_name);
					if (new_name != sv_name) {
						strncpy(group.szFileName, new_name.c_str(), sizeof(group.szFileName) - 1);
						group.pStatObj->SetFilePath(group.szFileName);
						group.pStatObj->Refresh(FRO_GEOMETRY);
					}
				}

				gEnv->p3DEngine->SetStatInstGroup(i, group);

			}
		}
	}
}

bool CWeatherSystem::IsWet() const {
	return active_mask && ((*active_mask & MTL_LAYER_WET) == MTL_LAYER_WET);
}

bool CWeatherSystem::IsFrozen() const {
	return active_mask && ((*active_mask & MTL_LAYER_FROZEN) == MTL_LAYER_FROZEN);
}

/// Utilities

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