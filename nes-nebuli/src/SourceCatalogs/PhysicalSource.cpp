/*
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        https://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <DataTypes/Schema.hpp>
#include <Identifiers/Identifiers.hpp>
#include <SourceCatalogs/PhysicalSource.hpp>
#include <Sources/SourceDescriptor.hpp>

namespace NES
{

PhysicalSource::PhysicalSource(std::string logicalSourceName, Sources::SourceDescriptor sourceDescriptor)
    : logicalSourceName(std::move(logicalSourceName)), sourceDescriptor(std::move(sourceDescriptor))
{
}

std::shared_ptr<PhysicalSource> PhysicalSource::create(Sources::SourceDescriptor sourceDescriptor)
{
    const auto logicalSourceName = sourceDescriptor.logicalSourceName;
    return std::make_shared<PhysicalSource>(PhysicalSource(logicalSourceName, std::move(sourceDescriptor)));
}

std::string PhysicalSource::toString() const
{
    std::stringstream ss;
    ss << "LogicalSource Name" << logicalSourceName;
    ss << "Source Type" << sourceDescriptor;
    return ss.str();
}

const std::string& PhysicalSource::getLogicalSourceName() const
{
    return logicalSourceName;
}

std::unique_ptr<Sources::SourceDescriptor> PhysicalSource::createSourceDescriptor(Schema schema) const
{
    auto copyOfConfig = sourceDescriptor.config;
    return std::make_unique<Sources::SourceDescriptor>(
        std::move(schema),
        sourceDescriptor.logicalSourceName,
        sourceDescriptor.sourceType,
        sourceDescriptor.numberOfBuffersInSourceLocalBufferPool,
        sourceDescriptor.parserConfig,
        std::move(copyOfConfig));
}
}
