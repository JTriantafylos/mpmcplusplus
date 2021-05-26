/*
 * test_mpscplusplus.cpp - Test code for mpscplusplus
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

#include <thread>

#include "mpscplusplus/mpscplusplus.h"

TEST_SUITE("queue") {
    TEST_CASE("popping") {
        SUBCASE("from empty queue") {
            mpscplusplus::Queue<int> queue;

            int val;
            CHECK(queue.pop(val) == false);
        }

        SUBCASE("from non-empty queue") {
            mpscplusplus::Queue<int> queue;
            int test_val = 1;

            queue.push(test_val);

            int val;
            CHECK(queue.pop(val) == true);
            CHECK(val == test_val);
        }
    }

    TEST_CASE("waiting and popping" *
              doctest::description("all subcases should take approxomately 50ms or less to execute")) {
        SUBCASE("from empty queue") {
            mpscplusplus::Queue<int> queue;

            int val;
            // TODO: Come up with a better way to test timeout functionality
            queue.wait_and_pop(val, std::chrono::milliseconds(50));
        }

        SUBCASE("from non-empty queue") {
            mpscplusplus::Queue<int> queue;
            int test_val = 1;

            queue.push(test_val);

            int val;
            queue.wait_and_pop(val, std::chrono::milliseconds(25));
            CHECK(val == test_val);
            queue.wait_and_pop(val, std::chrono::milliseconds(25));
        }
    }

    TEST_CASE("sequential pushing and popping") {
        SUBCASE("using lvalue primitives") {
            mpscplusplus::Queue<int> queue;

            for (int i = 0; i < 10; i++) {
                int val = i;
                queue.push(val);
            }

            for (int i = 0; i < 10; i++) {
                int val;
                queue.pop(val);
                CHECK(val == i);
            }
        }

        SUBCASE("using const lvalue primitives") {
            mpscplusplus::Queue<int> queue;

            for (int i = 0; i < 10; i++) {
                const int val = i;
                queue.push(val);
            }

            for (int i = 0; i < 10; i++) {
                int val;
                queue.pop(val);
                CHECK(val == i);
            }
        }

        SUBCASE("using rvalue primitives") {
            mpscplusplus::Queue<int> queue;

            for (int i = 0; i < 10; i++) {
                queue.push(int(i));
            }

            for (int i = 0; i < 10; i++) {
                int val;
                queue.pop(val);
                CHECK(val == i);
            }
        }

        SUBCASE("using lvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            for (int i = 0; i < 10; i++) {
                std::string string = std::to_string(i);
                queue.push(string);
            }

            for (int i = 0; i < 10; i++) {
                std::string val;
                queue.pop(val);
                CHECK(val == std::to_string(i));
            }
        }

        SUBCASE("using const lvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            for (int i = 0; i < 10; i++) {
                const std::string string = std::to_string(i);
                queue.push(string);
            }

            for (int i = 0; i < 10; i++) {
                std::string val;
                queue.pop(val);
                CHECK(val == std::to_string(i));
            }
        }

        SUBCASE("using rvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            for (int i = 0; i < 10; i++) {
                queue.push(std::to_string(i));
            }

            for (int i = 0; i < 10; i++) {
                std::string val;
                queue.pop(val);
                CHECK(val == std::to_string(i));
            }
        }
    }

    TEST_CASE("multi-threaded pushing and popping") {
        SUBCASE("using lvalue primitives") {
            mpscplusplus::Queue<int> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    int val = i;
                    queue.push(val);
                }
            });

            push_thread.join();

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    int val;
                    queue.pop(val);
                    CHECK(val == i);
                }
            });

            pop_thread.join();
        }

        SUBCASE("using const lvalue primitives") {
            mpscplusplus::Queue<int> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    const int val = i;
                    queue.push(val);
                }
            });

            push_thread.join();

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    int val;
                    queue.pop(val);
                    CHECK(val == i);
                }
            });

            pop_thread.join();
        }

        SUBCASE("using rvalue primitives") {
            mpscplusplus::Queue<int> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    queue.push(int(i));
                }
            });

            push_thread.join();

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    int val;
                    queue.pop(val);
                    CHECK(val == i);
                }
            });

            pop_thread.join();
        }

        SUBCASE("using lvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    std::string string = std::to_string(i);
                    queue.push(string);
                }
            });

            push_thread.join();

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    std::string val;
                    queue.pop(val);
                    CHECK(val == std::to_string(i));
                }
            });

            pop_thread.join();
        }

        SUBCASE("using const lvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    const std::string string = std::to_string(i);
                    queue.push(string);
                }
            });

            push_thread.join();

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    std::string val;
                    queue.pop(val);
                    CHECK(val == std::to_string(i));
                }
            });

            pop_thread.join();
        }

        SUBCASE("using rvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    queue.push(std::to_string(i));
                }
            });

            push_thread.join();

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    std::string val;
                    queue.pop(val);
                    CHECK(val == std::to_string(i));
                }
            });

            pop_thread.join();
        }
    }

    TEST_CASE("concurrent pushing and popping with waiting") {
        SUBCASE("using lvalue primitives") {
            mpscplusplus::Queue<int> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    int val = i;
                    queue.push(val);
                }
            });

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    int val;
                    queue.wait_and_pop(val);
                    CHECK(val == i);
                }
            });

            push_thread.join();
            pop_thread.join();
        }

        SUBCASE("using const lvalue primitives") {
            mpscplusplus::Queue<int> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    const int val = i;
                    queue.push(val);
                }
            });

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    int val;
                    queue.wait_and_pop(val);
                    CHECK(val == i);
                }
            });

            push_thread.join();
            pop_thread.join();
        }

        SUBCASE("using rvalue primitives") {
            mpscplusplus::Queue<int> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    queue.push(int(i));
                }
            });

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    int val;
                    queue.wait_and_pop(val);
                    CHECK(val == i);
                }
            });

            push_thread.join();
            pop_thread.join();
        }

        SUBCASE("using lvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    std::string string = std::to_string(i);
                    queue.push(string);
                }
            });

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    std::string val;
                    queue.wait_and_pop(val);
                    CHECK(val == std::to_string(i));
                }
            });

            push_thread.join();
            pop_thread.join();
        }

        SUBCASE("using const lvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    const std::string string = std::to_string(i);
                    queue.push(string);
                }
            });

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    std::string val;
                    queue.wait_and_pop(val);
                    CHECK(val == std::to_string(i));
                }
            });

            push_thread.join();
            pop_thread.join();
        }

        SUBCASE("using rvalue objects") {
            mpscplusplus::Queue<std::string> queue;

            std::thread push_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    queue.push(std::to_string(i));
                }
            });

            std::thread pop_thread([&queue]() {
                for (int i = 0; i < 10; i++) {
                    std::string val;
                    queue.wait_and_pop(val);
                    CHECK(val == std::to_string(i));
                }
            });

            push_thread.join();
            pop_thread.join();
        }
    }
}