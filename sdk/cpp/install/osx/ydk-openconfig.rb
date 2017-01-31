class YdkOpenconfig < Formula
  desc "generate API bindings to YANG data models"
  homepage "https://github.com/CiscoDevNet/ydk-cpp/blob/master/README.md"
  url "https://github.com/CiscoDevNet/ydk-cpp/archive/0.5.2.tar.gz"
  sha256 "39ca26b57e0d784243ebd0c07eb0e35fc0ad8600886fde2be4440eae898b844d"

  depends_on "ydk"
  depends_on "ydk-ietf"
  depends_on "cmake" => :build
  depends_on "boost"
  depends_on "boost-python"
  depends_on "curl"
  depends_on "libssh"
  depends_on "pcre"
  depends_on "xml2"
  depends_on "pkg-config" => :build
  
  def install
    mkdir "openconfig/build" do
      system "cmake", "..", *std_cmake_args
      system "make", "install"
    end
  end

  test do
    (testpath/"test.cpp").write <<-EOS.undent
      #include <ydk_openconfig/openconfig_bgp_policy.hpp>
      int main() {
        return 0;
      }
    EOS
    system ENV.cxx, "-std=c++11", "-Wall", "-Wextra", "-g", "-O0",
    "test.cpp", "-otest", "-lboost_log_setup-mt", "-lboost_log-mt",
    "-lboost_thread-mt", "-lboost_date_time-mt", "-lboost_system-mt",
    "-lboost_filesystem-mt", "-lboost_log_setup-mt", "-lboost_log-mt",
    "-lboost_thread-mt", "-lboost_date_time-mt", "-lboost_system-mt",
    "-lboost_filesystem-mt", "-lboost_log_setup-mt", "-lboost_log-mt",
    "-lboost_filesystem-mt", "-lboost_system-mt", "-lxml2", "-lcurl",
    "-lssh_threads", "-lpcre", "-lxslt", "-lssh", "-lpthread", "-ldl",
    "-lydk"
    system "./test"
  end
end
