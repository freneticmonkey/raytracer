
local SDL_PATH = "C:/Dev/Resources/C++/SDL2-2.0.0"

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
   includedirs { SDL_PATH .. "/include", "raytracer/glm" }
   libdirs { SDL_PATH .. "/lib/x86" }
   vpaths { ["include"] = "**.h" }
   vpaths { ["source"] = "**.h" }
   files { "raytracer/**.h", "raytracer/**.cpp" }
   links { "SDL2", "SDL2main" }
   
   -- NYI
   --configuration { "macosx" }
         --linkoptions { "-framework OpenGL"}

   filter "configurations:Debug"
      flags { "Symbols" }
      defines { "_DEBUG" }

   filter "configurations:Release"
      flags { "Optimize" }
      defines { "NDEBUG" }
