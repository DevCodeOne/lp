from conans import ConanFile, CMake

# udevpp, bcmhost ?
class LearnPlattform(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "spdlog/1.1.0@bincrafters/stable", "jsoncpp/1.0.0@theirix/stable", "Qt/5.9.7@bincrafters/stable", "libqrencode/4.0.0@bincrafters/stable"
    generators = "cmake", "ycm"
    default_options = {"Qt:shared" : True}

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
