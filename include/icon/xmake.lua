 
target("icon_test")
    set_kind("binary")
    add_files("/*.cpp")
    after_build(function (target)
        os.cp("$(scriptdir)/fa-solid-900.ttf", "$(builddir)/$(host)/$(arch)/$(mode)/assets/fa-solid-900.ttf")
        print("$(builddir)/$(host)/$(arch)/$(mode)/assets/fa-solid-900.ttf")
    end)