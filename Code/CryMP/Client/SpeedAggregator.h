#pragma once
#include <queue>
class SpeedAggregator {
	unsigned long long m_window = 1000;
	unsigned long long m_sum = 0;
	unsigned long long m_lastSum = 0;
	std::queue<unsigned long long> m_time;
	std::queue<unsigned long long> m_chunks;
public:
	SpeedAggregator(unsigned long long windowMs = 1000) : m_window(windowMs)
	{

	}

	void push(unsigned long long chunkSize);
	void reset();
	double getSpeed() const;
};