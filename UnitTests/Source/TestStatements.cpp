/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;

TEST_CASE("Test Statements", "[Statements]")
{
	SECTION("Test comments")
	{
		const char * scriptText =
			u8R"(

			-- Single line comment

			--- Multiline 
				comment ---
				
			---------
			Alternate
			multiline
			comment
			---------

			a --- some comment --- is 123
   
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 123);
	}

	SECTION("Test variables and basic statements")
	{
		const char * scriptText =
			u8R"(

			a is "Hello world!"
			b is 5.5
			c is 123
			d is true
			e is null

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetString() == "Hello world!");
		REQUIRE(script->GetVariable("b").GetNumber() == 5.5);
		REQUIRE(script->GetVariable("c").GetInteger() == 123);
		REQUIRE(script->GetVariable("d").GetBoolean() == true);
		REQUIRE(script->GetVariable("e").IsNull() == true);
	}

	SECTION("Test variable and property type")
	{
		const char * scriptText =
			u8R"(

			a is 123
			private b is 234
			c is a type
			d is false
			if a type = b type
				d is true
			end
			e is false
			if a type = integer
				e is true
			end
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c").IsValType());
		REQUIRE(script->GetVariable("c").GetValType() == ValueType::Integer);
		REQUIRE(script->GetVariable("d").GetBoolean() == true);
		REQUIRE(script->GetVariable("e").GetBoolean() == true);
	}

	SECTION("Test alternate numeric form parsing")
	{
		const char * scriptText =
			u8R"(
    
			a is -375.5
			b is .11111
			c is -999
			d is 00001

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetNumber() == -375.5);
		REQUIRE(script->GetVariable("b").GetNumber() == Approx(.11111));
		REQUIRE(script->GetVariable("c").GetInteger() == -999);
		REQUIRE(script->GetVariable("d").GetInteger() == 1);
	}

	SECTION("Test variable scope")
	{
		const char * scriptText =
			u8R"(

			-- Scope test - a will not be visible after end
			begin
				a is 42
			end

			-- Scope test - g will be visible inside scope block
			b is 999
			begin
				b is 55
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsNull() == true);
		REQUIRE(script->GetVariable("b").GetInteger() == 55);
	}

	SECTION("Test common math operators")
	{
		const char * scriptText =
			u8R"(
    
			-- Basic assignments 
			a is 2
			b is 3

			-- Basic math operations
			c is a + b
			d is b - a
			e is a * b
			f is b / 1
			g is 10 mod b

			-- Multiple operations with and without parentheses
			h is 1 + 2 * 3     -- h is 9
			i is 1 + (2 * 3)   -- i is 7

			-- Floating point assignments
			j is 123.456
			k is 234.567

			-- Floating point operations
			l is j * k
			m is j / k
			n is j + k
			o is j - k

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 2);
		REQUIRE(script->GetVariable("b").GetInteger() == 3);
		REQUIRE(script->GetVariable("c").GetInteger() == 5);
		REQUIRE(script->GetVariable("d").GetInteger() == 1);
		REQUIRE(script->GetVariable("e").GetInteger() == 6);
		REQUIRE(script->GetVariable("f").GetInteger() == 3);
		REQUIRE(script->GetVariable("g").GetInteger() == 1);
		REQUIRE(script->GetVariable("h").GetInteger() == 9);
		REQUIRE(script->GetVariable("i").GetInteger() == 7);
		REQUIRE(script->GetVariable("j").GetNumber() == Approx(123.456));
		REQUIRE(script->GetVariable("k").GetNumber() == Approx(234.567));
		REQUIRE(script->GetVariable("l").GetNumber() == Approx(28958.703552));
		REQUIRE(script->GetVariable("m").GetNumber() == Approx(0.52631444320812390489710828888974));
		REQUIRE(script->GetVariable("n").GetNumber() == Approx(358.023));
		REQUIRE(script->GetVariable("o").GetNumber() == Approx(-111.111));
	}

	SECTION("Test equality and inequality operators")
	{
		const char * scriptText =
			u8R"(
			
			a is true = true		-- true
			b is true != true		-- false
			c is not true = true	-- false
			d is true and false  	-- false
			e is true or false		-- true

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetBoolean() == true);
		REQUIRE(script->GetVariable("b").GetBoolean() == false);
		REQUIRE(script->GetVariable("c").GetBoolean() == false);
		REQUIRE(script->GetVariable("d").GetBoolean() == false);
		REQUIRE(script->GetVariable("e").GetBoolean() == true);
	}

	SECTION("Test less than or greater than operators")
	{
		const char * scriptText =
			u8R"(
			
			a is 1 < 2				
			b is 2 < 1	
			
			c is 1 > 2				
			d is 2 > 1	
			
			e is 1 <= 2
			f is 1 <= 1
			g is 2 <= 1

			h is 1 >= 2
			i is 1 >= 1
			j is 2 >= 1

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetBoolean() == true);
		REQUIRE(script->GetVariable("b").GetBoolean() == false);
		REQUIRE(script->GetVariable("c").GetBoolean() == false);
		REQUIRE(script->GetVariable("d").GetBoolean() == true);
		REQUIRE(script->GetVariable("e").GetBoolean() == true);
		REQUIRE(script->GetVariable("f").GetBoolean() == true);
		REQUIRE(script->GetVariable("g").GetBoolean() == false);
		REQUIRE(script->GetVariable("h").GetBoolean() == false);
		REQUIRE(script->GetVariable("i").GetBoolean() == true);
		REQUIRE(script->GetVariable("j").GetBoolean() == true);
	}

	SECTION("Test increment and decrement operators")
	{
		const char * scriptText =
			u8R"(
    
			-- Increment and decrement test
			a is 1
			increment a
			b is 1
			decrement b
			c is 1
			increment c by 4
			d is 1
			decrement d by 3
			e is 1
			increment e by 4 * (2 + 2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 2);
		REQUIRE(script->GetVariable("b").GetInteger() == 0);
		REQUIRE(script->GetVariable("c").GetInteger() == 5);
		REQUIRE(script->GetVariable("d").GetInteger() == -2);
		REQUIRE(script->GetVariable("e").GetInteger() == 17);
	}


}