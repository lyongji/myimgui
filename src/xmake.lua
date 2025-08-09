 
target("主项目")
    set_kind("binary")
    add_files("/**.cpp")
    add_deps("imsearch","imguiNodeEditor")
    after_build(function (target)
        os.cp("./assets", "$(builddir)/$(host)/$(arch)/$(mode)/")
        print("$(builddir)/$(host)/$(arch)/$(mode)/assets")
    end)

 