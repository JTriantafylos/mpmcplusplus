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

#include "mpscplusplus/mpscplusplus.h"

TEST_SUITE("queue") {
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
}