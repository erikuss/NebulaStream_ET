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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <Aggregation/Function/AggregationFunction.hpp>
#include <Aggregation/Function/AvgAggregationFunction.hpp>
#include <DataTypes/DataType.hpp>
#include <Functions/PhysicalFunction.hpp>
#include <Nautilus/DataTypes/VarVal.hpp>
#include <Nautilus/Interface/Record.hpp>
#include <Runtime/AbstractBufferProvider.hpp>
#include <nautilus/std/cstring.h>
#include <val.hpp>
#include <val_concepts.hpp>
#include <val_ptr.hpp>

namespace NES
{

AvgAggregationFunction::AvgAggregationFunction(
    DataType inputType,
    DataType resultType,
    PhysicalFunction inputFunction,
    Nautilus::Record::RecordFieldIdentifier resultFieldIdentifier,
    DataType countType)
    : AggregationFunction(std::move(inputType), std::move(resultType), std::move(inputFunction), std::move(resultFieldIdentifier))
    , countType(std::move(countType))
{
}

void AvgAggregationFunction::lift(
    const nautilus::val<AggregationState*>& aggregationState,
    PipelineMemoryProvider& pipelineMemoryProvider,
    const Nautilus::Record& record)
{
    /// Reading old sum and count from the aggregation state. The sum is stored at the beginning of the aggregation state and the count is stored after the sum
    const auto memAreaSum = static_cast<nautilus::val<int8_t*>>(aggregationState);
    const auto memAreaCount = static_cast<nautilus::val<int8_t*>>(aggregationState) + nautilus::val<uint64_t>(inputType.getSizeInBytes());
    const auto sum = Nautilus::VarVal::readVarValFromMemory(memAreaSum, inputType.type);
    const auto count = Nautilus::VarVal::readVarValFromMemory(memAreaCount, countType.type);

    /// Updating the sum and count with the new value
    const auto value = inputFunction.execute(record, pipelineMemoryProvider.arena);
    const auto newSum = sum + value;
    const auto newCount = count + nautilus::val<uint64_t>(1);

    /// Writing the new sum and count back to the aggregation state
    newSum.writeToMemory(memAreaSum);
    newCount.writeToMemory(memAreaCount);
}

void AvgAggregationFunction::combine(
    const nautilus::val<AggregationState*> aggregationState1,
    const nautilus::val<AggregationState*> aggregationState2,
    PipelineMemoryProvider&)
{
    /// Reading the sum and count from the first aggregation state
    const auto memAreaSum1 = static_cast<nautilus::val<int8_t*>>(aggregationState1);
    const auto memAreaCount1 = static_cast<nautilus::val<int8_t*>>(aggregationState1) + nautilus::val<uint64_t>(inputType.getSizeInBytes());
    const auto sum1 = Nautilus::VarVal::readVarValFromMemory(memAreaSum1, inputType.type);
    const auto count1 = Nautilus::VarVal::readVarValFromMemory(memAreaCount1, countType.type);

    /// Reading the sum and count from the second aggregation state
    const auto memAreaSum2 = static_cast<nautilus::val<int8_t*>>(aggregationState2);
    const auto memAreaCount2 = static_cast<nautilus::val<int8_t*>>(aggregationState2) + nautilus::val<uint64_t>(inputType.getSizeInBytes());
    const auto sum2 = Nautilus::VarVal::readVarValFromMemory(memAreaSum2, inputType.type);
    const auto count2 = Nautilus::VarVal::readVarValFromMemory(memAreaCount2, countType.type);

    /// Combining the sum and count
    const auto newSum = sum1 + sum2;
    const auto newCount = count1 + count2;

    /// Writing the new sum and count back to the first aggregation state
    newSum.writeToMemory(memAreaSum1);
    newCount.writeToMemory(memAreaCount1);
}

Nautilus::Record AvgAggregationFunction::lower(const nautilus::val<AggregationState*> aggregationState, PipelineMemoryProvider&)
{
    /// Reading the sum and count from the aggregation state
    const auto memAreaSum = static_cast<nautilus::val<int8_t*>>(aggregationState);
    const auto memAreaCount = static_cast<nautilus::val<int8_t*>>(aggregationState) + nautilus::val<uint64_t>(inputType.getSizeInBytes());
    const auto sum = Nautilus::VarVal::readVarValFromMemory(memAreaSum, inputType.type);
    const auto count = Nautilus::VarVal::readVarValFromMemory(memAreaCount, countType.type);

    /// Calculating the average and returning a record with the result
    const auto avg = sum.castToType(resultType.type) / count.castToType(resultType.type);
    return Nautilus::Record({{resultFieldIdentifier, avg}});
}

void AvgAggregationFunction::reset(const nautilus::val<AggregationState*> aggregationState, PipelineMemoryProvider&)
{
    /// Resetting the sum and count to 0
    const auto memArea = static_cast<nautilus::val<int8_t*>>(aggregationState);
    nautilus::memset(memArea, 0, getSizeOfStateInBytes());
}

void AvgAggregationFunction::cleanup(nautilus::val<AggregationState*>)
{
}

size_t AvgAggregationFunction::getSizeOfStateInBytes() const
{
    /// Size of the sum value + size of the count value
    const auto inputSize = inputType.getSizeInBytes();
    const auto countTypeSize = countType.getSizeInBytes();
    return inputSize + countTypeSize;
}

}
