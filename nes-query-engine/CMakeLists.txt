# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#    https://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

add_library(nes-query-engine QueryEngine.cpp RunningQueryPlan.cpp RunningSource.cpp QueryEngineConfiguration.cpp Task.cpp)
target_include_directories(nes-query-engine
        PUBLIC include
        PRIVATE .
)

# Set default ENGINE_LOG_LEVEL to ERROR if not specified. Set via: -DENGINE_LOG_LEVEL=TRACE
if(NOT DEFINED ENGINE_LOG_LEVEL)
    set(ENGINE_LOG_LEVEL "ERROR" CACHE STRING "Log level for query engine (TRACE, DEBUG, INFO, WARNING, ERROR)")
endif()

# Add definition to the target
target_compile_definitions(nes-query-engine PRIVATE ENGINE_LOG_LEVEL_${ENGINE_LOG_LEVEL})

find_package(folly CONFIG REQUIRED)
target_link_libraries(nes-query-engine
        PUBLIC nes-common nes-executable
        PRIVATE folly::folly
)

add_tests_if_enabled(tests)
