/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../../Source/Jinx.h"

using namespace Jinx;

#define REQUIRE assert

Jinx::RuntimePtr TestCreateRuntime()
{
	return Jinx::CreateRuntime();
}

Jinx::ScriptPtr TestCreateScript(const char * scriptText, Jinx::RuntimePtr runtime = nullptr)
{
	if (!runtime)
		runtime = CreateRuntime();

	// Compile the text to bytecode
	auto bytecode = runtime->Compile(scriptText, "Test Script", { "core" });
	if (!bytecode)
		return nullptr;

	// Create a runtime script with the given bytecode
	return runtime->CreateScript(bytecode);
}

Jinx::ScriptPtr TestExecuteScript(const char * scriptText, Jinx::RuntimePtr runtime = nullptr)
{
	// Create a runtime script 
	auto script = TestCreateScript(scriptText, runtime);
	if (!script)
		return nullptr;

	// Execute script until finished
	do
	{
		if (!script->Execute())
			return nullptr;
	} 
	while (!script->IsFinished());

	return script;
}

int main(int argc, char ** argv)
{
	printf("Jinx version: %s\n", Jinx::GetVersionString().c_str());

	GlobalParams params;
	params.logBytecode = true;
	params.logSymbols = true;
	params.errorOnMaxInstrunctions = false;
	params.maxInstructions = std::numeric_limits<uint32_t>::max();
	Initialize(params);
	// Scope block to ensure all objects are destroyed for shutdown test
	{
		const char * scriptText =
			u8R"(

				private function {a} minus {b}
					return a - b
				end

				set x to 0
				loop from 1 to 3
					increment x
					wait
				end

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->Execute());
		auto id = script->FindFunction(nullptr, { "{} minus {}" });
		REQUIRE(id != InvalidID);
		while (!script->IsFinished())
		{
			auto val = script->CallFunction(id, { 5, 2 });
			REQUIRE(val == 3);
			REQUIRE(script->Execute());
		}
		REQUIRE(script->GetVariable("x") == 3);
	}
	ShutDown();
	return 0;
}
