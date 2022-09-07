#!/usr/bin/env python
# Copyright (C) 2013  Jamie Duncan (jduncan@redhat.com)

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
"""
git2changelog module packaging configurationa

Fork of https://github.com/jduncan-rva/git2changelog
"""

from distutils.core import setup

version = '0.2'
name = 'git2changelog'

setup(
    name=name,
    license='GPLv2',
    version=version,
    description='To analyze git repositories and create formatted changelogs',
    author='Jamie Duncan',
    author_email='jduncan@redhat.com',
    url='https://github.com/weakcamel/git2changelog',
    maintainer='Waldek Maleska',
    maintainer_email='w.maleska@gmail.com',
    long_description='git2changelog analyze gits repositories and generates formatted changelogs',
    package_dir={'': 'src'},
    py_modules=['git2changelog'],
    scripts=['scripts/git2changelog'],
    data_files=[
        ('/usr/share/doc/%s-%s' % (name, version), ['doc/LICENSE']),
        ('/usr/share/man/man8', ['doc/git2changelog.8.gz']),
    ],
    install_requires=[
        "python-dateutil",
    ],
    )
