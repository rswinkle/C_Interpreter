-- A solution contains projects, and defines the available configurations
solution "CInterpreter"
	configurations { "Debug", "Release" }
	location "build"

	-- A project defines one build target
	project "cinterpreter"
		location "build"
		kind "ConsoleApp"
		language "C"
		files
		{
			"src/*.c",
			"src/*.h"
		}
		--excludes { }
		--      libdirs { }
		--links { } 
		targetdir "build"

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" }

		configuration { "linux", "gmake" }
			buildoptions { "-std=c99", "-pedantic-errors", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }
