#include "pch.h"
#include "common/basic_types.h"
#include "text.h"

void std_string_construct(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string empty_string;
	}
}

void codeunit_sequence_construct(benchmark::State& state)
{
	for (auto _ : state)
	{
		easy::codeunit_sequence empty_sequence;
	}
}

BENCHMARK(std_string_construct);
BENCHMARK(codeunit_sequence_construct);