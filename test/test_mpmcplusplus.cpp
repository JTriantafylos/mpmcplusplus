/*
 * test_mpmcplusplus.cpp - Test code for mpmcplusplus
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <atomic>
#include <thread>

#include "mpmcplusplus/mpmcplusplus.h"

TEST_SUITE("queue") {
    TEST_CASE("creating a queue") { mpmcplusplus::Queue<int> q; }

    TEST_CASE("popping from empty queue") {
        mpmcplusplus::Queue<int> q;

        int result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("pushing one lvalue") {
        mpmcplusplus::Queue<int> q;
        int val = 10;

        CHECK(q.push(val));
    }

    TEST_CASE("pushing and popping one lvalue") {
        mpmcplusplus::Queue<int> q;
        int val = 10;

        REQUIRE(q.push(val));

        int result;
        REQUIRE(q.pop(result));
        CHECK(result == val);
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("pushing one const lvalue") {
        mpmcplusplus::Queue<int> q;
        const int val = 10;

        CHECK(q.push(val));
    }

    TEST_CASE("pushing and popping one const lvalue") {
        mpmcplusplus::Queue<int> q;
        const int val = 10;

        REQUIRE(q.push(val));

        int result;
        REQUIRE(q.pop(result));
        CHECK(result == val);
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("pushing one rvalue") {
        mpmcplusplus::Queue<int> q;

        CHECK(q.push(10));
    }

    TEST_CASE("pushing and popping one rvalue") {
        mpmcplusplus::Queue<int> q;

        REQUIRE(q.push(10));

        int result;
        REQUIRE(q.pop(result));
        CHECK(result == 10);
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("pushing and then popping multiple values") {
        mpmcplusplus::Queue<int> q;

        for (int i = 0; i < 10000; ++i) {
            REQUIRE(q.push(i));
        }

        int result;
        for (int i = 0; i < 10000; ++i) {
            REQUIRE(q.pop(result));
            REQUIRE(result == i);
        }
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("interleaved pushing and popping") {
        mpmcplusplus::Queue<int> q;

        int result;
        for (int i = 0; i < 10000; ++i) {
            REQUIRE(q.push(i));
            REQUIRE(q.push(i + 1));
            REQUIRE(q.push(i + 2));

            REQUIRE(q.pop(result));
            REQUIRE(result == i);
            REQUIRE(q.pop(result));
            REQUIRE(result == i + 1);
            REQUIRE(q.pop(result));
            REQUIRE(result == i + 2);
        }
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("threaded pushing and popping") {
        mpmcplusplus::Queue<int> q;

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.push(i));
            }
        });
        push_thread.join();

        std::thread pop_thread([&q]() {
            int result;
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.pop(result));
                REQUIRE(result == i);
            }
        });
        pop_thread.join();

        int result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("single producer single consumer concurrently pushing and popping") {
        mpmcplusplus::Queue<int> q;

        std::thread pop_thread([&q]() {
            int popped_count = 0;
            int result;
            while (popped_count < 10000) {
                if (q.pop(result)) {
                    REQUIRE(result == popped_count);
                    popped_count++;
                }
            }
        });

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.push(i));
            }
        });

        pop_thread.join();
        push_thread.join();

        int result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("single producer single consumer concurrently pushing and popping with waiting") {
        mpmcplusplus::Queue<int> q;

        std::thread pop_thread([&q]() {
            int result;
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE(result == i);
            }
        });

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.push(i));
            }
        });

        pop_thread.join();
        push_thread.join();

        int result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("multi consumer single producer concurrently pushing and popping with waiting") {
        mpmcplusplus::Queue<int> q;
        std::atomic<int> popped_count(0);

        std::thread pop_thread_1([&q, &popped_count]() {
            int result;
            while (popped_count.fetch_add(1) < 10000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE(result == 1);
            }
        });

        std::thread pop_thread_2([&q, &popped_count]() {
            int result;
            while (popped_count.fetch_add(1) < 10000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE(result == 1);
            }
        });

        std::thread pop_thread_3([&q, &popped_count]() {
            int result;
            while (popped_count.fetch_add(1) < 10000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE(result == 1);
            }
        });

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.push(1));
            }
        });

        pop_thread_1.join();
        pop_thread_2.join();
        pop_thread_3.join();
        push_thread.join();

        int result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("multi consumer multi producer concurrently pushing and popping with waiting") {
        mpmcplusplus::Queue<int> q;
        std::atomic<int> popped_count(0);

        std::thread pop_thread_1([&q, &popped_count]() {
            int result;
            while (popped_count.fetch_add(1) < 30000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE((result == 1 || result == 2 || result == 3));
            }
        });

        std::thread pop_thread_2([&q, &popped_count]() {
            int result;
            while (popped_count.fetch_add(1) < 30000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE((result == 1 || result == 2 || result == 3));
            }
        });

        std::thread pop_thread_3([&q, &popped_count]() {
            int result;
            while (popped_count.fetch_add(1) < 30000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE((result == 1 || result == 2 || result == 3));
            }
        });

        std::thread push_thread_1([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.push(1));
            }
        });

        std::thread push_thread_2([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.push(2));
            }
        });

        std::thread push_thread_3([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.push(3));
            }
        });

        pop_thread_1.join();
        pop_thread_2.join();
        pop_thread_3.join();
        push_thread_1.join();
        push_thread_2.join();
        push_thread_3.join();

        int result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("popping from empty queue with waiting and timeout") {
        mpmcplusplus::Queue<int> q;
        std::chrono::milliseconds duration(10);

        int result;
        REQUIRE_FALSE(q.wait_and_pop(result, duration));
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("threaded pushing and popping with waiting and timeout") {
        mpmcplusplus::Queue<int> q;
        std::chrono::milliseconds duration(10);

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.push(i));
            }
        });
        push_thread.join();

        std::thread pop_thread([&q, &duration]() {
            int result;
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.wait_and_pop(result, duration));
                REQUIRE(result == i);
            }
            REQUIRE_FALSE(q.wait_and_pop(result, duration));
        });
        pop_thread.join();

        int result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("emplacing one value") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;

        CHECK(q.emplace(new int(10)));
    }

    TEST_CASE("emplacing and popping one value") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;

        REQUIRE(q.emplace(new int(10)));

        std::unique_ptr<int> result;
        REQUIRE(q.pop(result));
        CHECK(*result == 10);
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("emplacing and then popping multiple values") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;

        for (int i = 0; i < 10000; ++i) {
            REQUIRE(q.emplace(new int(i)));
        }

        std::unique_ptr<int> result;
        for (int i = 0; i < 10000; ++i) {
            REQUIRE(q.pop(result));
            REQUIRE(*result == i);
        }
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("interleaved emplacing and popping") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;

        std::unique_ptr<int> result;
        for (int i = 0; i < 10000; ++i) {
            REQUIRE(q.emplace(new int(i)));
            REQUIRE(q.emplace(new int(i + 1)));
            REQUIRE(q.emplace(new int(i + 2)));

            REQUIRE(q.pop(result));
            REQUIRE(*result == i);
            REQUIRE(q.pop(result));
            REQUIRE(*result == i + 1);
            REQUIRE(q.pop(result));
            REQUIRE(*result == i + 2);
        }
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("threaded pushing and popping") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.emplace(new int(i)));
            }
        });
        push_thread.join();

        std::thread pop_thread([&q]() {
            std::unique_ptr<int> result;
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.pop(result));
                REQUIRE(*result == i);
            }
        });
        pop_thread.join();

        std::unique_ptr<int> result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("single producer single consumer concurrently pushing and popping") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;

        std::thread pop_thread([&q]() {
            int popped_count = 0;
            std::unique_ptr<int> result;
            while (popped_count < 10000) {
                if (q.pop(result)) {
                    REQUIRE(*result == popped_count);
                    popped_count++;
                }
            }
        });

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.emplace(new int(i)));
            }
        });

        pop_thread.join();
        push_thread.join();

        std::unique_ptr<int> result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("single producer single consumer concurrently pushing and popping with waiting") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;

        std::thread pop_thread([&q]() {
            std::unique_ptr<int> result;
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE(*result == i);
            }
        });

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.emplace(new int(i)));
            }
        });

        pop_thread.join();
        push_thread.join();

        std::unique_ptr<int> result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("multi consumer single producer concurrently pushing and popping with waiting") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;
        std::atomic<int> popped_count(0);

        std::thread pop_thread_1([&q, &popped_count]() {
            std::unique_ptr<int> result;
            while (popped_count.fetch_add(1) < 10000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE(*result == 1);
            }
        });

        std::thread pop_thread_2([&q, &popped_count]() {
            std::unique_ptr<int> result;
            while (popped_count.fetch_add(1) < 10000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE(*result == 1);
            }
        });

        std::thread pop_thread_3([&q, &popped_count]() {
            std::unique_ptr<int> result;
            while (popped_count.fetch_add(1) < 10000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE(*result == 1);
            }
        });

        std::thread push_thread([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.emplace(new int(1)));
            }
        });

        pop_thread_1.join();
        pop_thread_2.join();
        pop_thread_3.join();
        push_thread.join();

        std::unique_ptr<int> result;
        CHECK_FALSE(q.pop(result));
    }

    TEST_CASE("multi consumer multi producer concurrently pushing and popping with waiting") {
        mpmcplusplus::Queue<std::unique_ptr<int>> q;
        std::atomic<int> popped_count(0);

        std::thread pop_thread_1([&q, &popped_count]() {
            std::unique_ptr<int> result;
            while (popped_count.fetch_add(1) < 30000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE((*result == 1 || *result == 2 || *result == 3));
            }
        });

        std::thread pop_thread_2([&q, &popped_count]() {
            std::unique_ptr<int> result;
            while (popped_count.fetch_add(1) < 30000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE((*result == 1 || *result == 2 || *result == 3));
            }
        });

        std::thread pop_thread_3([&q, &popped_count]() {
            std::unique_ptr<int> result;
            while (popped_count.fetch_add(1) < 30000) {
                REQUIRE(q.wait_and_pop(result));
                REQUIRE((*result == 1 || *result == 2 || *result == 3));
            }
        });

        std::thread push_thread_1([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.emplace(new int(1)));
            }
        });

        std::thread push_thread_2([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.emplace(new int(2)));
            }
        });

        std::thread push_thread_3([&q]() {
            for (int i = 0; i < 10000; ++i) {
                REQUIRE(q.emplace(new int(3)));
            }
        });

        pop_thread_1.join();
        pop_thread_2.join();
        pop_thread_3.join();
        push_thread_1.join();
        push_thread_2.join();
        push_thread_3.join();

        std::unique_ptr<int> result;
        CHECK_FALSE(q.pop(result));
    }
}