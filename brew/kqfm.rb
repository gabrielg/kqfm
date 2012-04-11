# Homebrew formula for kqfm.

require 'formula'

class Kqfm < Formula
  homepage 'https://github.com/gabrielg/kqfm'
  url 'https://github.com/gabrielg/kqfm/tarball/v1.0.0'
  md5 '43d8dd9594f08962b4b69689ad3e5f65'

  def install
    man1.mkpath
    bin.mkpath

    system "make install PREFIX=#{prefix}"
  end

  def test
    system "kqfm -v"
  end
end