
#pragma once

#ifdef _WIN64

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>

class scoped_memory_leak_detector
{
public:
	scoped_memory_leak_detector(const char* filename, const int line)
		: filename_(filename)
		, line_(line)
	{
		_CrtMemCheckpoint(&memory_state_);
	}

	~scoped_memory_leak_detector() {
		[[maybe_unused]] _CrtMemState state_ending;
		[[maybe_unused]] _CrtMemState state_difference;
		_CrtMemCheckpoint(&state_ending);
		if (_CrtMemDifference(&state_difference, &memory_state_, &state_ending))
		{
			_CrtMemDumpAllObjectsSince( &memory_state_ );
			std::cerr << "Detector@ " << filename_ << " (" << line_ << "): Memory leak detected!" << std::endl;
		}
	}

	scoped_memory_leak_detector(const scoped_memory_leak_detector&) = delete;
	scoped_memory_leak_detector(scoped_memory_leak_detector&&) = delete;
	scoped_memory_leak_detector& operator=(const scoped_memory_leak_detector&) = delete;
	scoped_memory_leak_detector& operator=(scoped_memory_leak_detector&&) = delete;
	
private:
	[[maybe_unused]] _CrtMemState memory_state_ {  };
	const char* filename_;
	int line_;
};

#define SCOPED_DETECT_MEMORY_LEAK() scoped_memory_leak_detector detector{ __FILE__, __LINE__ };

#else

#define SCOPED_DETECT_MEMORY_LEAK()

#endif
