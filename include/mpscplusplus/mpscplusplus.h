/*
 * mpscplusplus.h - Multi Producer Single Consumer queue implementation
 * Copyright (C) 2021 James Triantafylos
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MPSCPLUSPLUS_H
#define MPSCPLUSPLUS_H

#include <condition_variable>
#include <mutex>
#include <queue>

namespace mpscplusplus {
    template <typename T>
    class Queue {
      private:
        std::queue<T> m_backing_queue;
        mutable std::mutex m_mutex;
        std::condition_variable m_condition_variable;

      public:
        void push(const T& data) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_backing_queue.push(data);
            lock.unlock();
            m_condition_variable.notify_one();
        };

        void push(T&& data) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_backing_queue.push(data);
            lock.unlock();
            m_condition_variable.notify_one();
        };

        bool pop(T& data) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_backing_queue.empty()) {
                return false;
            } else {
                data = m_backing_queue.front();
                m_backing_queue.pop();
                return true;
            }
        };

        void wait_and_pop(T& data) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_backing_queue.empty()) {
                m_condition_variable.wait(lock);
            }
            data = m_backing_queue.front();
            m_backing_queue.pop();
        };

        template <typename Rep, typename Period>
        void wait_and_pop(T& data, const std::chrono::duration<Rep, Period>& timeout) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_backing_queue.empty()) {
                m_condition_variable.wait_for(lock, timeout);
            }
            data = m_backing_queue.front();
            m_backing_queue.pop();
        }
    };
}

#endif
