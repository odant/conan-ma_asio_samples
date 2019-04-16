from conans import ConanFile
import os


class MaasiosamplesConan(ConanFile):
    name = "ma_asio_samples"
    version = "1.3.0+0"
    license = "Boost Software License - Version 1.0. http://www.boost.org/LICENSE_1_0.txt"
    description = \
'''    
Examples (code samples) describing the construction of active objects on the top of Boost.Asio.
A code-based guide for client/server creation with usage of active object pattern by means of Boost C++ Libraries.
'''
    url = "https://github.com/odant/conan-ma_asio_samples"
    exports_sources = "src/*"
    no_copy_source = True
    
    def package(self):
        libs = os.path.join(self.source_folder, "src", "libs")
        for folder in os.listdir(libs):
            folder = os.path.join(libs, folder)
            if os.path.isdir(folder):
                self.output.info("Processing %s" % folder)
                src = os.path.join(folder, "include")
                self.copy("*.hpp", dst="include", src=src, keep_path=True)

    def package_id(self):
        self.info.header_only()
