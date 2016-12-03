class YdkOpenconfig < Formula
  desc "generate API bindings to YANG data models"
  homepage "https://github.com/abhikeshav/ydk-cpp/blob/master/README.md"
  url "https://github.com/CiscoDevNet/ydk-cpp/archive/0.5.2.tar.gz"
  sha256 "39ca26b57e0d784243ebd0c07eb0e35fc0ad8600886fde2be4440eae898b844d"

  depends_on "cmake" => :build
  depends_on "boost"
  depends_on "boost-python"
  depends_on "pkg-config" => :build
  depends_on "libssh"
  depends_on :x11 => :optional

  def install
    cd "openconfig" do
      mkdir("build")
      cd "build" do
        system "cmake", "..", *std_cmake_args
        system "make"
        system "make", "install"
      end
    end
  end

  test do
    system "brew", "ls", "--versions", "ydk-openconfig"
  end
end
