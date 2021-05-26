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

/**
 * The namespace encapsulating all mpscplusplus functionality.
 */
namespace mpscplusplus {
    /**
     * A wrapper structure around std::queue to allow for thread-safe operations.
     * Uses a @c std::mutex and a @c std::condition_variable to accomplish this.
     * @tparam T The type of object the queue will be storing.
     */
    template <typename T>
    class Queue {
      private:
        std::queue<T> m_backing_queue;
        mutable std::mutex m_mutex;
        std::condition_variable m_condition_variable;

      public:
        /**
         * Pushes the given object to the back of the queue.
         * @param[in] data The const lvalue reference to be pushed to the queue.
         */
        void push(const T& data) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_backing_queue.push(data);
            lock.unlock();
            m_condition_variable.notify_one();
        };

        /**
         * Pushes the given object to the back of the queue.
         * @param[in] data The rvalue reference to be pushed to the queue.
         */
        void push(T&& data) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_backing_queue.push(data);
            lock.unlock();
            m_condition_variable.notify_one();
        };

        /**
         * Pops an object from the front of the queue without blocking. This function will return immediately if the
         * queue is empty.
         * @param[out] data A reference to where the popped object will be stored.
         * @return true if an object was popped from the front of the queue, otherwise false.
         */
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

        /**
         * Pops an object from the front of the queue. This function will wait indefinitely for an object to be pushed
         * to the queue if the queue is empty.
         * @param[out] data A reference to where the popped object will be stored.
         */
        void wait_and_pop(T& data) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_backing_queue.empty()) {
                m_condition_variable.wait(lock);
            }
            data = m_backing_queue.front();
            m_backing_queue.pop();
        };

        /**
         * Pops an object from the front of the queue. This function will wait for as long as the specified timeout for
         * an object to be pushed to the queue if the queue is empty.
         * @param[out] data A reference to where the popped object will be stored.
         * @param[in] timeout A reference to a duration of how long this function should wait before returning.
         */
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
