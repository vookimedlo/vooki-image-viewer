#!/bin/sh

# Taken from: https://gist.github.com/nikicat/4653121 and changed for my needs

# Requires: sudo apt-get install -y moreutils git-buildpackage
#

rm debian/changelog || true

>debian/changelog
prevtag=v2017.10.27
pkgname=`cat debian/control | grep '^Package: ' | sed 's/^Package: //'`
git tag -l v* | sort -V | while read tag; do
    (echo "$pkgname (${tag#v}) unstable; urgency=low\n"; git log --pretty=format:'  * %s' $prevtag..$tag; git log --pretty='format:%n%n -- %aN <%aE>  %aD%n%n' $tag^..$tag) | cat - debian/changelog | sponge debian/changelog
        prevtag=$tag
done
