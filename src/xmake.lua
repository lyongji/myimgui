 
target("my_command")
    set_kind("binary")
    add_files("/*.cpp")
    add_deps("imsearch")
    after_build(function (target)
        os.cp("./assets", "$(builddir)/$(host)/$(arch)/$(mode)/")
        print("$(builddir)/$(host)/$(arch)/$(mode)/assets")
    end)

 