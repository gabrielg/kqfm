# Homebrew formula for kqfm.

require 'formula'

class Kqfm < Formula
  homepage 'https://github.com/gabrielg/kqfm'
  url 'https://github.com/gabrielg/kqfm/tarball/v1.0.1'
  md5 '042750478912bc11cc49323fa582af01'

  def install
    man1.mkpath
    bin.mkpath

    system "make install PREFIX=#{prefix}"
  end

  def test
    system "kqfm -v"
  end
end
