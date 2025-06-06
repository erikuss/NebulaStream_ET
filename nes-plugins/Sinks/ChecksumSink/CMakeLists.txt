# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#    https://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include(${PROJECT_SOURCE_DIR}/cmake/PluginRegistrationUtil.cmake)
add_plugin_as_library(Checksum Sink nes-sinks-registry checksum_sink_plugin ChecksumSink.cpp)
add_plugin_as_library(Checksum SinkValidation nes-sinks-registry checksum_sink_validation_plugin ChecksumSink.cpp)

target_include_directories(checksum_sink_plugin
        PUBLIC include
        PRIVATE .
)
target_include_directories(checksum_sink_validation_plugin
        PUBLIC include
        PRIVATE .
)

target_link_libraries(checksum_sink_plugin PRIVATE systest-checksum)
target_link_libraries(checksum_sink_validation_plugin PRIVATE systest-checksum)
