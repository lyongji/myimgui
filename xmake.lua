add_rules("mode.debug", "mode.release")

set_languages("cxx23")
set_encodings("utf-8")

add_requires("imgui v1.92.1-docking",{configs= {sdl3 = true ,sdl3_renderer = true } })
add_requires("libsdl3")
add_requires("libsdl3_image")
add_requires("libsdl3_ttf", { configs = { runtimes = "MDd" } })
add_requires("glm",{configs= {cxx_standard = "20" } })
add_packages("libsdl3","imgui","libsdl3_image","libsdl3_ttf","glm")


add_links("SDL3_mixer")--链接SDL3_mixer.lib
add_links("ib_pinyin_c")--链接ib_pinyin_c.lib
add_linkdirs("/lib")

add_includedirs("include/","include/icon/","include/imsearch/","include/ib_pinyin/","include/imguiNodeEditor/")

target("imsearch") --创建imsearch库
    set_kind("static")
    add_files("./include/imsearch/*.cpp")
target("imguiNodeEditor") --创建imguiNodeEditor库   
    set_kind("static")
    add_files("./include/imguiNodeEditor/*.cpp")


includes("**/xmake.lua")--搜索目录下所有子构建

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
