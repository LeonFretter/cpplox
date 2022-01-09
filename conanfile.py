
from conans import ConanFile, CMake, tools


class ProjectName(ConanFile):
    name = "cpplox"
    version = "0.1"
    author = "Leon Fretter"
    description = ""
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="")
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="include", keep_path=True)
        self.copy("*.so", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["project_name"]
        self.cpp_info.includedirs = ["include"]
