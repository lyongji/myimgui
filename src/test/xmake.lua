 
target("test")
    set_kind("binary")
    add_files("/*.cpp")
    after_build(function (target)
        os.cp("./assets", "$(builddir)/$(host)/$(arch)/$(mode)/assets")
        print("$(builddir)/$(host)/$(arch)/$(mode)/assets")
    end)

 