class YdkCore < Formula
  desc "generate API bindings to YANG data models"
  homepage "https://github.com/abhikeshav/ydk-cpp/blob/master/README.md"
  url "https://github.com/abhikeshav/ydk-cpp/archive/0.5.2.tar.gz"
  sha256 "5b3194f58d52ac08559fba7af2a026bc752cbc6fa1067cf361e54d354ac3c987"

  depends_on "cmake" => :build
  depends_on "boost"
  depends_on "boost-python"
  depends_on "pkg-config" => :build
  depends_on "libssh"
  depends_on :x11 => :optional

  def install
    cd "core/ydk" do
      mkdir("build")
      cd "build" do
        system "cmake", "..", *std_cmake_args
        system "make", "install"
      end
    end
  end

  test do
    system "brew", "ls", "--versions", "ydk-cpp"
  end
end
