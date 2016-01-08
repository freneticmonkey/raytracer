
local SDL_PATH = "C:/Development/Resources/SDL/SDL2-2.0.4"

solution "raytracer"
   filename "raytracer"
   location "projects"
   configurations { "Debug", "Release" }
   platforms { "Win32", "macosx" }

   filter "platforms:Win32"
      system "Windows"
      architecture "x32"

project "raytracer"
   kind "ConsoleApp"
   language "C++"
   targetdir( "build" )
   includedirs { "raytracer/glm", "task/task" }
   
   filter "platforms:Win32"
      includedirs { SDL_PATH .. "/include" }
      libdirs { SDL_PATH .. "/lib/x86" }
      links { "SDL2", "SDL2main" }
   
   filter "platforms:macosx"
      links { "SDL2.framework" }
      buildoptions "-std=c++11 -stdlib=libc++"

   vpaths { ["include"] = "**.h" }
   vpaths { ["source"] = "**.h" }
   files { "raytracer/**.h", "raytracer/**.cpp" }
   
   -- NYI
   --configuration { "macosx" }
         --linkoptions { "-framework OpenGL"}

   filter "configurations:Debug"
      flags { "Symbols" }
      defines { "_DEBUG" }

   filter "configurations:Release"
      flags { "Optimize" }
      defines { "NDEBUG" }
