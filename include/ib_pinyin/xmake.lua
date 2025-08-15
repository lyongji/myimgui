 
-- target("ibtest")--测试
--     set_kind("binary")
--     -- add_files("./include/ib_pinyin/ibtest.c")
--     add_files("./ibtestcpp.cpp")
--  -- 添加Windows系统库链接
--     if is_plat("windows") then
--         add_links("ws2_32", "userenv", "advapi32", "ntdll")
--     elseif is_plat("linux") then
--         add_links("pthread", "dl", "m", "rt")
--     end
 