project "N503.CppWin32.Renderer2D"
    kind          "SharedLib" -- ConsoleApp WindowedApp SharedLib StaticLib
    language      "C++"
    cppdialect    "C++20"
    staticruntime "off"
    characterset  "Unicode"

    pchheader "Pch.hpp"
    pchsource "Source/Pch.cpp"

    targetdir  ("%{wks.location}/Binaries/" .. OutputDir .. "/")
    objdir     ("%{wks.location}/Binaries-Intermediates/" .. OutputDir .. "/%{prj.name}")

    vsprops
    {
        -- VcpkgTriplet = "x64-windows-static",
        -- VcpkgEnabled = "true",
        PublicIncludeDirectories = "Include",
    }

    nuget
    {
        "Microsoft.Windows.ImplementationLibrary:1.0.260126.7",
    }

    defines
    {
        "N503_DLL_EXPORTS",
    }

    files
    {
        "%{prj.name}.dll.manifest", "Include/**.h", "Include/**.hpp", "Source/**.cpp", "Source/**.hpp",
    }

    includedirs
    {
        "Include", "Source",
    }

    links
    {
        "d2d1.lib", "d3d11.lib", "dxgi.lib", "dwrite.lib", "windowscodecs.lib", 
        "N503.CppWin32.Abi", "N503.CppWin32.Memory", "N503.CppWin32.Diagnostics", "N503.CppWin32.Core", 
    }

    prebuildcommands
    {
    }

    postbuildcommands
    {
    }
