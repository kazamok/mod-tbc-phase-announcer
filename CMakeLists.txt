# Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
#
# This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU Affero General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.

# CMake 최소 버전 설정 (Set minimum CMake version)
cmake_minimum_required(VERSION 3.5)

# 모듈 이름과 스크립트 설정 (Set module name and scripts)
set(MODULE_NAME "mod-tbc-phase-announcer")

# 데이터 디렉토리 추가 (Add data directories)
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/mod_tbc_phase_announcer.cpp")

# 설정 파일 설치 (Install configuration file)
if(NOT WIN32)
    set(CONF_DIR ${CMAKE_INSTALL_PREFIX}/etc)
else()
    set(CONF_DIR ${CMAKE_INSTALL_PREFIX}/configs)
endif()

install(FILES ${CMAKE_CURRENT_LIST_DIR}/conf/mod-tbc-phase-announcer.conf.dist DESTINATION ${CONF_DIR})


