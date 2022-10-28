#include "SpeedAggregator.h"
#include <chrono>

void SpeedAggregator::push(unsigned long long chunkSize) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    if (m_time.size() > 0) {
        auto first = m_time.front();
        auto diff = millis - first;
        if (diff > m_window) {
            auto firstChunk = m_chunks.front();
            m_sum -= firstChunk;
            m_chunks.pop();
            m_time.pop();
        }
    }
    m_sum += chunkSize;
    m_time.push(millis);
    m_chunks.push(chunkSize);
}

void SpeedAggregator::reset() {
    while (m_chunks.size()) m_chunks.pop();
    while (m_time.size()) m_time.pop();
    m_sum = 0;
}

double SpeedAggregator::getSpeed() const {
    return 1000.0 * (double)m_sum / (double)m_window;
}