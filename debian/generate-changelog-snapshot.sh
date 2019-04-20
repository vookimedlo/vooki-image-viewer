#!/bin/sh

tag=`git tag -l v* | sort -V | tail -1`
[ `git log --exit-code $tag..HEAD | wc -l` -ne 0 ] && gbp dch -s $tag -S --no-multimaint --nmu --ignore-branch --snapshot-number="'{:%Y%m%d%H%M%S}'.format(__import__('datetime').datetime.fromtimestamp(`git log -1 --pretty=format:%at`))"

sed -i 's/^\(vookiimageviewer [^)]\+\)/\1-1/' debian/changelog
