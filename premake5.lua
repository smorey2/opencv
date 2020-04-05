workspace "opencv"
    configurations { "Debug", "Release" }

project "cv"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfgs}"

    files { "src/**.h", "src/**.cpp" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

if os.ishost "windows" then

    -- Windows
    newaction
    {
        trigger     = "solution",
        description = "Open the solution",
        execute = function ()
            os.execute "start opencv.sln"
        end
    }

    newaction
    {
        trigger     = "native",
        description = "Build the native library",
        execute = function ()
             os.execute "mkdir _build64 & pushd _build64 \z
 && cmake -G \"Visual Studio 15 2017 Win64\" ..\\ \z
 && popd \z
 && cmake --build _build64 --config Debug"
        end
    }

    newaction
    {
        trigger     = "install",
        description = "Install Opencv",
        execute = function ()
             os.execute "cd _vcpkg && vcpkg integrate install"
        end
    }

    newaction
    {
        trigger     = "uninstall",
        description = "Uninstall Opencv",
        execute = function ()
             os.execute "cd _vcpkg && vcpkg integrate remove"
        end
    }

else

     -- MacOSX and Linux.
    
     newaction
     {
         trigger     = "solution",
         description = "Open the solution",
         execute = function ()
         end
     }
 
     newaction
     {
         trigger     = "loc",
         description = "Count lines of code",
         execute = function ()
             os.execute "wc -l *.cs"
         end
     }
     
end

newaction
{
    trigger     = "clean",
    description = "Clean all build files and output",
    execute = function ()
        files_to_delete = 
        {
            "*.make",
            "*.zip",
            "*.tar.gz",
            "*.db",
            "*.opendb"
        }
        directories_to_delete = 
        {
            "obj",
            "ipch",
            "bin",
            "nupkgs",
            ".vs",
            "Debug",
            "Release",
            "release"
        }
        for i,v in ipairs( directories_to_delete ) do
          os.rmdir( v )
        end
        if not os.ishost "windows" then
            os.execute "find . -name .DS_Store -delete"
            for i,v in ipairs( files_to_delete ) do
              os.execute( "rm -f " .. v )
            end
        else
            for i,v in ipairs( files_to_delete ) do
              os.execute( "del /F /Q  " .. v )
            end
        end

    end
}
