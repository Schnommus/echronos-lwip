#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that no right, title
# or interest in or to any trade mark, service mark, logo or trade name
# of NICTA or its licensors is granted.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# @TAG(NICTA_AGPL)
#

from prj import Module


class FpTestModule(Module):
    xml_schema = """
<schema>
    <entry name="variant" type="c_ident" />
    <entry name="doubles_test" type="bool" default="true" />
    <entry name="verbose" type="bool" default="true" />
    <entry name="super_verbose" type="bool" default="false" />
</schema>"""

    files = [
        {'input': 'fp-test.c', 'render': True, 'type': 'c'},
    ]

module = FpTestModule()
