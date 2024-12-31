#pragma once
#include <utility>
#include <optional>

struct WeatherVariable {
	float value;
	float min;
	float max;
};

class CWeatherSystem {
public:
	CWeatherSystem();
	void Update(float frameTime);
	void Reset(bool deapply = true);

	bool IsFrozen() const;
	bool IsWet() const;
private:
	bool SetWeatherVariable(int variableId, float value, bool interpolate = true, bool update = false);
	bool SetWeatherVariable(int variableId, float x, float y, float z, bool update = false);
	std::optional<WeatherVariable> GetWeatherVariable(int variableId);
	std::optional<WeatherVariable> GetWeatherVariableRange(int variableId);
	bool RestoreWeatherVariable(int variableId, bool update = false);
	bool RestoreWeather(bool update = false);
	bool UpdateWeather();

	void SyncEffects(const char *effects);
	void SpawnEffect(const char *eff);
	void RemoveEffect(const char *eff);

	void SyncLayers(const char* layers);
	void ApplyLayer(unsigned layer);
	void RemoveLayer(unsigned layer);

	float m_time;
	float m_lastUpdate;
};