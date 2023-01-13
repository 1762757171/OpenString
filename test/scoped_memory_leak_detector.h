#pragma once
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
		_CrtMemState state_ending, state_difference;
		_CrtMemCheckpoint(&state_ending);
		if (_CrtMemDifference(&state_difference, &memory_state_, &state_ending))
		{
			_CrtMemDumpAllObjectsSince( &memory_state_ );
			std::cout << "Detector@ " << filename_ << " (" << line_ << "): Memory leak detected!" << std::endl;
		}
	}
	
private:
	_CrtMemState memory_state_ { nullptr };
	const char* filename_;
	int line_;
};

#define SCOPED_DETECT_MEMORY_LEAK scoped_memory_leak_detector detector{ __FILE__, __LINE__ };
